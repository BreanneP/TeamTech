#include <SPI.h>
#include "mcp2518fd_can.h"

const int SPI_CS_PIN_SEND = 9;
const int SPI_CS_PIN_RECEIVE = 10;

mcp2518fd CAN_SEND(SPI_CS_PIN_SEND);
mcp2518fd CAN_RECEIVE(SPI_CS_PIN_RECEIVE);

unsigned char len = 0;
unsigned char buf[8];
const int tempSensor = A0;

void setup() {
    SERIAL_PORT_MONITOR.begin(9600);
    while(!Serial); // wait for Serial

    if (CAN_SEND.begin(CAN_500K_1M) != 0 || CAN_RECEIVE.begin(CAN_500K_1M) != 0) {
      SERIAL_PORT_MONITOR.println("CAN-BUS initialized error!");
      while(1);
    }
    
    SERIAL_PORT_MONITOR.println("CAN init ok!");
}

void loop() {
    //sends data
    int sensorValue = analogRead(tempSensor);
    int cantxValue = sensorValue / 4;
    unsigned char value = (unsigned char) cantxValue;
    unsigned char stmp[8] = {value, 0, 0, 0, 0, 0, 0, 0};
    CAN_SEND.sendMsgBuf(0x00, 0, 8, stmp);
    

    //receives data
    if (CAN_MSGAVAIL == CAN_RECEIVE.checkReceive()) {
    // read data,  len: data length, buf: data buf
      SERIAL_PORT_MONITOR.println("checkReceive");
      CAN_RECEIVE.readMsgBuf(&len, buf);
      
      float voltage = (buf[0] / 256.0) * 5.0;
      float tempC = (voltage - 0.5) * 100;
      float tempF = ((tempC * 9) / 5.0) + 32.0;
      SERIAL_PORT_MONITOR.print("temperature is ");
      SERIAL_PORT_MONITOR.print(tempF);
      SERIAL_PORT_MONITOR.print("\n");
      
      SERIAL_PORT_MONITOR.println(); 
    }
    
    delay(100);
}
