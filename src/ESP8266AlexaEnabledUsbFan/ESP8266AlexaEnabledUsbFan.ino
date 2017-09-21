/*
* ESP8266 Alexa Enabled USB Fan
* Copyright (c) 2017 bbtinkerer
* 
* Simple Alexa on/off switch with automatic off after configurable amount of time using
* the fauxmoESP library (https://bitbucket.org/xoseperez/fauxmoesp.git)
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//#define DEBUG // uncomment to print to Serial output

#include "DebugUtil.h" // a debug util to remove debug statements from build if DEBUG is not defined
#include <MyCredentials.h> // contains my credential information, outside of this project so I don't accidently commit to GitHub
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"

#define DEVICE_NAME "Daddy's fan" // name used for Alexa command. "Alexa, turn Daddy's fan on"

#define FAN_GPIO 4
#define FAN_ON_DURATION 3600000 // 45mins = 2700000ms; 1hr = 3600000ms; 2hrs = 7200000ms

fauxmoESP fauxmo;
bool fanOn;
unsigned long startTime;
int unusedGPIO[] = {5, 12, 13, 14, 16}; // 0 & 2 has external pull up and 15 has an external pull down on the Wemos D1 Mini
unsigned long timeLeft;

void setup() {
  DEBUG_UTIL_BEGIN;
  DEBUG_UTIL_PRINTLN("");
  DEBUG_UTIL_PRINTLN("Starting...");

  // setup static IP as this cuts down on time trying to obtain an IP through DHCP
  IPAddress espIP(192, 168, 1, 30);
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(espIP, gateway, subnet);

  // Set WIFI module to STA mode
  WiFi.mode(WIFI_STA);

  // Connect
  DEBUG_UTIL_PRINTF("Connecting to %s", HOME_WIFI_SSID);
  WiFi.begin(HOME_WIFI_SSID, HOME_WIFI_PASSWORD);
  // Wait
  while (WiFi.status() != WL_CONNECTED) {
      DEBUG_UTIL_PRINT(".");
      delay(100);
  }
  
  DEBUG_UTIL_PRINTLN();  
  DEBUG_UTIL_PRINTLN("---------------------------");
  DEBUG_UTIL_PRINTLN("WiFi connected");
  DEBUG_UTIL_PRINTF("IP address: %s\n", WiFi.localIP().toString().c_str());
  DEBUG_UTIL_PRINTF("RSSI: %d\n", WiFi.RSSI());
  DEBUG_UTIL_PRINTLN("---------------------------");

  for(int i = 0; i < 5; i++){
    pinMode(unusedGPIO[i], INPUT_PULLUP);
  }

  // Fan
  pinMode(FAN_GPIO, OUTPUT);
  digitalWrite(FAN_GPIO, LOW);
  fanOn = false;

  // Enable fauxmo
  fauxmo.enable(true);

  // Add virtual device
  fauxmo.addDevice(DEVICE_NAME);

  // fauxmoESP 2.0.0 has changed the callback signature to add the device_id, this WARRANTY
  // it's easier to match devices to action without having to compare strings.
  fauxmo.onMessage([](unsigned char device_id, const char * device_name, bool state) {
      DEBUG_UTIL_PRINTF("Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
      digitalWrite(FAN_GPIO, state);
      fanOn = state;
      if(fanOn){
        startTime = millis();
        timeLeft = FAN_ON_DURATION;
      }
  });
}

void loop() {
  // Since fauxmoESP 2.0 the library uses the "compatibility" mode by
  // default, this means that it uses WiFiUdp class instead of AsyncUDP.
  // The later requires the Arduino Core for ESP8266 staging version
  // whilst the former works fine with current stable 2.3.0 version.
  // But, since it's not "async" anymore we have to manually poll for UDP
  // packets
  fauxmo.handle();

  if(fanOn){
    unsigned long currentTime = millis();
    timeLeft -= currentTime - startTime;
    startTime = currentTime;
    if(timeLeft <= 0){
      fanOn = false;
      digitalWrite(FAN_GPIO, LOW);
      DEBUG_UTIL_PRINTF("%d milliseconds elapsed, turning fan off", FAN_ON_DURATION);
    }
  }
}
