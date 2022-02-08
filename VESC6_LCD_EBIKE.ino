/*
This code is used to gather data from a VESC6 and display on an LCD screen.
Written for Arduino Nano and LCD I2C 1602.

Original VESCUART.h code written by SolidGeek.
 */
/*
This code has been modified to display voltage to a BT Serial Monitor
Written for Pixel 3, FlipSky VESC and ESP8266
*/

#include <VescUart.h>
#include <SimpleKalmanFilter.h>

//Library for the Display
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16, 2); 

/** Initiate VescUart class */
VescUart UART;

int rpm;
float voltage;
float current;
int power;
float amphour;
float tach;
float distance;
float velocity;
float watthour;
float batpercentage;

SimpleKalmanFilter Filter1(2, 2, 0.01);



void setup() {

  /** Setup Serial port to display data */
  Serial.begin(115200);

  /** Setup UART port (Serial1 on Atmega32u4) */
// Serial1.begin(19200);
  
  while (!Serial) {;}

  /** Define which ports to use as UART */
 UART.setSerialPort(&Serial);


}
void loop() {
  
////////// Read values //////////  
 if ( UART.getVescValues() ) {

  rpm = (UART.data.rpm)/7;          // The '7' is the number of pole pairs in the motor. Most motors have 14 poles, therefore 7 pole pairs
  voltage = (UART.data.inpVoltage);
  current = (UART.data.avgInputCurrent);
  power = voltage*current;
  amphour = (UART.data.ampHours);
  watthour = amphour*voltage;
  tach = (UART.data.tachometerAbs)/42;          // The '42' is the number of motor poles multiplied by 3
  distance = tach*3.142*(1/1609)*0.72*(16/185);          // Motor RPM x Pi x (1 / meters in a mile or km) x Wheel diameter x (motor pulley / wheelpulley)
  velocity = rpm*3.142*(60/1609)*0.72*(16/185);          // Motor RPM x Pi x (seconds in a minute / meters in a mile) x Wheel diameter x (motor pulley / wheelpulley)
  batpercentage = ((voltage-38.4)/12)*100;          // ((Battery voltage - minimum voltage) / number of cells) x 100

////////// Filter //////////
  // calculate the estimated value with Kalman Filter
  float powerfiltered = Filter1.updateEstimate(power);
  

////////// BT Display //////////
  Serial.println(voltage + "V"); //Just add other print statements for other data as needed
  Serial.println(batpercentage + "%");
  
  delay(50);
  
}
