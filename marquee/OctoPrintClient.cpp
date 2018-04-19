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

#include "OctoPrintClient.h"

OctoPrintClient::OctoPrintClient(String ApiKey, String server, int port) {
  updateOctoPrintClient(ApiKey, server, port);
}

void OctoPrintClient::updateOctoPrintClient(String ApiKey, String server, int port) {
  server.toCharArray(myServer, 100);
  myApiKey = ApiKey;
  myPort = port;
}

void OctoPrintClient::getPrinterJobResults() {
  WiFiClient printClient;
  String apiGetData = "GET /api/job HTTP/1.1";

  Serial.println("Getting Octoprint Data");
  Serial.println(apiGetData);
  result = "";
  if (printClient.connect(myServer, myPort)) {  //starts client connection, checks for connection
    printClient.println(apiGetData);
    printClient.println("Host: " + String(myServer) + ":" + String(myPort));
    printClient.println("X-Api-Key: " + myApiKey);
    printClient.println("User-Agent: ArduinoWiFi/1.1");
    printClient.println("Connection: close");
    printClient.println();
  } 
  else {
    Serial.println("connection for OctoPrint data failed"); //error message if no client connect
    Serial.println();
    printerData.state = "";
    return;
  }

  while(printClient.connected() && !printClient.available()) delay(1); //waits for data
 
  Serial.println("Waiting for data...");

  boolean isBody = false;
  char c;
  while (printClient.connected() || printClient.available()) { //connected or data available
    c = printClient.read(); //gets byte from ethernet buffer
    if (c == '{' || c == '[') {
      isBody = true;
    }
    if (isBody) {
      result = result + c;
    }
  }
  
  printClient.stop(); //stop client
  Serial.println(result);

  char jsonArray [result.length()+1];
  result.toCharArray(jsonArray,sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';
  DynamicJsonBuffer json_buf;
  JsonObject& root = json_buf.parseObject(jsonArray);

  printerData.averagePrintTime = (const char*)root["job"]["averagePrintTime"];
  printerData.estimatedPrintTime = (const char*)root["job"]["estimatedPrintTime"];
  printerData.fileName = (const char*)root["job"]["file"]["name"];
  printerData.fileSize = (const char*)root["job"]["file"]["size"];
  printerData.lastPrintTime = (const char*)root["job"]["lastPrintTime"];
  printerData.progressCompletion = (const char*)root["progress"]["completion"];
  printerData.progressFilepos = (const char*)root["progress"]["filepos"];
  printerData.progressPrintTime = (const char*)root["progress"]["printTime"];
  printerData.progressPrintTimeLeft = (const char*)root["progress"]["printTimeLeft"];
  printerData.state = (const char*)root["state"];
}

String OctoPrintClient::getAveragePrintTime(){
  return printerData.averagePrintTime;
}

String OctoPrintClient::getEstimatedPrintTime() {
  return printerData.estimatedPrintTime;
}

String OctoPrintClient::getFileName() {
  return printerData.fileName;
}

String OctoPrintClient::getFileSize() {
  return printerData.fileSize;
}

String OctoPrintClient::getLastPrintTime(){
  return printerData.lastPrintTime;
}

String OctoPrintClient::getProgressCompletion() {
  return String(printerData.progressCompletion.toInt());
}

String OctoPrintClient::getProgressFilepos() {
  return printerData.progressFilepos;  
}

String OctoPrintClient::getProgressPrintTime() {
  return printerData.progressPrintTime;
}

String OctoPrintClient::getProgressPrintTimeLeft() {
  return printerData.progressPrintTimeLeft;
}

String OctoPrintClient::getState() {
  return printerData.state;
}

boolean OctoPrintClient::isPrinting() {
  boolean printing = false;
  if (printerData.state == "Printing") {
    printing = true;
  }
  return printing;
}

boolean OctoPrintClient::isOperational() {
  boolean operational = false;
  if (printerData.state == "Operational") {
    operational = true;
  }
  return operational;
}


