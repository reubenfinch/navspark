#include <Arduino.h>
#include "GNSS.h"
#include "sti_gnss_lib.h"
#include <Nokia5110.h>
/*
  NOTE: Configure GPIO pins. Note that a 1KOhm resistor is needed between
        LED pins of NavSpark module and Nokia5110.
*/

void setup() {
  // put your setup code here, to run once:
  GnssConf.init(); /* do initialization for GNSS */

  pinMode(PCD8544_LED_PIN, OUTPUT);
  pinMode(PCD8544_SCE_PIN, OUTPUT);
  pinMode(PCD8544_DC_PIN, OUTPUT);
  pinMode(PCD8544_SCLK_PIN, OUTPUT);
  pinMode(PCD8544_SDIN_PIN, OUTPUT);
  pinMode(PCD8544_RES_PIN, OUTPUT);

  digitalWrite(PCD8544_LED_PIN, HIGH);
  digitalWrite(PCD8544_SCE_PIN, HIGH);
  digitalWrite(PCD8544_DC_PIN, HIGH);
  digitalWrite(PCD8544_SCLK_PIN, LOW);
  digitalWrite(PCD8544_SDIN_PIN, LOW);
  digitalWrite(PCD8544_RES_PIN, HIGH);

  // init the LCD controller
  pcd8544Init();
}

void loop() {
  // put your main code here, to run repeatedly:
  static unsigned char fractal_on = 0;
  static uint32_t cntr = 0;

  // repeat "plot fractal and clean"
  if (cntr == 0) {
    cntr = 5;
    if (!fractal_on) {
      pcd8544FractalPlot();
      fractal_on = 1;
    }
    else {
      pcd8544CleanScreen();
      fractal_on = 0;
    }
  }
  else {
    delay(100);
    cntr --;
  }
}
