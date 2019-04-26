/** The MIT License (MIT)

Copyright (c) 2019 David Payne

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
#include "libs/ArduinoJson/ArduinoJson.h"

class PiHoleClient {

private:

  WiFiClient getSubmitRequest(String apiGetData, String myServer, int myPort);
  void resetClientsBlocked();
  void resetBlockedGraphData();
  
  String errorMessage = "";

  int blocked[144] = {0};  
  int blockedCount = 0;
  int blockedHigh = 0;

  typedef struct {
    String clientAddress;
    int blockedCount;
  } ClientBlocked;

  ClientBlocked blockedClients[3];

  typedef struct {
    String domains_being_blocked;
    String dns_queries_today;
    String ads_blocked_today;
    String ads_percentage_today;
    String unique_domains;
    String queries_forwarded;
    String queries_cached;
    String clients_ever_seen;
    String unique_clients;
    String dns_queries_all_types;
    String reply_NODATA;
    String reply_NXDOMAIN;
    String reply_CNAME;
    String reply_IP;
    String privacy_level;
    String piHoleStatus;
  } phd;
  
  phd piHoleData;
  
  
public:
  PiHoleClient();
  void getPiHoleData(String server, int port);
  void getGraphData(String server, int port);
  void getTopClientsBlocked(String server, int port, String apiKey);

  String getDomainsBeingBlocked();
  String getDnsQueriesToday();
  String getAdsBlockedToday();
  String getAdsPercentageToday();
  String getUniqueClients();
  String getClientsEverSeen();
  /*
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
  String getPiHoleStatus();
  String getError();

  int *getBlockedAds();  
  int getBlockedCount();
  int getBlockedHigh();

  String getTopClientBlocked(int index);
  int getTopClientBlockedCount(int index);
};
