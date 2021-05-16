#include <can.h>
#include <mcp2515.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(13, 12, 11, 10, 9, 8); // 

//set up the data structure for reading the data
struct can_frame canMsg;

//create an instance of MCP2515 class
MCP2515 mcp2515(10);

//initialize the pins
const int tempPin = A1;
const int levelSensor = 10;
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

  canMsg.can_id = 0x036; //CAN id as 0x036
  canMsg.can_dlc = 1; //CAN data length as 1 byte
}

void loop() {
  //read the temperature value
  float tempValue = analogRead(tempPin);
  
  //Analog Reading to Voltage 
  float voltage = (tempValue / 1024. ) * 5.;

  //Voltage to Celsius
  float tempC = (voltage - 0.5) * 100; 
  
  //Celsius to Farhenheit
  float tempF = ((tempC * 9.0) / 5.0 ) + 32.0 ;  
  
  //read the level sensor
  int levelValue = digitalRead(levelSensor);
  
  lcd.print("Temp = "); // printing the temperature from TMP36 in farenheit
  lcd.print(tempF);
  
  mcp2515.readMessage(&canMsg);
  delay(200);
}
