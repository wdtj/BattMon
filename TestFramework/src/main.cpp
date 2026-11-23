#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Wire.h>

namespace {
constexpr uint8_t adcAddress = 0x48;
constexpr uint8_t conversionRegister = 0;
constexpr uint8_t configRegister = 1;
constexpr uint16_t startOrReady = 0x8000;
// Single-ended A0, +/-4.096 V, single shot, 128 SPS, comparator disabled.
constexpr uint16_t baseConfig = 0x4383;
constexpr float voltsPerCount = 0.000125f;
constexpr uint32_t sampleIntervalMs = 1000;

Adafruit_ST7789 tft(TFT_CS, TFT_DC, TFT_RST);
GFXcanvas16 screen(240, 135);

struct Button {
    uint8_t pin;
    uint8_t pressedLevel;
    bool rawPressed;
    bool pressed;
    uint32_t changedAt;
    unsigned count;
};

Button buttons[] = {{0, LOW, false, false, 0, 0},
                    {1, HIGH, false, false, 0, 0},
                    {2, HIGH, false, false, 0, 0}};

enum class AdcStatus { Missing, Error, Ok };
AdcStatus adcStatus = AdcStatus::Missing;
int16_t codes[3] = {};
bool busReady = false;

bool readRegister(uint8_t reg, uint16_t& value) {
    Wire.beginTransmission(adcAddress);
    Wire.write(reg);
    if (Wire.endTransmission(false) != 0 || Wire.requestFrom(adcAddress, uint8_t{2}) != 2) {
        return false;
    }
    value = static_cast<uint16_t>(Wire.read()) << 8;
    value |= Wire.read();
    return true;
}

bool readChannel(uint8_t channel, int16_t& code) {
    const uint16_t config = startOrReady | baseConfig | (uint16_t{channel} << 12);
    Wire.beginTransmission(adcAddress);
    Wire.write(configRegister);
    Wire.write(static_cast<uint8_t>(config >> 8));
    Wire.write(static_cast<uint8_t>(config));
    if (Wire.endTransmission() != 0) {
        return false;
    }

    const uint32_t startedAt = millis();
    while (millis() - startedAt < 25) {
        delay(1);
        uint16_t status;
        if (!readRegister(configRegister, status) || (status & 0x7fff) != (config & 0x7fff)) {
            return false;
        }
        if (status & startOrReady) {
            uint16_t raw;
            if (!readRegister(conversionRegister, raw)) {
                return false;
            }
            code = static_cast<int16_t>(raw);
            return true;
        }
    }
    return false;
}

AdcStatus sampleAdc() {
    if (!busReady) {
        return AdcStatus::Error;
    }
    Wire.beginTransmission(adcAddress);
    const uint8_t probe = Wire.endTransmission();
    if (probe != 0) {
        return probe == 2 ? AdcStatus::Missing : AdcStatus::Error;
    }
    for (uint8_t channel = 0; channel < 3; ++channel) {
        if (!readChannel(channel, codes[channel])) {
            return AdcStatus::Error;
        }
    }
    return AdcStatus::Ok;
}

const char* adcMessage() {
    switch (adcStatus) {
        case AdcStatus::Missing: return "No ADC at 0x48";
        case AdcStatus::Ok: return "ADS1115 at 0x48";
        default: return "ADC / I2C error";
    }
}

void drawScreen() {
    screen.fillScreen(ST77XX_BLACK);
    screen.setCursor(6, 6);
    screen.setTextColor(ST77XX_CYAN);
    screen.println("BattMon bench");
    screen.setTextColor(adcStatus == AdcStatus::Ok ? ST77XX_GREEN : ST77XX_YELLOW);
    screen.println(adcMessage());
    screen.setTextColor(ST77XX_WHITE);
    if (adcStatus == AdcStatus::Ok) {
        for (uint8_t channel = 0; channel < 3; ++channel) {
            screen.printf("A%u %+.4f V\n", channel, codes[channel] * voltsPerCount);
        }
    }
    screen.setCursor(6, 108);
    screen.printf("D0:%u D1:%u D2:%u", buttons[0].count, buttons[1].count, buttons[2].count);
    tft.drawRGBBitmap(0, 0, screen.getBuffer(), screen.width(), screen.height());
}

bool updateButtons(uint32_t now) {
    bool changed = false;
    for (auto& button : buttons) {
        const bool rawPressed = digitalRead(button.pin) == button.pressedLevel;
        if (rawPressed != button.rawPressed) {
            button.rawPressed = rawPressed;
            button.changedAt = now;
        }
        if (button.rawPressed != button.pressed && now - button.changedAt >= 25) {
            button.pressed = button.rawPressed;
            if (button.pressed) {
                Serial.printf("D%u press #%u\n", button.pin, ++button.count);
                changed = true;
            }
        }
    }
    return changed;
}
}  // namespace

void setup() {
    Serial.begin(115200);
    for (const auto& button : buttons) {
        pinMode(button.pin, button.pressedLevel == LOW ? INPUT_PULLUP : INPUT_PULLDOWN);
    }
    pinMode(TFT_I2C_POWER, OUTPUT);
    digitalWrite(TFT_I2C_POWER, HIGH);
    delay(10);

    pinMode(TFT_BACKLITE, OUTPUT);
    digitalWrite(TFT_BACKLITE, HIGH);
    tft.init(135, 240);
    tft.setRotation(3);
    screen.setTextSize(2);
    screen.setTextWrap(false);

    busReady = Wire.begin(SDA, SCL, 100000);
    Wire.setTimeOut(20);
}

void loop() {
    static uint32_t lastSample = millis() - sampleIntervalMs;
    const uint32_t now = millis();
    bool redraw = updateButtons(now);
    if (now - lastSample >= sampleIntervalMs) {
        lastSample = now;
        adcStatus = sampleAdc();
        Serial.println(adcMessage());
        if (adcStatus == AdcStatus::Ok) {
            for (uint8_t channel = 0; channel < 3; ++channel) {
                Serial.printf("A%u: %d counts, %+.4f V\n", channel, codes[channel],
                              codes[channel] * voltsPerCount);
            }
        }
        redraw = true;
    }
    if (redraw) {
        drawScreen();
    }
    delay(2);
}
