<p align="center">
  <img width="2500" height="120" src="/images/Miami%20Header.jpg">
</p>

# ECE387 Project 2 Report - Spring 2020
## LoRa Environmental Sensor & Base Station
### Created by: Brian Egolf (‘21) & Owen Hardy (‘22)

<br>

<p align="center">
  <img width="292" height="436" src="/images/Cover%20header.jpg">
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

 We were already familiar with the growing LoRa wireless communication protocol and we thought it'd be a perfect platform to design our project around.  <img align="left" width="200" height="150" src="/images/feather%20product%20picture.jpg"> LoRa is a low power, low cost, long range wireless communication protocol.  There already exist many LoRa breakout modules from manufacturers like Sparkfun and Adafruit.   We decided to pick the [Adafruit Feather M0 RFM69HCE Packet Radio](https://www.adafruit.com/product/3176) for our microprocessor.  It has a built-in ARM Cortex M0 processor along with the LoRa radio.  It also supports hardware interrupts, deep sleep modes, analog inputs, I<sup>2</sup>C, and has provisions for a LiPo battery.  Everything we need in one package for $25, awesome.
 
 

## Design
## Implementation
## Final Product
## Challenges/issues
## Conclusion
## Works Cited
