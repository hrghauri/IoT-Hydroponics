#define DC_PUMP_1 3

void setup()  // setup loop
{
  pinMode(DC_PUMP_1, OUTPUT); // declares pin 5 as output
}
void loop()
{
analogWrite(DC_PUMP_1, 255);
delay(2000);
analogWrite(DC_PUMP_1, 0);
delay(2000);
}
