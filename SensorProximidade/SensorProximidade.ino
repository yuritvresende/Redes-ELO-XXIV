const int PIN_TO_SENSOR = 33; 
int pinStateCurrent   = LOW;  
int pinStatePrevious  = LOW;  
const int PIN_WRITE = 34;

void setup() 
{
 Serial.begin(115200);          
 pinMode(PIN_TO_SENSOR, INPUT);
}

// O loop abaixo funciona verificando a variação no estado de tensão do pino conectado ao sensor de movimento.
void loop() 
{
 pinStatePrevious = pinStateCurrent;
 pinStateCurrent = digitalRead(PIN_TO_SENSOR);   
 // Uma vez que o sensor identifique movimento, a leitura do microcontrolador informa pelo terminal serial e abaixa o nível de tensão da porta 34.
 if (pinStatePrevious == LOW && pinStateCurrent == HIGH) 
 {  
  digitalWrite(PIN_WRITE, LOW);
  Serial.println("Movimento detectado.");
 }
 else if (pinStatePrevious == HIGH && pinStateCurrent == LOW) 
 {   
  digitalWrite(PIN_WRITE, HIGH);
  Serial.println("Movimento cessou.");
 }
}
