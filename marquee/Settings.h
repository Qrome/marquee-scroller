/** The MIT License (MIT)

Copyright (c) 2018 David Payne

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/******************************************************************************
 * This is designed for the Wemos D1 ESP8266
 * Wemos D1 Mini:  https://amzn.to/2qLyKJd
 * MAX7219 Dot Matrix Module 4-in-1 Display For Arduino
 * Matrix Display:  https://amzn.to/2HtnQlD
 ******************************************************************************/
/******************************************************************************
 * NOTE: The settings here are the default settings for the first loading.  
 * After loading you will manage changes to the settings via the Web Interface.  
 * If you want to change settings again in the settings.h, you will need to 
 * erase the file system on the Wemos or use the “Reset Settings” option in 
 * the Web Interface.
 ******************************************************************************/
 
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include "FS.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Max72xxPanel.h>
#include "OpenWeatherMapClient.h"
#include "GeoNamesClient.h"
#include "TimeClient.h" // Using updated lib by Qrome
#include "NewsApiClient.h" 
#include "OctoPrintClient.h"
#include "AdviceSlipClient.h"
#include "BitcoinApiClient.h"

//******************************
// Start Settings
//******************************

const String APIKEY = ""; // Your API Key from http://openweathermap.org/
// Default City Location (use http://openweathermap.org/find to find city ID)
int CityIDs[] = { 5304391 }; //Only USE ONE for weather marquee
String marqueeMessage = "";
boolean IS_METRIC = false; // false = Imperial and true = Metric
boolean IS_24HOUR = false; // 23:00 millitary 24 hour clock
const int WEBSERVER_PORT = 80; // The port you can access this device on over HTTP
const boolean WEBSERVER_ENABLED = true;  // Device will provide a web interface via http://[ip]:[port]/
boolean IS_BASIC_AUTH = true;  // Use Basic Authorization for Configuration security on Web Interface
char* www_username = "admin";  // User account for the Web Interface
char* www_password = "password";  // Password for the Web Interface
int minutesBetweenDataRefresh = 15;  // Time in minutes between data refresh (default 15 minutes)
int minutesBetweenScrolling = 1; // Time in minutes between scrolling data (default 1 minutes and max is 10)
int displayScrollSpeed = 25; // In milliseconds -- Configurable by the web UI (slow = 35, normal = 25, fast = 15)

boolean NEWS_ENABLED = true;
String NEWS_API_KEY = ""; // Get your News API Key from https://newsapi.org
String NEWS_SOURCE = "reuters";  // https://newsapi.org/sources to get full list of news sources available

boolean ADVICE_ENABLED = false;

// Display Settings
// CLK -> D5 (SCK)  
// CS  -> D6 
// DIN -> D7 (MOSI)
const int pinCS = D6; // Attach CS to this pin, DIN to MOSI and CLK to SCK (cf http://arduino.cc/en/Reference/SPI )
int displayIntensity = 1;  //(This can be set from 1 - 15)
const int numberOfHorizontalDisplays = 4; // default 4 for standard 4 x 1 display
const int numberOfVerticalDisplays = 1; // default 1 for a single row height

String timeDisplayTurnsOn = "06:30";  // 24 Hour Format HH:MM -- Leave blank for always on. (ie 05:30)
String timeDisplayTurnsOff = "23:00"; // 24 Hour Format HH:MM -- Leave blank for always on. Both must be set to work.

const String GEONAMES_USER = "qrome"; // user account for  http://www.geonames.org/ -- this service is used to lookup TimeZone Offsets

// OctoPrint Monitoring -- Monitor your 3D printer OctoPrint Server
boolean OCTOPRINT_ENABLED = false;
String OctoPrintApiKey = "";  // ApiKey from your User Account on OctoPrint
String OctoPrintServer = ""; // IP or Address of your OctoPrint Server (DO NOT include http://)
int OctoPrintPort = 80; // the port you are running your OctoPrint server on (usually 80);
String OctoAuthUser = "";      // only used if you have haproxy or basic athentintication turned on (not default)
String OctoAuthPass = "";      // only used with haproxy or basic auth (only needed if you must authenticate)

// Bitcoin Client - NONE or empty is off
String BitcoinCurrencyCode = "NONE";  // Change to USD, GBD, EUR, or NONE -- this can be managed in the Web Interface

boolean ENABLE_OTA = true;  // this will allow you to load firmware to the device over WiFi (see OTA for ESP8266)
String OTA_Password = "";      // Set an OTA password here -- leave blank if you don't want to be prompted for password

//******************************
// End Settings
//******************************
