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

#pragma once
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

class GeoNamesClient {

private:
  String myLat = "";
  String myLon = "";
  String myUserName = "";
  boolean isDst = true; // Daylight Savings Time

  int hours = 0;
  int minutes = 0;
  String datetime = "";
  
  const char* servername = "api.geonames.org";  // remote server we will connect to

public:
  GeoNamesClient(String UserName, String lat, String lon, boolean useDst);
  void updateClient(String UserName, String lat, String lon, boolean useDst);
  float getTimeOffset();
  String getHours();
  String getMinutes();
  String getYear();
  String getMonth00();
  String getMonth(boolean zeroPad);
  String getMonthName();
  String getDay00();
  String getDay(boolean zeroPad);
};

