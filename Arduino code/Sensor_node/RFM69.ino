#define Radio_Frequency 915.0

void initRFM69() {
  pinMode(13, OUTPUT);    //Board defined LED pin on RFM radio
  pinMode(4, OUTPUT);     //Board defined RST pin on RFM radio
  digitalWrite(4, LOW);

  //Startup sequence
  digitalWrite(4, HIGH);
  delay(10);
  digitalWrite(4, LOW);
  delay(10);

  if (!Radio.init()) {
    Serial.println("RFM69 radio init failed");
    while (1);
  }

  if (!radio.setFrequency(Radio_Frequency)) {
    Serial.println("setFrequency failed");
  }

  radio.setTxPower(20, true);  // range from 14-20 for power, 2nd arg must be true for 69HCW

  Serial.print("RFM69 radio @ ");
  Serial.print((int)Radio_Frequency);
  Serial.println(" MHz");
}

void Blink(byte delay_ms, byte loops) {
  for (byte i = 0; i < loops; i++)  {
    digitalWrite(13, HIGH);   //Board defined LED pin on RFM radio
    delay(delay_ms);
    digitalWrite(13, LOW);
    delay(delay_ms);
  }
}
