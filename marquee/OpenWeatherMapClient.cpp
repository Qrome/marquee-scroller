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

#include "OpenWeatherMapClient.h"

OpenWeatherMapClient::OpenWeatherMapClient(String ApiKey, int CityIDs[], int cityCount, boolean isMetric) {
  updateCityIdList(CityIDs, cityCount);
  myApiKey = ApiKey;
  setMetric(isMetric);
}

void OpenWeatherMapClient::updateWeatherApiKey(String ApiKey) {
  myApiKey = ApiKey;
}

void OpenWeatherMapClient::updateWeather() {
  WiFiClient weatherClient;
  String apiGetData = "GET /v1/devices/?apiKey=" + myApiKey + "&applicationKey=9ca57c2ff32845cab6ac3c5bddb36b3209549a4bd3e84c76a2e37687629d21cb HTTP/1.1";

  Serial.println("Getting Weather Data");
  Serial.println(apiGetData);
  weathers[0].cached = false;
  weathers[0].error = "";
  if (weatherClient.connect(servername, 80)) {  //starts client connection, checks for connection
    weatherClient.println(apiGetData);
    weatherClient.println("Host: " + String(servername));
    weatherClient.println("User-Agent: ArduinoWiFi/1.1");
    weatherClient.println("Connection: close");
    weatherClient.println();
  }
  else {
    Serial.println("connection for weather data failed"); //error message if no client connect
    Serial.println();
    weathers[0].error = "Connection for weather data failed";
    return;
  }

  while (weatherClient.connected() && !weatherClient.available()) delay(1); //waits for data

  Serial.println("Waiting for data");

  // Check HTTP status
  char status[32] = {0};
  weatherClient.readBytesUntil('\r', status, sizeof(status));
  Serial.println("Response Header: " + String(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    weathers[0].error = "Weather Data Error: " + String(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!weatherClient.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    return;
  }

  const size_t bufferSize = 710;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // Parse JSON objectarray
  JsonArray& root = jsonBuffer.parseArray(weatherClient);
  if (!root.success()) {
    Serial.println(F("Weather Data Parsing failed!"));
    weathers[0].error = "Weather Data Parsing failed!";
    return;
  }

  weatherClient.stop(); //stop client

  if (root.measureLength() <= 150) {
    Serial.println("Error Does not look like we got the data.  Size: " + String(root.measureLength()));
    weathers[0].cached = true;
    weathers[0].error = (const char*)root[0]["message"];
    Serial.println("Error: " + weathers[0].error);
    return;
  }
  int inx = 0;



  const char* location = (const char*)root[0]["info"]["location"];
  weathers[inx].lat = getValue(location, ',', 0);
  weathers[inx].lon = getValue(location, ',', 1);
  weathers[inx].dt = (const char*)root[0]["lastData"]["dateutc"];
  weathers[inx].city = (const char*)root[0]["info"]["name"];
  weathers[inx].country = "country";
  weathers[inx].temp = (const char*)root[0]["lastData"]["tempf"];
  weathers[inx].humidity = (const char*)root[0]["lastData"]["humidity"];
  weathers[inx].condition = "condition";
  weathers[inx].wind = (const char*)root[0]["lastData"]["windspeedmph"];
  weathers[inx].weatherId = "weatherId";

  String description = "";
  if(weathers[inx].wind.toInt() > 20) {
    description = "REALLY WINDY AND ";
  } else if (weathers[inx].wind.toInt() <= 20 && weathers[inx].wind.toInt() > 5) {
    description = "KINDA WINDY AND ";
  } else {
    description = "";
  }

  
  if(weathers[inx].temp.toInt() > 90) { // Hot
    if(weathers[inx].humidity.toInt() > 95) { // Hot & Raining
      description = description + "HOT AND MAYBE RAINING!";
    } else if(weathers[inx].humidity.toInt() <= 95 && weathers[inx].humidity.toInt() > 60) { // Hot and Humid
      description = description + "HOT AND HUMID!";
    } else {
      description = description + "HOT!";
    }
  } else if(weathers[inx].temp.toInt() <= 80 && weathers[inx].temp.toInt() > 33) { // Okay
    if(weathers[inx].humidity.toInt() > 95) { // Raining
      description = description + "MAYBE RAINING";
    } else if(weathers[inx].humidity.toInt() <= 95 && weathers[inx].humidity.toInt() > 60) { // Humid
      description = description + "HUMID";
    } else {
      description = description + "OKAY'ish";
    }   
  } else if(weathers[inx].temp.toInt() <= 32 && weathers[inx].temp.toInt() > 25) { // Cold
    if(weathers[inx].humidity.toInt() > 80) { // Cold and Snowing
      description = description + "SNOWING MAYBE";
    } else if(weathers[inx].humidity.toInt() <= 80 && weathers[inx].humidity.toInt() > 50) { // Hot and Humid
      description = description + "PROBABLY WINTER LIKE";
    } else {
      description = description + "PROBABLY WINTER LIKE";
    }
  } else { // Really Cold
    if(weathers[inx].humidity.toInt() > 80) { // Cold and ICE
      description = description + "ICE FALLING FROM THE SKY MAYBE";
    } else if(weathers[inx].humidity.toInt() <= 80 && weathers[inx].humidity.toInt() > 50) { // Hot and Humid
      description = description + "COLD!";
    } else {
      description = description + "COLD!";
    }
  }

  weathers[inx].description = description;
  weathers[inx].icon = "";

  Serial.println("lat: " + weathers[inx].lat);
  Serial.println("lon: " + weathers[inx].lon);
  Serial.println("dt: " + weathers[inx].dt);
  Serial.println("city: " + weathers[inx].city);
  Serial.println("country: " + weathers[inx].country);
  Serial.println("temp: " + weathers[inx].temp);
  Serial.println("humidity: " + weathers[inx].humidity);
  Serial.println("condition: " + weathers[inx].condition);
  Serial.println("wind: " + weathers[inx].wind);
  Serial.println("weatherId: " + weathers[inx].weatherId);
  Serial.println("description: " + weathers[inx].description);
  Serial.println("icon: " + weathers[inx].icon);
  Serial.println();


}

String OpenWeatherMapClient::getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  String result = found > index ? data.substring(strIndex[0], strIndex[1]) : "";
  result.trim();
  return result;
}



String OpenWeatherMapClient::roundValue(String value) {
  float f = value.toFloat();
  int rounded = (int)(f + 0.5f);
  return String(rounded);
}

void OpenWeatherMapClient::updateCityIdList(int CityIDs[], int cityCount) {
  myCityIDs = "";
  for (int inx = 0; inx < cityCount; inx++) {
    if (CityIDs[inx] > 0) {
      if (myCityIDs != "") {
        myCityIDs = myCityIDs + ",";
      }
      myCityIDs = myCityIDs + String(CityIDs[inx]);
    }
  }
}

void OpenWeatherMapClient::setMetric(boolean isMetric) {
  if (isMetric) {
    units = "metric";
  } else {
    units = "imperial";
  }
}

String OpenWeatherMapClient::getLat(int index) {
  return weathers[index].lat;
}

String OpenWeatherMapClient::getLon(int index) {
  return weathers[index].lon;
}

String OpenWeatherMapClient::getDt(int index) {
  return weathers[index].dt;
}

String OpenWeatherMapClient::getCity(int index) {
  return weathers[index].city;
}

String OpenWeatherMapClient::getCountry(int index) {
  return weathers[index].country;
}

String OpenWeatherMapClient::getTemp(int index) {
  return weathers[index].temp;
}

String OpenWeatherMapClient::getTempRounded(int index) {
  return roundValue(getTemp(index));
}

String OpenWeatherMapClient::getHumidity(int index) {
  return weathers[index].humidity;
}

String OpenWeatherMapClient::getHumidityRounded(int index) {
  return roundValue(getHumidity(index));
}

String OpenWeatherMapClient::getCondition(int index) {
  return weathers[index].condition;
}

String OpenWeatherMapClient::getWind(int index) {
  return weathers[index].wind;
}

String OpenWeatherMapClient::getWindRounded(int index) {
  return roundValue(getWind(index));
}

String OpenWeatherMapClient::getWeatherId(int index) {
  return weathers[index].weatherId;
}

String OpenWeatherMapClient::getDescription(int index) {
  return weathers[index].description;
}

String OpenWeatherMapClient::getIcon(int index) {
  return weathers[index].icon;
}

boolean OpenWeatherMapClient::getCached() {
  return weathers[0].cached;
}

String OpenWeatherMapClient::getMyCityIDs() {
  return myCityIDs;
}

String OpenWeatherMapClient::getError() {
  return weathers[0].error;
}

String OpenWeatherMapClient::getWeatherIcon(int index)
{
  int id = getWeatherId(index).toInt();
  String W = ")";
  switch (id)
  {
    case 800: W = "B"; break;
    case 801: W = "Y"; break;
    case 802: W = "H"; break;
    case 803: W = "H"; break;
    case 804: W = "Y"; break;

    case 200: W = "0"; break;
    case 201: W = "0"; break;
    case 202: W = "0"; break;
    case 210: W = "0"; break;
    case 211: W = "0"; break;
    case 212: W = "0"; break;
    case 221: W = "0"; break;
    case 230: W = "0"; break;
    case 231: W = "0"; break;
    case 232: W = "0"; break;

    case 300: W = "R"; break;
    case 301: W = "R"; break;
    case 302: W = "R"; break;
    case 310: W = "R"; break;
    case 311: W = "R"; break;
    case 312: W = "R"; break;
    case 313: W = "R"; break;
    case 314: W = "R"; break;
    case 321: W = "R"; break;

    case 500: W = "R"; break;
    case 501: W = "R"; break;
    case 502: W = "R"; break;
    case 503: W = "R"; break;
    case 504: W = "R"; break;
    case 511: W = "R"; break;
    case 520: W = "R"; break;
    case 521: W = "R"; break;
    case 522: W = "R"; break;
    case 531: W = "R"; break;

    case 600: W = "W"; break;
    case 601: W = "W"; break;
    case 602: W = "W"; break;
    case 611: W = "W"; break;
    case 612: W = "W"; break;
    case 615: W = "W"; break;
    case 616: W = "W"; break;
    case 620: W = "W"; break;
    case 621: W = "W"; break;
    case 622: W = "W"; break;

    case 701: W = "M"; break;
    case 711: W = "M"; break;
    case 721: W = "M"; break;
    case 731: W = "M"; break;
    case 741: W = "M"; break;
    case 751: W = "M"; break;
    case 761: W = "M"; break;
    case 762: W = "M"; break;
    case 771: W = "M"; break;
    case 781: W = "M"; break;

    default: break;
  }
  return W;
}
