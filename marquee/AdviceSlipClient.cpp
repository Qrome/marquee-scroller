/** The MIT License (MIT)

Copyright (c) 2018 Nathan Glaus & David Payne

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

#include "AdviceSlipClient.h"

AdviceSlipClient::AdviceSlipClient() {

}

void AdviceSlipClient::updateAdvice() {
  HTTPClient http;
  
  String apiGetData = "http://api.adviceslip.com/advice";

  Serial.println("Getting Advice Data");
  Serial.println(apiGetData);
  int beginCode = http.begin(apiGetData, "*");
  Serial.println("httpBeginCode: " + String(beginCode));
  http.addHeader("Accept", "application/json");
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
    Serial.println("connection for Advice data failed: " + String(apiGetData) + " Code: " + httpCode); //error message if no client connect
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
  DynamicJsonBuffer json_buf;
  JsonObject& root = json_buf.parseObject(jsonArray);

  if (!root.success()) {
    Serial.println(F("Advice Data Parsing failed!"));
    return;
  }

  advice.adVice = (const char*)root["slip"]["advice"];
  advice.adVice = cleanText(String(advice.adVice));
  Serial.println("Advice: " + advice.adVice);
  Serial.println();
}

String AdviceSlipClient::getAdvice() {
  return advice.adVice;
}

String AdviceSlipClient::cleanText(String text) {
  text.replace("’", "'");
  text.replace("“", "\"");
  text.replace("”", "\"");
  text.replace("`", "'");
  text.replace("‘", "'");
  text.replace("\\\"", "'");
  text.replace("•", "-");
  return text;
}
