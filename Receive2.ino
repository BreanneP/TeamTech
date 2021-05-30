#include "mcp_can.h"
#include <SPI.h>
#include <stdio.h>

#define INT8U unsigned char
const int SPI_CS_PIN = 10;
MCP_CAN CAN(SPI_CS_PIN);

INT8U len = 0;
INT8U buf[8];
unsigned char canId;
char str[20];

int firstValue;
int secondValue;

void setup() {
  
  Serial.begin(115200);

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {  // canbus baudrate 500kbps
        Serial.println("CAN BUS Shield init fail!!!");
        Serial.println("Init CAN BUS Shield again...");
        delay(100);
    }

    Serial.println("CAN BUS Initialisation Succesful");
}

void loop() {
      while (CAN_MSGAVAIL == CAN.checkReceive()) {
        CAN.readMsgBuf(&len, buf);
        canId = CAN.getCanId();
        firstValue = buf[0];
        secondValue = buf[1];
      }
      
       Serial.print(" First Value : ");
       Serial.print(firstValue);
       Serial.print(" Second Value : ");
       Serial.println(secondValue);
    }

}
