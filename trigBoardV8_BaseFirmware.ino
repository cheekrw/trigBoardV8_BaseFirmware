/*
   Using trigBoard with MCP9808 temperature sensor and sending on time interval to gateway
   over ESP-NOW.
*/

#include "includes.h"
#include <EEPROM.h>
#define EEPROM_SIZE 5

const char fwVersion[] = "12/27/20";

char temperatureString[5];//where the temperature in F/C is stored

boolean getMCP9808();
void espnow();

void setup() {
  EEPROM.begin(EEPROM_SIZE);
  pinMode(ESPlatchPin, OUTPUT);
  digitalWrite(ESPlatchPin, HIGH);
  pinMode(LEDpin, OUTPUT);
  Serial.begin(115200);
  checkWakeupPins();
  loadConfiguration(filename, config);
  rtcInit(config.timerCountDown, false);
  Serial.println(getBattery(), 2);

  if (getMCP9808()) {
    Serial.println("Hello Roger");
    Serial.println(temperatureString);
    espnow();

  }
  
  killPower();
  waitForButton();
  initBluetooth();
}

void loop() {
  if (!OTAsetup)
    serviceBluetooth();
  else
    checkOTA();
}
