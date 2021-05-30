#include <Arduino.h>
#include <mcp_can.h>
#include <SPI.h>
#include <mcp2515_can.h>

const int SPI_CS_PIN = 10;
mcp2515_can CAN(SPI_CS_PIN);

void setup() {
  Serial.begin(9600);
  
  while (CAN_OK != CAN.begin(CAN_500KBPS)) { // baudrate 500kbps
      Serial.println("CAN BUS Shield init fail");
      Serial.println("Init CAN BUS Shield again");
      delay(100);
  }
      Serial.println("CAN BUS Initialisation Succesful!");
}

void loop() {
  int firstValue = 100;
  int secondValue = 200;

  //Create CanBus data pack 
  unsigned char canMsg[8] = {firstValue, secondValue, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  //Send Data Construction:  id = 0x07B ---  standart Flame ---  data lenght = 8 ----  stmp:data buf
  CAN.sendMsgBuf(0x07B, 0, 8, canMsg);
  delay(100);
}
