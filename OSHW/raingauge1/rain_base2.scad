use<triangles.scad>

$fn = 30;

module base(length, width, height)
{
	difference() {
		union() {
			translate([-width/2, -length/2, -2]) cube([width,length,2]);
			translate([width/2-2, 0, 0])  rotate(a=90, v=[0,1 ,0 ]) rotate(a=90, v=[0,0 ,1 ]) Isosceles_Triangle(b=length/2, H=height);
			translate([width/2-1, 0, 0])  rotate(a=90, v=[0,1 ,0 ]) rotate(a=90, v=[0,0 ,1 ]) Isosceles_Triangle(b=length/2, H=height);
			translate([-width/2, 0, 0])  rotate(a=90, v=[0,1 ,0 ]) rotate(a=90, v=[0,0 ,1 ]) Isosceles_Triangle(b=length/2, H=height);
			translate([-width/2+1, 0, 0])  rotate(a=90, v=[0,1 ,0 ]) rotate(a=90, v=[0,0 ,1 ]) Isosceles_Triangle(b=length/2, H=height);

			// big support columns
			color("green") translate([width/2-3.5,5,0]) rotate(a=270, v=[0,1 ,0 ]) rotate(a=60, v=[1,0 ,0 ]) Right_Angled_Triangle(a=6, b=height-16, centerXYZ=[false,true,true]);
			color("green") translate([width/2-3.5,-5,0]) rotate(a=270, v=[0,1 ,0 ]) rotate(a=120, v=[1,0 ,0 ]) Right_Angled_Triangle(a=6, b=height-16, centerXYZ=[false,true,true]);
			color("green") translate([-width/2+3.5,5,0]) rotate(a=270, v=[0,1 ,0 ]) rotate(a=300, v=[1,0 ,0 ]) Right_Angled_Triangle(a=6, b=height-16, centerXYZ=[false,true,true]);
			color("green") translate([-width/2+3.5,-5,0]) rotate(a=270, v=[0,1 ,0 ]) rotate(a=240, v=[1,0 ,0 ]) Right_Angled_Triangle(a=6, b=height-16, centerXYZ=[false,true,true]);
			// small support columns
			// color("green") translate([width/2-3.5,length/2-11,0]) rotate(a=270, v=[0,1 ,0 ]) rotate(a=90, v=[1,0 ,0 ]) Right_Angled_Triangle(a=5, b=height/4, centerXYZ=[fasle,true,true]);
			// color("green") translate([-width/2+3.5,length/2-11,0]) rotate(a=270, v=[0,1 ,0 ]) rotate(a=270, v=[1,0 ,0 ]) Right_Angled_Triangle(a=5, b=height/4, centerXYZ=[false,true,true]);
			// color("green") translate([width/2-3.5,-length/2+11,0]) rotate(a=270, v=[0,1 ,0 ]) rotate(a=90, v=[1,0 ,0 ]) Right_Angled_Triangle(a=5, b=height/4, centerXYZ=[fasle,true,true]);
			// color("green") translate([-width/2+3.5,-length/2+11,0]) rotate(a=270, v=[0,1 ,0 ]) rotate(a=270, v=[1,0 ,0 ]) Right_Angled_Triangle(a=5, b=height/4, centerXYZ=[false,true,true]);

			// calibration posts
			color("blue") translate([0, length/2-20, 0]) cylinder(h=4.5, r=3, center=false);
			color("blue") translate([0, -length/2+20, 0]) cylinder(h=4.5, r=3, center=false);
		}
		// hole for the bucket rod
		color("red") translate([0, 0, height-14]) rotate(a=90, v=[0,1 ,0 ]) cylinder(h=width+5, r=1, center=true);
		color("red") translate([0, 0, height-7]) rotate(a=90, v=[0,1 ,0 ]) cylinder(h=width+5, r=1, center=true);
		color("red") translate([0, 2.5, height-10]) rotate(a=90, v=[0,1 ,0 ]) cylinder(h=width+5, r=1, center=true);
		color("red") translate([0, -2.5, height-10]) rotate(a=90, v=[0,1 ,0 ]) cylinder(h=width+5, r=1, center=true);
		// calibration posts holes
		color("red") translate([0, length/2-20, -3]) cylinder(h=9, r=1.5, center=false);
		color("red") translate([0, -length/2+20, -3]) cylinder(h=9, r=1.5, center=false);
		// mounting holes
		color("red") translate([width/2-7, length/2-5, -3]) cylinder(h=4, r=2, center=false);
		color("red") translate([width/2-7, -length/2+5, -3]) cylinder(h=4, r=2, center=false);
		color("red") translate([-width/2+7, length/2-5, -3]) cylinder(h=4, r=2, center=false);
		color("red") translate([-width/2+7, -length/2+5, -3]) cylinder(h=4, r=2, center=false);
		// cut to top off the piramides
		color("red") translate([0, 0, height-1]) cube([width+2,10,5],center = true);
	}
}


base(60,30,30);