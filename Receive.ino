#include <can.h>
#include <mcp2515.h>

//set up the data structure for reading the data
struct can_frame canMsg;

//create an instance of MCP2515 class
MCP2515 mcp2515(10);

void setup() {
  Serial.begin(9600);

  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_8MHZ); //sets CAN at speed 500KBPS
  mcp2515.setNormalMode();

  canMsg.can_id = 0x036; //CAN id as 0x036
  canMsg.can_dlc = 2; //CAN data length as 2 bytes
}

void loop() {
  mcp2515.readMessage(&canMsg);

  //read the temperature value
  float voltage = (canMsg.data[0] / 256.0) * 5.0;
  float tempC = (voltage - 0.5) * 100;
  float tempF = ((tempC * 9) / 5.0) + 32.0;
  Serial.print("temperature is ");
  Serial.print(tempF);
  Serial.println("\n");

  //read the level value
  if(canMsg.data[1] == 1)
    Serial.print("The level is okay!");
  else
    Serial.print("The level is not okay!");
  
  delay(200);
}
