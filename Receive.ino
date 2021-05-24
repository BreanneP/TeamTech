#include <can.h>
#include <mcp2515.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(11, 12, 5, 4, 3, 2); // Arduino digital pins in interface of lcd

//set up the data structure for reading the data
struct can_frame canMsg1;
struct can_frame canMsg2;
struct can_frame canMsg3;

//create an instance of MCP2515 class
MCP2515 mcp2515(10);

//initialize the pins
const int tempPin = A1;
const int levelTop = 8;
const int levelBottom = 10;
const int yellowLED = 13; //led for the bottom level sensor
const int redLED = 9; //led for the top level sensor
const int blueLED = 7; //led for the temperature sensor

//limits for the temperature in fahrenheit
const int highTemp = 80;
const int lowTemp = -25;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
    
  pinMode(levelTop, INPUT);
  pinMode(redLED, OUTPUT);
  digitalWrite(redLED, LOW);
  pinMode(levelBottom, INPUT);
  pinMode(yellowLED, OUTPUT);
  digitalWrite(yellowLED, LOW);

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
  float tempF = ((tempC * 9.0) / 5.0 ) + 32.0;
    
  //reading if temp above/below threshold
  if (tempF < lowTemp) { 
    digitalWrite(blueLED, HIGH);
    lcd.print(" It's Cold.");
  }
  else if (tempF > highTemp) { 
    digitalWrite(blueLED, HIGH);
    lcd.print(" It's Hot.");
  }
  else {
    lcd.print("Temp: "); // printing the temperature from TMP36 in farenheit
    lcd.print(tempF);
    lcd.print(" F ");
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
    digitalWrite(yellowLED, HIGH);
  else
    digitalWrite(yellowLED, LOW);
      
  //print out level sensor reading
  if(!isDryBottom && isDryTop)
    lcd.print("Level okay");
  else
    lcd.print("Level not okay");
  
  Serial.print("Can message");
  for(int i = 0; i < 8; i++) {
    Serial.print(canMsg1.data[i]);
  }
  Serial.print("\n");
  
  //read the CAN messages 
  if (mcp2515.readMessage(&canMsg1) == MCP2515::ERROR_OK) {
    Serial.print("message received");
    if(canMsg1.can_id==0xAA) { //read the property sensor CAN message
     int vValue1 = canMsg1.data[0];
     int vValue2 = canMsg1.data[1];
     Serial.print(vValue1);
     Serial.print("\n");
     
//      int dValue = canMsg1.data[2..3];
//      int dcValue = canMsg2.data[6..7];

//      float viscosity = vValue * 0.015625;
//      float density = dValue * 0.00003052;
//      float dielectric = dcValue * 0.00012207;
     
//      lcd.setCursor(16, 0);
//      lcd.print("Viscosity: ");
//      lcd.print(viscosity);
     delay(200);
    }
  }

//   if (mcp2515.readMessage(&canMsg2) == MCP2515::ERROR_OK) {
//     if(canMsg2.can_id==0xBB) { //read the temperature sensor CAN message
//      int tValue = canMsg1.data[4..5];
//      int temp = tValue[2..3];

//      float celsius = (0.3125 * temp) - 273;
//      float fahrenheit = ((celsius * 9.0) / 5.0 ) + 32.0 ;  
     
//      lcd.print("Temperature: ");
//      lcd.print(fahrenheit);
//      delay(200);
//     }
//   }
  
  delay(1000);
}
