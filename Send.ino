#include <can.h>
#include <mcp2515.h>
#include <SPI.h>

//set up the data structure for sending the data
struct can_frame canMsg1;
struct can_frame canMsg2;
struct can_frame canMsg3;

//create an instance of MCP2515 class
MCP2515 mcp2515(10);

void setup() {
  Serial.begin(9600);

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS); //sets CAN at speed 500KBPS
  mcp2515.setNormalMode();

  canMsg1.can_id  = 0xAA;
  for(int i = 0; i < 8; i++)
    canMsg1.data[i] = i + 1;
  canMsg1.can_dlc = 8;

  canMsg2.can_id  = 0xBB;
  canMsg2.can_dlc = 8;               

  canMsg3.can_id  = 0xCC;
  canMsg3.can_dlc = 8;

}

void loop() {
  //send the message
  mcp2515.sendMessage(&canMsg1);
  mcp2515.sendMessage(&canMsg2);
  mcp2515.sendMessage(&canMsg3);
    
  delay(200);

}
