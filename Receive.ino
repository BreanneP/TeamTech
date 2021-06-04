#include <SPI.h>
#include <LiquidCrystal.h>
#define CAN_2515

LiquidCrystal lcd(11, 12, 6, 5, 4, 3); // Arduino digital pins in interface of lcd

//initialize the pins
const int tempPin = A1;
const int levelTop = 8;
const int levelBottom = 13;
const int yellowLED = A0; //led for the bottom sensor
const int redLED = 10; //led for the top sensor
const int blueLED = 7; //led for the temperature sensor

//limits for the temperature in fahrenheit
const int highTemp = 80;
const int lowTemp = -25;

//set SPI CS Pin according to your hardware
#if defined(SEEED_WIO_TERMINAL) && defined(CAN_2518FD)
  const int SPI_CS_PIN  = BCM8;
  const int CAN_INT_PIN = BCM25;
#else
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

//set up code here runs once
void setup() {
  SERIAL_PORT_MONITOR.begin(115200);
  lcd.begin(16, 2);
  
  while(!SERIAL_PORT_MONITOR) {}
  
  #if MAX_DATA_SIZE > 8
    CAN.setMode(CAN_NORMAL_MODE);
  #endif

  while (CAN_OK != CAN.begin(CAN_250KBPS)) {  // init can bus : baudrate = 500k
     SERIAL_PORT_MONITOR.println(F("CAN init fail, retry..."));
     delay(100);
  }
  
  SERIAL_PORT_MONITOR.println(F("CAN init ok!"));
    
  pinMode(levelTop, INPUT);
  pinMode(redLED, OUTPUT);
  digitalWrite(redLED, LOW);
  pinMode(levelBottom, INPUT);
}

//set up CAN message to be read
uint32_t id;
uint8_t  type; // bit0: ext, bit1: rtr
uint8_t  len;
byte cdata[MAX_DATA_SIZE] = {0};
  
//main code that runs repeatedly
void loop() {
  //check if data coming
  if (CAN_MSGAVAIL != CAN.checkReceive())
      return;

  char prbuf[32 + MAX_DATA_SIZE * 3];
  unsigned long t = millis();
  
  // read data, len: data length, buf: data buf
  CAN.readMsgBuf(&len, cdata);
  SERIAL_PORT_MONITOR.print();

  //get ID and type of CAN message
  id = CAN.getCanId();
  type = (CAN.isExtendedFrame() << 0) |
         (CAN.isRemoteRequest() << 1);

  int n = sprintf(prbuf, "%04lu.%03d ", t / 1000, int(t % 1000));
  static const byte type2[] = {0x00, 0x02, 0x30, 0x32};
  n += sprintf(prbuf + n, "RX: [%08lX](%02X) ", (unsigned long)id, type2[type]);

  for (int i = 0; i < len; i++)
      n += sprintf(prbuf + n, "%02X ", cdata[i]);
    
  SERIAL_PORT_MONITOR.println(prbuf);
  delay(100);
  
//   int vValue = ((int16_t)cdata[1] << 8) | cdata[0];
//   float viscosity = vValue * 0.015625;
//   SERIAL_PORT_MONITOR.println("Visocity is ");
//   SERIAL_PORT_MONITOR.println(viscosity);
  
//   int dValue = ((int16_t)cdata[3] << 8) | cdata[2];
//   float density = dValue * 0.00003052;
//   SERIAL_PORT_MONITOR.println("Density is ");
//   SERIAL_PORT_MONITOR.println(density);

//   int dcValue = ((int16_t)cdata[7] << 8) | cdata[6];
//   float dielectric = dcValue * 0.00012207;
//   SERIAL_PORT_MONITOR.println("Dielectric constant is ");
//   SERIAL_PORT_MONITOR.println(dielectric);
  
  
  //read the temperature value
  float tempValue = analogRead(tempPin);
  
  //convert analog temperature value to Fahrenheit
  float voltage = (tempValue / 1024.0 ) * 5.0;
  float tempC = (voltage - 0.5) * 100; 
  float tempF = ((tempC * 9.0) / 5.0 ) + 32.0;
    
  SERIAL_PORT_MONITOR.print("Temperature is ");
  SERIAL_PORT_MONITOR.print(tempF);
  
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
    analogWrite(yellowLED, 255);
  else
    analogWrite(yellowLED, 255);
  
  
  SERIAL_PORT_MONITOR.print("Level top is ");
  SERIAL_PORT_MONITOR.print(isDryTop);
  
  SERIAL_PORT_MONITOR.print("Level bottom is ");
  SERIAL_PORT_MONITOR.print(isDryBottom);
  
  //print out level sensor reading
  if(!isDryBottom && isDryTop)
    lcd.print("Level okay");
  else
    lcd.print("Level not okay");
  
  delay(1000);
}
