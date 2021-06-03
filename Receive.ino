#include <SPI.h>
#include <LiquidCrystal.h>

LiquidCrystal lcd(11, 12, 5, 4, 3, 2); // Arduino digital pins in interface of lcd
define CAN_2515

//initialize the pins
const int tempPin = A1;
const int levelTop = 8;
const int levelBottom = 6;
const int yellowLED = 13; //led for the bottom level sensor
const int redLED = 9; //led for the top level sensor
const int blueLED = 7; //led for the temperature sensor

//limits for the temperature in fahrenheit
const int highTemp = 80;
const int lowTemp = -25;

#if defined(SEEED_WIO_TERMINAL) && defined(CAN_2518FD)
// For Wio Terminal w/ MCP2518FD RPi Hat：
// Channel 0 SPI_CS Pin: BCM 8
// Channel 1 SPI_CS Pin: BCM 7
// Interupt Pin: BCM25
const int SPI_CS_PIN  = BCM8;
const int CAN_INT_PIN = BCM25;

#else
// For Arduino MCP2515 Hat:
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;
const int CAN_INT_PIN = 2;
#endif

#ifdef CAN_2518FD
#include "mcp2518fd_can.h"
mcp2518fd CAN(SPI_CS_PIN); // Set CS pin

#define MAX_DATA_SIZE 8

#endif

#ifdef CAN_2515
#include "mcp2515_can.h"
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
#define MAX_DATA_SIZE 8
#endif

void setup() {
  SERIAL_PORT_MONITOR.begin(115200);
  lcd.begin(16, 2);
  
  while(!SERIAL_PORT_MONITOR) {}
  
  #if MAX_DATA_SIZE > 8
    CAN.setMode(CAN_NORMAL_MODE);
  #endif

  while (CAN_OK != CAN.begin(CAN_500KBPS)) {  // init can bus : baudrate = 500k
     SERIAL_PORT_MONITOR.println(F("CAN init fail, retry..."));
     delay(100);
  }
  
  SERIAL_PORT_MONITOR.println(F("CAN init ok!"));
    
  pinMode(levelTop, INPUT);
  pinMode(redLED, OUTPUT);
  digitalWrite(redLED, LOW);
  pinMode(levelBottom, INPUT);
  pinMode(yellowLED, OUTPUT);
  digitalWrite(yellowLED, LOW);

}

uint32_t id;
uint8_t  type; // bit0: ext, bit1: rtr
uint8_t  len;
byte cdata[MAX_DATA_SIZE] = {0};
  
void loop() {
    //check if data coming
    if (CAN_MSGAVAIL != CAN.checkReceive())
        return;

    char prbuf[32 + MAX_DATA_SIZE * 3];
    int i, n;

    unsigned long t = millis();
    // read data, len: data length, buf: data buf
    CAN.readMsgBuf(&len, cdata);

    id = CAN.getCanId();
    type = (CAN.isExtendedFrame() << 0) |
           (CAN.isRemoteRequest() << 1);
    /*
     * MCP2515(or this driver) could not handle properly
     * the data carried by remote frame
     */

    n = sprintf(prbuf, "%04lu.%03d ", t / 1000, int(t % 1000));
    /* Displayed type:
     *
     * 0x00: standard data frame
     * 0x02: extended data frame
     * 0x30: standard remote frame
     * 0x32: extended remote frame
     */
    static const byte type2[] = {0x00, 0x02, 0x30, 0x32};
    n += sprintf(prbuf + n, "RX: [%08lX](%02X) ", (unsigned long)id, type2[type]);
    // n += sprintf(prbuf, "RX: [%08lX](%02X) ", id, type);

    for (i = 0; i < len; i++)
        n += sprintf(prbuf + n, "%02X ", cdata[i]);
    
    SERIAL_PORT_MONITOR.println(prbuf);
  
  
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
  
  delay(1000);
}
