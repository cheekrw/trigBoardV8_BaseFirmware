// ESP-NOW https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html#

void espnow() {

// RECEIVER (other) MAC Address (ESP8266 "E")
// uint8_t broadcastAddress[] = {0x5C, 0xCF, 0x7F, 0xC6, 0xA1, 0x0B};

// RECEIVER (other) MAC Address (huzzah32 "C")
uint8_t broadcastAddress[] = {0x24, 0x0A, 0xC4, 0x00, 0x29, 0x18};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  char a[32];
  int b;
  float c;
  String d;
  bool e;
} struct_message;

// Create a struct_message called myData 
struct_message myData;

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

  // Set values to send
  strcpy(myData.a, "THIS IS A CHAR");
  myData.b = random(1,20);
  myData.c = 1.2;
  myData.d = temperatureString;
  myData.e = false;


  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));

  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}
