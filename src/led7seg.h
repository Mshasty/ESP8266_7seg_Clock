#include <LedControl.h>           // https://github.com/wayoda/LedControl/releases  MAX7219 7 segment driver


/*For ESP-01 Don't Change GPIO Values
// GPIO0= DIN pin, GPIO1 = CLK pin, GPIO 2 = LOAD pin
LedControl lc = LedControl(0, 1, 2, 1)
//Reset Button Connected On GPIO3 and Ground
int inPin = 3;
*/

//For ESP-07 Tested GPIO Values
// GPIO 13 = DIN pin, GPIO 12 = CLK pin, GPIO 14 = LOAD pin. 
//For ESP-12E Tested GPIO Values
// GPIO 13 = DIN pin, GPIO 14 = CLK pin, GPIO 15 = LOAD pin. 
LedControl lc = LedControl(13, 14, 15, 1);

void digitalClockDisplay() {
  tmElements_t tm;
  char *dayOfWeek;
  breakTime(now(), tm);
  lc.clearDisplay(0);
  int wd = weekday(CE.toLocal(utc, &tcr)) - 1;
  if (!wd) wd = 7;
  lc.setDigit(0, 7, wd, false);
  lc.setDigit(0, 5, int(hour(CE.toLocal(utc, &tcr)) / 10), false);
  lc.setDigit(0, 4, (hour(CE.toLocal(utc, &tcr)) % 10), true);
  lc.setDigit(0, 3, (minute() / 10), false);
  lc.setDigit(0, 2, (minute() % 10), true);
  lc.setDigit(0, 1, int(second() / 10), false);
  lc.setDigit(0, 0, (second() % 10), false);
}

//To Display <Setup> if not connected to AP
void configModeCallback (WiFiManager *myWiFiManager) {
  lc.clearDisplay(0);
  lc.setRow(0, 7, 0x5b);
  lc.setChar(0, 6, 'E', false);
  lc.setRow(0, 5, 0x0f);
  lc.setRow(0, 4, 0x03e);
  lc.setChar(0, 3, 'P', false);
  delay(2000);
}

//Display <StArt>
void ShowStart() {
  lc.setRow(0, 6, 0x5b);
  lc.setRow(0, 5, 0x0f);
  lc.setRow(0, 4, 0x77);
  lc.setRow(0, 3, 0x05);
  lc.setRow(0, 2, 0x0f);
  delay(500);
}

//Display <Connect> Once Connected to AP
void ShowConnect() {
  lc.setRow(0, 7, 0x4e);
  lc.setRow(0, 6, 0x1d);
  lc.setRow(0, 5, 0x15);
  lc.setRow(0, 4, 0x15);
  lc.setRow(0, 3, 0x6f);
  lc.setRow(0, 2, 0x0d);
  lc.setRow(0, 1, 0x0f);
  delay(3000);  
}