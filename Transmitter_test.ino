#include <Wire.h>
#include <SSD1306.h>
#include <SPI.h>
#include <LoRa.h>
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>


#define OLED_SDA    4
#define OLED_SCL    15
/*#define OLED_RST    16*/

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

#define S_RX    3   // define software serial RX pin
#define S_TX    1   // define software serial TX pin

float Latitude , Longitude;
int year , month , date, hour , minute , second;
String DateString , TimeString , LatitudeString , LongitudeString;

SSD1306 display(0x3c, OLED_SDA, OLED_SCL);
SoftwareSerial gpsSerial(16,17);
TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600);
  while (!Serial); 
    // Configure the LoRA radio
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.begin(920E6);
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(920E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("init ok");

/*  // Configure OLED by setting the OLED Reset HIGH
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, HIGH);
  display.init();
  display.flipScreenVertically();
  display.clear();
  
  // Configure word and position
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(display.getWidth() / 2, display.getHeight() / 2, "LoRa Transmitter");
  display.display();
  delay(2000);
*/

}


  
void getreading()
{
 if (gps.location.isValid())
      {
        Latitude = gps.location.lat();
        LatitudeString = String(Latitude , 6);
        Longitude = gps.location.lng();
        LongitudeString = String(Longitude , 6);
      }

      if (gps.date.isValid())
      {
        DateString = "";
        date = gps.date.day();
        month = gps.date.month();
        year = gps.date.year();

        if (date < 10)
        DateString = '0';
        DateString += date;
        DateString += " / ";

        if (month < 10)
        DateString += '0';
        DateString += month;
        DateString += " / ";

        if (year < 10)
        DateString += '0';
        DateString += year;
      }

      if (gps.time.isValid())
      {
        TimeString = "";
        hour = gps.time.hour()+ 7; //adjust UTC
        minute = gps.time.minute();
        second = gps.time.second();
    
        if (hour < 10)
        TimeString = '0';
        TimeString += hour;
        TimeString += " : ";

        if (minute < 10)
        TimeString += '0';
        TimeString += minute;
        TimeString += " : ";

        if (second < 10)
        TimeString += '0';
        TimeString += second;
      }
}

void sendReadings() {
  String LoRalat = "";
  String LoRalon = "";
  String LoRadate = "";
  String LoRatime = "";
  LoRalat = "Latitude: " + LatitudeString;
  LoRalon = "Longitude: " + LongitudeString;
  LoRadate = "Date:" + DateString;
  LoRatime = "Time:" + TimeString;
  //Send LoRa packet to receiver
  LoRa.beginPacket();
  LoRa.println(LoRalat);
  LoRa.println(LoRalon);
  LoRa.println(LoRadate);
  LoRa.println(LoRatime);
  LoRa.endPacket();
}

void displayInfo()
{
  if (gps.location.isValid())
  {
    Serial.println("Latitude: " + LatitudeString);
    Serial.println("Longitude: " + LongitudeString);
  }
  else
  {
    Serial.println("Location: Not Available");
  }
  
  Serial.print("Date: ");
  if (gps.date.isValid())
  {
    Serial.println(DateString);
  }
  else
  {
    Serial.println("Date: Not Available");
  }

  Serial.print("Time: ");
  if (gps.time.isValid())
  {    
    Serial.println(TimeString);
  }
  else
  {
    Serial.println("Time: Not Available");
  }
  delay(1000);
}

void loop() 
{
  while (gpsSerial.available() > 0)
  if (gps.encode(gpsSerial.read()))
  {
    getreading();
    sendReadings();
    displayInfo();
  }
  
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected"));
    delay(2000); 
  }
  LoRa.endPacket();
}
