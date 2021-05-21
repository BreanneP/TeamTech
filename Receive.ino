#include <can.h>
#include <mcp2515.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8); // Arduino digital pins in interface of lcd

//set up the data structure for reading the data
struct can_frame canMsg1;
struct can_frame canMsg2;
struct can_frame canMsg3;

//create an instance of MCP2515 class
MCP2515 mcp2515(10);

//initialize the pins
const int tempPin = A1;
const int levelSensor1 = 8;
const int levelSensor2 = 10;
const int yellowLED = 13;
const int redLED = 9;

//limits for the temperature
const int highTemp = 50;
const int lowTemp = -25;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  
  pinMode(levelSensor, INPUT); //set the levelSensor as an input pin
  pinMode(yellowLED, OUTPUT); //set the yellowLED as an output pin
  pinMode(redLED, OUTPUT); //set the redLED as an output pin

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ); //sets CAN at speed 500KBPS
  mcp2515.setNormalMode();

}

void loop() {
  //read the temperature value
  float tempValue = analogRead(tempPin);
  
  //convert analog temperature value to Fahrenheit
  float voltage = (tempValue / 1024.0 ) * 5.0;
  float tempC = (voltage - 0.5) * 100; 
  float tempF = ((tempC * 9.0) / 5.0 ) + 32.0 ;  
  
  //read the level sensors
  int lowLevel = digitalRead(levelSensor1);
  int highLevel = digitalRead(levelSensor2);

  //temperature, level, and/or viscosity out of range
  if(tempF > highTemp || tempF < lowTemp || lowLevel == 0 || highLevel == 1)
    digitalWrite(redLED, HIGH);

  //print out temperature reading
  lcd.setCursor(0, 0);
  lcd.print("Temperature: "); // printing the temperature from TMP36 in farenheit
  lcd.print(tempF);

  //print out level sensor reading
  if(lowLevel == 1 && highLevel == 0)
    lcd.print("Level okay");
  else
    lcd.print("Level not okay");

 
  //read the CAN messages 
  if (mcp2515.readMessage(&canMsg1) == MCP2515::ERROR_OK) {
    if(canMsg1.can_id==0xAA) { //read the property sensor CAN message
     int vValue = canMsg1.data[0..1];
     int dValue = canMsg1.data[2..3];
     int dcValue = canMsg2.data[6..7];

     float viscosity = vValue * 0.015625;
     float density = dValue * 0.00003052;
     float dielectric = dcValue * 0.00012207;
     
     lcd.setCursor(16, 0);
     lcd.print("Viscosity: ");
     lcd.print(viscosity);
     delay(200);
    }
  }

  if (mcp2515.readMessage(&canMsg2) == MCP2515::ERROR_OK) {
    if(canMsg2.can_id==0xBB) { //read the temperature sensor CAN message
     int tValue = canMsg1.data[4..5];
     int temp = tValue[2..3];

     float celsius = (0.3125 * temp) - 273;
     float fahrenheit = ((celsius * 9.0) / 5.0 ) + 32.0 ;  
     
     lcd.print("Temperature: ");
     lcd.print(fahrenheit);
     delay(200);
    }
  }
  
  delay(200);
}
