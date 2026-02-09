#include <WiFi.h>
#include <esp_now.h>

// -------------------- CONFIG --------------------
#define GROUP_ID    1        // Same for all 5 in a set
#define MODULE_ID   1        // UNIQUE: 1–5

// -------------------- PINS ---------------------
#define BUTTON_PIN  4
#define LED_PIN     5

// -------------------- MESSAGE ------------------
typedef struct {
  uint8_t group_id;
  uint8_t module_id;
  uint8_t pressed;
} ButtonMessage;

ButtonMessage msg;

// -------------------- MASTER MAC ---------------
// ⚠️ Replace with your MASTER node MAC
uint8_t masterMac[] = { 0x24, 0x6F, 0x28, 0xAA, 0xBB, 0xCC };

// -------------------- SEND CALLBACK (C3 FIX) ---
void onSend(const wifi_tx_info_t* info,
            esp_now_send_status_t status)
{
  if (status == ESP_NOW_SEND_SUCCESS) {
    digitalWrite(LED_PIN, HIGH);
    delay(20);
    digitalWrite(LED_PIN, LOW);
  }
}

// -------------------- SETUP --------------------
void setup() {
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  Serial.begin(115200);

  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_send_cb(onSend);

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, masterMac, 6);
  peer.channel = 0;
  peer.encrypt = false;

  if (esp_now_add_peer(&peer) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.printf("Slave ready | Group %d | Module %d\n",
                GROUP_ID, MODULE_ID);
}

// -------------------- LOOP ---------------------
void loop() {
  static bool lastState = HIGH;
  bool currentState = digitalRead(BUTTON_PIN);

  // Button press (edge detect)
  if (lastState == HIGH && currentState == LOW) {
    msg.group_id  = GROUP_ID;
    msg.module_id = MODULE_ID;
    msg.pressed   = 1;

    esp_now_send(masterMac,
                 (uint8_t*)&msg,
                 sizeof(msg));
  }

  lastState = currentState;
  delay(10);
}
