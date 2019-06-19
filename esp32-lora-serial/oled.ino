//----------------------------------------//
//  oled.ino
//
//  created 23/06/2018
//  by Luiz Henrique Cassettari
//----------------------------------------//

#include <Wire.h>
#include "SSD1306.h"

#define OLED_RUNEVERY 500
#define OLED_TIMEOUT 4

#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_ADDR 0x3C
#define OLED_RST 16

SSD1306 display(OLED_ADDR, OLED_SDA, OLED_SCL);// i2c ADDR & SDA, SCL on wemos

static String oled_status_string;
static int oled_status_timeout;
static String oled_strings;

void oled_display(String s) {
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64, 0, s);
  display.display();
}

void oled_setup() {
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, HIGH);
  
  display.init();
  display.resetDisplay();
  display.displayOn();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);

  display.setContrast(255);

  oled_display("...");
}

boolean oled_loop() {
  if (oled_runEvery(OLED_RUNEVERY)) {
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    String str = "";
    str += timeOn(0);
    str += "\n";
    str += oled_strings;
    str += "\n";
    str += oled_status_string;
    display.drawString(64,0,str);
    display.display();

    if (--oled_status_timeout == 0)
    {
      oled_status_timeout = 0;
      oled_status_string = "";
    }
    
    return true;
  }
  return false;
}

void oled_status(String status) {
  oled_status_string = status;
  oled_status_timeout = OLED_TIMEOUT;
}

void oled_string(String status) {
  oled_strings = status;
}

boolean oled_runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}

String timeOn(unsigned long diff)
{
    String str = "";
    unsigned long t = millis() / 1000;
    int s = t % 60;
    int m = (t / 60) % 60;
    int h = (t / 3600);
    str += h;
    str += ":";
    if (m < 10)
        str += "0";
    str += m;
    str += ":";
    if (s < 10)
        str += "0";
    str += s;
    return str;
}
