#include <can.h>
#include <mcp2515.h>

//initialize the pins
const int tempPin = A0;
const int levelSensor = 0;

//set up the data structure for sending the data
struct can_frame canMsg;

//create an instance of MCP2515 class
MCP2515 mcp2515(10);

void setup() {
  Serial.begin(9600);
  pinMode(levelSensor, INPUT); //set the levelSensor as an input pin

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ); //sets CAN at speed 500KBPS
  mcp2515.setNormalMode();

  canMsg.can_id = 0x036; //CAN id as 0x036
  canMsg.can_dlc = 2; //CAN data length as 2 bytes

}

void loop() {
  //get the temperature value
  int tempValue = analogRead(tempPin);
  tempValue = map(tempValue, 0, 1023, 0, 255); //maps the reading to 0 to 255
  canMsg.data[0] = tempValue; //adds the tempValue to the CAN message

  //get the level value
  int levelValue = digitalRead(levelSensor);
  levelValue = map(levelValue, 0, 1023, 0, 255); //maps the reading to 0 to 255
  canMsg.data[1] = levelValue; //adds the levelValue to the CAN message

  //send the message
  mcp2515.sendMessage(&canMsg);
    
  delay(200);

}
