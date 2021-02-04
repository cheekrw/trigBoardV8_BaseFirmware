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
// Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

// Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }

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
