screwHole_h = 6;

screwHead_h = 1.5;
screwHead_r = 2;
screwBracket_r = 1;

screwMakerbeam_r = 1.5;

width1 = 2.5;
width2 = 1;

fn = 180;

difference()
{
	intersection()
	{
        union()
        {
		    cube([47,15,width1],center=true);
		    translate([0,0,0.5*(width1+width2)]) cube([27,15,width2],center=true);
        }
        cylinder(r=23,h=5,center=true);
	}
	
	union()
	{
		translate([9,0,0])   cylinder($fn=fn,r=screwBracket_r  ,h=screwHole_h,center=true);
		translate([9,0,-0.5*width1])  cylinder($fn=fn,r=screwMakerbeam_r,h=screwHead_h,center=true);
		translate([-9,0,0])  cylinder($fn=fn,r=screwBracket_r  ,h=screwHole_h,center=true);
		translate([-9,0,-0.5*width1]) cylinder($fn=fn,r=screwMakerbeam_r,h=screwHead_h,center=true);

		translate([18,0,0])   cylinder($fn=fn,r=screwMakerbeam_r,h=screwHole_h,center=true);
		translate([-18,0,0])  cylinder($fn=fn,r=screwMakerbeam_r,h=screwHole_h,center=true);
	}
}
