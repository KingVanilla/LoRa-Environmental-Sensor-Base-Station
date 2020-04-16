#include <SPI.h>    //Included with Arduino IDE
#include <RH_RF69.h>    //LoRa library used with permission of instructor
#include <RHReliableDatagram.h>   //LoRa library used with permission of instructor

#define RF69_FREQ 915.0

#define MY_ADDRESS     1

#define RFM69_CS      8
#define RFM69_INT     3
#define RFM69_RST     4
#define LED           13

RH_RF69 radio(RFM69_CS, RFM69_INT);
RHReliableDatagram Radio(radio, MY_ADDRESS);

uint8_t buf[RH_RF69_MAX_MESSAGE_LEN];

void setup() {
  Serial.begin(115200);   //Arduino IDE Serial monitor
  Serial1.begin(9600);    //2nd hardware serial port connected to NodeMCU

  pinMode(LED, OUTPUT);
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  //Startup sequence
  digitalWrite(RFM69_RST, HIGH);
  delay(10);
  digitalWrite(RFM69_RST, LOW);
  delay(10);

  if (!Radio.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }

  if (!radio.setFrequency(RF69_FREQ)) {
    Serial.println("setFrequency failed");
  }

  radio.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW


  Serial.print("RFM69 radio initalized @ ");  Serial.print((int)RF69_FREQ);  Serial.println(" MHz");
}

void loop() {
  if (Radio.available()) {
    // Wait for a message addressed to us from the client
    uint8_t len = sizeof(buf);
    uint8_t from;
    if (Radio.recvfromAck(buf, &len, &from)) {
      buf[len] = 0; // zero out remaining string

      Serial.print("Got packet from #"); Serial.print(from);
      Serial.print(" [RSSI :");
      Serial.print(radio.lastRssi());
      Serial.print("] : ");
      
      char dataStruc[24];
      sprintf(dataStruc, "%s,%d", buf, radio.lastRssi());   //We want to add our RSSI signal strength to the data packet
      Serial.println(dataStruc);
      Blink(LED, 40, 4);    //Visual indicator we got a data packet

      Serial1.write(dataStruc);   //Send the data packet to the NodeMCU
    }
  }
}

void Blink(byte PIN, byte DELAY_MS, byte loops) {
  for (byte i = 0; i < loops; i++)  {
    digitalWrite(PIN, HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN, LOW);
    delay(DELAY_MS);
  }
}
