#include <SPI.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(11, 12, 6, 5, 4, 3); // Arduino digital pins in interface of lcd

//initialize the sensors
const int tempPin = A1;
const int levelTop = 8;
const int levelBottom = 13;

//initialize the LEDs
const int yellowLED = A0; //led for the bottom sensor
const int redLED = 10; //led for the top sensor
const int blueLED = 7; //led for the temperature sensor

//limits for the temperature in fahrenheit
const int highTemp = 80;
const int lowTemp = -25;

//set up code here runs once
void setup() {
  SERIAL_PORT_MONITOR.begin(115200);
  lcd.begin(16, 2);
  
//defining LED and sensor communication
 pinMode(levelTop, INPUT);
 pinMode(redLED, OUTPUT);
 digitalWrite(redLED, LOW);
 pinMode(levelBottom, INPUT);

}

//main code that runs repeatedly
void loop() {
  //read the temperature value
  float tempValue = analogRead(tempPin);
  
  //convert analog temperature value to Fahrenheit
  float voltage = (tempValue / 1024.0 ) * 5.0;
  float tempC = (voltage - 0.5) * 100; 
  float tempF = ((tempC * 9.0) / 5.0 ) + 32.0;
  SERIAL_PORT_MONITOR.print(", temperature: ");
  SERIAL_PORT_MONITOR.print(tempF);
  
  //reading if temp above/below threshold
  if (tempF < lowTemp) { 
    digitalWrite(blueLED, HIGH);
    lcd.print(F(" It's Cold."));
  }
  else if (tempF > highTemp) { 
    digitalWrite(blueLED, HIGH);
    lcd.print(F(" It's Hot."));
  }
  else {
    lcd.print("Temp: "); // printing the temperature from TMP36 in farenheit
    lcd.print(tempF);
    lcd.print(F(" F "));
  }

  //read the top level sensor
  int isDryTop = digitalRead(levelTop);
  if ( isDryTop )
    digitalWrite(redLED, LOW);
  else
    digitalWrite(redLED, HIGH);
    
  //read the bottom level sensor
  int isDryBottom = digitalRead(levelBottom);
  if ( isDryBottom )
    analogWrite(yellowLED, 255);
  else
    analogWrite(yellowLED, 255);
  
  //print out level sensor reading
  if(!isDryBottom && isDryTop) {
    lcd.print(F("Level okay"));
    SERIAL_PORT_MONITOR.print(", level is okay");
  }
  else {
    lcd.print(F("Level not okay"));
    SERIAL_PORT_MONITOR.print(", level is not okay");
  }
  
  delay(1000);
}
