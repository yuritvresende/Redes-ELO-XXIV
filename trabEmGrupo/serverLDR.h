/*******************************
 * Servidor WEB com dois links
 * um para ligar um led no D5 - GND
 * outro para deligar o LED
/******************************/

//https://www.iomaker.com.br/sensor-ldr-com-arduino-e-esp32/

#include <WiFi.h>
const char *ssid = "Breno";
const char *password = "12345678";
WiFiServer server(80);
#include <Arduino.h>

#include "./sensorLDR.h"


// definir porta de leitura do sensor de ldr
const int portaLDR = GPIO_NUM_35;
void setup7()
{
  Serial.begin(9600);

  // needs to be after serial.begin
  setupLDR();

  //LDR
  pinMode(portaLDR, INPUT);

  pinMode(5, OUTPUT); // modo pino 5
  delay(10);
  Serial.println();
  Serial.println();
  Serial.print("Conectando com "); // Inicia conexão Wifi
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("Endereco IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

int value = 0;

void loop7()
{

  loopLDR();

  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("Endereco IP: ");
  Serial.println("WiFi conectado.");
  Serial.println("Endereco IP: ");
  //192.168.139.22
  //192.168.22.56

  int leitura = analogRead(portaLDR);


  Serial.println(WiFi.localIP());
  WiFiClient client = server.available(); // listen for incoming clients
  if (client)
  {                                  // if you get a client,
    Serial.println("Novo Cliente."); // print a message out the serial port
    String currentLine = "";         // make a String to hold incoming data from the client
    while (client.connected())
    { // loop while the client's connected
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c);        // print it out the serial monitor
        if (c == '\n')
        { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Access-Control-Allow-Origin: *");
            client.println();

            // the content of the HTTP response follows the header:
            client.print(luminance_read_lux);
            //client.print(leitura);
            client.println(); // The HTTP response ends with another blank line:
            // break out of the while loop:
            break;
          }
          else
          { // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
        
      }
    }
    client.stop(); // fecha conexão
    Serial.println("Cliente desconectado.");
  }
} // final do loop()