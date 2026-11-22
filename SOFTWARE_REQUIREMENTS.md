# BattMon software requirements

BattMon monitors a nominal 12 V RV house battery bank. The ESP32-S3 Feather measures and estimates battery condition, reports it locally on the LCD, and shares it with paired Android phones over Bluetooth Low Energy (BLE). Equipment control and Windows support are outside scope.

## Power and measurements

- **BM-000 — Current draw.** Minimize average battery draw while meeting measurement and reporting requirements. Measure the complete assembly at the battery input over full polling cycles, with the LCD asleep and awake and with zero, one, and two phones connected.
- **BM-001 — Voltage.** Read battery terminal voltage in volts from ADS1115 A1 through the 16:1 divider. The documented measurement range is approximately 0–18 V while the electronics are powered.
- **BM-002 — Current.** Read signed net battery current from the Tamura L37S100S05M Hall sensor on ADS1115 A0 over the documented −100 A to +100 A range. Positive means charging and negative means discharging. The sensed conductor must carry all charge and discharge current.
- **BM-003 — Calibration.** Calibrate current zero under confirmed zero-current conditions and calibrate gain against a known current. Verify voltage against a reference meter. Persist calibration across restarts.
- **BM-004 — Sensor validity.** Read the Hall sensor supply on ADS1115 A2. Mark current invalid if the supply is outside 4.75–5.25 V or the current measurement fails. Continue to report independently valid voltage.
- **BM-005 — Charge direction.** Report Charging, Discharging, or Idle from valid signed current. Use a threshold around zero to avoid switching states due to noise. Report direction as unavailable if current is invalid.
- **BM-008 — Adaptive polling.** Sample voltage and current at each poll, including when the display is asleep and all phones are disconnected. The shortest regular interval is **30 seconds**. Use this interval when valid current is high or changing in either direction. Lengthen the interval only after consecutive low, stable readings and cap it at the maximum configured in Android. Poll timing is independent of BLE advertising. A change that begins and ends between polls may be missed.

The documented voltage divider remains connected to battery power between polls. Polling less often can reduce controller activity, while switching off the divider between readings requires a hardware change. [Hardware design notes](Hardware/README.md) · [Sensor wiring and calibration](Hardware/SENSOR_WIRING.md)

## Battery profiles and charge estimation

**BM-009 and BM-010 — Profile and capacity.** Support selectable flooded deep-cycle lead-acid, AGM deep-cycle lead-acid, and LiFePO₄ profiles. Use the bank’s entered capacity in amp-hours, based on the 20-hour rating for lead-acid and rated capacity for LiFePO₄. Allow battery-specific overrides.

| Profile | Proposed reserve | Proposed charge efficiency | Proposed Peukert exponent |
| --- | ---: | ---: | ---: |
| 12 V flooded deep-cycle lead-acid | 50% | 95% | 1.25 |
| 12 V AGM deep-cycle lead-acid | 50% | 95% | 1.25 |
| 12.8 V LiFePO₄ | 20% | 98% | 1.01 |

These starting values are subject to battery-specific guidance and bench validation. The reserve is the endpoint for the runtime estimate. [Trojan battery guidance](https://www.trojanbattery.com/resources/battery-maintenance) · [Victron monitor settings](https://www.victronenergy.com/media/pg/SmartShunt_IP65/en/all-features-and-settings.html) · [Victron lithium settings](https://www.victronenergy.com/media/pg/SmartShunt/en/configuration.html)

- **BM-006 — Estimated charge remaining.** Report percent and amp-hours using the best available starting point and subsequent measured current. Apply the profile’s charge efficiency and discharge-rate correction. Label both values as estimates.
- **BM-007 and BM-015 — Estimated time to reserve.** During sustained discharge with valid inputs, estimate runtime to the configured reserve from estimated usable charge above reserve and recent average discharge. Show Reserve reached when a valid estimate is at or below the reserve. Show runtime as unavailable when charging, idle, or when the inputs are unavailable.
- **BM-012 — Elapsed time and restart.** Integrate current using actual elapsed time from a monotonic clock that includes sleep. Save the charge estimate periodically. On restart with the same battery, restore the estimate, flag the unmeasured gap, and continue updating from new valid samples. Mark charge uncertain during measurement gaps and suppress runtime while required measurements are invalid.
- **BM-013 — Full-charge detection.** Synchronize to 100% when multiple consecutive valid samples span the configured detection period and show voltage at or above the charged-voltage threshold with positive current below the tail-current threshold. A missing or nonqualifying sample resets the detection period. Use battery- and charger-specific voltage criteria. A tail current of 4 A per 100 Ah and a three-minute detection period are initial test values. [Victron full-charge synchronization](https://www.victronenergy.com/media/pg/SmartShunt_IP65/en/operation.html)
- **BM-014 — Starting estimate.** At first setup, after battery replacement, or after a change in profile or capacity, allow entry of a known starting percentage or wait for full-charge detection. For flooded and AGM profiles, also allow a rough estimate using the manufacturer’s open-circuit voltage table when the user confirms the battery met the manufacturer’s rest conditions. Label that result Voltage estimate until resynchronization. For LiFePO₄, use current tracking from an established starting point. Show Estimate pending when there is no usable starting point, and continue reporting valid voltage and current. [Trojan flooded voltage guidance](https://www.trojanbattery.com/resources/battery-maintenance) · [Trojan AGM voltage guidance](https://www.trojanbattery.com/trojan-aes-batteries/aes-agm-standard-agm-battery-maintenance) · [Victron LiFePO₄ discharge curves](https://www.victronenergy.com/live/ve.direct%3Aphoenix-inverters-dynamic-cutoff)

## Android and Bluetooth

- **BM-011 — Reporting.** BattMon is the BLE peripheral and GATT server. The Android app discovers and connects to it, reads the latest report, and subscribes to updates. Support simultaneous monitoring by multiple paired phones. Each receives voltage, signed current, charge direction, estimated charge remaining, estimated time to reserve, and validity and alert status. Indicate when displayed readings are stale. Use low-duty-cycle advertising and allow the radio to sleep between events. Monitoring continues when all phones disconnect.
- **BM-016 — Low-battery alerts.** Raise one alert when a valid charge estimate reaches the configured low-charge threshold. Rearm only after charge rises above the threshold by the configured recovery margin. Notify subscribed phones as the alert occurs. Retain at most one pending missed alert per paired phone, even if the battery recovers or goes low again before that phone reconnects. Deliver the pending alert on reconnect and clear it only after that phone acknowledges receipt. Deduplicate retries in the app and distinguish an active warning from a missed warning that has cleared. Android shall show the notification with the app in the background when connected and notification permission is granted. [Android background BLE guidance](https://developer.android.com/develop/connectivity/bluetooth/ble/background)
- **BM-017 — Pairing and access.** In local pairing mode, the module displays its identifier so the user can select the correct device in Android and establish a BLE bond. Every paired phone can monitor, receive alerts, change settings, and remove paired phones. Provide a local way to clear pairings if all phones are lost. Preserve battery settings and calibration when clearing pairings.
- **BM-018 — Shared configuration.** Any paired phone can read and update the saved profile, capacity, full-charge criteria, reserve, low-battery threshold, maximum polling interval, and display timeout. Keep settings on BattMon. Validate and save updates as a complete set, then return the applied settings so all phones reflect the module’s configuration.

## Feather display and buttons

**BM-019 — Display and local controls.** After the inactivity timeout configured in Android, turn off the LCD backlight and put the display in its supported low-power mode. Preserve the timeout across restarts. The first press of any button wakes the display, shows the latest readings, and restarts the inactivity timer. That press is consumed by wake. Monitoring and alerts continue while the display sleeps. BLE activity leaves it asleep.

| Button | Action while the display is awake |
| --- | --- |
| Home | Return to the summary showing estimated charge remaining, charge direction, and any active low-battery warning. |
| Next | Cycle through voltage, signed current, estimated amp-hours remaining, and estimated time to reserve. |
| Pair | Hold for three seconds to open time-limited pairing mode. Show the module identifier and time remaining. |

## Acceptance checks and open parameters

| Area | Acceptance check | Open parameters |
| --- | --- | --- |
| Measurement | Compare voltage and current with reference instruments, verify charge and discharge signs, exercise invalid sensor supply, and verify calibration survives a restart. | Allowable voltage and current errors and Idle threshold. |
| Polling and power | Compare accumulated amp-hours with a reference under steady loads, short bursts, charging, and idle. Verify adaptive timing and elapsed time across sleep. Measure assembly current with different display and BLE states. | Average current budgets, default and allowed maximum polling intervals, transition thresholds and sample count, and allowable amp-hour error. |
| Charge and runtime | Test complete and interrupted charge cycles for each profile. Verify a single qualifying reading cannot trigger full-charge synchronization. Test restart, missing measurements, battery replacement, reserve, and rested-voltage startup. | Battery-specific full-charge defaults, estimate checkpoint cadence, runtime averaging period, and uncertainty criteria. |
| Pairing and alerts | Pair and connect two phones, change settings from either, and verify both receive readings and alerts. Trigger and clear an alert with both phones out of range, then verify each gets one missed alert on reconnect. Include a module restart in the missed-alert test. | Pairing timeout, local clear-pairings action, final phone limits, and missed-alert persistence across module power loss. |
| Display | Verify the timeout, lower asleep current, wake with all three buttons, wake-press consumption, and each awake button action. Verify updates and alerts with the LCD asleep. | Default and allowable display timeout values. |

The hardware and firmware target is the Adafruit ESP32-S3 Reverse TFT Feather. Confirm the carrier-to-Feather wiring and button mapping against the build before finalizing firmware pin assignments. [Firmware board configuration](BattMonFirmware/platformio.ini)
