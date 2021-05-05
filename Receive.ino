#include "mcp_can.h"
#include <SPI.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

void setup() {
  lcd.begin(20, 4);
  CAN.begin(CAN_100KBPS);                   // init can bus : baudrate = 100k
  attachInterrupt(0, MCP2515_ISR, FALLING); // start interrupt
  Serial.begin(115200);
}


void loop() {
    CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
    lcd.setCursor(0, 0);
    
    
     //This loops through each byte of data and prints it
     for(int i = 0; i<len; i++) {

        if(i == 0) { //this is the temperature sensor
          float voltage = ((buf[0] * 4) / 1024 ) * 5.0;
          float tempC = (voltage - 0.5) * 100;
          float tempF = ((tempC * 9) / 5.0) + 32.0;
          lcd.print(tempF);
        }
        
        if(i == 1) { //this is the level sensor
          if(buf[1] == 1) 
            Serial.print("Level is okay");
          else
            Serial.print("Level is not okay");
        }
     }
     
      Serial.println();
      delay(50);
    }
}
