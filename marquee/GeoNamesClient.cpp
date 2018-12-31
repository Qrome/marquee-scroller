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
  //isDst = useDst;

  if(useDst)
  {
    isDst = getDST();
  }
  else
  {
    isDst = false;
  }

  Serial.println("status isDst " + String(isDst));
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
    offset = (const char*)root["gmtOffset"];
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

//GodzIvan
String GeoNamesClient::getYear00() {
  String rtnValue = "";
  if (datetime.length() > 4) {
    rtnValue = datetime.substring(0, 4);
  }
  return rtnValue;
}

//GodzIvan
String GeoNamesClient::getYear(boolean zeroPad) {
  String rtnValue = getYear00();
  if (zeroPad) {
    return rtnValue;
  }
  int year = rtnValue.toInt();
  return String(year);
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


/*--------------------------------------------------------------------------
  FUNC: 6/11/11 - Returns day of week for any given date
  PARAMS: year, month, date
  RETURNS: day of week (0-6 is Sun-Sat)
  NOTES: Sakamoto's Algorithm
    http://en.wikipedia.org/wiki/Calculating_the_day_of_the_week#Sakamoto.27s_algorithm
    Altered to use char when possible to save microcontroller ram
  https://it.wikipedia.org/wiki/Ora_legale_nel_mondo
--------------------------------------------------------------------------*/
char GeoNamesClient::dow(int y, char m, char d)
   {
       static char t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
       y -= m < 3;
       return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
   }

/*--------------------------------------------------------------------------
  FUNC: 6/11/11 - Returns the date for Nth day of month. For instance,
    it will return the numeric date for the 2nd Sunday of April
  PARAMS: year, month, day of week, Nth occurence of that day in that month
  RETURNS: date
  NOTES: There is no error checking for invalid inputs.
--------------------------------------------------------------------------*/
char GeoNamesClient::NthDate(int y, char m, char DOW, char NthWeek)
 {
  char targetDate = 1;
  char targetDay=31;

  char firstDOW = dow(y,m,targetDate);
  while (firstDOW != DOW){ 
    firstDOW = (firstDOW+1)%7;
    targetDate++;
  }
  //Adjust for weeks
  targetDate += (NthWeek-1)*7;

  switch (m)
  {
  default:
    targetDay=31;
    break;
  case 2:
    if ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0)
    {
      targetDay=28;
    }
    else
    {
      targetDay=29;
    }
    break;
  case 4:
    targetDay=30;
    break;
  case 9:
    targetDay=30;
    break;
  case 11:
    targetDay=30;
    break;
  }
  
  if(targetDate > targetDay){targetDate=targetDate-7;}
  
  return targetDate;
}

boolean GeoNamesClient::getDST()
{
  boolean tmpDST = false;

  int year;
  int month;
  int day;
  
  day   = getDay00().toInt();
  month = getMonth00().toInt();
  year  = getYear00().toInt();
  
  if ( month > startMonth && month < endMonth )
  {
    tmpDST = true;
  }

  if ( month == startMonth )
  {
    if (day >= NthDate(year,startMonth,startDOW,startWeek))
    {
      tmpDST = true;
    }
    tmpDST = false;
  }
  
  if ( month == endMonth )
  {
    if (day >= NthDate(year,endMonth,endDOW,endWeek))
    {
      tmpDST = false;
    }
    tmpDST = true;
  }

#if 0
 Serial.println("Year " + String(year) + " Month " + String(month) + " Day " + String(day) +  " DST " + String(tmpDST));
 Serial.println("startMonth " + String(startMonth) + " startDOW " + String(startDOW) + " startWeek " + String(startWeek));
 Serial.println("endMonth " + String(endMonth) + " endDOW " + String(endDOW) + " endWeek " + String(endWeek));
#endif
  
  return tmpDST;
}

void GeoNamesClient::setDST(int smonth,int sweek,int sdow,int emonth,int eweek,int edow)
{
  startMonth = smonth;
  endMonth = emonth;
  startWeek = sweek;
  endWeek = eweek;
  startDOW = sdow;
  endDOW = edow;

#if 0
 Serial.println("startMonth " + String(startMonth) + " startDOW " + String(startDOW) + " startWeek " + String(startWeek));
 Serial.println("endMonth " + String(endMonth) + " endDOW " + String(endDOW) + " endWeek " + String(endWeek));
#endif


  
}
