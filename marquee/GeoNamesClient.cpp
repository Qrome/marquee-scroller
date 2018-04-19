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

#include "GeoNamesClient.h"

GeoNamesClient::GeoNamesClient(String UserName, String lat, String lon) {
  myLat = lat;
  myLon = lon;
  myUserName = UserName;
}

float GeoNamesClient::getTimeOffset() {
  WiFiClient client;
  String apiGetData = "GET /timezoneJSON?lat=" + myLat + "&lng=" + myLon + "&username=" + myUserName + " HTTP/1.1";

  Serial.println("Getting TimeZone Data for " + myLat + "," + myLon);
  Serial.println(apiGetData);
  String result = "";
  if (client.connect(servername, 80)) {  //starts client connection, checks for connection
    client.println(apiGetData);
    client.println("Host: " + String(servername));
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
  } 
  else {
    Serial.println("connection for timezone data failed"); //error message if no client connect
    Serial.println();
    return 0;
  }

  while(client.connected() && !client.available()) delay(1); //waits for data
 
  Serial.println("Waiting for data");

  boolean record = false;
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    if (String(c) == "{") {
      record = true;
    }
    if (record) {
      result = result+c; 
    }
    if (String(c) == "}") {
      record = false;
    }
  }
  client.stop(); //stop client
  Serial.println(result);

  char jsonArray [result.length()+1];
  result.toCharArray(jsonArray,sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';
  DynamicJsonBuffer json_buf;
  JsonObject& root = json_buf.parseObject(jsonArray);
  String offset = (const char*)root["dstOffset"];
  // Sample time: "2018-03-19 21:22"
  String timeDate = (const char*)root["time"];
  hours = timeDate.substring(11, 13).toInt();
  minutes = timeDate.substring(14, 16).toInt();
  Serial.println("rawOffset for " + String((const char*)root["timezoneId"]) + " is: " + offset);
  Serial.println("Geo Time: " + String(hours) + ":" + String(minutes));
  Serial.println();
  return offset.toFloat();
}

int GeoNamesClient::getHours() {
  return hours;
}

int GeoNamesClient::getMinutes() {
  return minutes;
}



