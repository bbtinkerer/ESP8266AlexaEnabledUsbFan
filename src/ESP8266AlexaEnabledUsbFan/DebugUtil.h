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

#ifndef DEBUGUTIL_H
#define DEBUGUTIL_H

#ifdef DEBUG
  #define DEBUG_UTIL_BEGIN       Serial.begin(115200); while(!Serial) { }
  #define DEBUG_UTIL_PRINT(...)    Serial.print(__VA_ARGS__)
  #define DEBUG_UTIL_PRINTF(...)   Serial.printf(__VA_ARGS__)
  #define DEBUG_UTIL_PRINTLN(...)  Serial.println(__VA_ARGS__)
#else
  #define DEBUG_UTIL_BEGIN
  #define DEBUG_UTIL_PRINT(...)
  #define DEBUG_UTIL_PRINTF(...)
  #define DEBUG_UTIL_PRINTLN(...)
#endif

#endif
