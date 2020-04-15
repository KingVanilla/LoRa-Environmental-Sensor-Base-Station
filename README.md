<p align="center">
  <img height="110" src="/images/Miami%20Header.jpg">
</p>

# ECE387 Project 2 Report - Spring 2020
## LoRa Environmental Sensor & Base Station
### Created by: Brian Egolf (‘21) & Owen Hardy (‘22)

<p align="center">
  <img height="500" src="/images/Cover%20header.jpg">
</p>


## Table of Contents
**[Project Introduction](#project-introduction)**<br>
**[Design](#design)**<br>
**[Implementation](#implementation)**<br>
**[Final Product](#final-product)**<br>
**[Challenges/issues](#challengesissues)**<br>
**[Conclusion](#conclusion)**<br>
**[Works Cited](#works-cited)**<br>

## Project Introduction
Being passionate about DIY home automation solutions, we wanted to create some sort of a room environmental sensor that could trigger different actions within a home automation sensor.  A temperature sensor could signify the heating system to activate if one room becomes colder than what the heating system things.  A light sensor could signify that a light should turn on.  A motion sensor could signify that a room is occupied and a light should turn on.

Although these sort of "multi-sensor" solutions already exist, we couldn't find one that would suit our needs.  We wanted a solution that was <b>battery powered</b> but also <b>cost-efective</b>.  The cost-effective part ruled out many pre-existing solution since many of those operate on a proprietary communication protocol called Z-Wave.  Z-Wave is a commonly used protocol in battery powered home automation devices since it is low power draw, however, it's expsenive to prototype and license these devices.  We thought that we could create a solution that worked with existing home automation systems while keeping the device low cost while also battery powered.

We were already familiar with the growing LoRa wireless communication protocol and we thought it'd be a perfect platform to design our project around.  <img align="left" width="200" height="150" src="/images/feather%20product%20picture.jpg"> LoRa is a low power, low cost, long range wireless communication protocol.  There already exist many LoRa breakout modules from manufacturers like Sparkfun and Adafruit.   We decided to pick the [Adafruit Feather M0 RFM69HCW Packet Radio](https://www.adafruit.com/product/3176) for our microprocessor.  It has a built-in ARM Cortex M0 processor along with the LoRa radio.  It also supports hardware interrupts, deep sleep modes, analog inputs, I<sup>2</sup>C, and has provisions for a LiPo battery.  Everything we need in one package for $25, awesome.
 
Now that we had our MCU picked out, we were able to pick our sensors that we would use.  We didn't want to pick our sensors first because we thought that it'd be important to pick our MCU hardware first since this was the most constraining part of our design.  We didn't want to settle on a temperature sensor, for example, that supported I<sup>2</sup>C but our MCU didn't.  There are many more sensor options to pick from compared to MCUs.  Luckily we were able to pick all of the exact hardware we wanted.  We selected a [BME280 Temperature & Humidity sensor](https://www.adafruit.com/product/2652) which supports I<sup>2</sup>C.  We selected a "dumb" [analog photo cell](https://www.adafruit.com/product/161) for detecting ambient lighting conditions.  Our third sensor we selected was a [PIR sensor](https://www.amazon.com/gp/product/B07QY7GPWT/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1) for motion detection.  There was no particular reasoning behind picking the BME280 or photo cell, they are popular, cheape, widely available, and well documented.  We knew that whatever motion sensor we were going to use had to be powered on all the time to trigger an interrupt on our MCU to come out of deep sleep if motion was detected.  We initally wanted to go with a microwave sensor since the sensor is much more granular compared to a PIR. This would allow us to detect a stationary human body compared to just motion with a PIR sensor.  Just because there is no motion in a room doesn't mean it's not occupied.  The reason we decided to go with the PIR sensor is that it has a much lower power consumption comapred to a microwave sensor.  We also decided to include a pushbutton which will bring the MCU out of deep sleep and force publish new sensor data.  The button could also be used elsewhere in the home automation system to maybe manually trigger a function in the room such as toggling a light.

The second component to our design is a LoRa base station which will be located at some nearby location.  The purpose of this device is to communicate any data on the LoRa network to the home automation system.  This would be accomplished through a pre-established communication system within the home automation network.  Owen already had a home automation system setup at his house called [OpenHAB](https://www.openhab.org/).  It runs on a Raspberry Pi on his local Internet network.  OpenHAB supports a lightweight communnication protocal called [MQTT](https://www.openhab.org/addons/bindings/mqtt/).  An instance of a [Mosquitto MQTT broker](https://mosquitto.org/) runs on the same Raspberry Pi.  OpenHAB interfaces with the MQTT broker and monitors so called <b>inbound channels</b> for <b>messages</b> which can contain different sensor data and information.  OpenHAB is able to respond to the information within the messages to execute different functions on other components of the home automation.  This is also controlled through MQTT by publishing messagges on different <b>outbound channels</b>.  Each device on the home automation network has its own unique channel that it listens and responds to.  An example flowchart of how a lamp would be turned on is below.

<p align="center">
  <img height="500" src="/images/OpenHAB%20flow%20diagram.png">
</p>


## Design
<p align="center">
  <img height="500" src="/images/wiring%20diagram.jpg">
</p>

## Implementation
## Final Product
## Challenges/issues
## Conclusion
## Works Cited
