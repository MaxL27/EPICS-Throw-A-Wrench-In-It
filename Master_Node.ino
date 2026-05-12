#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// ---------------- LED ----------------
#define LED_PIN 0
#define NUM_LED 1

Adafruit_NeoPixel led(NUM_LED, LED_PIN, NEO_GRB + NEO_KHZ800);

// ---------------- PINS ----------------
#define BUTTON_PIN 4
#define AUDIO_PIN 3
#define POT_PIN 1

// ---------------- PWM AUDIO ----------------
#define AUDIO_CH 0
#define AUDIO_RES 8

bool lastButton = HIGH;

void setWhite() {
  led.setPixelColor(0, led.Color(255, 255, 255));
  led.show();
}

void setGreen() {
  led.setPixelColor(0, led.Color(0, 255, 0));
  led.show();
}

void startSound() {
  int pot = analogRead(POT_PIN);          // 0–4095
  int volume = map(pot, 0, 4095, 0, 255); // “volume feel”

  ledcWriteTone(AUDIO_CH, 1200);         // tone frequency
  ledcWrite(AUDIO_CH, volume);           // duty cycle
}

void stopSound() {
  ledcWriteTone(AUDIO_CH, 0);
  ledcWrite(AUDIO_CH, 0);
}

void setup() {
  Serial.begin(115200);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  analogReadResolution(12);

  // LED
  led.begin();
  led.setBrightness(120);
  setWhite();

  // PWM AUDIO SETUP
  ledcAttach(AUDIO_PIN, 1200, AUDIO_RES);

  Serial.println("PWM Audio System Ready");
}

void loop() {
  bool button = digitalRead(BUTTON_PIN);

  // press event
  if (lastButton == HIGH && button == LOW) {
    Serial.println("Pressed");
    setGreen();
    startSound();
  }

  // release event
  if (button == HIGH) {
    setWhite();
    stopSound();
  }

  lastButton = button;
}