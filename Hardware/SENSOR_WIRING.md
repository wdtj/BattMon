# Tamura L37S100S05M — BattMon revision I

DigiKey: **132-L37S100S05M-ND**. This is a solid-aperture Hall sensor. It measures bidirectional DC while the existing insulated cable passes through its 20.4 x 10.4 mm opening. It does not clamp open. A cable end must be disconnected; check that both insulation and terminal hardware will fit. Mount externally and do not route primary battery current through the PCB.

## Sensor and carrier connection

Use the Tamura M-variant connector drawing to identify pin 1; cable and header views can appear mirrored. The sensor header is Molex **22-04-1041**. The matching **0022011042** housing is J7 in the schematic and BOM, with three **0008500160** contacts. Leave cavity 1 empty. Use appropriately crimped, strain-relieved wiring and verify continuity/polarity before power.

| Sensor pin | Function | Carrier header J6 | Connection |
| --- | --- | --- | --- |
| 1 | VREF input/output | 1 unused | Leave unconnected; no crimp contact |
| 2 | OUT | 2 | HALL_OUT, then R1/R2 to ADS1115 A0 |
| 3 | GND | 3 | Common monitor ground |
| 4 | +VCC | 4 | Regulated 5V before D5/JP1 |

The carrier J6 is a 2.54 mm Samtec header, while the sensor end uses its specified Molex housing. These are different connector families; this is a wired harness, not a directly mating pair. Select appropriate mating carrier contacts/housing or a prepared cable for the build. J6 pin 1 remains electrically unused even if a four-position carrier plug is used.

The 5 V sensor rail must be 4.75-5.25 V. Do not connect RV 12 V to any sensor terminal. Maximum specified sensor consumption is 20 mA. C3 bypasses this rail near the sensor connector.

## Other off-board interfaces

| Header | Pin 1 | Pin 2 | Pin 3 | Pin 4 | Pin 5 |
| --- | --- | --- | --- | --- | --- |
| J5 — XIAO ESP32S3 + expansion base | 5V_ESP | GND | Shared 3V3 | SDA / D4 / GPIO5 | SCL / D5 / GPIO6 |
| J6 — external Tamura | Unused | OUT | GND | +5V | — |

Header pin numbers are carrier assignments. Wire the external modules by printed signal names and the documented manufacturer pinout, not by an assumed common header order. J5/J6 are low-current power/signal interfaces. F1 is an external inline 1 A fuse at the battery takeoff. Heavy-current J2/J3 and the proposed 277-6402-ND terminal block are not needed.

## Current direction and location

Pass only one current-carrying conductor through the sensor. Select a battery trunk cable carrying every charger/load contribution, with no bypass connections. Positive and negative in the same aperture cancel. Set charging positive by sensor orientation or a firmware sign multiplier; verify with a known load/charging source. The existing main fuse and cable ratings still apply.

## Calibration change

Sensor sensitivity is nominally **0.00625 V/A**. R1/R2 retain the 0.6 divider, so ADS1115 A0 sensitivity is **0.00375 V/A**.

At PGA +/-4.096 V:

```text
A0_volts = ADC_A0_code * 0.000125
A1_volts = ADC_A1_code * 0.000125
A2_volts = ADC_A2_code * 0.000125

battery_volts = A1_volts * 16
sensor_supply_volts = A2_volts / 0.6
current_amps = direction_sign * (A0_volts - zero_A0_volts) / 0.00375
```

Measure and store zero_A0_volts with no current in the sensed cable. Nominally it is about 1.5 V, but actual offset must be calibrated. Calibrate gain against a reference after zero. Supply monitoring is diagnostic; the old ACS758 ratio formula is retired. Reject sensor measurements if its supply is invalid or disconnected. This calculation is guidance for future firmware; no firmware is included.

The Tamura VREF pin remains unused, as specified when an external reference is not applied. Offset, temperature drift and magnetic hysteresis limit small-current accuracy even with the high-resolution ADC. Recheck zero after installation or significant overloads, under a confirmed zero-current condition.

Source: https://www.tamuracorp.com/global/products/download/pdf/L37SxxxS05_en.pdf — connector drawing on page 3, electrical specifications on page 2.

The buck regulator is now on the carrier: U1 LM2575-5.0WT, L1 330 uH and D6 MBR360. J4 has been removed. The sensor remains powered from the regulated 5 V rail before D5/JP1. Do not wire the sensor to the switching node SW.

J7 is explicitly drawn as the external sensor-end cable plug. Its housing is WM2022-ND and its three contacts are J7-CONTACTS, WM16517-ND. The contact part replaces obsolete 0008500114 / WM1114-ND. Leave cavity 1 empty; connect cavities 2/3/4 to HALL_OUT/GND/5V.

Revision I shows J7 and J6 directly along the OUT signal path before R1 and the ADC. Connector symbols use contact circles as continuous electrical nodes. J7 and J6 are connected by the three-wire harness; J7 plugs into U2, while J6 is on the carrier.
