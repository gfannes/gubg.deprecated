function main()
{
    var screwHole_h = 5;
    var screwHead_h = 1.5;
    var screwHead_r = 2;
    var screwBracket_r = 1;
    var screwMakerbeam_r = 1.5;
    var width1 = 2.5;
    var width2 = 1;
    var fn = 180;

    return difference(
            intersection(
                union(
                    cube({size: [47,15,width1], center: true}),
                    cube({size: [27,15,width2], center: true}).translate([0,0,0.4*(width1+width2)])
                    ),
                cylinder({r: 23, h: 5, center: true})
                ),
            union(
                cylinder({fn: fn,r: screwBracket_r  ,h: screwHole_h,center: true}).translate([9,0,0])           ,
                cylinder({fn: fn,r: screwMakerbeam_r,h: screwHead_h,center: true}).translate([9,0,-0.5*width1]) ,
                cylinder({fn: fn,r: screwBracket_r  ,h: screwHole_h,center: true}).translate([-9,0,0])          ,
                cylinder({fn: fn,r: screwMakerbeam_r,h: screwHead_h,center: true}).translate([-9,0,-0.5*width1]),

                cylinder({fn: fn,r: screwMakerbeam_r,h: screwHole_h,center: true}).translate([18,0,0]),
                cylinder({fn: fn,r: screwMakerbeam_r,h: screwHole_h,center: true}).translate([-18,0,0])
                )
            );
}

