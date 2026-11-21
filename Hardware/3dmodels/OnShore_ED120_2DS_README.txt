J1: On Shore Technology ED120/2DS / DigiKey ED1609-ND

Generated visual/mechanical STEP approximation using the manufacturer's ED120/XDS dimensional drawing:
https://www.sigmaelectronica.net/manuals/ED1610.pdf

Pitch 5.08 mm; body 10.16 x 9 x 12.5 mm plus approximate 0.7 mm interlock tongue. Housing top 13 mm above PCB, pins 4 mm below PCB, diameter 1 mm. Screw and wire-entry details are approximations, not production tooling geometry.

Designed for current BattMon:Terminal_2P_P5.08 footprint: pad 1 origin (0,0), pad 2 (5.08,0). Wire entry faces footprint +Y. In KiCad set scale XYZ to 1, offset XYZ to 0 mm, rotation XYZ to 0 degrees.

Copy OnShore_ED120_2DS.step to project's 3dmodels directory. In J1 Footprint Properties > 3D Models replace Terminal_2P_Generic.step with ${KIPRJMOD}/3dmodels/OnShore_ED120_2DS.step.

Current PCB placement, routing and library footprint have not been modified. The existing footprint has a shallower generic body outline than the actual 9 mm depth, so verify body clearance in the 3D viewer.
