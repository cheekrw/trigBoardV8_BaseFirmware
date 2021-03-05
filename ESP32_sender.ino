/*
   Using trigBoard with MCP9808 temperature sensor and sending on time interval to gateway
   over ESP-NOW.
*/

#include "includes.h"
#include <EEPROM.h>
#include <esp_now.h>
#define EEPROM_SIZE 5

// RECEIVER (other) MAC Address (ESP8266 "E")
// uint8_t broadcastAddress[] = {0x5C, 0xCF, 0x7F, 0xC6, 0xA1, 0x0B};
// RECEIVER (other) MAC Address (huzzah32 "C")
uint8_t broadcastAddress[] = {0x24, 0x0A, 0xC4, 0x00, 0x29, 0x18};
//uint8_t broadcastAddress[] = {0x26, 0x0A, 0xC4, 0x00, 0x29, 0x18};

const char fwVersion[] = "12/27/20";

int fail = 0;
int success = 0;

char temperatureString[5];//where the temperature in F/C is stored

boolean getMCP9808();

// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  fail = fail + ESP_NOW_SEND_SUCCESS;
  success = success + ESP_NOW_SEND_FAIL;
  Serial.println(fail);
  Serial.println(success);
}


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
    
 // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

//all of the ESPNOW stuff

// Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
 //esp_now_register_send_cb(OnDataSent);

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
 
// Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
//  if (esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 0, NULL, 0) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
//  char a[32];
  int b;
  String c;
  String d;
//  bool e;
} struct_message;

// Create a struct_message called myData 
struct_message myData;


  // Set values to send
//  strcpy(myData.a, "THIS IS A CHAR");
  myData.b = 0;
  myData.c = batCharString;
  myData.d = temperatureString;
//  myData.e = false;

  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }

  }
//delay(100);
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
