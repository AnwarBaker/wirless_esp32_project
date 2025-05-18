#include <esp_now.h>
#include <WiFi.h>

#define IR_PIN 14  // GPIO14 connected to IR sensor output

// Receiver MAC
uint8_t receiverMAC[] = {0xE8, 0x6B, 0xEA, 0xD0, 0xDB, 0xF0};

void setup() {
  Serial.begin(115200);
  pinMode(IR_PIN, INPUT);

  // WiFi setup
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  Serial.print("IR Sender MAC: ");
  Serial.println(WiFi.macAddress());

  // Set channel
  WiFi.begin("dummy", "dummy");
  delay(100);
  WiFi.disconnect();
  WiFi.setChannel(1);

  // ESP-NOW init
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    while (1);
  }

  // Register receiver as peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 1;
  peerInfo.ifidx = WIFI_IF_STA;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to Add Peer");
    while (1);
  }

  Serial.println("IR Sender Ready!");
}

void loop() {
  int irValue = digitalRead(IR_PIN); // Read IR sensor (0 = object detected, 1 = no object)
  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&irValue, sizeof(irValue));
  
  if (result == ESP_OK) {
    Serial.print("Sent IR Value: ");
    Serial.println(irValue);
  } else {
    Serial.println("Failed to Send");
  }

  delay(200); // Send every 200ms for responsive detection
}
