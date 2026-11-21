# BattMon — RV battery monitor, revision I

Open **BattMon.kicad_pro** in KiCad 9 or 10, then open the schematic. **BattMon-schematic.pdf** is the printable drawing. This is a prototype module-wiring design with 32 assigned board-component footprints and bundled 3D models. It does not include a routed fabrication PCB or firmware. Footprint_Preview.kicad_pcb is an unrouted inventory board for 3D inspection only; see FOOTPRINTS_AND_MODELS.md for model provenance and mechanical verification limits.

## Parts and schematic fields

Every circuit component has the exact custom field **DigiKey Part Number**. Manufacturer, Manufacturer Part Number, Package and DigiKey URL are also populated. These fields are hidden on the drawing and available in Symbol Properties / the symbol-fields table. They are retained in the exported netlist.

- **BOM.csv** lists every reference, selected supplier SKU, package, quantity and fit status.
- **DigiKey_Shopping_List.csv** consolidates repeated supplier SKUs and omits DNP parts and external wiring points.
- **PART_SELECTIONS.md** provides clickable product links and explains substitutions.
- Supplier selections were checked on November 14, 2026. Check availability when ordering; no order has been placed.

All carrier-board discrete parts now use through-hole packages, including the TO-220 LM2575, inductor and axial TVS/catch diode. The ADS1115 is an assembled Adafruit breakout with solderable headers. The 5KP18A TVS has a 5 kW specified pulse rating; this does not establish survival of every vehicle transient.

U4 represents the complete XIAO/base assembly. Its main DigiKey Part Number field identifies the XIAO processor; separate Expansion Base DigiKey Part Number and Header DigiKey Part Number fields identify the base and headers. The BOM lists these as separate purchasing items. F1 has a separate Fuse Holder DigiKey Part Number field, and JP1 has a separate Shunt DigiKey Part Number field; those accessories also have purchasing rows.

The former high-current J2/J3 connection points are removed. The LM2575 buck supply is now on-board and J4 is removed. J5 connects the external XIAO/base assembly and J6 the external Tamura sensor. These carry monitor power/signals only. The discussed 277-6402-ND heavy-current terminal block is not required.

## Installation — external pass-through sensor

U2 is **Tamura L37S100S05M**, DigiKey **132-L37S100S05M-ND**, replacing ACS758. The existing insulated battery cable passes through the sensor's solid 20.4 x 10.4 mm aperture. It is **not split-core/clamp-on**: disconnect a cable end to install it. Measure the insulated cable and any lug before ordering/installation; the narrow opening is 10.4 mm. Do not strip insulation to make a cable fit. If the lug will not pass, safe professional retermination or a different split-core sensor is needed.

The cable remains the entire primary-current path. No battery charging/load current passes through the PCB, sensor signal connector, or J5/J6. Normal RV cable/fuse protection remains unchanged. Mount the sensor securely and protect its connector from moisture and strain. The sensor's temperature rating is -40 to +85 C; this is not the rating of the entire monitor assembly.

Choose one trunk conductor carrying all battery charge/discharge current. Either battery-positive or battery-negative can be measured. Do not include positive and negative together in the aperture, and make sure alternate battery connections do not bypass the sensed cable. Orient the indicated current direction to make charging positive; verify against a known charging/discharging condition and invert the firmware sign if necessary. Disconnect battery and charging sources before moving a cable end.

J1 remains a separate low-current voltage-sense/power connection: pin 1 to battery positive through F1 (1 A) at the battery takeoff, pin 2 to battery negative. The BATTERY_SIDE net now denotes that sense/power connection only.

Sensor pinout, low-current carrier headers and calibration are detailed in **SENSOR_WIRING.md**. The Tamura M variant has a Molex 22-04-1041 header. Sensor pin 1 is VREF and is intentionally unused; pin 2 is OUT, pin 3 GND, pin 4 +5 V. Leave housing cavity 1 empty. Do not connect VREF to ground or to 3V3.

Supply U2 from the regulated 5V net before D5/JP1, not from raw battery voltage or the XIAO/base power rail. Locate C3 near the sensor connector and use short, quiet signal wiring. The regulator must remain on for current readings even when the XIAO is powered by USB.

## Protected supply and XIAO expansion base

The selected base is Seeed **103030356**, DigiKey **1597-103030356-ND**. It requires a separately purchased standard **XIAO ESP32S3** (non-Sense) and suitable headers. It is represented as an external assembly rather than a reproduction of its internal circuitry. Its OLED can display measurements when firmware is added.

F1 protects the monitor power lead only; it does not protect the RV main current path. D1 (axial SB2100, 100 V / 2 A) provides reverse-polarity protection. D2 suppresses positive transients after D1. U1 is the LM2575-5.0WT 5 V / 1 A buck IC. L1 (330 uH) and D6 (MBR360) form its switching power stage; pin 4 senses the output after L1. Pin 5 is grounded for continuous operation. D2 is now a 5KP18A with 18 V standoff and 29.2 V maximum specified clamp. See POWER_SUPPLY.md for selection, thermal and layout requirements. C1/C2 provide input decoupling and C9/C10 provide output decoupling. Battery voltage is sensed before D1 so its forward drop does not create a measurement offset.

Actual clamp overshoot, wiring inductance, surge energy, thermal behavior and load-dump operation require bench validation. This is not an ISO 7637/ISO 16750 qualified design. Keep protection connections short and use the specified or higher voltage ratings. A destructive TVS fault may open F1.

D5 (axial 1N5819) provides series isolation in the XIAO/base 5 V feed, followed by JP1. Close JP1 for RV-powered operation. **Remove JP1 before USB power** to avoid simultaneously driving the shared USB/5 V rail. For valid live measurements during USB use, the RV regulator must still power the Hall sensor. USB-only power does not power that sensor; reject its readings.

Never connect RV 12 V to the base's 5 V header, Grove power pins or single-cell lithium battery socket. Leave the lithium battery socket empty. Turn the base switch on and verify its 3.3 V rail before attaching the ADC. The sensor uses the regulator's independent 5 V output, not the base servo-boost output.

| BattMon net | Expansion base location in published v1.0 schematic | XIAO signal |
| --- | --- | --- |
| 5V_ESP (after D5/JP1) | 5 V header, J11 pin 1 | 5V / VBUS |
| GND | Ground header, J11 pin 2 | GND |
| 3V3 | 3.3 V header, J11 pin 3 | Shared 3V3 rail |
| SDA | D4 header, J10 pin 5 | GPIO5 |
| SCL | D5 header, J10 pin 6 | GPIO6 |

Either base Grove I2C socket can alternatively supply SCL on pin 1, SDA on pin 2, 3V3 on pin 3 and GND on pin 4 in the published drawing. Follow the printed labels and connector orientation. Seeed has changed the PMIC under this SKU; verify connections against the actual revision supplied.

U4 schematic numbers 1..5 are functional identifiers, not physical header positions. Wire by the labels above. Other XIAO functions are unused in this prototype.

## ADS1115 breakout wiring

U3 is now **Adafruit 1085**, DigiKey **1528-1085-ND**, rather than a bare ADS1115IDGS. Its schematic pin identifiers are its printed breakout labels; do not use the old IC pin numbers.

| Breakout label | BattMon net / connection |
| --- | --- |
| VIN | 3V3 |
| GND | GND |
| SDA | SDA / XIAO D4 / GPIO5 |
| SCL | SCL / XIAO D5 / GPIO6 |
| ADDR | GND (address 0x48; leave optional address jumper open) |
| ALRT | Unconnected |
| A0 | HALL_ADC |
| A1 | VBAT_ADC |
| A2 | V5_ADC |
| A3 | GND |

If extra analog supply-output pads are present on the breakout, leave them unused. Follow its silkscreen rather than assuming a connector pin order. Its ADC chip and other SMD components are already assembled; solder only the header/wires required for this build.

The base has onboard I2C pull-ups. The Adafruit breakout also has 10 kOhm pull-ups to VIN; powering it at 3.3 V keeps both sets on the correct logic rail. R8/R9 remain **DNP** and are not in the shopping quantities. Do not power the ADC module from 5 V. For the XIAO ESP32S3, Arduino I2C initialization is `Wire.begin(5, 6)`.

The base OLED and RTC share I2C with the ADC. Published base addresses convert to 7-bit 0x3C and 0x51, distinct from ADC address 0x48. Confirm all attached devices with an I2C scan. No firmware is included.

## Measurement and calibration

Use single-ended ADS1115 conversions, PGA +/-4.096 V and 128 samples/s. The ADC supply is 3.3 V, and its input rails must still be respected. Read after switching channels has settled and average samples to reduce noise.

| Channel | Analog signal | Scaling / purpose |
| --- | --- | --- |
| A0 | Tamura OUT through R1/R2 | 0.6 times sensor output |
| A1 | Battery voltage through R5/R6/R7 | Battery / 16 |
| A2 | Regulator supply through R3/R4 | 0.6 times sensor supply; diagnostic only |

For this PGA, signed ADC code times 0.000125 gives volts. The sensor's nominal sensitivity is 0.625 V per 100 A, or 0.00625 V/A. With the existing 0.6 divider, A0 changes by **0.00375 V/A**. Current uses an absolute calibrated zero; **do not use the former ACS758 supply-ratio formula**.

- Battery volts = A1 volts * 16.
- Sensor supply volts = A2 volts / 0.6. Reject current measurements when this is outside 4.75-5.25 V.
- Current amps = direction_sign * (A0 volts - measured_zero_A0_volts) / 0.00375, before gain calibration.

At zero current, sensor OUT is approximately 2.5 V and A0 approximately 1.5 V. A0 is approximately 1.125 V at -100 A and 1.875 V at +100 A. These are nominal values; measure zero rather than hardcoding 1.5 V. Tamura specifies 100 A nominal and a larger measuring range, but this design is documented/calibrated for +/-100 A. Do not treat a sensor range as a cable/fuse rating.

Calibrate zero only when no current flows through the sensed cable. Power the monitor separately or account for its own battery consumption during that procedure. Then calibrate gain against a known current and voltage against a reliable meter. Store zero/gain in nonvolatile memory when firmware is added. Do not automatically zero during normal operation: that can erase a real battery current. Check charging/discharging sign.

Nominal ADC granularity is about 0.033 A/code. Actual accuracy is limited by sensor offset, hysteresis, temperature drift, noise, resistor tolerance and calibration. This sensor is not a precision instrument for integrating small standby loads over weeks. Normal voltage measurement is approximately 0-18 V; the divider's extra headroom is not approval to use a 48 V battery bank.

R5/R6 remain two 75 kOhm resistors, equivalent to 150 kOhm. All divider resistors are axial 0.1% parts. C8/C9 are polarized with positive to 3V3/5V respectively. The 100 nF capacitors use one 100 V through-hole X7R type for simpler purchasing.

D3/D4 BAT85S clamps limit ADC excursions at the small current set by the divider. They do not replace the TVS or constitute a tested transient withstand rating. The upper clamp may inject a small current into 3V3 with the sense lead attached and electronics off; remove F1 for storage/service.

Voltage and current alone do not establish reliable state of charge. Amp-hour tracking requires firmware, offset/gain calibration, elapsed-time integration and battery-specific synchronization. Assess continuous monitor/base/regulator drain and disconnect for long storage or design a low-power mode.

## Verification

KiCad 9.0.2 parsed the revised schematic and exported the PDF/netlists. ERC reports zero errors/warnings. All 15 nets and 85 wired component-pin connections were compared with an independently updated expected connection map; sensor VREF, J6 pin 1 and ADC ALRT are explicit no-connects. Supplier/footprint fields were checked in the exported netlist.

All 32 assigned footprints loaded successfully and their pad identifiers were checked against circuit pins. All relative model paths resolve to bundled files. The unrouted inspection board exported successfully to STEP with its component models. Generic visual models and assembly-height assumptions are identified in FOOTPRINTS_AND_MODELS.md. Exact external Tamura and XIAO/base STEP assemblies were not obtained and are not represented by substitute models.

The sensor, regulator, XIAO/base and inline fuse are external and excluded from PCB transfer; their carrier interfaces are explicit headers. Custom module symbols use passive pin types, so ERC cannot validate actual module power direction or internal behavior. These are file/connectivity/CAD checks, not bench tests, a completed PCB layout or automotive qualification. Complete placement, routing, mechanical/thermal checks and hardware validation before fabrication/installation.

## References

- Selected supplier product pages: see PART_SELECTIONS.md and BOM.csv.
- Tamura sensor datasheet: https://www.tamuracorp.com/global/products/download/pdf/L37SxxxS05_en.pdf
- TI ADS1115 datasheet: https://www.ti.com/lit/ds/symlink/ads1115.pdf
- Adafruit breakout pinouts: https://learn.adafruit.com/adafruit-4-channel-adc-breakouts/pinouts
- Microchip regulator: https://ww1.microchip.com/downloads/aemDocuments/documents/APID/ProductDocuments/DataSheets/LM2575-52kHz-Simple-1A-Buck-Regulator-DS20007078.pdf
- Seeed expansion base: https://wiki.seeedstudio.com/Seeeduino-XIAO-Expansion-Board/
- XIAO ESP32S3: https://wiki.seeedstudio.com/xiao_esp32s3_getting_started/

## Revision I — discrete buck and phone connection

U1 is Microchip LM2575-5.0WT, DigiKey 576-1516-5-ND. L1 is Wurth 7447480331, D6 is Panjit MBR360, C9 is Panasonic EEU-FC1E331 (330 uF / 25 V), and D2 is Littelfuse 5KP18A. These replace the external RECOM module and its J4 interface. Supplier fields, footprints, model assignments, BOM and shopping list have been updated. See POWER_SUPPLY.md for design calculations and the required hardware checks. This remains an unrouted prototype schematic, not a fabrication-ready PCB.

The ESP32S3 supports Bluetooth Low Energy, suitable for a custom Android or iPhone app. It does not support Bluetooth Classic/SPP serial connections. Plan a BLE GATT peripheral on BattMon with the phone as central. Notify battery voltage, signed charge/discharge current, sensor-supply voltage and measurement/calibration status about once per second. Continue monitoring when disconnected. Require an explicit user action and authenticated connection for calibration changes. Place the antenna outside a metal battery enclosure or verify RF performance in the final installation.

This revision documents the intended BLE interface; neither BLE firmware nor a phone app is included. State of charge requires a separate battery-specific estimation/calibration design and is not inferred directly from current.

BLE reference: https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/api-guides/ble/overview.html

## External fuse and Tamura cable plug

The drawing now places F1 in a dashed EXTERNAL section with the existing RV battery BT1. The wiring order is battery positive -> F1 (1 A, at battery takeoff) -> supply cable -> J1 pin 1 -> board. Battery negative goes directly to J1 pin 2. J1 pin 1 is wired continuously to D1; its BAT_FUSED net also feeds the voltage divider. F1/holder are not mounted on the carrier PCB. BT1 is an existing installation item excluded from the BOM and PCB.

J7 is the sensor-end Molex 0022011042 housing (WM2022-ND), explicitly shown in the schematic with contact ordering fields and visible callouts. Fit three Molex 0008500160 contacts (WM16517-ND, 22-30 AWG) in cavities 2 OUT, 3 GND and 4 +5 V; leave cavity 1 VREF empty. These contacts replace the obsolete 0008500114 selection. J7-CONTACTS is a separate quantity-three BOM row, not another housing. J7 is a cable plug, so no carrier-board footprint is assigned. J6 remains the carrier end of the harness; verify the two connector families and pin orientation before applying power.

## Revision I drawing clarification

J1 is drawn as a two-contact terminal block with continuous wires through each electrical contact: battery positive -> external F1 -> J1.1 -> D1 -> buck input, and battery negative -> J1.2 -> PCB ground. Each circle is a single terminal/contact node, not an extra pin. Physical footprint pins remain 1 and 2.

The Hall output is now visibly wired U2 OUT -> J7.2 -> three-wire cable -> J6.2 -> R1/R2 divider -> ADS1115 A0. J7 is the external sensor-end plug, and J6 is the carrier header at the other end of the cable; they are different connector families and do not mate directly to each other. Ground and 5 V are drawn across cable contacts 3 and 4, with supply labels at the sensor and carrier. Cavity 1 remains unused at both ends. J7 housing/contact ordering callouts remain on the drawing. This changes presentation only; the verified electrical connectivity, BOM quantities and footprints remain unchanged.
