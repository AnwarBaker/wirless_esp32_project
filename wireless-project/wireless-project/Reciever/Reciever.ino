#include <esp_now.h>
#include <WiFi.h>

#define ULTRASONIC_LED 32  // LED for ultrasonic
#define LDR_LED 33         // LED for LDR
#define IR_LED 27          // LED for IR sensor

// MAC addresses of all senders
uint8_t ultrasonicMAC[] = {0xA0, 0xA3, 0xB3, 0xAB, 0x7A, 0xF8}; // Ultrasonic sender
uint8_t ldrMAC[] = {0xD4, 0x8A, 0xFC, 0xC7, 0xB8, 0xDC};        // LDR sender
uint8_t irMAC[] = {0xD4, 0x8A, 0xFC, 0xCF, 0x85, 0x4C};          // IR sender

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  const uint8_t *mac = info->src_addr;
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  // Check which device sent data
  if (memcmp(mac, ultrasonicMAC, 6) == 0) {
    // Data from ultrasonic sensor
    if (len == sizeof(int)) {
      int distance = *((int*)data);
      Serial.print("Received from Ultrasonic (");
      Serial.print(macStr);
      Serial.print(") - Distance: ");
      Serial.println(distance);
      digitalWrite(ULTRASONIC_LED, HIGH);
      delay(200);
      digitalWrite(ULTRASONIC_LED, LOW);
    }
  } 
  else if (memcmp(mac, ldrMAC, 6) == 0) {
    // Data from LDR
    if (len == sizeof(int)) {
      int ldrValue = *((int*)data);
      Serial.print("Received from LDR (");
      Serial.print(macStr);
      Serial.print(") - Light Level: ");
      Serial.println(ldrValue);
      digitalWrite(LDR_LED, ldrValue < 1000 ? HIGH : LOW); // Turn on LED in dark
    }
  }
  else if (memcmp(mac, irMAC, 6) == 0) {
    // Data from IR sensor
    if (len == sizeof(int)) {
      int irValue = *((int*)data);
      Serial.print("Received from IR (");
      Serial.print(macStr);
      Serial.print(") - Detection: ");
      Serial.println(irValue == 0 ? "Object Detected" : "No Object");
      digitalWrite(IR_LED, irValue == 0 ? HIGH : LOW); // Turn on LED when object detected
    }
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize LEDs
  pinMode(ULTRASONIC_LED, OUTPUT);
  pinMode(LDR_LED, OUTPUT);
  pinMode(IR_LED, OUTPUT);
  digitalWrite(ULTRASONIC_LED, LOW);
  digitalWrite(LDR_LED, LOW);
  digitalWrite(IR_LED, LOW);

  // WiFi setup
  WiFi.disconnect(true);
  delay(1000);
  WiFi.mode(WIFI_STA);
  Serial.print("Receiver MAC: ");
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

  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Receiver Ready for 3 Devices!");
}

void loop() {}
