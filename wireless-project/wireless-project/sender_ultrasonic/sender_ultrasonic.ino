#include <esp_now.h>
#include <WiFi.h>

#define TRIG_PIN 14
#define ECHO_PIN 12

uint8_t receiverMAC[] = {0xE8, 0x6B, 0xEA, 0xD0, 0xDB, 0xF0}; // Receiver's MAC

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Reset WiFi and read MAC
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  Serial.print("Sender MAC: ");
  Serial.println(WiFi.macAddress());

  // Set WiFi Channel
  WiFi.begin("dummy", "dummy");
  delay(100);
  WiFi.disconnect();
  WiFi.setChannel(1);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW Init Failed");
    while (1);
  }

  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 1;
  peerInfo.ifidx = WIFI_IF_STA;  // Critical fix
  peerInfo.encrypt = false;

  esp_err_t addStatus = esp_now_add_peer(&peerInfo);
  if (addStatus != ESP_OK) {
    Serial.print("Failed to Add Peer. Error: ");
    Serial.println(addStatus);
    while (1);
  }

  Serial.println("Sender Ready!");
}

void loop() {
  // Ultrasonic sensor logic
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;

  if (distance < 50) {
    esp_err_t result = esp_now_send(receiverMAC, (uint8_t *)&distance, sizeof(distance));
    if (result == ESP_OK) {
      Serial.print("Sent Distance: ");
      Serial.println(distance);
    } else {
      Serial.println("Failed to Send");
    }
  }
  delay(500);
}
