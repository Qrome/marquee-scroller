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

#include "MqttClient.h"

WiFiClient wclient;

char lastMqttMessage[128] = {0};
bool lastMqttMessageNew = true;

void callback(char* topic, uint8_t* message, unsigned int length) {
  memcpy(lastMqttMessage, message, length);
  lastMqttMessage[length] = 0;
  lastMqttMessageNew = true;
}

MqttClient::MqttClient(String passedServer, int port, String passedTopic):
  client("", 0, callback, wclient) {
  updateMqttClient(passedServer, port, passedTopic);
}

void MqttClient::updateMqttClient(String passedServer, int port, String passedTopic) {
  this->port = port;
  passedServer.toCharArray(server, MAX_SERVER_LEN);
  passedTopic.toCharArray(topic, MAX_TOPIC_LEN);
  client.setServer(server, port);
  client.disconnect();
}

char* MqttClient::getLastMqttMessage() {
  if (strlen(lastMqttMessage) != 0) {
    return lastMqttMessage;
  }
  return "";
};

char* MqttClient::getNewMqttMessage() {
  if (lastMqttMessageNew == true) {
    lastMqttMessageNew = false;
    return getLastMqttMessage();
  }
  return "";
};


String MqttClient::getError() {
  return failMessage;
}
  
void MqttClient::loop() {
  if (!client.connected()) {
    if (client.connect("marquee")) {
      failMessage[0] = 0;
      if (!client.subscribe(topic))
        sprintf(failMessage, "Failed to connect to topic:%s", topic);
    } else {
      sprintf(failMessage, "Failed to connect to: %s:%d, reason: %d", server, port, client.state());
    }
  }
  if (client.connected()) {
    client.loop();
  }
}
