import cadquery as cq
from pathlib import Path
out=Path(__file__).parent
# ED120/XDS vendor drawing: pitch 5.08, depth 9, body height 12.5,
# total seating height 13, lead length 4, lead diameter 1.
# KiCad footprint pad 1 at origin; wire side is footprint +Y / model -Y.
body=cq.Workplane('XY').box(10.16,9,12.5,centered=(True,True,False)).translate((2.54,-.5,.5))
# Sloped upper front shoulder, approximate molded details.
wedge=cq.Workplane('YZ').polyline([(-5,9),(-5,13),(-3.6,13)]).close().extrude(12).translate((-3.1,0,0))
body=body.cut(wedge)
for x in (0,5.08):
    recess=cq.Workplane('XY').center(x,0).circle(2).extrude(3).translate((0,0,10.4))
    opening=cq.Workplane('XY').box(3.7,4.5,4.2,centered=(True,True,False)).translate((x,-3.6,2.8))
    body=body.cut(recess).cut(opening)
# Small interlocking tongue at end, within drawing's 0.7 protrusion.
body=body.union(cq.Workplane('XY').box(.7,1,6,centered=(False,True,False)).translate((-3.24,2.6,2)))
assy=cq.Assembly(name='OnShore_ED120_2DS_Approx')
assy.add(body,name='Blue_housing',color=cq.Color(.04,.22,.64))
for i,x in enumerate((0,5.08),1):
    pin=cq.Workplane('XY').center(x,0).circle(.5).extrude(4.6).translate((0,0,-4))
    clamp=cq.Workplane('XY').box(3.5,3.3,4.8,centered=(True,True,False)).translate((x,-.3,5.4))
    bore=cq.Workplane('XZ').center(x,6.6).circle(1.35).extrude(7,both=True)
    clamp=clamp.cut(bore)
    screw=cq.Workplane('XY').center(x,0).circle(1.65).extrude(.85).translate((0,0,10.45))
    slot=cq.Workplane('XY').box(4,.6,.5,centered=(True,True,False)).translate((x,0,11.0))
    screw=screw.cut(slot)
    assy.add(pin,name=f'Pin_{i}',color=cq.Color(.72,.75,.78))
    assy.add(clamp,name=f'Clamp_{i}',color=cq.Color(.65,.60,.37))
    assy.add(screw,name=f'Screw_{i}',color=cq.Color(.75,.77,.80))
assy.export(str(out/'OnShore_ED120_2DS.step'))
s=cq.importers.importStep(str(out/'OnShore_ED120_2DS.step')).val()
assert s.isValid()
b=s.BoundingBox()
print('Valid STEP. Bounding box:', b.xlen,b.ylen,b.zlen, 'solids',len(s.Solids()))
