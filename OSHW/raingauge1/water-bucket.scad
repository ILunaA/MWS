use<triangles.scad>

$fn = 30;

module bucket(length, width, height)
{
	difference() {
		union() {
			translate([-width/2, -length/2, -1]) cube([width,length,1]);
			translate([width/2-1, 0, 0])  rotate(a=90, v=[0,1 ,0 ]) rotate(a=90, v=[0,0 ,1 ]) Isosceles_Triangle(b=length, H=height);
			translate([-width/2, 0, 0])  rotate(a=90, v=[0,1 ,0 ]) rotate(a=90, v=[0,0 ,1 ]) Isosceles_Triangle(b=length, H=height);

			// vertical schot
			color("blue") translate([0, 0, height-5]) rotate(a=90,v=[1,0,0]) cube([width-2,height-5,1],center = true);

			// inside angles
			color("green") translate([0, -2.5, 2.5]) rotate(a=45,v=[1,0,0]) cube([width-2,length/8+2,1],center = true);
			color("green") translate([0, 2.5, 2.5]) rotate(a=-45,v=[1,0,0]) cube([width-2,length/8+2,1],center = true);

			// sensor arm
			color("green") translate([width/2-0.5, 0, height]) rotate(a=90,v=[0,1,0]) cube([width/2+2,length/8+2,1],center = true);

			//color("blue") translate([0, 0, 0]) rotate(a=90, v=[0,1 ,0 ]) cylinder(h=width, r=3, center=true);

		}
	// remove bottom parts
	color("red") translate([0, 0, -3]) cube([width+2,length+2,4], center = true);
	// whole for shaft
	color("red") translate([0, 0, 1]) rotate(a=90, v=[0,1 ,0 ]) cylinder(h=width+2, r=1, center=true);
	}
}

bucket(50,15,15);
