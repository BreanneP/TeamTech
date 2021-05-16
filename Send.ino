#include <can.h>
#include <mcp2515.h>

//set up the data structure for sending the data
struct can_frame canMsg;

//create an instance of MCP2515 class
MCP2515 mcp2515(10);

void setup() {
  Serial.begin(9600);

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ); //sets CAN at speed 500KBPS
  mcp2515.setNormalMode();

  canMsg.can_id = 0x036; //CAN id as 0x036
  canMsg.can_dlc = 1; //CAN data length as 1 byte

}

void loop() {
  //send the message
  mcp2515.sendMessage(&canMsg);
    
  delay(200);

}
