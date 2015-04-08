$fn = 18;

// lenght & width are inside sizes
module base(length, width, height)
{
	wall=0.5;
	holesize=2;
	step = width/(holesize*holesize*2);	
	// echo(step);
	difference() {
		union() {
			// bottom
			echo("lenght:", length);
			echo("width:", width);			
			cube([length+2*wall,width+2*wall,1],center = true);
			// sides - long
			color("gray") translate([0,width/2+wall-(wall/2), height/2]) rotate([90,0 ,0 ]) cube([length+wall*2,height, wall], center=true);
			color("gray") translate([0,-width/2-wall+(wall/2) , height/2]) rotate([90,0 ,0 ]) cube([length+wall*2,height, wall], center=true);
			// sides - short
			translate([-length/2-wall+(wall/2),0 , height/2]) rotate([0,0 ,90 ])  rotate([90,0 ,0 ]) cube([width+wall,height, wall], center=true);
			translate([length/2+wall-(wall/2),0 , height/2]) rotate([0,0 ,90 ])  rotate([90,0 ,0 ])  cube([width+wall,height, wall], center=true);
			// corners - vertical
			// color("blue") translate([length/2+wall-0.5,width/2+wall-0.5 , 0]) cylinder(r=0.5, h=height, center=false);
			// color("blue") translate([-length/2-wall+0.5,width/2+wall-0.5 , 0]) cylinder(r=0.5, h=height, center=false);
			// color("blue") translate([length/2+wall-0.5,-width/2-wall+0.5 , 0]) cylinder(r=0.5, h=height, center=false);
			// color("blue") translate([-length/2-wall+0.5,-width/2-wall+0.5 , 0]) cylinder(r=0.5, h=height, center=false);

		}
		//mounting holes
		color("red") translate([25,8, 0]) cylinder(h=3, r=2, center=true);
		color("red") translate([-25,8, 0]) cylinder(h=3, r=2, center=true);
		color("red") translate([25,-8, 0]) cylinder(h=3, r=2, center=true);
		color("red") translate([-25,-8, 0]) cylinder(h=3, r=2, center=true);

		// drain holes
		/*
		for (i = [-width/2+step:step:width/2-step]) {
			color("red") translate([length/2,i,2.5]) rotate([0,90,0]) cylinder(h=3*wall, r=holesize, center=true);
			color("red") translate([-length/2,i,2.5]) rotate([0,90,0]) cylinder(h=3*wall, r=holesize, center=true);
		}
		for (i = [-length/2+2*wall:8*wall:length/2-2*wall]) {
			color("red") translate([i,width/2,3]) rotate([90,0,0]) cylinder(h=3*wall, r=1, center=true);
			color("red") translate([i,-width/2,3]) rotate([90,0,0]) cylinder(h=3*wall, r=1, center=true);
		}
		// cable holes
		color("red") translate([0,-width/2,10])  rotate([90,0,0]) cylinder(h=4*wall, r=3, center=true);
		color("red") translate([0,width/2,10])  rotate([90,0,0]) cylinder(h=4*wall, r=3, center=true);
		*/

		// mounting holes
		// color("red") translate([length/2-10,-width/2,height-5])  rotate([90,0,0]) cylinder(h=4*wall, r=2, center=true);
	}
}

base(63,45,60);