#include <SoftwareSerial.h>   //Included with Arduino IDE
#include <ESP8266WiFi.h>    //WiFi library used with permission of instructor
#include <PubSubClient.h>   //MQTT library used with permission of instructor

SoftwareSerial Port(12, 14);    //Define our Serial ports on these pins

String data;

const char* ssid = "******";   //WiFi SSID (redacted)
const char* password = "************";   //WiFi password (redacted)

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char* mqtt_server = "192.168.1.x";    //MQTT server IP Address (redacted)

void setup() {
  Port.begin(9600);   //Software Serial port connected to LoRa MCU
  Serial.begin(115200);   //Arduino IDE Serial monitor

  networkInitialization();    //Startup our WiFi connection
}

void loop() {
  //Check our serial port for new data from the LoRa network
  if (Port.available()) {
    data = Port.readString();
    publishData(data);    //Push the data string out over MQTT
  }

  //Reconnect our WiFi if we get disconnected
  if (!wifiClient.connected()) {
    reconnect();
  }

  //Do our MQTT heartbeat to the server
  mqttClient.loop();


}

void publishData(String dataPacket) {
  //Data parsing
  
  //This is our MQTT topic template defined by the unique ID of the LoRa that sent the original communication
  String address = String("/home/LoRa/node" + dataPacket.substring(0,2));


  //Parse out all of our data from individual bits of the string
  String temperature = dataPacket.substring(3,7);
  String humidity = dataPacket.substring(8,10);
  String light = dataPacket.substring(11,13);
  String motion = dataPacket.substring(14,15);
  String button = dataPacket.substring(16,17);
  String battery = dataPacket.substring(18,20);
  String rssi = dataPacket.substring(21,24);

  Serial.println("Publishing to " + address);
  Serial.println("Temperature: " + temperature);
  Serial.println("Humidity: " + humidity);
  Serial.println("Light: " + light);
  Serial.println("Motion: " + motion);
  Serial.println("Button: " + button);
  Serial.println("Battery: " + battery);
  Serial.println("RSSI: " + rssi);

  Serial.println("");

  //Now we publish each piece of data to its corresponding MQTT channel using the channel template from above
  mqttClient.publish((String(address + "/temperature")).c_str(), temperature.c_str());
  mqttClient.publish((String(address + "/humidity")).c_str(), humidity.c_str());
  mqttClient.publish((String(address + "/light")).c_str(), light.c_str());
  mqttClient.publish((String(address + "/motion")).c_str(), motion.c_str());
  mqttClient.publish((String(address + "/button")).c_str(), button.c_str());
  mqttClient.publish((String(address + "/battery")).c_str(), battery.c_str());
  mqttClient.publish((String(address + "/rssi")).c_str(), rssi.c_str());
}
