#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

SoftwareSerial Port(12, 14);

String data;

const char* ssid = "*******";
const char* password = "***********";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

const char* mqtt_server = "192.168.1.x";

void setup() {
  Port.begin(9600);
  Serial.begin(115200);

  networkInitialization();
}

void loop() {
  //Check our serial port for new data from the LoRa network
  if (Port.available()) {
    data = Port.readString();
    publishData(data);
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
  String address = String("/home/LoRa/node" + dataPacket.substring(0,2));
  
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

  mqttClient.publish((String(address + "/temperature")).c_str(), temperature.c_str());
  mqttClient.publish((String(address + "/humidity")).c_str(), humidity.c_str());
  mqttClient.publish((String(address + "/light")).c_str(), light.c_str());
  mqttClient.publish((String(address + "/motion")).c_str(), motion.c_str());
  mqttClient.publish((String(address + "/button")).c_str(), button.c_str());
  mqttClient.publish((String(address + "/battery")).c_str(), battery.c_str());
  mqttClient.publish((String(address + "/rssi")).c_str(), rssi.c_str());
}
