// Constant fields
#define PH_SENSOR_PIN A0
#define EC_SENSOR_PIN A1
#define EC_PUMP_PIN 3
#define PH_PUMP_PIN 5
#define PUMP_PIN 7
#define LIGHT_PIN 8
#define PUMP_ON_TIME 30000 //ms
#define PUMP_OFF_TIME 86400000 //ms
#define READ_SERIAL 1000 //ms
#define OFFSET 0.00 //deviation compensate
#define SAMPLE_INTERVAL 20 //ms
#define PRINT_INTERVAL 800 //ms
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
static bool userDefinedMode;
//static float pHValue, ppmValue, voltage;
double pHValue = 0;
double voltage = 1;
double ppmValue = 0;

unsigned long ecValueTotal = 0;
double ecAverage = 0;

boolean isPumpOn = false;

String newLine = "";

void setup() {
  Serial.begin(9600);
  pinMode(PUMP_PIN, OUTPUT);
  samplingTime = millis();
  printTime = millis();
  serialReadTime = millis();
  pumpOffTime = millis();

  //start with pump on????
  digitalWrite(PUMP_PIN, LOW);
  pumpOnTime = millis();
  isPumpOn = true;
}

void loop() {


  if (millis() - samplingTime > SAMPLE_INTERVAL)
  {
    getReadingPH();
    getReadingEC();
    
    samplingTime = millis();
  }

  if ((millis() - pumpOnTime > PUMP_ON_TIME) && isPumpOn){
    //turn on pump: this works...the digital write logic is switched
    Serial.println("#####");
    Serial.println("TURN PUMP OFF");
    Serial.println("#####");
    digitalWrite(PUMP_PIN, HIGH);
    isPumpOn = false;
    pumpOffTime = millis();
  }
  
  if ((millis() - pumpOffTime > PUMP_OFF_TIME) && !isPumpOn){
    //turn off pump: this works...the digital write logic is switched
    Serial.println("#####");
    Serial.println("TURN PUMP ON");
    Serial.println("#####");
    digitalWrite(PUMP_PIN, LOW);
    isPumpOn = true;
    pumpOnTime = millis();
  }

  if (millis() - printTime > PRINT_INTERVAL){
    Serial.print("Arduino: pH Value: ");
    Serial.println(pHValue);
    Serial.print("Arduino: ppm Value: ");
    Serial.println(ppmValue);
    printTime = millis();
  }

  if (millis() - serialReadTime > READ_SERIAL){
    clearInputBuffer();
    if (Serial.available()){
      while( Serial.available() > 0){
        char c = Serial.read();
        if (c == '\n'){
          break;
        }
        newLine += c;  
      }
      //check string to match "PI:"
      interpretString();
    }
    serialReadTime = millis();
  }

}

void clearInputBuffer(){
  while(Serial.available())
  Serial.read();
}

void interpretString(){
  if(newLine.length() > 0){
    clearInputBuffer();
    Serial.println(newLine);
    if(contains(newLine, "PI: TIME:")){
      Serial.println("contains TIME");
    }else if(contains(newLine, "PI: PUMP: ON")){
      Serial.println("contains PI: PUMP: ON");
      digitalWrite(PUMP_PIN, LOW);
      pumpOnTime = millis();
      isPumpOn = true;
      //if(contains(newLine, "PUMP: ON")){
        //Serial.println("contains PUMP ON");
        //digitalWrite(PUMP_PIN, LOW);
        //pumpOnTime = millis();
        //isPumpOn = true;
      //}
    }else if(contains(newLine, "PI: PUMP: OFF")){
      Serial.println("contains PI: PUMP: OFF");
      digitalWrite(PUMP_PIN, HIGH);
      isPumpOn = false;
      //TODO JOSH: This will reset the off time, is this what you want? or to not change it?
      pumpOffTime = millis();
    }else if(contains(newLine, "PI: LIGHTS: ON")){
      digitalWrite(LIGHT_PIN, HIGH);
      userDefinedMode = true;
      //TODO JOSH: Once we use the CLOCK feature from the PI, 
      //we can use a boolean to determine if the preprogrammed schedule is 
      //being overridden by the user, and we can reset the schedule with a button press.
    }else if(contains(newLine, "PI: LIGHTS: OFF")){
      digitalWrite(LIGHT_PIN, LOW);
      userDefinedMode = true;
    }else if(contains(newLine, "PI: EC PUMP: ON")){
      //TODO JOSH: ADD METHOD TO TURN PUMP ON FOR A COUPLE SECONDS.
      analogWrite(EC_PUMP_PIN, HIGH);
      userDefinedMode = true;
    }else if(contains(newLine, "PI: PH PUMP: ON")){
      //TODO JOSH: ADD METHOD TO TURN PUMP ON FOR A COUPLE SECONDS.
      analogWrite(PH_PUMP_PIN, HIGH);
      userDefinedMode = true;
    }else if(contains(newLine, "PI: RESET CYCLE")){
      //TODO JOSH: THIS IS DONE ONCE CLOCK IS IMPLEMENTED. 
      //TODO JOSH: NEED METHOD TO RESET TO PRE-DEFINED CYCLE
    }
  }
  newLine = "";
}
  

bool contains(String s, String search) {
    int max = s.length() - search.length();

    for (int i = 0; i <= max; i++) {
        if (s.substring(i) == search) return true; // or i
    }

    return false; //or -1
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
    Serial.println("Error number for the array to avraging!/n");
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

