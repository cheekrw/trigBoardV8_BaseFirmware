/*
   Using trigBoard with MCP9808 temperature sensor and sending on time interval to gateway
   over ESP-NOW.
*/

#include "includes.h"
#include <EEPROM.h>
#define EEPROM_SIZE 5

const char fwVersion[] = "12/27/20";

// ESPNOW STUFF
uint8_t broadcastAddress[] = {0x24, 0x0A, 0xC4, 0x00, 0x29, 0x18};// MAC ADDRESS OF huzzah32 "C"

char temperatureString[5];//where the temperature in F/C is stored

boolean getMCP9808();

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
    //all of the ESPNOW stuff
    esp_now_init();
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_add_peer(gatewayMac, ESP_NOW_ROLE_SLAVE, 1, key, 16);
    esp_now_set_peer_key(gatewayMac, key, 16);
    esp_now_register_send_cb([](uint8_t* mac, uint8_t sendStatus) {//this is the function that is called to send data
      Serial.printf("send_cb, send done, status = %i\n", sendStatus);
      digitalWrite(DONEpin, HIGH);//all good, kill power
    });

    char payload[100];//limit is liek 200bytes, but we don't need anything close to that
    //below you can see how a message is formated. The title is before the $, the message is after.  We'll use that to parse at the Cell modem
    if (batteryVoltage >= batteryThreshold) {
      if (digitalRead(stillONpin) && !externalWake && stillOnMode)
        sprintf(payload, "%s STILL %s$Battery=%sV\0", trigBoardName, trigBoardMessage, batteryBuf);
      else
        sprintf(payload, "%s %s$Battery=%sV\0", trigBoardName, trigBoardMessage, batteryBuf);
    } else {
      sprintf(payload, "BATTERY LOW!$%s Battery=%sV\0", trigBoardName, batteryBuf);
    }

    uint8_t bs[strlen(payload)];
    memcpy(bs, &payload, strlen(payload));
    esp_now_send(gatewayMac, bs, strlen(payload));
    delay(1000);
  }

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
