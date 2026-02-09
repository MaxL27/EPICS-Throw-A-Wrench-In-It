#include <WiFi.h>
#include <esp_now.h>

// -------------------- PINS --------------------
#define POT_PIN        1     // Volume control
#define SPEAKER_PIN    3     // PWM to amplifier
#define BUTTON_PIN     4     // Main button
#define LED_PIN        5     // Status LED
#define BATTERY_PIN    0     // Battery sense (via divider)

// -------------------- AUDIO --------------------
#define AUDIO_RES      8
#define BASE_FREQ      1000

// -------------------- ESP-NOW ------------------
typedef struct {
  uint8_t trigger;
} Message;

Message incoming;

// -------------------- ESP-NOW CALLBACK ---------
void onReceive(const esp_now_recv_info* info,
               const uint8_t* data,
               int len)
{
  if (len == sizeof(Message)) {
    memcpy(&incoming, data, sizeof(Message));

    if (incoming.trigger == 1) {
      int pot = analogRead(POT_PIN);
      int volume = map(pot, 0, 4095, 0, 255);

      ledcWriteTone(SPEAKER_PIN, BASE_FREQ);
      ledcWrite(SPEAKER_PIN, volume);

      digitalWrite(LED_PIN, HIGH);
      delay(300);
      digitalWrite(LED_PIN, LOW);

      ledcWriteTone(SPEAKER_PIN, 0);
    }
  }
}

// -------------------- SETUP --------------------
void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  analogReadResolution(12);

  // WiFi + ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP-NOW init failed");
    return;
  }

  esp_now_register_recv_cb(onReceive);

  // Audio PWM (ESP32-C3 API)
  ledcAttach(SPEAKER_PIN, BASE_FREQ, AUDIO_RES);

  digitalWrite(LED_PIN, HIGH);
  delay(150);
  digitalWrite(LED_PIN, LOW);

  Serial.println("System powered ON");
}

// -------------------- LOOP ---------------------
void loop() {
  // Button example (optional local action)
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(20);
    if (digitalRead(BUTTON_PIN) == LOW) {
      digitalWrite(LED_PIN, HIGH);
      delay(100);
      digitalWrite(LED_PIN, LOW);
    }
  }

  // Battery indicator (optional)
  int batt = analogRead(BATTERY_PIN);
  if (batt < 3000) {
    digitalWrite(LED_PIN, millis() % 500 < 250);
  }

  delay(20);
}
