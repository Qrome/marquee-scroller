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

#include "BitcoinApiClient.h"

BitcoinApiClient::BitcoinApiClient() {
  //Constructor
}

void BitcoinApiClient::updateBitcoinData(String currencyCode) {
  if (currencyCode == "" || currencyCode == "NONE") {
    bpiData.code = "";
    bpiData.rate = "";
    bpiData.description = "";
    bpiData.rate_float = 0;
    return; // nothing to do here
  }
  HTTPClient http;
  
  String apiGetData = "http://" + String(servername) + "/v1/bpi/currentprice/" + currencyCode + ".json";

  Serial.println("Getting Bitcoin Data");
  Serial.println(apiGetData);
  http.begin(apiGetData);
  int httpCode = http.GET();

  String result = "";

  if (httpCode > 0) {  // checks for connection
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if(httpCode == HTTP_CODE_OK) {
      // get length of document (is -1 when Server sends no Content-Length header)
      int len = http.getSize();
      // create buffer for read
      char buff[128] = { 0 };
      // get tcp stream
      WiFiClient * stream = http.getStreamPtr();
      // read all data from server
      Serial.println("Start reading...");
      while(http.connected() && (len > 0 || len == -1)) {
        // get available data size
        size_t size = stream->available();
        if(size) {
          // read up to 128 byte
          int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          for(int i=0;i<c;i++) {
            result += buff[i];
          }
            
          if(len > 0)
            len -= c;
          }
        delay(1);
      }
    }
    http.end();
  } else {
    Serial.println("connection for BitCoin data failed: " + String(apiGetData)); //error message if no client connect
    Serial.println();
    return;
  }
  //Clean dirty results
  result.remove(0, result.indexOf("{"));
  result.remove(result.lastIndexOf("}") + 1);
  Serial.println("Results:");
  Serial.println(result);
  Serial.println("End");

  char jsonArray [result.length()+1];
  result.toCharArray(jsonArray,sizeof(jsonArray));
  //jsonArray[result.length() + 1] = '\0';
  DynamicJsonBuffer json_buf;
  JsonObject& root = json_buf.parseObject(jsonArray);

  if (!root.success()) {
    Serial.println(F("Bitcoin Data Parsing failed!"));
    return;
  }
  
  bpiData.code = (const char*)root["bpi"][String(currencyCode)]["code"];
  bpiData.rate = (const char*)root["bpi"][String(currencyCode)]["rate"];
  bpiData.description = (const char*)root["bpi"][String(currencyCode)]["description"];
  bpiData.rate_float = String((const char*)root["bpi"][String(currencyCode)]["rate_float"]).toFloat();

  Serial.println("code: " + bpiData.code);
  Serial.println("rate: " + bpiData.rate);
  Serial.println("description: " + bpiData.description);
  Serial.println("rate_float: " + String(bpiData.rate_float));

  Serial.println();
}

String BitcoinApiClient::getCode() {
  return bpiData.code;
}

String BitcoinApiClient::getRate() {
  String rate = bpiData.rate;
  rate.remove(rate.indexOf(".") + 3);
  return rate;
}

String BitcoinApiClient::getDescription() {
  return bpiData.description;
}

float BitcoinApiClient::getRateFloat() {
  return bpiData.rate_float;
}

