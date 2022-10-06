/*NTP ESP-8266 Clock By Sebouh
Please Use the Provided Librarerires
For Compiling USE Arduino IDE 1.6.5
*/
#include <Arduino.h>
#include <Wire.h>
#include <ESP8266WiFi.h>          // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
#include <DNSServer.h>            // https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer
#include <ESP8266WebServer.h>     // https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WebServer
#include <WiFiManager.h>          // https://github.com/tzapu/WiFiManager
#include <Time.h>                 // https://github.com/PaulStoffregen/Time
#include <Timezone.h>             // https://github.com/JChristensen/Timezone

// the GPIO number For The "Reset" Push Button Switch Pin For ESp-07 Tested
int inPin = 5;
//Edit These Lines According To Your Timezone and Daylight Saving Time
//TimeZone Settings Details https://github.com/JChristensen/Timezone
//TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     //Central European Time (Frankfurt, Paris)
//TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       //Central European Time (Frankfurt, Paris)
//Timezone CE(CEST, CET);
TimeChangeRule msk = {"MSK", Last, Sun, Mar, 1, 180};
Timezone CE(msk);
//Pointer To The Time Change Rule, Use to Get The TZ Abbrev
TimeChangeRule *tcr;
time_t utc ;


//NTP Server http://tf.nist.gov/tf-cgi/servers.cgi
static const char ntpServerName[] = "time.nist.gov";
WiFiUDP Udp;
// Local Port to Listen For UDP Packets
uint16_t localPort;

#include "NTPcode.h"
#include "led7seg.h"

// Setup Details
void setup() {

  // Initialize LCD
  lc.shutdown(0, false);
  // Set the brightness to a low value
  lc.setIntensity(0, 1);
  // and clear the display
  lc.clearDisplay(0);

  ShowStart();

  // Switch Type
  pinMode(inPin, INPUT_PULLUP);

  Serial.begin(57600);

  //WiFiManager
  //Local intialization.
  WiFiManager wifiManager;
  //AP Configuration
  wifiManager.setAPCallback(configModeCallback);
  //Exit After Config Instead of connecting
  wifiManager.setBreakAfterConfig(true);

  //Reset Settings - If Button Pressed
  if (digitalRead(inPin) == LOW) {
    //Display <Reset>
    lc.clearDisplay(0);
    lc.setRow(0, 6, 0x05);
    lc.setRow(0, 5, 0x4f);
    lc.setRow(0, 4, 0x5b);
    lc.setRow(0, 5, 0x4f);
    lc.setRow(0, 2, 0x0f);
    delay(5000);
    wifiManager.resetSettings();
    //ESP.reset();
    ESP.restart();
  }
  /*
  Tries to connect to last known settings
  if it does not connect it starts an access point with the specified name
  here  "AutoConnectAP" without password (uncomment below line and comment
  next line if password required)
  and goes into a blocking loop awaiting configuration
  */
  //If You Require Password For Your NTP Clock
  // if (!wifiManager.autoConnect("NTP Clock", "password"))
  //If You Dont Require Password For Your NTP Clock
  if (!wifiManager.autoConnect("NTP Clock")) {
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  //Display <Connect> Once Connected to AP
  // lc.setRow(0, 7, 0x4e);
  // lc.setRow(0, 6, 0x1d);
  // lc.setRow(0, 5, 0x15);
  // lc.setRow(0, 4, 0x15);
  // lc.setRow(0, 3, 0x6f);
  // lc.setRow(0, 2, 0x0d);
  // lc.setRow(0, 1, 0x0f);
  // delay(3000);
  ShowConnect();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Seed Random With vVlues Unique To This Device
  uint8_t macAddr[6];
  WiFi.macAddress(macAddr);
  //uint32_t seed1 =
  unsigned long seed1 =
    (macAddr[5] << 24) | (macAddr[4] << 16) |
    (macAddr[3] << 8)  | macAddr[2];
  //randomSeed(WiFi.localIP() + seed1 + micros());
  randomSeed(seed1 + micros());
  localPort = random(1024, 65535);
  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  //Set Sync Intervals
  setSyncInterval(5 * 60);
}

void loop() {
  // when the digital clock was displayed
  static time_t prevDisplay = 0;
  timeStatus_t ts = timeStatus();
  utc = now();
  switch (ts) {
    case timeNeedsSync:
    case timeSet:
      //update the display only if time has changed
      if (now() != prevDisplay) {
        prevDisplay = now();
        digitalClockDisplay();
        tmElements_t tm;
        breakTime(now(), tm);
        //If Time Needs Sync Display a "-" On Last Digit
        if (ts == timeNeedsSync) {
          lc.setChar(0, 1, '-', false);
        }
        //else {
        //lc.setDigit(0, 2, (second() % 10), false);
        //  }
      }
      break;
    case timeNotSet:
      //Display <No Sync> If Time Not Displayed
      lc.clearDisplay(0);
      lc.setRow(0, 7, 0x15);
      lc.setRow(0, 6, 0x1d);
      lc.setRow(0, 4, 0x5b);
      lc.setRow(0, 3, 0x3b);
      lc.setRow(0, 2, 0x15);
      lc.setRow(0, 1, 0x0d);
      now();
      delay(3000);
      ESP.restart();
  }
}
