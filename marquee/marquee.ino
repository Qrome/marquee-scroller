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

 /**********************************************
 * Edit Settings.h for personalization
 ***********************************************/

#include "Settings.h"

#define VERSION "2.0"

#define HOSTNAME "CLOCK-" 
#define CONFIG "/conf.txt"
#define BUZZER_PIN  D2

/* Useful Constants */
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)

/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_) (_time_ % SECS_PER_MIN)  
#define numberOfMinutes(_time_) ((_time_ / SECS_PER_MIN) % SECS_PER_MIN) 
#define numberOfHours(_time_) (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_) ( _time_ / SECS_PER_DAY)  

//declairing prototypes
void configModeCallback (WiFiManager *myWiFiManager);
int8_t getWifiQuality();

// LED Settings
const int offset = 1;
int refresh = 0;
String message = "hello";
int spacer = 1;  // dots between letters
int width = 5 + spacer; // The font width is 5 pixels + spacer
Max72xxPanel matrix = Max72xxPanel(pinCS, numberOfHorizontalDisplays, numberOfVerticalDisplays);

float UtcOffset;  //time zone offsets that correspond with the CityID above (offset from GMT)

// Time 
TimeClient timeClient(UtcOffset);
String lastMinute = "xx";
int displayRefreshCount = 1;
long lastEpoch = 0;
long firstEpoch = 0;
long displayOffEpoch = 0;
boolean displayOn = true;
boolean timeOffsetFetched = false;

// News Client
NewsApiClient newsClient(NEWS_API_KEY, NEWS_SOURCE);
int newsIndex = 0;

// Advice Client
AdviceSlipClient adviceClient;

// Weather Client
OpenWeatherMapClient weatherClient(APIKEY, CityIDs, 1, IS_METRIC);

// OctoPrint Client
OctoPrintClient printerClient(OctoPrintApiKey, OctoPrintServer, OctoPrintPort, OctoAuthUser, OctoAuthPass);
int printerCount = 0;

// Bitcoin Client
BitcoinApiClient bitcoinClient;

ESP8266WebServer server(WEBSERVER_PORT);

const String WEB_ACTIONS =  "<a class='w3-bar-item w3-button' href='/'><i class='fa fa-home'></i> Home</a>"
                            "<a class='w3-bar-item w3-button' href='/configure'><i class='fa fa-cog'></i> Configure</a>"
                            "<a class='w3-bar-item w3-button' href='/pull'><i class='fa fa-cloud-download'></i> Refresh Data</a>"
                            "<a class='w3-bar-item w3-button' href='/display'>%TOGGLEDISPLAY%</a>"
                            "<a class='w3-bar-item w3-button' href='/systemreset' onclick='return confirm(\"Do you want to reset to default weather settings?\")'><i class='fa fa-undo'></i> Reset Settings</a>"
                            "<a class='w3-bar-item w3-button' href='/forgetwifi' onclick='return confirm(\"Do you want to forget to WiFi connection?\")'><i class='fa fa-wifi'></i> Forget WiFi</a>"
                            "<a class='w3-bar-item w3-button' href='https://www.thingiverse.com/thing:2867294' target='_blank'><i class='fa fa-question-circle'></i> About</a>";
                            
const String CHANGE_FORM1 = "<form class='w3-container' action='/locations' method='get'><h2>City ID:</h2>"
                            "<p><label>%CITYNAME1% (<a href='http://openweathermap.org/find' target='_BLANK'><i class='fa fa-search'></i> Search for City ID</a>)</label>"
                            "<input class='w3-input w3-border w3-margin-bottom' type='text' name='city1' value='%CITY1%' onkeypress='return isNumberKey(event)'></p>"
                            "<p><input name='is24hour' class='w3-check w3-margin-top' type='checkbox' %IS_24HOUR_CHECKED%> Use 24 Hour Clock (military time)</p>"
                            "<p><input name='metric' class='w3-check w3-margin-top' type='checkbox' %CHECKED%> Use Metric (Celsius)</p>"
                            "<p><input name='displaynews' class='w3-check w3-margin-top' type='checkbox' %NEWSCHECKED%> Display News Headlines</p>"
                            "<p>Select News Source <select class='w3-option w3-padding' name='newssource'>%NEWSOPTIONS%</select></p>";
                           
const String BITCOIN_FORM = "<p>Select Bitcoin Currency <select class='w3-option w3-padding' name='bitcoincurrency'>%BITCOINOPTIONS%</select></p>";
                            
const String CHANGE_FORM2 = "<p><input name='displayadvice' class='w3-check w3-margin-top' type='checkbox' %ADVICECHECKED%> Display Advice</p>"
                            "<p><label>Marquee Message (up to 60 chars)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='marqueeMsg' value='%MSG%' maxlength='60'></p>"
                            "<p><label>Start Time </label><input name='startTime' type='time' value='%STARTTIME%'></p>"
                            "<p><label>End Time </label><input name='endTime' type='time' value='%ENDTIME%'></p>"
                            "<p>Select Display Brightness <input class='w3-border w3-margin-bottom' name='ledintensity' type='number' min='1' max='15' value='%INTENSITYOPTIONS%'></p>"
                            "<p>Select Display Scroll Speed <select class='w3-option w3-padding' name='scrollspeed'>%SCROLLOPTIONS%</select></p>"
                            "<p>Minutes Between Refresh Data <select class='w3-option w3-padding' name='refresh'>%OPTIONS%</select></p>"
                            "<p>Minutes Between Scrolling Data <input class='w3-border w3-margin-bottom' name='refreshDisplay' type='number' min='1' max='10' value='%REFRESH_DISPLAY%'></p>";

const String CHANGE_FORM3 = "<hr><input name='displayoctoprint' class='w3-check w3-margin-top' type='checkbox' %OCTOCHECKED%> Show OctoPrint Status"
                            "<label>OctoPrint API Key (get from your server)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='octoPrintApiKey' value='%OCTOKEY%' maxlength='60'>"
                            "<label>OctoPrint Address (do not include http://)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='octoPrintAddress' value='%OCTOADDRESS%' maxlength='60'>"
                            "<label>OctoPrint Port</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='octoPrintPort' value='%OCTOPORT%' maxlength='5'  onkeypress='return isNumberKey(event)'>"
                            "<label>OctoPrint User (only needed if you have haproxy or basic auth turned on)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='octoUser' value='%OCTOUSER%' maxlength='30'>"
                            "<label>OctoPrint Password </label><input class='w3-input w3-border w3-margin-bottom' type='password' name='octoPass' value='%OCTOPASS%'><hr>"
                            "<input name='isBasicAuth' class='w3-check w3-margin-top' type='checkbox' %IS_BASICAUTH_CHECKED%> Use Security Credentials for Configuration Changes"
                            "<label>Marquee User ID (for this web interface)</label><input class='w3-input w3-border w3-margin-bottom' type='text' name='userid' value='%USERID%' maxlength='20'>"
                            "<label>Marquee Password </label><input class='w3-input w3-border w3-margin-bottom' type='password' name='stationpassword' value='%STATIONPASSWORD%'>"
                            "<button class='w3-button w3-block w3-green w3-section w3-padding' type='submit'>Save</button></form>"
                            "<script>function isNumberKey(e){var h=e.which?e.which:event.keyCode;return!(h>31&&(h<48||h>57))}</script>";

const String NEWS_OPTIONS = "<option>bbc-news</option>"
                            "<option>cnn</option>"
                            "<option>crypto-coins-news</option>"
                            "<option>engadget</option>"
                            "<option>espn</option>"
                            "<option>fox-news</option>"
                            "<option>fox-sports</option>"
                            "<option>google-news</option>"
                            "<option>hacker-news</option>"
                            "<option>mtv-news</option>"
                            "<option>national-geographic</option>"
                            "<option>newsweek</option>"
                            "<option>nfl-news</option>"
                            "<option>recode</option>"
                            "<option>reddit-r-all</option>"
                            "<option>reuters</option>"
                            "<option>the-new-york-times</option>"
                            "<option>time</option>"
                            "<option>usa-today</option>"
                            "<option>wired</option>";
                            
const String CURRENCY_OPTIONS = "<option value='NONE'>NONE</option>"
                            "<option value='USD'>United States Dollar</option>"
                            "<option value='AUD'>Australian Dollar</option>"
                            "<option value='BRL'>Brazilian Real</option>"
                            "<option value='BTC'>Bitcoin</option>"
                            "<option value='CAD'>Canadian Dollar</option>"
                            "<option value='CNY'>Chinese Yuan</option>"
                            "<option value='EUR'>Euro</option>"
                            "<option value='GBP'>British Pound Sterling</option>"
                            "<option value='XAU'>Gold (troy ounce)</option>";

const int TIMEOUT = 500; // 500 = 1/2 second
int timeoutCount = 0;

// Change the externalLight to the pin you wish to use if other than the Built-in LED
int externalLight = LED_BUILTIN; // LED_BUILTIN is is the built in LED on the Wemos

void setup() {
  Serial.begin(115200);
  SPIFFS.begin();
  //SPIFFS.remove(CONFIG);
  delay(10);
  
  // Initialize digital pin for LED
  pinMode(externalLight, OUTPUT);

  //New Line to clear from start garbage
  Serial.println();

  readCityIds();

  Serial.println("Number os LED Displays: " + String(numberOfHorizontalDisplays));
  // initialize dispaly
  matrix.setIntensity(0); // Use a value between 0 and 15 for brightness

  int maxPos = numberOfHorizontalDisplays * numberOfVerticalDisplays;
  for (int i = 0; i < maxPos; i++) {
    matrix.setRotation(i,3);
    matrix.setPosition(i, maxPos - i - 1, 0);
  }

  Serial.println("matrix created");
  matrix.fillScreen(LOW); // show black
  centerPrint("hello");

  tone(BUZZER_PIN, 415, 500);
  delay(500*1.3);
  tone(BUZZER_PIN, 466, 500);
  delay(500*1.3);
  tone(BUZZER_PIN, 370, 1000);
  delay(1000*1.3);
  noTone(BUZZER_PIN);
  
  for (int inx = 0; inx <= 15; inx++) {
    matrix.setIntensity(inx);
    delay(100);
  }
  for (int inx = 15; inx > 0; inx--) {
    matrix.setIntensity(inx);
    delay(60);
  }
  delay(1000);
  matrix.setIntensity(displayIntensity);
  //noTone(BUZZER_PIN);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  
  // Uncomment for testing wifi manager
  //wifiManager.resetSettings();
  wifiManager.setAPCallback(configModeCallback);
  
  //Custom Station (client) Static IP Configuration - Set custom IP for your Network (IP, Gateway, Subnet mask)
  //wifiManager.setSTAStaticIPConfig(IPAddress(192,168,0,99), IPAddress(192,168,0,1), IPAddress(255,255,255,0));
 
  String hostname(HOSTNAME);
  hostname += String(ESP.getChipId(), HEX);
  if (!wifiManager.autoConnect((const char *)hostname.c_str())) {// new addition
    delay(3000);
    WiFi.disconnect(true);
    ESP.reset();
    delay(5000);
  }

  // print the received signal strength:
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(getWifiQuality());
  Serial.println("%");

  if (ENABLE_OTA) {
    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.setHostname((const char *)hostname.c_str());
    if (OTA_Password != "") {
      ArduinoOTA.setPassword(((const char *)OTA_Password.c_str()));
    }
    ArduinoOTA.begin();
  }
  
  
  if (WEBSERVER_ENABLED) {
    server.on("/", displayWeatherData);
    server.on("/pull", handlePull);
    server.on("/locations", handleLocations);
    server.on("/systemreset", handleSystemReset);
    server.on("/forgetwifi", handleForgetWifi);
    server.on("/configure", handleConfigure);
    server.on("/display", handleDisplay);
    server.onNotFound(redirectHome);
    // Start the server
    server.begin();
    Serial.println("Server started");
    // Print the IP address
    String webAddress = "http://" + WiFi.localIP().toString() + ":" + String(WEBSERVER_PORT) + "/";
    Serial.println("Use this URL : " + webAddress);
    scrollMessage(WiFi.localIP().toString() + "  ");
  } else {
    Serial.println("Web Interface is Disabled");
    scrollMessage("Web Interface is Disabled");
  }
   
  flashLED(1, 500);
}

//************************************************************
// Main Looop
//************************************************************
void loop() {
  //Get some Weather Data to serve
  if((getMinutesFromLastRefresh() >= minutesBetweenDataRefresh) || lastEpoch == 0) {
    getWeatherData();
  }

  checkDisplay(); // this will see if we need to turn it on or off for night mode.

  if (lastMinute != timeClient.getMinutes()) {
    lastMinute = timeClient.getMinutes();
    if (displayOn) {
      matrix.shutdown(false);
    }
    matrix.fillScreen(LOW); // show black
    if (OCTOPRINT_ENABLED) {
      if (displayOn && ((printerClient.isOperational() || printerClient.isPrinting()) || printerCount == 0)) {
        // This should only get called if the printer is actually running or if it has been 5 minutes
        printerClient.getPrinterJobResults();
      }
      printerCount += 1;
      if (printerCount > 5) {
        printerCount = 0;
      }
    }

    displayRefreshCount --;
    // Check to see if we need to Scroll some Data
    if (displayRefreshCount <= 0) {
      displayRefreshCount = minutesBetweenScrolling;
      String temperature = weatherClient.getTempRounded(0);
      String description = weatherClient.getDescription(0);
      description.toUpperCase();
      String msg;
      msg += " " + weatherClient.getCity(0) + "    ";
      msg += temperature + getTempSymbol() + "    ";
      msg += description + "    ";
      msg += "Humidity:" + weatherClient.getHumidityRounded(0) + "%   ";
      msg += "Wind:" + weatherClient.getWindRounded(0) + getSpeedSymbol() + "  ";
      msg += marqueeMessage + " ";

      if (NEWS_ENABLED) {
        msg += "  " + NEWS_SOURCE + ": " + newsClient.getTitle(newsIndex) + "   ";
        newsIndex += 1;
        if (newsIndex > 9) {
          newsIndex = 0;
        }
      }
      if (ADVICE_ENABLED) {
        msg += "  Advice: " + adviceClient.getAdvice() + " ";
      }
      if (OCTOPRINT_ENABLED && printerClient.isPrinting()) {
        msg += "   " + printerClient.getFileName() + " ";
        msg += "(" + printerClient.getProgressCompletion() + "%)   ";
      }
      if (BitcoinCurrencyCode != "NONE" && BitcoinCurrencyCode != "") {
        msg += "    Bitcoin: " + bitcoinClient.getRate() + " " + bitcoinClient.getCode() + " ";
      }
    
      scrollMessage(msg);
    }
  }

  String hourMinutes = timeClient.getAmPmHours() + ":" + timeClient.getMinutes();
  if (IS_24HOUR) {
    hourMinutes = timeClient.getHours() + ":" + timeClient.getMinutes();
  }
  centerPrint(hourMinutes);
  
  if (WEBSERVER_ENABLED) {
    server.handleClient();
  }
  if (ENABLE_OTA) {
    ArduinoOTA.handle();
  }
}

boolean athentication() {
  if (IS_BASIC_AUTH) {
    return server.authenticate(www_username, www_password);
  } 
  return true; // Authentication not required
}

void handlePull() {
  timeOffsetFetched = false;
  getWeatherData(); // this will force a data pull for new weather
  displayWeatherData();
}

void handleLocations() {
  if (!athentication()) {
    return server.requestAuthentication();
  }
  CityIDs[0] = server.arg("city1").toInt();
  NEWS_ENABLED = server.hasArg("displaynews");
  ADVICE_ENABLED = server.hasArg("displayadvice");
  IS_24HOUR = server.hasArg("is24hour");
  IS_METRIC = server.hasArg("metric");
  NEWS_SOURCE = server.arg("newssource");
  BitcoinCurrencyCode = server.arg("bitcoincurrency");
  marqueeMessage = decodeHtmlString(server.arg("marqueeMsg"));
  timeDisplayTurnsOn = decodeHtmlString(server.arg("startTime"));
  timeDisplayTurnsOff = decodeHtmlString(server.arg("endTime"));
  displayIntensity = server.arg("ledintensity").toInt();
  minutesBetweenDataRefresh = server.arg("refresh").toInt();
  minutesBetweenScrolling = server.arg("refreshDisplay").toInt();
  displayScrollSpeed = server.arg("scrollspeed").toInt();
  OCTOPRINT_ENABLED = server.hasArg("displayoctoprint");
  OctoPrintApiKey = server.arg("octoPrintApiKey");
  OctoPrintServer = server.arg("octoPrintAddress");
  OctoPrintPort = server.arg("octoPrintPort").toInt();
  OctoAuthUser = server.arg("octoUser");
  OctoAuthPass = server.arg("octoPass");
  IS_BASIC_AUTH = server.hasArg("isBasicAuth");
  String temp = server.arg("userid");
  temp.toCharArray(www_username, sizeof(temp));
  temp = server.arg("stationpassword");
  temp.toCharArray(www_password, sizeof(temp));
  weatherClient.setMetric(IS_METRIC);
  matrix.fillScreen(LOW); // show black
  writeCityIds();
  getWeatherData(); // this will force a data pull for new weather
  if (OCTOPRINT_ENABLED) {
    printerClient.getPrinterJobResults();
  }
  redirectHome();
}

void handleSystemReset() {
  if (!athentication()) {
    return server.requestAuthentication();
  }
  Serial.println("Reset System Configuration");
  if (SPIFFS.remove(CONFIG)) {
    redirectHome();
    ESP.restart();
  }
}

void handleForgetWifi() {
  if (!athentication()) {
    return server.requestAuthentication();
  }
  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  redirectHome();
  WiFiManager wifiManager;
  wifiManager.resetSettings();
  ESP.restart();
}

void handleConfigure() {
  if (!athentication()) {
    return server.requestAuthentication();
  }
  digitalWrite(externalLight, LOW);
  String html = "";

  server.sendHeader("Cache-Control", "no-cache, no-store");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");

  html = getHeader();
  server.sendContent(html);

  String form = String(CHANGE_FORM1);
  for (int inx = 0; inx < 1; inx++) {
    String cityName = "";
    if (CityIDs[inx] > 0) {
      cityName = weatherClient.getCity(inx) + ", " + weatherClient.getCountry(inx);
    } else {
      cityName = "<i>Available</i>";
    }
    form.replace(String("%CITYNAME" + String(inx +1) + "%"), cityName);
    form.replace(String("%CITY" + String(inx +1) + "%"), String(CityIDs[inx]));
  }
  String is24hourChecked = "";
  if (IS_24HOUR) {
    is24hourChecked = "checked='checked'";
  }
  form.replace("%IS_24HOUR_CHECKED%", is24hourChecked);
  String checked = "";
  if (IS_METRIC) {
    checked = "checked='checked'";
  }
  form.replace("%CHECKED%", checked);
  String isNewsDisplayedChecked = "";
  if (NEWS_ENABLED) {
    isNewsDisplayedChecked = "checked='checked'";
  }
  form.replace("%NEWSCHECKED%", isNewsDisplayedChecked);
  String newsOptions = String(NEWS_OPTIONS);
  newsOptions.replace(">" + String(NEWS_SOURCE) + "<", " selected>" + String(NEWS_SOURCE) + "<");
  form.replace("%NEWSOPTIONS%", newsOptions);
  server.sendContent(form); //Send first Chunk of form

  form = String(BITCOIN_FORM);
  String bitcoinOptions = String(CURRENCY_OPTIONS);
  bitcoinOptions.replace(BitcoinCurrencyCode + "'", BitcoinCurrencyCode + "' selected");
  form.replace("%BITCOINOPTIONS%", bitcoinOptions);
  server.sendContent(form); //Send another Chunk of form

  form = String(CHANGE_FORM2);
  String isAdviceDisplayedChecked = "";
  if (ADVICE_ENABLED) {
    isAdviceDisplayedChecked = "checked='checked'";
  }
  form.replace("%ADVICECHECKED%", isAdviceDisplayedChecked);
  form.replace("%MSG%", marqueeMessage);
  form.replace("%STARTTIME%", timeDisplayTurnsOn);
  form.replace("%ENDTIME%", timeDisplayTurnsOff);
  form.replace("%INTENSITYOPTIONS%", String(displayIntensity));
  String dSpeed = String(displayScrollSpeed);
  String scrollOptions = "<option value='35'>Slow</option><option value='25'>Normal</option><option value='15'>Fast</option>";
  scrollOptions.replace(dSpeed + "'", dSpeed + "' selected" );
  form.replace("%SCROLLOPTIONS%", scrollOptions);
  String minutes = String(minutesBetweenDataRefresh);
  String options = "<option>10</option><option>15</option><option>20</option><option>30</option><option>60</option>";
  options.replace(">" + minutes + "<", " selected>" + minutes + "<");
  form.replace("%OPTIONS%", options);
  form.replace("%REFRESH_DISPLAY%", String(minutesBetweenScrolling));
  
  server.sendContent(form); //Send another chunk of the form

  form = String(CHANGE_FORM3);
  String isOctoPrintDisplayedChecked = "";
  if (OCTOPRINT_ENABLED) {
    isOctoPrintDisplayedChecked = "checked='checked'";
  }
  form.replace("%OCTOCHECKED%", isOctoPrintDisplayedChecked);
  form.replace("%OCTOKEY%", OctoPrintApiKey);
  form.replace("%OCTOADDRESS%", OctoPrintServer);
  form.replace("%OCTOPORT%", String(OctoPrintPort));
  form.replace("%OCTOUSER%", OctoAuthUser);
  form.replace("%OCTOPASS%", OctoAuthPass);
  String isUseSecurityChecked = "";
  if (IS_BASIC_AUTH) {
    isUseSecurityChecked = "checked='checked'";
  }
  form.replace("%IS_BASICAUTH_CHECKED%", isUseSecurityChecked);
  form.replace("%USERID%", String(www_username));
  form.replace("%STATIONPASSWORD%", String(www_password));

  server.sendContent(form); // Send the second chunk of Data

  html = getFooter();
  server.sendContent(html);
  server.sendContent("");
  server.client().stop();
  digitalWrite(externalLight, HIGH);
}

void handleDisplay() {
  if (!athentication()) {
    return server.requestAuthentication();
  }
  enableDisplay(!displayOn);
  String state = "OFF";
  if (displayOn) {
    state = "ON";
  }
  displayMessage("Display is now " + state);
}

//***********************************************************************
void getWeatherData() //client function to send/receive GET request data.
{
  digitalWrite(externalLight, LOW);
  matrix.fillScreen(LOW); // show black
  Serial.println();

  if (displayOn) {
    // only pull the weather data if display is on
    centerPrint(".");
    weatherClient.updateWeather();
    if (weatherClient.getError() != "") {
      do {
        scrollMessage(weatherClient.getError());
      } while (true);
    }
  }

  Serial.println("Updating Time...");
  //Update the Time
  centerPrint("..");
  timeClient.updateTime();
  lastEpoch = timeClient.getCurrentEpoch();
  if (firstEpoch == 0) {
    // This is the first time running record the time
    String curDate = weatherClient.getDt(0);
    curDate = curDate.substring(0, curDate.length() - String(lastEpoch).length());
    curDate = curDate + String(lastEpoch);
    firstEpoch = curDate.toInt();
    Serial.println("firstEpoch is: " + String(firstEpoch));
  }

  if (NEWS_ENABLED && displayOn) {
    centerPrint("...");
    Serial.println("Getting News Data for " + NEWS_SOURCE);
    newsClient.updateNews();
  }

  if (ADVICE_ENABLED && displayOn) {
    centerPrint("...");
    Serial.println("Getting some Advice");
    adviceClient.updateAdvice();
  }

  if (!timeOffsetFetched) {
    // we need to get offsets
    centerPrint("....");
    timeOffsetFetched = true;
    GeoNamesClient geoNames(GEONAMES_USER, weatherClient.getLat(0), weatherClient.getLon(0));
    UtcOffset = geoNames.getTimeOffset();
  }

  if (displayOn) {
    bitcoinClient.updateBitcoinData(BitcoinCurrencyCode);  // does nothing if BitCoinCurrencyCode is "NONE" or empty
  }
  
  matrix.fillScreen(LOW); // show black
  Serial.println("Version: " + String(VERSION));
  Serial.println();
  digitalWrite(externalLight, HIGH);
}

void displayMessage(String message) {
  digitalWrite(externalLight, LOW);

  server.sendHeader("Cache-Control", "no-cache, no-store");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  String html = getHeader();
  server.sendContent(String(html));
  server.sendContent(String(message));
  html = getFooter();
  server.sendContent(String(html));
  server.sendContent("");
  server.client().stop();
  
  digitalWrite(externalLight, HIGH);
}

void redirectHome() {
  // Send them back to the Root Directory
  server.sendHeader("Location", String("/"), true);
  server.sendHeader("Cache-Control", "no-cache, no-store");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.send(302, "text/plain", "");
  server.client().stop();
  delay(1000);
}

String getHeader() {
  String menu = String(WEB_ACTIONS);
  menu.replace("%TOGGLEDISPLAY%", (displayOn) ? "<i class='fa fa-eye-slash'></i> Turn Display OFF" : "<i class='fa fa-eye'></i> Turn Display ON");
  String html = "<!DOCTYPE HTML>";
  html += "<html><head><title>Marquee Scroller</title><link rel='icon' href='data:;base64,='>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<link rel='stylesheet' href='https://www.w3schools.com/w3css/4/w3.css'>";
  html += "<link rel='stylesheet' href='https://www.w3schools.com/lib/w3-theme-blue-grey.css'>";
  html += "<link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css'>";
  html += "</head><body>";
  html += "<nav class='w3-sidebar w3-bar-block w3-card' style='margin-top:88px' id='mySidebar'>";
  html += "<div class='w3-container w3-theme-d2'>";
  html += "<span onclick='closeSidebar()' class='w3-button w3-display-topright w3-large'><i class='fa fa-times'></i></span>";
  html += "<div class='w3-left'><img src='http://openweathermap.org/img/w/" + weatherClient.getIcon(0) + ".png' alt='" + weatherClient.getDescription(0) + "'></div>";
  html += "<div class='w3-padding'>Menu</div></div>";
  html += menu;
  html += "</nav>";
  html += "<header class='w3-top w3-bar w3-theme'><button class='w3-bar-item w3-button w3-xxxlarge w3-hover-theme' onclick='openSidebar()'><i class='fa fa-bars'></i></button><h2 class='w3-bar-item'>Weather Marquee</h2></header>";
  html += "<script>";
  html += "function openSidebar(){document.getElementById('mySidebar').style.display='block'}function closeSidebar(){document.getElementById('mySidebar').style.display='none'}closeSidebar();";
  html += "</script>";
  html += "<br><div class='w3-container w3-large' style='margin-top:88px'>";
  return html;
}

String getFooter() {
  int8_t rssi = getWifiQuality();
  Serial.print("Signal Strength (RSSI): ");
  Serial.print(rssi);
  Serial.println("%");
  String html = "<br><br><br>";
  html += "</div>";
  html += "<footer class='w3-container w3-bottom w3-theme w3-margin-top'>";
  html += "<i class='fa fa-paper-plane-o'></i> Version: " + String(VERSION) + "<br>";
  html += "<i class='fa fa-clock-o'></i> Next Update: " + getTimeTillUpdate() + "<br>";
  html += "<i class='fa fa-rss'></i> Signal Strength: ";
  html += String(rssi) + "%";
  html += "</footer>";
  html += "</body></html>";
  return html;
}

void displayWeatherData() {
  digitalWrite(externalLight, LOW);
  String html = "";

  server.sendHeader("Cache-Control", "no-cache, no-store");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "-1");
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");
  server.sendContent(String(getHeader()));
  
  String temperature = weatherClient.getTemp(0);

  if ((temperature.indexOf(".") != -1) && (temperature.length() >= (temperature.indexOf(".") + 2))) {
    temperature.remove(temperature.indexOf(".") + 2);
  }

  timeClient.setUtcOffset(getTimeOffset());
  String time = timeClient.getAmPmFormattedTime();
  
  Serial.println(weatherClient.getCity(0));
  Serial.println(weatherClient.getCondition(0));
  Serial.println(weatherClient.getDescription(0));
  Serial.println(temperature);
  Serial.println(time);

  html += "<div class='w3-cell-row' style='width:100%'><h2>" + weatherClient.getCity(0) + ", " + weatherClient.getCountry(0) + "</h2></div><div class='w3-cell-row'>";
  html += "<div class='w3-cell w3-left w3-medium' style='width:120px'>";
  html += "<img src='http://openweathermap.org/img/w/" + weatherClient.getIcon(0) + ".png' alt='" + weatherClient.getDescription(0) + "'><br>";
  html += weatherClient.getHumidity(0) + "% Humidity<br>";
  html += weatherClient.getWind(0) + " <span class='w3-tiny'>" + getSpeedSymbol() + "</span> Wind<br>";
  html += "</div>";
  html += "<div class='w3-cell w3-container' style='width:100%'><p>";
  html += weatherClient.getCondition(0) + " (" + weatherClient.getDescription(0) + ")<br>";
  html += temperature + " " + getTempSymbol() + "<br>";
  html += time + "<br>";
  html += "<a href='https://www.google.com/maps/@" + weatherClient.getLat(0) + "," + weatherClient.getLon(0) + ",10000m/data=!3m1!1e3' target='_BLANK'><i class='fa fa-map-marker' style='color:red'></i> Map It!</a><br>";
  html += "</p></div></div><hr>";

  server.sendContent(String(html)); // spit out what we got
  html = ""; // fresh start


  if (OCTOPRINT_ENABLED) {
    html = "<div class='w3-cell-row'>OctoPrint Status: ";
    if (printerClient.isPrinting()) {
      html += printerClient.getState() + " " + printerClient.getFileName() + " (" + printerClient.getProgressCompletion() + "%)";
    } else if (printerClient.isOperational()) {
      html += printerClient.getState();
    } else if (printerClient.getError() != "") {
      html += printerClient.getError();
    } else {
      html += "Not Connected";
    }
    html += "</div><br><hr>";
    server.sendContent(String(html));
    html = "";
  }

  if (BitcoinCurrencyCode != "NONE" && BitcoinCurrencyCode != "") {
    html = "<div class='w3-cell-row'>Bitcoin value: " + bitcoinClient.getRate() + " " + bitcoinClient.getCode() + "</div><br><hr>";
    server.sendContent(String(html));
    html = "";
  }

  if (NEWS_ENABLED) {
    html = "<div class='w3-cell-row' style='width:100%'><h2>News (" + NEWS_SOURCE + ")</h2></div>";
    for (int inx = 0; inx < 10; inx++) {
      html += "<div class='w3-cell-row'><a href='" + newsClient.getUrl(inx) + "' target='_BLANK'>" + newsClient.getTitle(inx) + "</a></div>";
      html += "<div class='w3-cell-row'>" + newsClient.getDescription(inx) + "</div><br>";
      server.sendContent(String(html));
      html = "";
    }
  }

  if (ADVICE_ENABLED) {
    html = "<div class='w3-cell-row' style='width:100%'><h2>Advice Slip</h2></div>";
    html += "<div class='w3-cell-row'>Current Advice: </div>";
    html += "<div class='w3-cell-row'>" + adviceClient.getAdvice() + "</div><br>";
    server.sendContent(String(html));
    html = "";
  }
  
  server.sendContent(String(getFooter()));
  server.sendContent("");
  server.client().stop();
  digitalWrite(externalLight, HIGH);
}

float getTimeOffset() {
  if (timeOffsetFetched) {
    return UtcOffset;
  }
  // we need to get offsets
  timeOffsetFetched = true;

  GeoNamesClient geoNames(GEONAMES_USER, weatherClient.getLat(0), weatherClient.getLon(0));
  UtcOffset = geoNames.getTimeOffset();

  return UtcOffset;
}

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());  
  Serial.println("Wifi Manager");
  Serial.println("Please connect to AP");
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.println("To setup Wifi Configuration");
  scrollMessage("Please Connect to AP: " + String(myWiFiManager->getConfigPortalSSID()));
  centerPrint("wifi");
}

void flashLED(int number, int delayTime) {
  for (int inx = 0; inx < number; inx++) {
      tone(BUZZER_PIN, 440, delayTime);
      delay(delayTime);
      digitalWrite(externalLight, LOW);
      delay(delayTime);
      digitalWrite(externalLight, HIGH);
      delay(delayTime);
  }
  noTone(BUZZER_PIN);
}

String getTempSymbol() {
  String rtnValue = "F";
  if (IS_METRIC) {
    rtnValue = "C";
  }
  return rtnValue;
}

String getSpeedSymbol() {
  String rtnValue = "mph";
  if (IS_METRIC) {
    rtnValue = "kph";
  }
  return rtnValue;
}

// converts the dBm to a range between 0 and 100%
int8_t getWifiQuality() {
  int32_t dbm = WiFi.RSSI();
  if(dbm <= -100) {
      return 0;
  } else if(dbm >= -50) {
      return 100;
  } else {
      return 2 * (dbm + 100);
  }
}

String getTimeTillUpdate() {
  String rtnValue = "";

  long timeToUpdate = (((minutesBetweenDataRefresh * 60) + lastEpoch) - timeClient.getCurrentEpoch());

  int hours = numberOfHours(timeToUpdate);
  int minutes = numberOfMinutes(timeToUpdate);
  int seconds = numberOfSeconds(timeToUpdate);
  
  rtnValue += String(hours) + ":";
  if (minutes < 10) {
    rtnValue += "0";
  }
  rtnValue += String(minutes) + ":";
  if (seconds <10) {
    rtnValue += "0";
  }
  rtnValue += String(seconds);
  
  return rtnValue;
}

int getMinutesFromLastRefresh() {
  int minutes = (timeClient.getCurrentEpoch() - lastEpoch) / 60;
  return minutes;
}

int getMinutesFromLastDisplay() {
  int minutes = (timeClient.getCurrentEpoch() - displayOffEpoch) / 60;
  return minutes;
}

void enableDisplay(boolean enable) {
  displayOn = enable;
  if (enable) {
    if (getMinutesFromLastDisplay() >= minutesBetweenDataRefresh) {
      // The display has been off longer than the minutes between refresh -- need to get fresh data
      lastEpoch = 0; // this should force a data pull of the weather
      displayOffEpoch = 0;  // reset
    }
    matrix.shutdown(false);
    matrix.fillScreen(LOW); // show black
    Serial.println("Display was turned ON: " + timeClient.getFormattedTime());
  } else {
    matrix.shutdown(true);
    Serial.println("Display was turned OFF: " + timeClient.getFormattedTime());
    displayOffEpoch = lastEpoch;
  }
}

// Toggle on and off the display if user defined times
void checkDisplay() {
  if (timeDisplayTurnsOn == "" || timeDisplayTurnsOff == "") {
    return; // nothing to do
  }
  timeClient.setUtcOffset(getTimeOffset());
  String currentTime = timeClient.getHours() + ":" + timeClient.getMinutes(); 

  if (currentTime == timeDisplayTurnsOn && !displayOn) {
    Serial.println("Time to turn display on: " + currentTime);
    flashLED(1, 500);
    enableDisplay(true);
  }

  if (currentTime == timeDisplayTurnsOff && displayOn) {
    Serial.println("Time to turn display off: " + currentTime);
    flashLED(2, 500);
    enableDisplay(false);
  }
}

String writeCityIds() {
  // Save decoded message to SPIFFS file for playback on power up.
  File f = SPIFFS.open(CONFIG, "w");
  if (!f) {
    Serial.println("File open failed!");
  } else {
    Serial.println("Saving settings now...");
    f.println("CityID=" + String(CityIDs[0]));
    f.println("marqueeMessage=" + marqueeMessage);
    f.println("newsSource=" + NEWS_SOURCE);
    f.println("timeDisplayTurnsOn=" + timeDisplayTurnsOn);
    f.println("timeDisplayTurnsOff=" + timeDisplayTurnsOff);
    f.println("ledIntensity=" + String(displayIntensity));
    f.println("scrollSpeed=" + String(displayScrollSpeed));
    f.println("isNews=" + String(NEWS_ENABLED));
    f.println("isAdvice=" + String(ADVICE_ENABLED));
    f.println("is24hour=" + String(IS_24HOUR));
    f.println("isMetric=" + String(IS_METRIC));
    f.println("refreshRate=" + String(minutesBetweenDataRefresh));
    f.println("minutesBetweenScrolling=" + String(minutesBetweenScrolling));
    f.println("isOctoPrint=" + String(OCTOPRINT_ENABLED));
    f.println("octoKey=" + OctoPrintApiKey);
    f.println("octoServer=" + OctoPrintServer);
    f.println("octoPort=" + String(OctoPrintPort));
    f.println("octoUser=" + OctoAuthUser);
    f.println("octoPass=" + OctoAuthPass);
    f.println("www_username=" + String(www_username));
    f.println("www_password=" + String(www_password));
    f.println("IS_BASIC_AUTH=" + String(IS_BASIC_AUTH));
    f.println("BitcoinCurrencyCode=" + BitcoinCurrencyCode);
  }
  f.close();
  readCityIds();
  timeOffsetFetched = false;
  weatherClient.updateCityIdList(CityIDs, 1);
  String cityIds = weatherClient.getMyCityIDs();
  return cityIds;
}

void readCityIds() {
  if (SPIFFS.exists(CONFIG) == false) {
    Serial.println("Settings File does not yet exists.");
    writeCityIds();
    return;
  }
  File fr = SPIFFS.open(CONFIG, "r");
  String line;
  while(fr.available()) {
    line = fr.readStringUntil('\n');
    if (line.indexOf("CityID=") >= 0) {
      CityIDs[0] = line.substring(line.lastIndexOf("CityID=") + 7).toInt();
      Serial.println("CityID: " + String(CityIDs[0]));
    }
    if (line.indexOf("newsSource=") >= 0) {
      NEWS_SOURCE = line.substring(line.lastIndexOf("newsSource=") + 11);
      NEWS_SOURCE.trim();
      Serial.println("newsSource=" + NEWS_SOURCE);
    }
    if (line.indexOf("isNews=") >= 0) {
      NEWS_ENABLED = line.substring(line.lastIndexOf("isNews=") + 7).toInt();
      Serial.println("NEWS_ENABLED=" + String(NEWS_ENABLED));
    }
    if (line.indexOf("is24hour=") >= 0) {
      IS_24HOUR = line.substring(line.lastIndexOf("is24hour=") + 9).toInt();
      Serial.println("IS_24HOUR=" + String(IS_24HOUR));
    }
    if (line.indexOf("isMetric=") >= 0) {
      IS_METRIC = line.substring(line.lastIndexOf("isMetric=") + 9).toInt();
      Serial.println("IS_METRIC=" + String(IS_METRIC));
    }
    if (line.indexOf("refreshRate=") >= 0) {
      minutesBetweenDataRefresh = line.substring(line.lastIndexOf("refreshRate=") + 12).toInt();
      Serial.println("minutesBetweenDataRefresh=" + String(minutesBetweenDataRefresh));
    }
    if (line.indexOf("minutesBetweenScrolling=") >= 0) {
      displayRefreshCount = 1;
      minutesBetweenScrolling = line.substring(line.lastIndexOf("minutesBetweenScrolling=") + 24).toInt();
      Serial.println("minutesBetweenScrolling=" + String(minutesBetweenScrolling));
    }
    if (line.indexOf("marqueeMessage=") >= 0) {
      marqueeMessage = line.substring(line.lastIndexOf("marqueeMessage=") + 15);
      marqueeMessage.trim();
      Serial.println("marqueeMessage=" + marqueeMessage);
    }
    if (line.indexOf("timeDisplayTurnsOn=") >= 0) {
      timeDisplayTurnsOn = line.substring(line.lastIndexOf("timeDisplayTurnsOn=") + 19);
      timeDisplayTurnsOn.trim();
      Serial.println("timeDisplayTurnsOn=" + timeDisplayTurnsOn);
    }
    if (line.indexOf("timeDisplayTurnsOff=") >= 0) {
      timeDisplayTurnsOff = line.substring(line.lastIndexOf("timeDisplayTurnsOff=") + 20);
      timeDisplayTurnsOff.trim();
      Serial.println("timeDisplayTurnsOff=" + timeDisplayTurnsOff);
    }
    if (line.indexOf("ledIntensity=") >= 0) {
      displayIntensity = line.substring(line.lastIndexOf("ledIntensity=") + 13).toInt();
      Serial.println("displayIntensity=" + String(displayIntensity));
    }
    if (line.indexOf("scrollSpeed=") >= 0) {
      displayScrollSpeed = line.substring(line.lastIndexOf("scrollSpeed=") + 12).toInt();
      Serial.println("displayScrollSpeed=" + String(displayScrollSpeed));
    }
    if (line.indexOf("isOctoPrint=") >= 0) {
      OCTOPRINT_ENABLED = line.substring(line.lastIndexOf("isOctoPrint=") + 12).toInt();
      Serial.println("OCTOPRINT_ENABLED=" + String(OCTOPRINT_ENABLED));
    }
    if (line.indexOf("octoKey=") >= 0) {
      OctoPrintApiKey = line.substring(line.lastIndexOf("octoKey=") + 8);
      OctoPrintApiKey.trim();
      Serial.println("OctoPrintApiKey=" + OctoPrintApiKey);
    }
    if (line.indexOf("octoServer=") >= 0) {
      OctoPrintServer = line.substring(line.lastIndexOf("octoServer=") + 11);
      OctoPrintServer.trim();
      Serial.println("OctoPrintServer=" + OctoPrintServer);
    }
    if (line.indexOf("octoPort=") >= 0) {
      OctoPrintPort = line.substring(line.lastIndexOf("octoPort=") + 9).toInt();
      Serial.println("OctoPrintPort=" + String(OctoPrintPort));
    }
    if (line.indexOf("octoUser=") >= 0) {
      OctoAuthUser = line.substring(line.lastIndexOf("octoUser=") + 9);
      OctoAuthUser.trim();
      Serial.println("OctoAuthUser=" + OctoAuthUser);
    }
    if (line.indexOf("octoPass=") >= 0) {
      OctoAuthPass = line.substring(line.lastIndexOf("octoPass=") + 9);
      OctoAuthPass.trim();
      Serial.println("OctoAuthPass=" + OctoAuthPass);
    }
    if (line.indexOf("www_username=") >= 0) {
      String temp = line.substring(line.lastIndexOf("www_username=") + 13);
      temp.trim();
      temp.toCharArray(www_username, sizeof(temp));
      Serial.println("www_username=" + String(www_username));
    }
    if (line.indexOf("www_password=") >= 0) {
      String temp = line.substring(line.lastIndexOf("www_password=") + 13);
      temp.trim();
      temp.toCharArray(www_password, sizeof(temp));
      Serial.println("www_password=" + String(www_password));
    }
    if (line.indexOf("IS_BASIC_AUTH=") >= 0) {
      IS_BASIC_AUTH = line.substring(line.lastIndexOf("IS_BASIC_AUTH=") + 14).toInt();
      Serial.println("IS_BASIC_AUTH=" + String(IS_BASIC_AUTH));
    }
    if (line.indexOf("BitcoinCurrencyCode=") >= 0) {
      BitcoinCurrencyCode = line.substring(line.lastIndexOf("BitcoinCurrencyCode=") + 20);
      BitcoinCurrencyCode.trim();
      Serial.println("BitcoinCurrencyCode=" + BitcoinCurrencyCode);
    }
  }
  fr.close();
  matrix.setIntensity(displayIntensity);
  newsClient.updateNewsSource(NEWS_SOURCE);
  weatherClient.setMetric(IS_METRIC);
  weatherClient.updateCityIdList(CityIDs, 1);
  printerClient.updateOctoPrintClient(OctoPrintApiKey, OctoPrintServer, OctoPrintPort, OctoAuthUser, OctoAuthPass);
}

void scrollMessage(String msg) {
  msg += " "; // add a space at the end
  for ( int i = 0 ; i < width * msg.length() + matrix.width() - 1 - spacer; i++ ) {
    if (WEBSERVER_ENABLED) {
      server.handleClient();
    }
    if (ENABLE_OTA) {
      ArduinoOTA.handle();
    }
    if (refresh==1) i=0;
    refresh=0;
    matrix.fillScreen(LOW);

    int letter = i / width;
    int x = (matrix.width() - 1) - i % width;
    int y = (matrix.height() - 8) / 2; // center the text vertically
 
    while ( x + width - spacer >= 0 && letter >= 0 ) {
      if ( letter < msg.length() ) {
        matrix.drawChar(x, y, msg[letter], HIGH, LOW, 1);
      }

      letter--;
      x -= width;
    }

    matrix.write(); // Send bitmap to display
    delay(displayScrollSpeed);
  }
  matrix.setCursor(0,0);
}

void centerPrint(String msg) {
  int x = (matrix.width() - (msg.length() * width)) / 2;
  matrix.setCursor(x, 0);
  matrix.print(msg);
  matrix.write();
}

String decodeHtmlString(String msg) {
  String decodedMsg = msg;
  // Restore special characters that are misformed to %char by the client browser
  decodedMsg.replace("+", " ");      
  decodedMsg.replace("%21", "!");  
  decodedMsg.replace("%22", "");  
  decodedMsg.replace("%23", "#");
  decodedMsg.replace("%24", "$");
  decodedMsg.replace("%25", "%");  
  decodedMsg.replace("%26", "&");
  decodedMsg.replace("%27", "'");  
  decodedMsg.replace("%28", "(");
  decodedMsg.replace("%29", ")");
  decodedMsg.replace("%2A", "*");
  decodedMsg.replace("%2B", "+");  
  decodedMsg.replace("%2C", ",");  
  decodedMsg.replace("%2F", "/");   
  decodedMsg.replace("%3A", ":");    
  decodedMsg.replace("%3B", ";");  
  decodedMsg.replace("%3C", "<");  
  decodedMsg.replace("%3D", "=");  
  decodedMsg.replace("%3E", ">");
  decodedMsg.replace("%3F", "?");  
  decodedMsg.replace("%40", "@");
  decodedMsg.toUpperCase();
  decodedMsg.trim();
  return decodedMsg;
}
