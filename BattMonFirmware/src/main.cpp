#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
    Serial.begin(115200);

    // Enable the Feather's display power and backlight.
    pinMode(TFT_I2C_POWER, OUTPUT);
    digitalWrite(TFT_I2C_POWER, HIGH);
    delay(10);

    pinMode(TFT_BACKLITE, OUTPUT);
    digitalWrite(TFT_BACKLITE, HIGH);

    tft.init(135, 240);
    tft.setRotation(3);
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextWrap(false);

    tft.setCursor(10, 10);
    tft.setTextSize(3);
    tft.setTextColor(ST77XX_CYAN);
    tft.println("BattMon");

    tft.setCursor(10, 50);
    tft.setTextSize(2);
    tft.setTextColor(ST77XX_GREEN);
    tft.println("Feather TFT OK");
}

void loop() {
    static uint32_t previous = 0;
    const uint32_t now = millis();

    if (now - previous >= 1000) {
        previous = now;
        const unsigned long seconds = now / 1000;

        Serial.printf("BattMon running: %lu seconds\n", seconds);

        tft.fillRect(0, 90, tft.width(), 30, ST77XX_BLACK);
        tft.setCursor(10, 95);
        tft.setTextSize(2);
        tft.setTextColor(ST77XX_WHITE);
        tft.printf("Uptime: %lu s", seconds);
    }
}