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

#include "PiHoleClient.h"

PiHoleClient::PiHoleClient() {
  //Constructor
}

void PiHoleClient::getPiHoleData(String server, int port) {

  errorMessage = "";
  String response = "";

  String apiGetData = "http://" + server + ":" + String(port) + "/admin/api.php?summary";
  Serial.println("Sending: " + apiGetData);
  HTTPClient http;  //Object of class HTTPClient
  http.begin(apiGetData);// get the result
  int httpCode = http.GET();
  //Check the returning code
  if (httpCode > 0) {
    response = http.getString();
    http.end();   //Close connection
    if (httpCode != 200) {
      // Bad Response Code
      errorMessage = "Error response (" + String(httpCode) + "): " + response;
      Serial.println(errorMessage);
      return;  
    }
    Serial.println("Response Code: " + String(httpCode));
    Serial.println("Response: " + response);
  } else {
    errorMessage = "Failed to connect and get data: " + apiGetData;
    Serial.println(errorMessage);
    return;
  }
  
  const size_t bufferSize = 2*JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(17) + 470;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // Parse JSON object
  JsonObject& root = jsonBuffer.parseObject(response);
  if (!root.success()) {
    errorMessage = "Data Summary Parsing failed: " + apiGetData;
    Serial.println(errorMessage);
    return;
  }

  piHoleData.domains_being_blocked = (const char*)root["domains_being_blocked"];
  piHoleData.dns_queries_today = (const char*)root["dns_queries_today"];
  piHoleData.ads_blocked_today = (const char*)root["ads_blocked_today"];
  piHoleData.ads_percentage_today = (const char*)root["ads_percentage_today"];
  piHoleData.unique_domains = (const char*)root["unique_domains"];
  piHoleData.queries_forwarded = (const char*)root["queries_forwarded"];
  piHoleData.queries_cached = (const char*)root["queries_cached"];
  piHoleData.clients_ever_seen = (const char*)root["clients_ever_seen"];
  piHoleData.unique_clients = (const char*)root["unique_clients"];
  piHoleData.dns_queries_all_types = (const char*)root["dns_queries_all_types"];
  piHoleData.reply_NODATA = (const char*)root["reply_NODATA"];
  piHoleData.reply_NXDOMAIN = (const char*)root["reply_NXDOMAIN"];
  piHoleData.reply_CNAME = (const char*)root["reply_CNAME"];
  piHoleData.reply_IP = (const char*)root["reply_IP"];
  piHoleData.privacy_level = (const char*)root["privacy_level"];
  piHoleData.piHoleStatus = (const char*)root["status"];

  Serial.println("Pi-Hole Status: " + piHoleData.piHoleStatus);
  Serial.println("Todays Percentage Blocked: " + piHoleData.ads_percentage_today);
  Serial.println();
}

void PiHoleClient::getTopClientsBlocked(String server, int port, String apiKey) {
  errorMessage = "";
  resetClientsBlocked();

  if (apiKey == "") {
    errorMessage = "Pi-hole API Key is required to view Top Clients Blocked.";
    return;
  }

  String response = "";

  String apiGetData = "http://" + server + ":" + String(port) + "/admin/api.php?topClientsBlocked=3&auth=" + apiKey;
  Serial.println("Sending: " + apiGetData);
  HTTPClient http;  //Object of class HTTPClient
  http.begin(apiGetData);// get the result
  int httpCode = http.GET();
  //Check the returning code
  if (httpCode > 0) {
    response = http.getString();
    http.end();   //Close connection
    if (httpCode != 200) {
      // Bad Response Code
      errorMessage = "Error response (" + String(httpCode) + "): " + response;
      Serial.println(errorMessage);
      return;  
    }
    Serial.println("Response Code: " + String(httpCode));
    Serial.println("Response: " + response);
  } else {
    errorMessage = "Failed to get data: " + apiGetData;
    Serial.println(errorMessage);
    return;
  }

  const size_t bufferSize = JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + 70;
  DynamicJsonBuffer jsonBuffer(bufferSize);

  // Parse JSON object
  JsonObject& root = jsonBuffer.parseObject(response);
  if (!root.success()) {
    errorMessage = "Data Parsing failed -- verify your Pi-hole API key.";
    Serial.println(errorMessage);
    return;
  }

  JsonObject& blocked = root["top_sources_blocked"];
  int count = 0;
  for (JsonPair p : blocked) {
    blockedClients[count].clientAddress = (const char*)p.key;
    blockedClients[count].blockedCount = p.value.as<int>();
    Serial.println("Blocked Client " + String(count+1) + ": " + blockedClients[count].clientAddress + " (" + String(blockedClients[count].blockedCount) + ")");
    count++;
  }
  Serial.println();
}

void PiHoleClient::getGraphData(String server, int port) {
    
  String apiGetData = "http://" + server + ":" + String(port) + "/admin/api.php?overTimeData10mins";

  resetBlockedGraphData();
  errorMessage = "";
  String response = "";
  Serial.println("Sending: " + apiGetData);

  HTTPClient http;  //Object of class HTTPClient
  http.begin(apiGetData);// get the result
  int httpCode = http.GET();
  //Check the returning code
  if (httpCode > 0) {
    response = http.getString();
    http.end();   //Close connection
    if (httpCode != 200) {
      // Bad Response Code
      errorMessage = "Error response (" + String(httpCode) + "): " + response;
      Serial.println(errorMessage);
      return;  
    }
    Serial.println("Response Code: " + String(httpCode));
    //Serial.println("Response: " + response);
  } else {
    errorMessage = "Failed to get data: " + apiGetData;
    Serial.println(errorMessage);
    return;
  }

  // Remove half of the stuff -- it is too large to parse
  response = response.substring(response.indexOf("\"ads_over_time"));
  response = "{" + response;


  Serial.println("Modified Response: " + response);

  char jsonArray [response.length()+1];
  response.toCharArray(jsonArray, sizeof(jsonArray));
  //jsonArray[result.length() + 1] = '\0';
  DynamicJsonBuffer json_buf;
  JsonObject& root = json_buf.parseObject(jsonArray);

  if (!root.success()) {
    errorMessage = "Data Parsing failed: " + apiGetData;
    Serial.println(errorMessage);
    return;
  }
  
  JsonObject& ads = root["ads_over_time"];
  int count = 0;
  for (JsonPair p : ads) {
    blocked[count] = p.value.as<int>();
    if (blocked[count] > blockedHigh) {
      blockedHigh = blocked[count];
    }
    //Serial.println("Pi-hole Graph point (" + String(count+1) + "): " + String(blocked[count]));
    count++;
  }
  blockedCount = count;

  Serial.println("High Value: " + String(blockedHigh));
  Serial.println("Count: " + String(blockedCount));
  Serial.println();
}

void PiHoleClient::resetClientsBlocked() {
  for (int inx = 0; inx < 3; inx++) {
    blockedClients[inx].clientAddress = "";
    blockedClients[inx].blockedCount = 0;
  }
}

void PiHoleClient::resetBlockedGraphData() {
  for (int inx = 0; inx < 144; inx++) {
    blocked[inx] = 0;
  }
  blockedCount = 0;
  blockedHigh = 0;
}

String PiHoleClient::getDomainsBeingBlocked() {
  return piHoleData.domains_being_blocked;
}

String PiHoleClient::getDnsQueriesToday() {
  return piHoleData.dns_queries_today;
}

String PiHoleClient::getAdsBlockedToday() {
  return piHoleData.ads_blocked_today;
}

String PiHoleClient::getAdsPercentageToday() {
  return piHoleData.ads_percentage_today;
}

String PiHoleClient::getUniqueClients() {
  return piHoleData.unique_clients;
}

String PiHoleClient::getClientsEverSeen() {
  return piHoleData.clients_ever_seen;
}

/* //Need to add the following
  String getUniqueDomains();
  String getQueriesForwarded();
  String getQueriesCached();
  String getDnsQueriesAllTypes();
  String getReplyNODATA();
  String getReplyNXDOMAIN();
  String getReplyCNAME();
  String getReplyIP();
  String getPrivacyLevel();
 */


String PiHoleClient::getPiHoleStatus() {
  return piHoleData.piHoleStatus;
}

String PiHoleClient::getError() {
  return errorMessage;
}

int *PiHoleClient::getBlockedAds() {
  return blocked;
}

int PiHoleClient::getBlockedCount() {
  return blockedCount;
}

int PiHoleClient::getBlockedHigh() {
  return blockedHigh;
}

String PiHoleClient::getTopClientBlocked(int index) {
  return blockedClients[index].clientAddress;
}
  
int PiHoleClient::getTopClientBlockedCount(int index) {
  return blockedClients[index].blockedCount;
}
