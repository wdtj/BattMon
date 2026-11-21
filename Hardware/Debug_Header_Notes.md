# BattMon debug header J2

Added a 1x4, 2.54 mm straight male header (Samtec TSW-104-07-G-S, DigiKey 612-TSW-104-07-G-S-ND), using existing BattMon:Header_1x4_P2.54 footprint. Initial board placement is at (111,57.5) mm, rotated 90 degrees, along the top edge. Existing placement and routing are preserved. New header pads require routing.

| J2 pin | Silkscreen | Net | Feather U4 pin |
|---|---|---|---|
| 1 | GND | GND | 13 |
| 2 | DB | DBG_TX | 1 (symbol labels it TDX0; hardware debug TX) |
| 3 | TX | UART_TX | 2 |
| 4 | RX | UART_RX | 3 |

All signals are Feather-side names, 3.3 V logic. USB-UART adapter RX connects to J2 TX for the exposed TX/RX pair; adapter TX to J2 RX; ground to J2 GND. To observe the hardware debug stream, connect adapter RX to DB instead of TX. DB is output only. Do not drive DB, use RS-232 voltage levels, or connect a 5 V TX output. No supply voltage is exposed on this header.

Adafruit describes DB as hardware debug TX for low-level ESP-IDF output. It is not the default USB Serial.print/CircuitPython print stream; firmware configuration determines actual UART logging behavior.
https://learn.adafruit.com/esp32-s3-reverse-tft-feather/pinouts#debug-pin

Added Debug_UART netclass: 0.25 mm trace, 0.25 mm clearance, 0.8/0.4 mm via pad/drill for /DBG_TX, /UART_TX, /UART_RX. Removed three U4 no-connect markers corresponding to newly connected pins.

Simplified 17 schematic values: removed resistor tolerance suffixes and capacitor voltage suffixes. Corresponding PCB values match. Full prior values are retained in hidden Rated Value fields in both files, and manufacturer/MPN fields are preserved. DNP marking and the inductor current value were retained. Existing board U1 value LM2575-5.0 and schematic U1 value LM2575-5.0WT were already different; neither was changed in this edit.

Validation: parsed all generated files; existing component-to-component connectivity preserved; J2 connectivity independently traced; existing PCB positions, rotations, pad geometry, model settings and board geometry preserved. Existing netclass and DKRed rules retained. Full KiCad 10 ERC/DRC must still be run locally. Reload after Drive sync; do not save an older open copy over the updates.
