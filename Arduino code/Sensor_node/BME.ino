uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;

/*
uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;
*/

uint8_t dig_H1;
int16_t dig_H2;
uint8_t dig_H3;
int16_t dig_H4;
int16_t dig_H5;
int8_t dig_H6;

int32_t uniTemp;

void initBME() {
  uint8_t controlData;
  
  delay(2000);
 
  Wire.begin();

  Serial.print("BME initalizing...");
  while (readRegister(0xD0) != 0x60) { //Read the ID register and see if it matches the expected value from the datasheet.
    Serial.print(".");  //We need to loop here, there's a weird issue with the BME cold starting
    delay(200);
  }
  Serial.println(" ready");
  
  //Now we set the calibration data to each sensor register as specified in the datasheet
  dig_T1 = ((uint16_t)((readRegister(0x89) << 8) + readRegister(0x88)));
  dig_T2 = ((int16_t)((readRegister(0x8B) << 8) + readRegister(0x8A)));
  dig_T3 = ((int16_t)((readRegister(0x8D) << 8) + readRegister(0x8C)));

  //These are the pressure configuration registers, however, we don't use this data
  /*  
  dig_P1 = ((uint16_t)((readRegister(0x8F) << 8) + readRegister(0x8E)));
  dig_P2 = ((int16_t)((readRegister(0x91) << 8) + readRegister(0x90)));
  dig_P3 = ((int16_t)((readRegister(0x93) << 8) + readRegister(0x92)));
  dig_P4 = ((int16_t)((readRegister(0x95) << 8) + readRegister(0x94)));
  dig_P5 = ((int16_t)((readRegister(0x97) << 8) + readRegister(0x96)));
  dig_P6 = ((int16_t)((readRegister(0x99) << 8) + readRegister(0x98)));
  dig_P7 = ((int16_t)((readRegister(0x9B) << 8) + readRegister(0x9A)));
  dig_P8 = ((int16_t)((readRegister(0x9D) << 8) + readRegister(0x9C)));
  dig_P9 = ((int16_t)((readRegister(0x9F) << 8) + readRegister(0x9E)));
  */

  dig_H1 = ((uint8_t)(readRegister(0xA1)));
  dig_H2 = ((int16_t)((readRegister(0xE2) << 8) + readRegister(0xE1)));
  dig_H3 = ((uint8_t)(readRegister(0xE3)));
  dig_H4 = ((int16_t)((readRegister(0xE4) << 4) + (readRegister(0xE5) & 0x0F)));
  dig_H5 = ((int16_t)((readRegister(0xE6) << 4) + ((readRegister(0xE5) >> 4) & 0x0F)));
  dig_H6 = ((int8_t)readRegister(0xE7));

  //Now we must configure the chip operation parameters

  //Standby time
  controlData = readRegister(0xF5); //Configuration register
  controlData &= ~( (1 << 7) | (1 << 6) | (1 << 5) ); //Clear bits 7,6,5
  controlData |= (0 << 5);
  writeRegister(0xF5, controlData);

  //Filter
  controlData = readRegister(0xF5); //Configuration register
  controlData &= ~( (1 << 4) | (1 << 3) | (1 << 2) ); //Clear bits 4,3,2
  controlData |= (0 << 2); //Insert our config bit of 0 with the existing bits
  writeRegister(0xF5, controlData);

  //We must enter sleep mode to write the following config bits
  setMode(0b00);

  /*
  //Pressure oversample
  controlData = readRegister(0xF4); //Control measure register
  controlData &= ~( (1 << 4) | (1 << 3) | (1 << 2) ); //Clear bits 4,3,2
  controlData |= (1 << 2); //Insert our config bit of 1 with the existing bits
  writeRegister(0xF4, controlData);
  */

  //Humidity oversample
  controlData = readRegister(0xF2); //Humidity control measure register
  controlData &= ~( (1 << 2) | (1 << 1) | (1 << 0) ); //Clear bits 2,1,0
  controlData |= (1 << 0); //Insert our config bit of 1 with the existing bits
  writeRegister(0xF2, controlData);

  //Temperature oversample
  controlData = readRegister(0xF4); //Humidity control measure register
  controlData &= ~( (1 << 7) | (1 << 6) | (1 << 5) ); //Clear bits 7,6,5
  controlData |= (1 << 5); //Insert our config bit of 1 with the existing bits
  writeRegister(0xF4, controlData);

  //Return to normal mode.  We are done with configuration
  setMode(0b11);

}

uint8_t readRegister(uint8_t loc_offset) {
  uint8_t result;

  Wire.beginTransmission(0x76);
  Wire.write(loc_offset);
  Wire.endTransmission();

  Wire.requestFrom(0x76, 1);
  while (Wire.available()) {
    result = Wire.read();
  }

  return result;
}

uint8_t writeRegister(uint8_t loc_offset, uint8_t data) {
  Wire.beginTransmission(0x76);
  Wire.write(loc_offset);
  Wire.write(data);
  Wire.endTransmission();
}

void setMode(uint8_t mode) { //00 = Sleep, 11 = Normal
  uint8_t controlData = readRegister(0xF4);
  controlData &= ~( (1 << 1) | (1 << 0) );
  controlData |= mode;
  writeRegister(0xF4, controlData);
}

void readRegisterPage(uint8_t *pointer , uint8_t offset, uint8_t length) {
  uint8_t i = 0;
  char c = 0;

  Wire.beginTransmission(0x76);
  Wire.write(offset);
  Wire.endTransmission();

  Wire.requestFrom(0x76, length);
  while ( (Wire.available()) && (i < length)) {
    c = Wire.read();
    *pointer = c;
    pointer++;
    i++;
  }
}

//These calculations are performed according to the BME datasheet
float readTemp() {
  uint8_t buffer[3];
  readRegisterPage(buffer, 0xFA, 3);
  int32_t adc_T = ((uint32_t)buffer[0] << 12) | ((uint32_t)buffer[1] << 4) | ((buffer[2] >> 4) & 0x0F);


  int64_t var1, var2;
  var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
  uniTemp = var1 + var2;
  float output = (uniTemp * 5 + 128) >> 8;

  output = ((output / 100) * 9) / 5 + 32;

  return output;
}

//These calculations are performed according to the BME datasheet
float readHumidity() {
  uint8_t buffer[2];
  readRegisterPage(buffer, 0xFD, 2);
  int32_t adc_H = ((uint32_t)buffer[0] << 8) | ((uint32_t)buffer[1]);

  int32_t var1;
  var1 = (uniTemp - ((int32_t)76800));
  var1 = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * var1)) + ((int32_t)16384)) >> 15) * (((((((var1 * ((int32_t)dig_H6)) >> 10) * (((var1 * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
  var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)dig_H1)) >> 4));
  var1 = (var1 < 0 ? 0 : var1);
  var1 = (var1 > 419430400 ? 419430400 : var1);

  return (float)(var1 >> 12) / 1024.0;
}
