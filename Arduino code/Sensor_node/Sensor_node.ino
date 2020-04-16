#include <Wire.h>   //Included with Arduino IDE
#include <SPI.h>    //Included with Arduino IDE
#include <RH_RF69.h>    //LoRa library used with permission of instructor
#include <RHReliableDatagram.h>   //LoRa library used with permission of instructor

const int lightSensorPin = A0;
const int PIRSensorPin = 10;
const int buttonPin = 9;
const int batterySense = A3;

float temp;
int humidity;
int lux;
int butt;
int motion;
int battery;

//Radio address (must be unique)
const int Destination_Address = 1;
const int My_Address = 2;

RH_RF69 radio(8, 3);  //Board defined CS and INT pins on RFM radio
RHReliableDatagram Radio(radio, My_Address);

char dataPacket[20] = "";

void setup() {
  Serial.begin(115200);

  pinMode(PIRSensorPin, INPUT);
  pinMode(lightSensorPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(batterySense, INPUT);

  initBME();
  initRFM69();

  readSensors();
  transmitData();
}

void loop() {
  readSensors();
  transmitData();

  delay(5000);
}

void readSensors() {
  temp = constrain(readTemp(), 0, 99);    //Just in case, we want our temperature to be between 0-99, hopefully it'll never be out of that range! 
  humidity = constrain(readHumidity(), 0, 99);   //We want our humidity value to be 2 characters maximum
  lux = map(analogRead(lightSensorPin), 0, 1024, 0, 99);    //We want to scale down our analog reading from 0-1024 to 0-99
  battery = constrain(map(analogRead(batterySense), 485, 636, 0, 99), 0, 99);   //We want to scale down our battery reading from 485-636 (analog reading for 3.6-4.2V) to 0-99
  motion = digitalRead(PIRSensorPin);
  butt = !digitalRead(buttonPin);

  Serial.print(temp);
  Serial.print("°F, ");

  Serial.print(humidity);
  Serial.print("%, ");

  Serial.print(lux);
  Serial.print(" lux, Motion ");

  Serial.print(motion);
  Serial.print(", Button ");

  Serial.print(butt);
  Serial.print(", Battery ");

  Serial.println(battery);
}

void transmitData() {
  /*
      Data packet format:
      ID,Temp,Humidity,Light,Motion,Button,Battery
      xx,xxxx,xx      ,xx   ,x     ,x     ,xx
  */

  String struc = "";

  //Address
  if (My_Address <= 9) {
    struc = "0";    //This is important because we always want our address to be 2 characters, so 9 would convert to 09
  }
  struc += My_Address;
  struc += ",";

  //Temperature
  struc += String(temp, 1);
  struc += ",";

  //Humidity
  struc += humidity;
  struc += ",";

  //Light
  if (lux <= 9) {
    struc += "0";   //This is important because we always want our lux value to be 2 characters, so 9 would convert to 09
  }
  struc += lux;
  struc += ",";

  //Motion
  struc += motion;
  struc += ",";

  //Button
  struc += butt;
  struc += ",";

  //Battery
  if (battery <= 9) {
    struc += "0";   //This is important because we always want our battery value to be 2 characters, so 9 would convert to 09
  }
  struc += battery;

  struc.toCharArray(dataPacket, sizeof(dataPacket) + 1);
  Serial.print("Sending ");
  Serial.println(dataPacket);

  Radio.sendtoWait((uint8_t *)dataPacket, strlen(dataPacket), Destination_Address);   //Send the data packet to the LoRa radio

  Blink(40, 4);   //Visual indicator
}

//This wake routine was meant for the deep sleep function, however, it didn't work out as we cannot connect and disconnect interrupts like usual in sleep

/*
void wakeRoutine() {
  readSensors();

  if (motion == 1) {    //If we read motion
    if (interruptIgnore == false) {   //And its not been in the past 10 minutes
      interruptIgnore = true;

      
      detachInterrupt(PIRSensorPin);
    }
  }
  if (interruptIgnore != lastInterruptIgnore) {
    LowPower.attachInterruptWakeup(PIRSensorPin, wakeRoutine, RISING);
  }

  transmitData();

  lastInterruptIgnore = interruptIgnore;
}
*/
