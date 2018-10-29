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

GeoNamesClient::GeoNamesClient(String UserName, String lat, String lon, boolean useDst) {
  updateClient(UserName, lat, lon, useDst);
}

void GeoNamesClient::updateClient(String UserName, String lat, String lon, boolean useDst) {
  myLat = lat;
  myLon = lon;
  myUserName = UserName;
  isDst = useDst;
}

float GeoNamesClient::getTimeOffset() {
  datetime = "";
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
  if (!isDst) {
    offset = (const char*)root["rawOffset"];
  }
  // Sample time: "2018-03-19 21:22"
  datetime = (const char*)root["time"];
  Serial.println("rawOffset for " + String((const char*)root["timezoneId"]) + " is: " + offset);
  Serial.println("Geo Date & Time: " + getMonthName() + " " + getDay(false) + ", " + getHours() + ":" + getMinutes());
  Serial.println();
  return offset.toFloat();
}

String GeoNamesClient::getHours() {
  String rtnValue = "";
  if (datetime.length() >= 13) {
    rtnValue = datetime.substring(11, 13);
  }
  return rtnValue;
}

String GeoNamesClient::getMinutes() {
  String rtnValue = "";
  if (datetime.length() >= 16) {
    rtnValue = datetime.substring(14, 16);
  }
  return rtnValue;
}

String GeoNamesClient::getYear() {
  String rtnValue = "";
  if (datetime.length() > 4) {
    rtnValue = datetime.substring(0, 4);
  }
  return rtnValue;
}

String GeoNamesClient::getMonth00() {
  String rtnValue = "";
  if (datetime.length() > 7) {
    rtnValue = datetime.substring(5, 7);
  }
  return rtnValue;
}

String GeoNamesClient::getMonth(boolean zeroPad) {
  String rtnValue = getMonth00();
  if (zeroPad) {
    return rtnValue;
  }
  int month = rtnValue.toInt();
  return String(month);
}

String GeoNamesClient::getMonthName() {
  String rtnValue = "";
  int month = getMonth00().toInt();
  switch (month) {
    case 1:
      rtnValue = "Jan";
      break;
    case 2:
      rtnValue = "Feb";
      break;
    case 3:
      rtnValue = "Mar";
      break;
    case 4:
      rtnValue = "Apr";
      break;
    case 5:
      rtnValue = "May";
      break;
    case 6:
      rtnValue = "June";
      break;
    case 7:
      rtnValue = "July";
      break;
    case 8:
      rtnValue = "Aug";
      break;
    case 9:
      rtnValue = "Sep";
      break;
    case 10:
      rtnValue = "Oct";
      break;
    case 11:
      rtnValue = "Nov";
      break;
    case 12:
      rtnValue = "Dec";
      break;
    default:
      rtnValue = "";
  }
  return rtnValue;
}

String GeoNamesClient::getDay(boolean zeroPad) {
  String rtnValue = getDay00();
  if (zeroPad) {
    return rtnValue;
  }
  int day = rtnValue.toInt();
  return String(day);
}

String GeoNamesClient::getDay00() {
  String rtnValue = "";
  if (datetime.length() > 10) {
    rtnValue = datetime.substring(8, 10);
  }
  return rtnValue;
}




