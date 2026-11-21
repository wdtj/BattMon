# BattMon DKRed board setup

Configured for 2 copper layers, nominal 1.60 mm FR4 TG170-180, 1 oz (0.035 mm) copper per side, red soldermask, white silkscreen, ENIG. Existing outline, footprints, placement, nets and routing preserved. Stackup core thickness is a nominal model (1.51 mm plus copper/mask); dielectric constants are placeholders, not a controlled-impedance manufacturer stackup.

Official sources checked 2026-11-17:
- https://www.digikey.com/en/resources/design-tools/pcb-builder-technical-specs
- https://www.digikey.com/en/resources/dkred

DKRed publishes 5 mil trace/space, 8 mil minimum drill/via hole, 16 mil minimum via pad, and 245 mil (6.22 mm) maximum drill. Project limits are deliberately more generous: minimum copper clearance 0.20 mm, minimum track width 0.20 mm, drill minimum 0.30 mm, edge clearance 0.50 mm, hole/copper clearance 0.25 mm and hole/hole clearance 0.25 mm. Minimum via diameter 0.60 mm, via annular ring 0.15 mm. All dimensions below in mm.

| Netclass | Preferred trace width | Clearance | Via pad/drill | Assigned nets |
|---|---:|---:|---|---|
| Default | 0.25 | 0.25 | 0.8/0.4 | Unassigned and unused pins |
| Analog | 0.25 | 0.30 | 0.8/0.4 | HALL_OUT, HALL_ADC, VBAT_ADC, V_DIV_MID, V5_ADC |
| I2C | 0.25 | 0.25 | 0.8/0.4 | SDA, SCL |
| Power_3V3 | 0.50 | 0.25 | 0.8/0.4 | 3V3 |
| Power_1A | 1.00 | 0.30 | 1.0/0.5 | BAT_FUSED, VBAT_PROT, 5V, 5V_ISO, 5V_ESP |
| Switching | 1.00 | 0.40 | 1.0/0.5 | SW |
| Ground | 1.00 | 0.25 | 1.0/0.5 | GND |

Routing widths are defaults, not DRC minimum widths for each class. Select Use Netclass Width in the router. Short neckdowns where necessary are permitted down to the global 0.20 mm limit; do not use this minimum for long power traces. Use the 1 mm power width or wider copper for main current paths; size any narrow section by its actual current and length. Main board current is the monitor's approximately 1 A supply budget, not the current flowing through the external Hall sensor's aperture. Avoid vias in the SW/current-loop paths. If a main-current layer change is needed, use a suitable group of power vias and review the return path.

Zone defaults: clearance 0.25 mm (netclass clearance can increase this), minimum fill thickness 0.25 mm, thermal gap 0.30 mm, spoke width 0.50 mm. These set future-zone defaults; no copper pours were added. Keep the power-current return short and away from analog returns. Preserve Feather antenna keepout; no antenna keepout was inferred or added.

Routing presets: 0.25, 0.30, 0.50, 0.75, 1.00, 1.50, 2.00 mm; vias 0.8/0.4 and 1.0/0.5. Silk defaults 1.0 mm height, 0.15 mm stroke/line, 0.15 mm clearance. Existing footprint silkscreen may trigger warnings and needs review.

BattMon.kicad_dru adds maximum/minimum drill and 0.10 mm plated annular-width checks. It deliberately flags undersized imported pads. In particular, U1 currently has 1.275 mm pad width and 1.10 mm drill: narrow-axis ring is only 0.0875 mm, below this conservative 0.10 mm rule. Review U1 lead/drill dimensions and enlarge pads or otherwise correct the library/board footprint before manufacture; this setup does not alter pad geometry or hide the violation. Other imported footprints should be checked by DRC too.

Validation: project JSON parses; all 15 connected named nets mapped exactly once; schematic/ERC settings preserved. Board edits add stackup metadata only. Stackup, project settings and custom-rule syntax tested by KiCad 9 DRC on a native test board. Full BattMon DRC still must run in KiCad 10; no full-board DRC pass is claimed. Unrouted nets will remain errors until routed.

After Drive synchronization, close and reopen the BattMon project to reload project settings and the .kicad_dru file. Avoid saving an older open project over these updated settings. Fabricator ordering options still need to be selected in PCB Builder; settings here do not place an order.
