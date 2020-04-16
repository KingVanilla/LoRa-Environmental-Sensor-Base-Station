<p align="center">
  <img height="110" src="/images/Miami%20Header.jpg">
</p>

# ECE387 Embeded Systems Project 2 Report - Spring 2020
## LoRa Environmental Sensor & Base Station
### Created by: Brian Egolf (‘21) & Owen Hardy (‘22)

<p align="center">
  <img height="500" src="/images/Cover%20header.jpg">
</p>

## Table of Contents
**[Introduction](#introduction)**<br>
**[Design](#design)**<br>
**[Implementation](#implementation)**<br>
**[Results](#results)**<br>
**[Conclusion](#conclusion)**<br>
**[Works Cited](#works-cited)**<br>

## Introduction
Being passionate about DIY home automation solutions, we wanted to create some sort of a room environmental sensor that could trigger different actions within a home automation sensor.  A temperature sensor could signify the heating system to activate if one room becomes colder than the heating system thinks.  A light sensor could signify that a light should turn on.  A motion sensor could signify that a room is occupied and a light should turn on.

Although these sorts of "multi-sensor" solutions already exist, we couldn't find one that would suit our needs.  We wanted a solution that was <b>battery powered</b> but also <b>cost-effective</b>.  The cost-effective part ruled out many pre-existing solutions since many of those operate on a proprietary communication protocol called Z-Wave.  Z-Wave is a commonly used protocol in battery-powered home automation devices since it is low power draw, however, it's expensive to prototype and license these devices.  We thought that we could create a solution that worked with existing home automation systems while keeping the device low cost while also battery powered.

<img align="left" height="150" src="/images/feather%20product%20picture.jpg">  We were already familiar with the growing LoRa wireless communication protocol and we thought it'd be a perfect platform to design our project around.  LoRa is a low power, low cost, long-range wireless communication protocol.  There already exist many LoRa breakout modules from manufacturers like Sparkfun and Adafruit.   We decided to pick the [Adafruit Feather M0 RFM69HCW Packet Radio](https://www.adafruit.com/product/3176) for our microprocessor.  It has a built-in ARM Cortex M0 processor along with the LoRa radio.  It also supports hardware interrupts, deep sleep modes, analog inputs, I<sup>2</sup>C, and has provisions for a LiPo battery.  Everything we need in one package for $25, awesome.
 
### Sensor Module 
Now that we had our MCU picked out, we were able to pick the sensors that we would use.  We didn't want to pick our sensors first because we thought that it'd be important to pick our MCU hardware first since this was the most constraining part of our design.  We didn't want to settle on a temperature sensor, for example, that supported I<sup>2</sup>C but our MCU didn't.  There are many more sensor options to pick from compared to MCUs.  Luckily we were able to pick all of the exact hardware we wanted.  We selected a [BME280 Temperature & Humidity sensor](https://www.adafruit.com/product/2652) which supports I<sup>2</sup>C.  We selected a "dumb" [analog photocell](https://www.adafruit.com/product/161) for detecting ambient lighting conditions.  The third sensor we selected was a [PIR sensor](https://www.amazon.com/gp/product/B07QY7GPWT/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1) for motion detection.  There was no particular reasoning behind picking the BME280 or photocell, they are popular, cheap, widely available, and well documented.  We knew that whatever motion sensor we were going to use had to be powered on all the time to trigger an interrupt on our MCU to come out of deep sleep if motion was detected.  We initially wanted to go with a microwave sensor since the sensor is much more granular compared to a PIR. This would allow us to detect a stationary human body compared to just motion with a PIR sensor.  Just because there is no motion in a room doesn't mean it's not occupied.  The reason we decided to go with the PIR sensor is that it has a much lower power consumption compared to a microwave sensor.  We also decided to include a pushbutton which will bring the MCU out of deep sleep and force publish new sensor data.  The button could also be used elsewhere in the home automation system to maybe manually trigger a function in the room such as toggling a light.  We knew it would also be important to publish the battery capacity of the device so that the home automation service could monitor this and send the user a notification if a charge was required.

### LoRa to WiFi Base Station
The second component to our design is a LoRa base station which will be located at some nearby location.  The purpose of this device is to communicate any data on the LoRa network to the home automation system.  This would be accomplished through a pre-established communication system within the home automation network.  Owen already had a home automation system set up called [OpenHAB](https://www.openhab.org/).  It runs on a Raspberry Pi on his local Internet network.  OpenHAB supports a lightweight communication protocol called [MQTT](https://www.openhab.org/addons/bindings/mqtt/).  An instance of a [Mosquitto MQTT broker](https://mosquitto.org/) runs on the same Raspberry Pi.  OpenHAB interfaces with the MQTT broker and monitors so-called <b>inbound channels</b> for <b>messages</b> which can contain different sensor data and information.  OpenHAB is able to respond to the information within the messages to execute different functions on other components of the home automation.  This is also controlled through MQTT by publishing messages on different <b>outbound channels</b>.  Each device on the home automation network has its own unique channel that it listens and responds to.  An example flowchart of how a lamp would be turned on is below.

<p align="center">
  <img height="500" src="/images/OpenHAB%20flow%20diagram.png">
</p>

<img align="right" height="150" src="/images/nodemcu.jpg"> Interfacing the LoRa network with MQTT would require passing LoRa data packets.  We already had a LoRa radio, and we decided to use an [ESP8266 NodeMCU](https://nodemcu.readthedocs.io/en/master/) for our WiFi interface.  The reasons we went with this option is they're cheap, widely available, Arduino-compatible, and we have experience using them.  The LoRa radio would simply listen for new data to be pushed on to the network and the NodeMCU would wait for incoming LoRa data packets and push them out on MQTT over WiFi.

<br>

## Design
### Sensor Module <img align="right" img height="400" src="/images/wiring%20diagram.jpg">
Connecting all of our sensors together was relatively straight forward.  Our BME280 temperature and humidity sensor communications via I<sup>2</sup>C.  Our PIR sensor simply had a digital output that was active `HIGH`.  Our pushbutton was wired to a digital input that was connected to a pullup resistor on the ARM MCU through software.  Our photocell was connected to an analog input and a 10k voltage divider.  Lastly, our battery monitor was connected to the Feather's `"BATT"` pin which exposed the raw battery voltage.  This was connected to an analog input via a dual 100k voltage divider as detailed on the Adafruit hookup guide for the Feather.  A voltage divider is required here because the LiPo's battery voltage can reach a maximum of 4.2V which is too high for the Feather which can only handle a maximum of 3.3V.  The voltage divider will half the apparent voltage of the battery so the highest analog voltage the Feather will see is 2.2V.

As for the software, we wanted the device to operate in a very specific way to conserve as much power as possible.  We wanted the MCU to be in a deep sleep mode most of the time.  The MCU would come out of sleep only under the following conditions.  (1) The pushbutton has been pressed. (2) Motion has been detected for the first time in over 10 minutes. (3) 10 minutes have passed without a wake cycle from either the PIR sensor or pushbutton.  We mustn't come out of sleep every time the PIR sensor outputs `HIGH` because then the MCU may be on for minutes at a time when there are many guests in the room or when someone is up and moving around.  We assume that the room is unoccupied if there has been no motion for 10 minutes.  When the device does come out of sleep, we turn on our sensors and LoRa radio, take data readings, transmit them over LoRa, and then return to deep sleep.

<p align="center">
  <img height="200" src="/images/FSM.jpg">
</p>

### LoRa to WiFi Base Station <img align="right" img height="350" src="/images/Lora%20to%20WiFi%20Base%20Station.jpg">
Our LoRa to WiFi base station was the most simple part of our solution.  It simply had to relay data from the LoRa radio to the NodeMCU so that it can be transmitted to the proper MQTT channel.  This will be communicated across a Serial line for simplicity.

<br><br><br><br><br><br><br><br>

## Implementation
### Sensor Integration
The first part of our software consisted of integrating all of our sensors.  We specifically started with the BME280 temeperature and humidity sensor since our instructor wanted us to create this code from scratch without using a library.  We started off by jumping right into the information in the datasheet.  You should have been able to read from the `"id"` register to see if you had the chip connected properly.  We initally couldn't get a reading from this and thought that we weren't able to read and write on our I<sup>2</sup>C bus correctly.  Eventually we downloaded the Adafruit BME280 just to test the part and sure enough we had no luck with that library.  We figured the part was DoA and ordered a replacement.  Unfortunately this happened right at the start of the COVID-19 pandemic so it we had to wait a while on shipping.  We ordered extra this time just in case.  We were able to read back the correct `0x60` chip ID with our new part.  Next we moved on to setting the configuration registers.  These set things like oversampling correction, filters, and standby times.  This was relativley easy, all we had to do was implement the instructions from the datasheet into Arduino code which involved reading, writing, and shifting various bits and registers.  The way that we tested we set our configuration parameters correctly was by using a part of a pre-existing libraries for the device to read the temperature.  We knew that if we got a temperature reading back we had sucessfully implemented configurating the device's parameters.  Once we got that working, we moved on the creating the code for reading the temperature and humidty.  This involved reading from various registers and shifting the bits.  Again, this was detailed in the datasheet and was identicle for both the temperature and humidity except for the register addresses.  This part of the software was by far the most challenging part of the project which is why we wanted to tackle it first that way we could come up with a backup plan if we had unresolvable issues.

<img align="right" img width="500" src="/images/Sensor%20output%20screenshot.jpg"> The software for remaining sensors we had to integrate was quite simple.  All of our sensor readings were analog/digital readings implemented with a simple `analogRead` or `digitalRead`.  For debugging purposes, we printed these values out to the Serial Monitor.  Once we could see all of our sensor data printing to the Serial Monitor, we decided it would be best to compile all of the data into a data packet that is fixed in length and has data points in the same recurring positions.  This would make it easy to parse and segregate the data before we published each value to its own MQTT topic.  We define a unique ID to each LoRa client on the network so that we can identify and publish to unique MQTT topics.  We constrained all of our data values to a fixed length of 1, 2, or 4 characters depending on the data point so their position indicies are fixed in space.  Below is how the data packet is structured.

```
Data packet structure: "xx,xxxx,xxxx,xx,x,x,xx"

Index position:
id       = [0,1]
temp     = [3,4,5,6]
humidity = [8,9,10,11]
lux      = [13,14]
motion   = [16]
button   = [18]
battery  = [20,21]
```
 
### LoRa Communication
We wanted to get our LoRa network working independently before sending any sensor data to make it as easy as possible to debug.  We first used the example sketches provided in the Adafruit Radiohead library.  One LoRa radio was setup as the base station and the other was setup as a client.  We simply sent `"Hello"` as our data packet every 2 seconds.  We had our client connected to power sending this data packet and blinking its onboard `LED` upon packet transmission so we could have a visual indicator.  Our base station was connected to our Arduino Serial Monitor so we could see the incomming data transmissions.  As expected, we recieved a `"Hello"` every 2 seconds.

<img align="right" img width="500" src="/images/LoRa%20base%20station%20output%20screenshot.jpg"> Once we had our LoRa radios communicating properly, we started transmitting our constructed data packet which worked as expected.  This signified the near completion of the sensor module.  The only thing left to do is implement the deep sleep functionality which will be done after the entire system is working as a whole.

### LoRa to WiFi Translation
<img align="right" img width="500" src="/images/ESP%20output%20screenshot.jpg"> Now that we had our base station LoRa receiving the sensor module's data, we needed to get that information sent to our NodeMCU so that we can publish it over MQTT.  When our LoRa module recieves a data packet, we also recieve a value called `RSSI` which represents our signal quality.  It's reported as a negative value and the closer it is to 0, the stronger our signal.  Adafruit says you can expect to recieve a signal between -15 (highest) and -80 (lowest).  Before we added any sort of antenna, we had a signal quality of -96 when the two radios were only a few feet from each other.  We added a 3" quarter-whip antenna to both radios as suggested by Adafruit.  This increased our signal strength dramatically  The modules could be separated the distance of the home with an acceptable signal quality of -50.  We thought this might be an important metric to publish to the home automation server so we added an additional 3 characters at the end of our constructed data packet to fit it in.  Our new data packet structred was as follows.

```
Data packet structure: "xx,xxxx,xxxx,xx,x,x,xx,xxx"

Index position:
id       = [0,1]
temp     = [3,4,5,6]
humidity = [8,9,10,11]
lux      = [13,14]
motion   = [16]
button   = [18]
battery  = [20,21]
rssi     = [22,23,24]
```

This 24-bit long packet is sent over Serial from the LoRa MCU to the NodeMCU.  Once the data packet has reached the NodeMCU, we are able to decompose our data packet into 8 data points.  We need to do this because we need to publish each value on its own separate MQTT topic so that we have separate metrics for our home automation server to react to.  Here is an example of how an incoming data packet would be decomposed and published to MQTT by the NodeMCU.

```
Data packet: "02,70.1,43,07,1,0,99,-32"
Publish on topic: "/home/LoRa/module02/temperature" "70.1"
Publish on topic: "/home/LoRa/module02/humidity" "43"
Publish on topic: "/home/LoRa/module02/lux" "07"
Publish on topic: "/home/LoRa/module02/motion" "1"
Publish on topic: "/home/LoRa/module02/button" "0"
Publish on topic: "/home/LoRa/module02/battery" "99"
Publish on topic: "/home/LoRa/module02/rssi" "-32"
```

## Results

## Conclusion

### Challenges/issues

## Works Cited
**[Adafruit Feather M0 RFM69HCW Hookup Guide](https://learn.adafruit.com/adafruit-feather-m0-radio-with-rfm69-packet-radio)**<br>
**[Bosch BME280 Datasheet (pages 23-31)](https://ae-bst.resource.bosch.com/media/_tech/media/datasheets/BST-BME280-DS002.pdf)**<br>
**[Adafruit Radiohead LoRa Arduino Library](https://github.com/adafruit/RadioHead)**<br>
**[Arduino MQTT Library](https://github.com/knolleary/pubsubclient)**<br>
**[Arduino ESP8266 Core Library](https://github.com/esp8266/Arduino)**<br>
**Visual aids and diagrams made using [Creatley](https://creately.com/)**<br>
