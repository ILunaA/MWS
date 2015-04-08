$fn = 18;

module cone(length, width, height, funnel, edge)
{
	difference() {
		union() {
			polyhedron(
				points = [
					[length/2, width/2, 0],
					[length/2, -width/2, 0],
					[-length/2, -width/2, 0],
					[-length/2, width/2, 0],
					[0, 0, height]
				],
				triangles = [
					[3, 2, 1, 0],
					[1, 4, 0],
					[2, 4, 1],
					[3, 4, 2],
					[0, 4, 3]
				]
			);
			// Funnel part
			color("blue") translate([0, 0, height-funnel/2]) cylinder(r=funnel/2,h=funnel-1,center = true);
		}
		// Remove center of pyramide
		translate([0, 0, -1])
		polyhedron(
			points = [
				[length/2, width/2, 0],
				[length/2, -width/2, 0],
				[-length/2, -width/2, 0],
				[-length/2, width/2, 0],
				[0, 0, height]
			],
			triangles = [
				[3, 2, 1, 0],
				[1, 4, 0],
				[2, 4, 1],
				[3, 4, 2],
				[0, 4, 3]
			]
		);
		// Funnel part hole
		color("red") translate([0, 0, height-funnel/2]) cylinder(r=2,h=funnel+2,center = true);
	}
	// Edge
	difference() {
		echo("lenght:", length+(edge*2));
		echo("width:", width+(edge*2));
		color("green")  translate([0, 0, 0]) cube([length+(edge*2), width+(edge*2), 1], center=true)	;
		color("blue")  translate([0, 0, -0.05]) cube([length-2, width-1, 1.2], center=true)	;
	}
	// Grill
	step1=length/4;
	step2=width/4;
	union() {
		for (i = [-length/2+step1:step1:length/2-step1]) {
			translate([i,0 ,0 ]) cube([1, width, 1], center=true);
		}

		for (j = [-width/2+step2:step2:width/2-step2]) {
		 translate([0,j ,0 ]) rotate([0, 0, 90])  cube([1, length, 1], center=true);
		}
	}
	// Lip
	difference() {
		translate([0, 0, edge/2]) cube([length+edge*2,width+edge*2,edge], center=true);
		translate([0, 0, edge/2]) cube([length+edge*2-1,width+edge*2-1,edge+1], center=true);
	}
}

cone(55,37,30, 5, 5);