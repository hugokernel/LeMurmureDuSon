
$fn = 20;

SIZE = 76;
THICKNESS = 1;

sizes = [
    [ 76.3, 76.5 ],     // 0
    [ 76.5, 76.2 ],     // 1
    [ 77.94, 77.94 ],   // 2
    [ 77.94, 77.94 ],   // 3
    [ 77.94, 76 ],      // 4
    [ 77.94, 76 ],      // 5
];

RIDGE_WIDTH = 8;
HOLE_DIAMETER = 2 + .5;

module holes() {
    rotate([0, 0, 45]) {
        for(pos = [
            [SIZE / 2 - RIDGE_WIDTH - 2, SIZE / 2 - RIDGE_WIDTH / 2, 0],
            [-SIZE / 2 + RIDGE_WIDTH + 2, SIZE / 2 - RIDGE_WIDTH / 2, 0],
            [SIZE / 2 - RIDGE_WIDTH - 2, -SIZE / 2 + RIDGE_WIDTH / 2, 0],
            [-SIZE / 2 + RIDGE_WIDTH + 2, -SIZE / 2 + RIDGE_WIDTH / 2, 0],
        ]) {
            translate(pos) {
                cylinder(r = HOLE_DIAMETER / 2, h = 100, center = true);

                if ($children) {
                    for (i = [0 : $children - 1]) {
                        child(0);
                    }
                }
            }
        }
    }
}

module bump(index, size = SIZE, thick = THICKNESS) {
    stl_width = 300;
    stl_thickness = 10;
    thickness = 1;

    coeff_w = stl_width / sizes[index][0];
    coeff_l = stl_width / sizes[index][1];
    coeff_t = stl_thickness / thick;

    translate([stl_width / coeff_w / 2, stl_width / coeff_l / 2, thickness / 2]) {
        scale([1 / coeff_w, 1 / coeff_l, 1 / coeff_t]) {
            import(str("pattern/", index, ".stl"));
        }
    }

    if (index == 5) {
        translate([0, 0, thickness / 2 - 0.4]) {
            cube(size = [sizes[index][0], sizes[index][1], 0.2], center = true);
        }
    } else {
        translate([0, 0, thickness / 2 - 0.5]) {
            cube(size = [sizes[index][0], sizes[index][1], 1], center = true);
        }
    }
}



module faces(which = -1, offset = 30) {

    nut_head_diameter = 4.6; // Before : 4: serré, 5: too large
    nut_head_height = 1.3;

    offset = offset + SIZE / 2;

    difference() {
        for (data = [
            [ 0, [0, 0, -offset],   [180, 0, 0], "RED" ],
            [ 1, [0, 0, offset],    [0, 0, 0]   ],
            [ 2, [offset, 0, 0],    [0, 90, 0], "BLUE" ],
            [ 3, [-offset, 0, 0],   [0, -90, 0] ],
            [ 4, [0, -offset, 0],   [90, 0, 0]  ],
            [ 5, [0, offset, 0],    [-90, 0, 0] ]
        ]) {
            if (which == -1 || which == data[0]) {
                translate(data[1]) {
                    rotate(data[2]) {
                        //color(data[3])
                        bump(data[0]);
                    }
                }
            }
        }

        rotate([0, 0, 45]) {
            holes() {
                if (which == 0) {
                    translate([0, 0, -(SIZE / 2 + nut_head_height + THICKNESS)]) {
                        cylinder(r = nut_head_diameter / 2, h = 2);
                    }
                } else if (which == 1) {
                    translate([0, 0, SIZE / 2 - nut_head_height + THICKNESS]) {
                        cylinder(r = nut_head_diameter / 2, h = 2);
                    }
                }
            }
        }
    
        if (which == 1) {
            // Mic hole
            translate([0, -20, 0]) {
                cylinder(r = 2, h = 200);
            }
        }
    }
}

//bump(1, SIZE);

%cube(size = [SIZE, SIZE, SIZE], center = true);

faces(4, 0);


