# Feather bench test

Open `TestFramework` as a standalone PlatformIO project and select the
`feather_bench` environment. From the repository root, you can also run:

```powershell
pio run -d TestFramework -t upload
pio device monitor -d TestFramework
```

The monitor uses 115200 baud. With only the Feather connected over USB, expect
the LCD to show **BattMon bench**, **No ADC at 0x48**, and three button counts.
Press D0, D1, and D2 one at a time. Each press should increment its matching
counter once and appear in the serial monitor. Use D0 after the sketch starts
because it doubles as the boot button.

## Optional ADS1115 breakout

With USB power disconnected, connect a 3.3 V ADS1115 breakout using the
Feather's STEMMA QT port or wire Feather 3V to breakout VIN, GND to GND, SDA
to SDA, and SCL to SCL. Use the breakout's default address, 0x48. Power up
and expect A0, A1, and A2 to appear on the LCD once per second. The serial
monitor also shows raw counts.

Ground each input to check the zero reading. Then apply a known voltage of
about 1 V, referenced to breakout ground, to one input at a time. Compare the
corresponding reading with a meter. Unconnected analog inputs can float.
Keep bench inputs between GND and 3.3 V and apply them only while the ADC is
powered. The ADC gain setting does not raise its input voltage limit.

The displayed voltages are at the ADC pins. When testing the assembled
carrier, compare those readings against the carrier's expected divider
outputs. See [sensor wiring and calibration](../Hardware/SENSOR_WIRING.md).

[Feather buttons](https://learn.adafruit.com/esp32-s3-reverse-tft-feather/digital-input-multiple-buttons) ·
[ADS1115 datasheet](https://www.ti.com/lit/ds/symlink/ads1115.pdf)
