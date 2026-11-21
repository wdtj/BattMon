# BattMon revision I — LM2575 power supply

The fixed 5 V Microchip LM2575-5.0WT replaces the external regulator module. Its TO-220-5 package is through-hole. The board only handles the monitor supply current; the battery charging/load cable still passes through the external Hall sensor.

| Reference | Selection | DigiKey Part Number | Function |
| --- | --- | --- | --- |
| U1 | LM2575-5.0WT | 576-1516-5-ND | 52 kHz, fixed 5 V, up to 1 A |
| L1 | Wurth 7447480331 | 732-4196-ND | 330 uH +/-10%, 1.4 A rated, 1.6 A typical at 10% L loss |
| D6 | Panjit MBR360_R2_00001 | 3757-MBR360_R2_00001CT-ND | 60 V / 3 A Schottky catch diode |
| C1 | Nichicon UPW1J101MPD | 493-1936-ND | 100 uF / 63 V input bypass |
| C9 | Panasonic EEU-FC1E331 | P10273-ND | 330 uF / 25 V low-impedance output electrolytic |
| D2 | Littelfuse 5KP18A | 5KP18ALFCT-ND | 18 V standoff, 29.2 V specified clamp; axial P600 |

C2/C10 retain 100 nF ceramic bypassing. D1 retains reverse-polarity protection, and F1 remains an external 1 A battery-takeoff fuse. The 1 A output is the regulator design ceiling, not a guaranteed available current under every ambient condition or external load. Keep the shared XIAO/base/sensor load within the tested power budget.

## Connections

U1 pin 1 connects to VBAT_PROT; pin 2 is SW; pin 3 and the metal tab are ground; pin 4 senses 5 V after L1; pin 5 ON/OFF connects to ground (enabled). L1 connects SW to 5 V. D6 cathode pin 1 connects to SW and its anode pin 2 to ground. C9 positive connects to 5 V, negative to ground. Fixed-output U1 has an internal divider: no external feedback resistors are needed. J4 is removed because U1 now mounts on the carrier board.

## Component checks

At 1 A output, nominal 330 uH and 52 kHz, ideal buck ripple is approximately 0.17 A peak-to-peak at 12 V input, and 0.24 A at 29.2 V. Using 297 uH (-10%), 42 kHz minimum frequency and 29.2 V input gives about 0.33 A peak-to-peak and a 1.17 A peak. These first-order calculations exclude switch/diode losses and inductance reduction. L1's 1.4 A thermal rating and typical 1.6 A saturation figure provide operating margin. Its 0.33 ohm maximum DCR can dissipate about 0.33 W at 1 A. Startup and overload can approach U1's higher current limit, so check saturation and temperature during those conditions; the inductor rating is not a short-circuit guarantee.

C9 follows the datasheet's 330 uF output example. The selected FC electrolytic lists 0.09 ohm impedance and 755 mA high-frequency ripple rating. Impedance is not a complete ESR specification across temperature/frequency. Keep the electrolytic; do not substitute ceramic-only output capacitance. Confirm loop stability, ripple, startup and response to ESP32 BLE load steps with the complete XIAO/base connected, including its existing capacitors. Verify sensor supply stays within 4.75-5.25 V and reject out-of-range readings using A2. The regulator's worst-case DC tolerance reaches those sensor limits, leaving no guaranteed ripple margin at the extremes; bench characterization across actual load and temperature is required. Light-load operation below the datasheet's 0.2 A regulation test range also needs verification.

D2 changes from a 24 V standoff / 38.9 V clamp SMD TVS to an 18 V standoff / 29.2 V clamp through-hole TVS. This improves margin below U1's 40 V operating input limit. The 29.2 V clamp applies at the specified 174.7 A 10/1000 us pulse; overshoot, pulse duration, source impedance and temperature matter. Its 5 kW pulse rating is lower than the previous TVS's 6.6 kW. This design is for a nominal 12 V battery, not a 24 V system, and has not been qualified for vehicle load dump. Check TVS energy, wiring inductance, sustained overvoltage behavior and fuse coordination with the intended RV charging system.

## Layout and validation

Place C1/C2, U1 and D6 together with very short power loops and a low-impedance ground return. Put L1 and C9 close to U1. SW is a noisy switching node, not a regulated supply: minimize its copper area and keep it away from Hall/voltage-divider traces, ADS1115 inputs and the antenna. Run FB separately from the quiet positive terminal of C9 and join signal ground at the output-capacitor return, away from catch-diode current. Use a ground plane or the manufacturer's single-point grounding arrangement. Do not build the switching power loop using long jumper leads or a solderless breadboard.

U1 has thermal shutdown but its 1 A rating does not remove the need for thermal design. Verify junction/case rise in the intended enclosure; add a suitable grounded-tab heatsink if measurements require one. Keep metalwork clear of other live nets. The supplied Footprint_Preview board is only a model inventory; it deliberately does not implement this placement or routing.

The enabled LM2575 draws about 5 mA typical quiescent current (10 mA maximum specified under the datasheet condition). Five mA alone is about 0.12 Ah/day from the battery, in addition to the sensor, base and ESP32 consumption. Grounded ON/OFF means it stays enabled during ESP32 sleep. The low standby-current specification applies when the regulator is shut down, which also removes sensor/ESP32 power; this circuit does not implement that mode.

Before installation, test input range, startup, USB isolation, BLE current peaks, ADC noise, sensor supply validity, output capacitor stability, thermal behavior and transient protection. No bench tests or routed production PCB are included in this revision.

## Cost and sources

Direct DigiKey unit listings checked November 14, 2026: U1 $1.72, L1 $1.93, D6 $0.55. Those three parts total $4.20 before capacitors, TVS, other existing parts, shipping or tax; this is not the complete power-supply price. Stock and prices change. C9 replaces an existing capacitor and D2 replaces the existing TVS.

- Microchip circuit, pinout, ratings and layout: https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/LM2575-52kHz-Simple-1A-Buck-Regulator-DS20007078.pdf
- Wurth electrical/mechanical drawing: https://www.we-online.com/components/products/datasheet/7447480331.pdf
- Supplier links: see PART_SELECTIONS.md and BOM.csv.
