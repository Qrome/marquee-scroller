/** The MIT License (MIT)

Copyright (c) 2023 Michael Dawson

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
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <PubSubClient.h>

#define MAX_TOPIC_LEN 128
#define MAX_SERVER_LEN 128


class MqttClient {

  private:
    WiFiClient wclient;
    String lastMessage = "";
    PubSubClient client;
    char topic[MAX_TOPIC_LEN] = {0};
    char server[MAX_SERVER_LEN] = {0};
    int port = 0;
    char failMessage[MAX_TOPIC_LEN + 100] = {0};
  
  public:
    MqttClient(String passedServer, int port, String passedTopic);
    String getError();
    char* getLastMqttMessage();
    char* getNewMqttMessage();
    void updateMqttClient(String passedServer, int port, String passedTopic);

    void loop();
};
