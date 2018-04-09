#include <Time.h>
#include <TimeLib.h>

// Constant fields
#define PH_SENSOR_PIN A0
#define EC_SENSOR_PIN A1
#define EC_PUMP_PIN 3
#define PH_PUMP_PIN 5
#define PUMP_PIN 7
#define LIGHT_PIN 8
#define MAX_OVERRIDE_TIME 300000 //ms = 5 minutes
#define PUMP_ON_TIME 30000 //ms
#define PUMP_OFF_TIME 86400000 //ms
#define UPDATE_CLOCK 10000 //ms
#define READ_SERIAL 1000 //ms
#define OFFSET 0.00 //deviation compensate
#define SAMPLE_INTERVAL 20 //ms
#define PRINT_INTERVAL 20000 //ms
#define ARRAY_LENGTH 40
int phArray[ARRAY_LENGTH];
int ecArray[ARRAY_LENGTH];
int ph_array_index = 0;
int ec_array_index = 0;
static unsigned long samplingTime;
static unsigned long printTime;
static unsigned long pumpOnTime;
static unsigned long pumpOffTime;
static unsigned long serialReadTime;
static unsigned long updateClockTime;
static unsigned long overrideTime;
static bool userDefinedMode;
static time_t currentTime;
//static float pHValue, ppmValue, voltage;
double pHValue = 0;
double voltage = 1;
double ppmValue = 0;

unsigned long ecValueTotal = 0;
double ecAverage = 0;

boolean isECPumpOn = false;
boolean isPHPumpOn = false;
boolean isPumpOn = false;
boolean isLightOn = false;
boolean startedPump = false;
//Used to determine if we should follow the schedule or not.
boolean overrideSchedule = false;

String serialString = "";

void setup() {
  Serial.begin(9600);
  pinMode(PUMP_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);
  samplingTime = millis();
  printTime = millis();
  serialReadTime = millis();
  pumpOffTime = millis();
  updateClockTime = millis();

  //Start with pump on????
  digitalWrite(PUMP_PIN, HIGH); //todo josh: high is off for testing...
  pumpOnTime = millis();
  isPumpOn = false;

  //Start with lights on
  digitalWrite(LIGHT_PIN, LOW); //Low should start it as on
  isLightOn = true;
  
}

void loop() {
  //Read values from serial port for commands and time update.
  if (millis() - serialReadTime > READ_SERIAL){
    if (Serial.available()){
      while( Serial.available() > 0){
        serialString = Serial.readString();
        //check string to match "PI: *"
        interpretString(serialString);
      }
    }
    serialReadTime = millis();
  }

  //Get samples often for accurate measurements.
  if (millis() - samplingTime > SAMPLE_INTERVAL){
    getReadingPH();
    getReadingEC();

    samplingTime = millis();
  }

  //Send reading data over serial port.
  if (millis() - printTime > PRINT_INTERVAL){
    Serial.print("ARDUINO: PH VALUE: ");
    Serial.println(pHValue);
    Serial.print("ARDUINO: PPM VALUE: ");
    Serial.println(ppmValue);
    printTime = millis();
  }

  //This is our safety measure to kill the pump after max PUMP_ON_TIME.
  if (isPumpOn && (millis() - pumpOnTime > PUMP_ON_TIME)){
    turnOffPump();
  }

  if (millis() - overrideTime > MAX_OVERRIDE_TIME){
    overrideSchedule = false;
  }
  
  //If the user overrides the schedule, ignore these checks
  if(!overrideSchedule){
    // Logic for the timing of lights and tank pump.
    if(hour() > 1 && hour() < 17){
      //Have the lights on for these 16 hours
      turnOnLights(false);
    }else{
      //have lights off for these 8 hours
      turnOffLights(false);
    }
  }

  if(hour()==1 && !startedPump){
    //at 1am turn the pump on for 30 seconds
    if(!isPumpOn){
      turnOnPump();
    }
    //Set timer to back to zero.
    pumpOnTime = millis();
    startedPump = true;
  }
  
  if(hour()==2 && startedPump){
    //Reset the daily pump scheudle.
    startedPump = false;
    
  }

  
}

void clearInputBuffer(){
  while(Serial.available())
  Serial.read();
}

void interpretString(String currentString){
  if(currentString.length() > 0){
   //clearInputBuffer();
    if(currentString.startsWith("PI: TIME: ")){
      String hms = currentString.substring(10);
      //int msTimeInt = msTime.toInt();
      int h = hms.substring(0,2).toInt();
      int m = hms.substring(2,4).toInt();
      int s = hms.substring(4,6).toInt();
      setTime(h,m,s,0,0,0);      
    }else if(contains(currentString, "PI: PUMP: ON")){
      turnOnPump();
      //Now we depend on the loops millis check to shut off the pump!
      pumpOnTime = millis();
    }else if(contains(currentString, "PI: PUMP: OFF")){
      turnOffPump(); 
    }else if(contains(currentString, "PI: LIGHTS: ON")){
      turnOnLights(true);
    }else if(contains(currentString, "PI: LIGHTS: OFF")){
      turnOffLights(true);
    }else if(contains(currentString, "PI: ECPUMP: ON")){
      turnOnECPump();
    }else if(contains(currentString, "PI: ECPUMP: OFF")){
      turnOffECPump();
    }else if(contains(currentString, "PI: PHPUMP: ON")){
      turnOnPHPump();
    }else if(contains(currentString, "PI: PHPUMP: OFF")){
      turnOffPHPump();
    }
  }
}

//This will turn the tank pump on for 30 seconds MAX.
//todo future josh: MAYBE WAIT x HOURS BEFORE ALLOWING IT TO TURN ON AGAIN
void turnOnPump(){
  digitalWrite(PUMP_PIN, LOW);
  isPumpOn = true;
  pumpOnTime = millis();
}

void turnOffPump(){
  digitalWrite(PUMP_PIN, HIGH);
  isPumpOn = false;
}

void turnOnLights(bool userOverride){
  setOverrideSchedule(userOverride);
  digitalWrite(LIGHT_PIN, LOW);
  isLightOn = true;
}

void turnOffLights(bool userOverride){
  setOverrideSchedule(userOverride);
  digitalWrite(LIGHT_PIN, HIGH);
  isLightOn = false;
}

void turnOnECPump(){
  analogWrite(EC_PUMP_PIN, 255);
  isECPumpOn = true;
}

void turnOffECPump(){
  analogWrite(EC_PUMP_PIN, 0);
  isECPumpOn = false;
}

void turnOnPHPump(){
  analogWrite(PH_PUMP_PIN, 255);
  isPHPumpOn = true;
}

void turnOffPHPump(){
  analogWrite(PH_PUMP_PIN, 0);
  isPHPumpOn = false;
}

void setOverrideSchedule(bool userOverride){
  overrideSchedule = userOverride;
  overrideTime = millis();
}
  
//This is exact patern matching from i to end of String always.
//Use startsWith to check front of string.
bool contains(String s, String search) {
    int max = s.length() - search.length();

    for (int i = 0; i <= max; i++) {
        if (s.substring(i) == search) return true; // or i
    }

    return false; //or -1
}

void getTimeFromSerial(String newLine){
  if(newLine.length()>0){
    clearInputBuffer();
    if(contains(newLine, "PI: TIME: ")){
      String msTime = newLine.substring(10);
      int msTimeInt = msTime.toInt();
      //currentTime.setTime(msTimeInt);
    }
  }
  
}

void getReadingPH(){
  phArray[ph_array_index++] = analogRead(PH_SENSOR_PIN);
  if (ph_array_index == ARRAY_LENGTH){
    ph_array_index = 0;
  }
  voltage = averagearray(phArray, ARRAY_LENGTH);
  pHValue = 3.5 * voltage + OFFSET;
  // ((analogRead(PH_SENSOR_PIN)*5.0/1024.0)*3.5);
  //voltage calculation for the ph reading
  //voltage = averageArray(phArray, ARRAY_LENGTH) * 5.0 /1024.0;
}

void getReadingEC(){
  if (ec_array_index == ARRAY_LENGTH) ec_array_index = 0;
  // todo josh: uncertain where the position for the increment shoudl be!
  ec_array_index += 1;
  ecValueTotal -= ecArray[ec_array_index];
  ecArray[ec_array_index] == analogRead(EC_SENSOR_PIN);
  // add ec reading to the array
  ecValueTotal += ecArray[ec_array_index];
  // todo Josh: not sure if to calculate average directly or to use method
  ecAverage = ecValueTotal / ARRAY_LENGTH;
  // todo Josh: not certain if the read in value is the voltage??
  ppmValue = ecAverage * 1000 / 1.56;
  // ecAverageVoltage = ecAverage * (float)5000/1024

  //Strange math I cannot find online anymore...
  // float tempCoefficient = 1.0 + 0.0185 * (22.0 - 25.0); //This can be done 1.0+0.0185*(temperature-25.0) of water temp for higher accuracy
  // float coefficientVoltage = (float)ecAverageVoltage / tempCoefficient;
}

double averagearray(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;
  if (number <= 0) {
    return 0;
  }
  if (number < 5) { //less than 5, calculated directly statistics
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0]; max = arr[1];
    }
    else {
      min = arr[1]; max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;
        //arr<min
        min = arr[i];
      } else {
        if (arr[i] > max) {
          amount += max;  //arr>max
          max = arr[i];
        } else {
          amount += arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount / (number - 2);
  }//if
  return avg;
}

