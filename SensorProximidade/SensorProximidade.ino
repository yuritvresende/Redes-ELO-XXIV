const int PIN_TO_SENSOR = 33; // GIOP19 pin connected to OUTPUT pin of sensor
int pinStateCurrent   = LOW;  // current state of pin
int pinStatePrevious  = LOW;  // previous state of pin
const int PIN_WRITE = 34;

void setup() 
{
 Serial.begin(115200);          
 pinMode(PIN_TO_SENSOR, INPUT);
}

void loop() 
{
 pinStatePrevious = pinStateCurrent;
 pinStateCurrent = digitalRead(PIN_TO_SENSOR);   

 if (pinStatePrevious == LOW && pinStateCurrent == HIGH) 
 {  
  digitalWrite(PIN_WRITE, LOW);
  Serial.println("Motion detected!");
 }
 else if (pinStatePrevious == HIGH && pinStateCurrent == LOW) 
 {   
  digitalWrite(PIN_WRITE, HIGH);
  Serial.println("Motion stopped!");
 }
}
