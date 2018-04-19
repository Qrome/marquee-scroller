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

#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) )

AdviceSlipClient::AdviceSlipClient() {

}

void AdviceSlipClient::updateAdvice() {
  JsonStreamingParser parser;
  parser.setListener(this);
  WiFiClient adviceClient;

  String apiGetData = "GET /advice HTTP/1.1";

  Serial.println("Getting Advice Data");
  Serial.println(apiGetData);

  if (adviceClient.connect(servername, 80)) {  //starts client connection, checks for connection
    adviceClient.println(apiGetData);
    adviceClient.println("Host: " + String(servername));
    adviceClient.println("User-Agent: ArduinoWiFi/1.1");
    adviceClient.println("Connection: close");
    adviceClient.println();
  } 
  else {
    Serial.println("connection for advice data failed: " + String(servername)); //error message if no client connect
    Serial.println();
    return;
  }
  
  while(adviceClient.connected() && !adviceClient.available()) delay(1); //waits for data
 
  Serial.println("Waiting for data");

  int size = 0;
  char c;
  boolean isBody = false;
  while (adviceClient.connected() || adviceClient.available()) { //connected or data available
    c = adviceClient.read(); //gets byte from ethernet buffer
    if (c == '{' || c == '[') {
      isBody = true;
    }
    if (isBody) {
      parser.parse(c);
    }
  }
  adviceClient.stop(); //stop client
}

String AdviceSlipClient::getAdvice() {
  return advice.adVice;
}

void AdviceSlipClient::whitespace(char c) {

}

void AdviceSlipClient::startDocument() {
  
}

void AdviceSlipClient::key(String key) {
  currentKey = key;
}

void AdviceSlipClient::value(String value) {
  if (currentKey == "advice") {
    advice.adVice = cleanText(value);
  }
  Serial.println(currentKey + "=" + value);
}

void AdviceSlipClient::endArray() {
}

void AdviceSlipClient::endObject() {
}
void AdviceSlipClient::startArray() {
}

void AdviceSlipClient::startObject() {
}

void AdviceSlipClient::endDocument() {
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
