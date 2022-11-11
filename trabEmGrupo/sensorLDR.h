#include <Arduino.h>
#include <analogWrite.h>

#define LEDPIN 35
#define LDRPIN 34

#define VIN 3.3    // V power voltage, 3.3v in case of NodeMCU
#define R 10000    // Voltage devider resistor value
#define MAXLUM 500 // Maximum luminance value [lux]

int read_value = 0;
int luminance_read_lux;

int wait_time = 500;            // Time delay [ms]
int counter = 6;                // Defines interval for set the desired lumination value
                                // Refreshing time = counter * wait_time
float input_value = 0;          // Variable to store the value obtained when read serial port
int luminance_input = 0;        // Variable to store the value of luminance desired
int led_output = 0;             // Variable that defines the duty cycle of PWM
int output_value_remap = 0;     // Variable to store the rescale value of luminance
int adjust_increment_value = 1; // Step of adjust the PWM duty cycle

// This function convert the value read with ADC into luminance value
int conversion(int raw_val)
{
  // Conversion rule
  float Vout = float(raw_val) * (VIN / float(4096)); // Conversion analog to voltage
  float RLDR = (R * (VIN - Vout)) / Vout;            // Conversion voltage to resistance
  int lux = 500 / (RLDR / 1000);                     // Conversion resitance to lumen
  return lux;
}

// This function is used to read and return the serial port
float readSerialInput()
{
  Serial.println("Introduce the luminance desired: ");
  while (Serial.available() == 0)
  {
    // Wait 2000ms, if no input the program continue
    delay(2000);
    break;
  }

  float numeric_input = Serial.parseFloat();

  // Print on serial port what you received
  Serial.print("I received: ");
  Serial.println(numeric_input);
  return numeric_input;
}

void setupLDR()
{
  // Start serial port
  Serial.begin(9600);

  // Read the voltage in D15 and convert to luminance
  read_value = analogRead(LDRPIN);
  luminance_read_lux = conversion(read_value);
}

void loopLDR()
{

  if (counter == 6)
  {

    Serial.println("\n***********************");

    // Read serial port
    input_value = readSerialInput();
    if (input_value != 0)
    {
      // If reading is not 0, the value will be resccaled for PWM duty cycle output
      luminance_input = input_value;
      output_value_remap = map(luminance_input, 0, MAXLUM, 0, 4096);
      led_output = output_value_remap;

      // Set the PWM duty cycle
      analogWrite(LEDPIN, led_output, 4095);
    }
    // Print the luminance input value
    Serial.print("Luminance desired: ");
    Serial.println(luminance_input);
    counter = 0;
  }
  else
  {
    Serial.println("\n----------------------------------------------------------------");
    Serial.print("\t Luminance desired: ");
    Serial.println(luminance_input);

    counter++;

    // Read the voltage in D15 and convert to luminance
    read_value = analogRead(LDRPIN);
    luminance_read_lux = conversion(read_value);

    if (luminance_read_lux < luminance_input)
    {
      // Increase PWM duty cycle, if obtained is lower than desired
      led_output = led_output + adjust_increment_value;
    }
    if (luminance_read_lux > luminance_input)
    {
      // Decrease PWM duty cycle, if desired is lower than obtained
      led_output = led_output - adjust_increment_value;
    }

    // Makes sure that the value that defines the duty cycle belongs to [0, 4095]
    if (led_output < 0)
    {
      led_output = 0;
    }
    if (led_output > 4095)
    {
      led_output = 4095;
    }

    // Set the PWM duty cycle
    analogWrite(LEDPIN, (uint32_t)led_output, 4095);

    // Print the luminance read value
    Serial.print("\t Luminance read: ");
    Serial.println(luminance_read_lux);

    // Print the PWM duty cycle [0, 4095]
    Serial.print("\t PWM output: ");
    Serial.println(led_output);

    delay(wait_time);
  }
}