#include <Wire.h>
#include <SSD1306.h>
#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>


#define OLED_SDA    4
#define OLED_SCL    15
#define OLED_RST    16

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

SSD1306 display(0x3c, OLED_SDA, OLED_SCL);


void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println();
  Serial.println("LoRa Receiver");

  // Configure OLED by setting the OLED Reset HIGH
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, HIGH);
  display.init();
  display.flipScreenVertically();
  display.clear();

  // Configure word and position
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(display.getWidth() / 2, display.getHeight() / 2, "LoRa Receiver");
  display.display();
  delay(2000);

  // Configure the LoRA radio
  LoRa.begin(920E6);
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DI0);
  if (!LoRa.begin(920 * 1E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("init ok");

  // Set the radio into receive mode
  LoRa.receive();
  delay(1500);
}

void displayLoraData(String LoRaData) {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0 , 0 , LoRaData);
  display.display();
}

void loop() {
  int packetSize = LoRa.parsePacket();
    if (packetSize) {
    String LoRaData = LoRa.readString();
    Serial.println(LoRaData);
    displayLoraData(LoRaData);
    }
   
  delay(1000);
}
