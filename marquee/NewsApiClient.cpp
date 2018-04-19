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

#include "NewsApiClient.h"

#define arr_len( x )  ( sizeof( x ) / sizeof( *x ) )

NewsApiClient::NewsApiClient(String ApiKey, String NewsSource) {
  
  mySource = NewsSource;
  myApiKey = ApiKey;

}

void NewsApiClient::updateNews() {
  JsonStreamingParser parser;
  parser.setListener(this);
  WiFiClient newsClient;

  String apiGetData = "GET /v2/top-headlines?sources=" + mySource + "&apiKey=" + myApiKey + " HTTP/1.1";

  Serial.println("Getting News Data");
  Serial.println(apiGetData);

  if (newsClient.connect(servername, 80)) {  //starts client connection, checks for connection
    newsClient.println(apiGetData);
    newsClient.println("Host: " + String(servername));
    newsClient.println("User-Agent: ArduinoWiFi/1.1");
    newsClient.println("Connection: close");
    newsClient.println();
  } 
  else {
    Serial.println("connection for news data failed: " + String(servername)); //error message if no client connect
    Serial.println();
    return;
  }
  
  while(newsClient.connected() && !newsClient.available()) delay(1); //waits for data
 
  Serial.println("Waiting for data");

  int size = 0;
  char c;
  boolean isBody = false;
  while (newsClient.connected() || newsClient.available()) { //connected or data available
    c = newsClient.read(); //gets byte from ethernet buffer
    if (c == '{' || c == '[') {
      isBody = true;
    }
    if (isBody) {
      parser.parse(c);
    }
  }
  newsClient.stop(); //stop client
}

String NewsApiClient::getTitle(int index) {
  return news[index].title;
}

String NewsApiClient::getDescription(int index) {
  return news[index].description;
}

String NewsApiClient::getUrl(int index) {
  return news[index].url;
}

void NewsApiClient::updateNewsSource(String source) {
  mySource = source;
}

void NewsApiClient::whitespace(char c) {

}

void NewsApiClient::startDocument() {
  counterTitle = 0;
}

void NewsApiClient::key(String key) {
  currentKey = key;
}

void NewsApiClient::value(String value) {
  if (counterTitle == 10) {
    // we are full so return
    return;
  }
  if (currentKey == "title") {
    news[counterTitle].title = cleanText(value);
  }
  if (currentKey == "description") {
    news[counterTitle].description = cleanText(value);
  }
  if (currentKey == "url") {
    news[counterTitle].url = value;
    counterTitle++;
  }
  Serial.println(currentKey + "=" + value);
}

void NewsApiClient::endArray() {
}

void NewsApiClient::endObject() {
}
void NewsApiClient::startArray() {
}

void NewsApiClient::startObject() {
}

void NewsApiClient::endDocument() {
}

String NewsApiClient::cleanText(String text) {
  text.replace("’", "'");
  text.replace("“", "\"");
  text.replace("”", "\"");
  text.replace("`", "'");
  text.replace("‘", "'");
  text.replace("\\\"", "'");
  text.replace("•", "-");
  return text;
}




