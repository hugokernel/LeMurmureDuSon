
use <lib/Spiff.scad>;

$fn = 20;

SIZE = 76;
RIDGE_WIDTH = 8;

THICKNESS = 3;

HOLE_DIAMETER = 2 + .5;

ELEC_WIDTH = 42.3;
ELEC_LENGTH = 40;
ELEC_THICKNESS = 0.8;

ELEC_SUPPORT_WIDTH = 37;
ELEC_SUPPORT_LENGTH = 35;
ELEC_HOLE_DIAMETER = 3;

GY27_WIDTH = 15.55;
GY27_LENGTH = 32.55;
GY27_THICKNESS = 1.6;
GY27_HEIGHT = 3.8;
GY27_HOLE_DIAMETER = 3;

REFLECTOR_HEIGHT = 12;

DEBUG = false;

module gy27() {
    difference() {
        cube(size = [GY27_WIDTH, GY27_LENGTH, GY27_THICKNESS], center = true);
        for (pos = [
            [GY27_WIDTH / 2 - 2, GY27_LENGTH / 2 - 3, 0],
            [-GY27_WIDTH / 2 + 2, GY27_LENGTH / 2 - 3, 0],
        ]) {
            translate(pos) {
                cylinder(r = GY27_HOLE_DIAMETER / 2, h = 10, center = true);
            }
        }
    }
}

module electronic(version = 1, diff = false) {
    module bluetooth() {
        width = 10;
        length = 15;
        translate([-ELEC_WIDTH / 2, -ELEC_LENGTH / 2 + 16 / 2 - 0.5, 0]) {
            cube(size = [width, length, ELEC_THICKNESS], center = true);
        }
    }

    module holes() {
        for(pos = [
            [ELEC_SUPPORT_WIDTH / 2, ELEC_SUPPORT_LENGTH / 2, -5, 2],
            [-ELEC_SUPPORT_WIDTH / 2, ELEC_SUPPORT_LENGTH / 2, -5, 2],
            [ELEC_SUPPORT_WIDTH / 2, -ELEC_SUPPORT_LENGTH / 2, -5, 2],
            [-ELEC_SUPPORT_WIDTH / 2, -ELEC_SUPPORT_LENGTH / 2, -5, 1],
        ]) {
            if (pos[3] >= version) {
                translate([pos[0], pos[1], -25]) {
                    cylinder(r = ELEC_HOLE_DIAMETER / 2, h = 50);
                }

                translate([pos[0], pos[1], 0]) {
                    child(0);
                }
            }
        }
    }

    module body() {
        cube(size = [ELEC_WIDTH, ELEC_LENGTH, ELEC_THICKNESS], center = true);
        if (version == 2) {
            bluetooth();
        }
    }

    if (diff) {
        body(13);
        holes() {
            child(0);
        };
    } else {
        difference() {
            body();
            holes();
        }
    }
}

LIPO_WIDTH = 40;
LIPO_LENGTH = 65;
LIPO_THICKNESS = 7;

module lipo() {
    cube(size = [LIPO_LENGTH, LIPO_WIDTH, LIPO_THICKNESS], center = true);
}

SPEAKER_DIAMETER = 28.5;
module speaker() {
    thickness = 5;
    clear = 0.5;

    module holes() {
        for (pos = [
            [31.7 / 2, 0, 0],
            [-31.7 / 2, 0, 0],
        ]) {
            translate(pos) {
                child(0);
            }
        }
    }

    hull() {
        cylinder(r = SPEAKER_DIAMETER / 2, h = thickness, center = true);
        holes() {
            cylinder(r = 3.5, h = thickness, center = true);
        }
    }

    holes() {
        child(0);
    }
}

module vibrator() {
    translate([29, -25, 0]) {
        cylinder(r = 1.5, h = 10, center = true);
        translate([0, 12, 0]) {
            cylinder(r = 1.5, h = 10, center = true);
        }

        hull() {
            translate([0, 0, -2]) {
                cylinder(r = 3.5, h = 5, center = true);
            }

            translate([0, 12, -2]) {
                cylinder(r = 3.5, h = 5, center = true);
            }
        }
    }
}

module struct() {

    thickness = 4;
    height = SIZE - RIDGE_WIDTH * 2;
    size = sqrt(pow(height, 2) + pow(height, 2));
    ear = 20;

    lipo_support_thickness = 3;

    module lipo_support() {
        translate([0, -LIPO_WIDTH / 2 - lipo_support_thickness / 2 + 2, - thickness / 2 - LIPO_THICKNESS / 2]) {
            cube(size = [LIPO_LENGTH + thickness, lipo_support_thickness + thickness, LIPO_THICKNESS + thickness], center = true);
        }
    }

    module _speaker() {
        translate([25, 5, 6]) {
            cylinder(r = SPEAKER_DIAMETER / 2, h = 50, center = true);
            rotate([0, 0, 45]) {
                color("ORANGE") speaker() {
                    child(0);
                }
            }

            /*
            translate([8, SPEAKER_DIAMETER / 2 + 5, 0]) {
                cylinder(r = 3, h = 50, center = true);
            }
            */
        }
    }

    rotate([90, 0, 45]) {
        difference() {

            union() {
                // Helper
                //%cube(size = [size, height, 1], center = true);

                cube(size = [size, height - REFLECTOR_HEIGHT - 2, thickness], center = true);
                for (pos = [
                    [size / 2 - ear / 2, height / 2 - ear / 2, 0],
                    [-size / 2 + ear / 2, height / 2 - ear / 2, 0],
                    [size / 2 - ear / 2, -height / 2 + ear / 2, 0],
                    [-size / 2 + ear / 2, -height / 2 + ear / 2, 0]
                ]) {
                    translate(pos) {
                        cube(size = [ear, ear, thickness], center = true);
                    }
                }

                if (DEBUG) {
                    translate([-12, 0, thickness]) {
                        color("GREEN") electronic(2);
                    }
                }

                if (DEBUG) {
                    translate([0, 0, - thickness / 2 - LIPO_THICKNESS / 2]) {
                        //%color("GREY") lipo();
                    }
                }

                lipo_support();

                if (DEBUG) {
                    rotate([90, 45, 0]) {
                        translate([-22, 10, height / 2]) {
                            color("BLUE") gy27();
                        }
                    }
                }

                //_speaker();
            }


            for (pos = [
                [0, SIZE / 2 - RIDGE_WIDTH / 2, 0, 45, 90],
                [0, - SIZE / 2 + RIDGE_WIDTH / 2, 0, -45, 90],
                [0, SIZE / 2 - RIDGE_WIDTH / 2, 0, -45, 90],
                [0, - SIZE / 2 + RIDGE_WIDTH / 2, 0, 45, 90]
            ]) {
                rotate([pos[3], 0, pos[4]]) {
                    translate([pos[0], pos[1], pos[2]]) {
                        cube(size = [SIZE, RIDGE_WIDTH, SIZE], center = true);
                    }
                }
            }

            translate([-12, 0, thickness]) {
                electronic(2, true) {
                    union() {
                        translate([0, 0, -7]) {
                            cylinder(r = 3.5, h = 5, center = true);
                        }
                    }
                }
            }

            translate([0, 0, - thickness / 2 - LIPO_THICKNESS / 2 - .005]) {
                lipo();
            }

            translate([0, 0, -3]) {
                _speaker() {
                    union() {
                        cylinder(r = 1.5, h = 50, center = true);
                        translate([0, 0, -6]) {
                            cylinder(r = 3.5, h = 5, center = true);
                        }
                    }
                }
            }

            vibrator();
        }
    }
}

module front(index, connection) {
    edge_size = SIZE / 2 * sqrt(2);

    module plot(add, substract = true) {
        width = 4;
        length = 10;
        height = 8;

        clear = 0.4;
        clear_height = 8;

        module _plot(clear = 1, clear_height = 1) {
            //cube(size = [length + clear, width + clear, height + clear_height], center = true);
            //cube(size = [length - 5 + clear, width + 2 + clear, height + clear_height], center = true);

            scale([clear, clear, clear_height]) {
                translate([0, -4, 0.5]) {
                    linear_extrude(height = height, center = true) {
                        //polygon([[-5,0],[-3,0],[-3,-1],[3,-1],[3,0],[5,0],[5,3],[0,5],[-5,3]]);
                        polygon([[0,1],[5,4],[0,7],[-5,4]]);
                    }
                }
            }
        }

        for (data = [
            [0, SIZE / 2 - RIDGE_WIDTH / 2, 4.5],
            [20, SIZE / 2 - RIDGE_WIDTH / 2, 4.5],
            [-20, SIZE / 2 - RIDGE_WIDTH / 2, 4.5],
        ]) {
            translate(data) {
                if (add && !substract) {
                    _plot();
                } else if (substract) {
                    rotate([90, 0, 0]) {
                        //_plot(clear, clear_height);
                        _plot(1.1, 1.3);
                    }
                }
            }
        }
    }

    module blocker(connection, substract = false) {
        data = [0, 90, 180, 270];
        for (i = [0 : 3]) {
            rotate([0, 0, 45 + data[i]]) {
                if (substract && !connection[i]) {
                    plot(connection[i], substract);
                } else if (!substract && connection[i]) {
                    plot(connection[i], substract);
                }
            }
        }
    }

    module reflector(fill = false) {
        edge_thickness = 0.2;
        hole_width = 5.8;
        size = ((SIZE - RIDGE_WIDTH * 2) / 2) * sqrt(2) + 2;

        if (fill) {
            translate([0, 0, 1.3]) {
                rotate([0, 0, 45]) {
                    cylinder(r1 = hole_width / sqrt(2), r2 = size, h = REFLECTOR_HEIGHT + 1.01, $fn = 4, center = true);
                }
            }
        } else {
            difference() {
                union() {
                    rotate([0, 0, 45]) {
                        cylinder(r1 = 5, r2 = size, h = REFLECTOR_HEIGHT, $fn = 4, center = true);
                    }

                    translate([0, 0, -REFLECTOR_HEIGHT + 8.5]) {
                        cube(size = [13, 13, 5], center = true);
                    }
                }

                translate([0, 0, 1.3]) {
                    rotate([0, 0, 45]) {
                        cylinder(r1 = hole_width / sqrt(2), r2 = size, h = REFLECTOR_HEIGHT + 1.01, $fn = 4, center = true);
                    }
                }

                translate([0, 0, 1]) {
                    cube(size = [hole_width, hole_width, 50], center = true);
                }
            }
        }
    }

    module holes_top() {
        rotate([0, 0, 45]) {
            for(pos = [
                [SIZE / 2 - RIDGE_WIDTH - 2, SIZE / 2 - RIDGE_WIDTH / 2, 0],
                [-SIZE / 2 + RIDGE_WIDTH + 2, SIZE / 2 - RIDGE_WIDTH / 2, 0],
                [SIZE / 2 - RIDGE_WIDTH - 2, -SIZE / 2 + RIDGE_WIDTH / 2, 0],
                [-SIZE / 2 + RIDGE_WIDTH + 2, -SIZE / 2 + RIDGE_WIDTH / 2, 0],
            ]) {
                translate(pos) {
                    cylinder(r = HOLE_DIAMETER / 2, h = 50, center = true);
                }
            }
        }
    }

    module holes_test() {
        holes_clear = 0.1;
        rotate([90, 0, 45]) {
            for(pos = [
                [SIZE / 2 - RIDGE_WIDTH - 2, RIDGE_WIDTH / 2, 0],
                [-SIZE / 2 + RIDGE_WIDTH + 2, RIDGE_WIDTH / 2, 0],
            ]) {
                translate(pos) {
                    cylinder(r = HOLE_DIAMETER / 2 - holes_clear, h = 150, center = true);
                }
            }
        }
    }

    module mark(index) {
        diameter = 3;
        for (i = [1 : index]) {
            rotate([0, 0, 45]) {
                translate([- SIZE / 2 + i * diameter * 2, SIZE / 2 - RIDGE_WIDTH / 2, 0]) {
                    cylinder(r = diameter / 2, h = 2, center = true, $fn = 3);
//                translate([- SIZE / 2 + i * diameter * 2, SIZE / 2 - RIDGE_WIDTH / 2 + 3, 0]) {
                    //cube(size = [3, 2, 1]);
                }
            }
        }
    }

    if (connection[0] || connection[1] || connection[2] || connection[3]) {
        blocker(connection, false);
    }

    difference() {
        union() {
            difference() {
                cylinder(r1 = edge_size, r2 = edge_size - RIDGE_WIDTH / 2 * sqrt(2) * 2, h = RIDGE_WIDTH, $fn = 4);

                translate([0, 0, 6.4]) {
                    rotate([0, 180, 45]) {
                        reflector(true);
                    }
                }
            }

            translate([0, 0, 6.4]) {
                rotate([0, 180, 45]) {
                    reflector();
                }
            }
        }

        blocker(connection, true);

        // Top holes
        if (index == 1) {
            holes_top();
        }

        if (index == 2) {
            holes_test();
        }

        if (index == 3) {
            holes_test();
        }

        if (index) {
            mark(index);
        }
    }
}

module main(side = -1, offset = 10) {

    for (data = [
        [0, [0, 0, -SIZE / 2 - offset], [0, 0, 45], [true, true, true, true]], // Bottom
        [1, [0, 0, SIZE / 2 + offset], [180, 0, 45], [true, true, true, true]],    // Top
        [2, [0, SIZE / 2 + offset, 0], [90, 45, 0], [false, true, false, true]],
        [3, [0, -SIZE / 2 - offset, 0], [-90, 135, 0], [false, true, false, true]],
        [4, [-SIZE / 2 - offset, 0, 0], [90, 45, 90], [false, false, false, false]],
        [5, [SIZE / 2 + offset, 0, 0], [90, 45, -90], [false, false, false, false]],
    ]) {
        if (side < 0 || side == data[0]) {
            translate(data[1]) {
                rotate(data[2]) {
                    front(data[0], data[3]);
                }
            }
        }
    }
}

module getside(side) {
    data = [
        [0, 0, 0],
        [180, 0, 0],
        [-90, 0, 0],
        [90, 0, 0],
        [0, -90, 0],
        [0, 90, 0]
    ];

    rotate(data[side]) {
        main(side, offset = 0);
    }
}

module getsides(sides, offset = 10) {
    for (side = sides) {
        main(side, offset);
    }
}

if (0) {

    main();

} else {

    //getside(0);
    //getsides([0, 2, 4], 0);

    struct();

/*
    intersection() {
        translate([0, SIZE / 2, -SIZE / 2]) {
            cube(size = [100, 30, 20], center = true);
        }

        rotate([0, 0, 180]) {
            getside(3);
        }
    }

    translate([0, -50, 0]) {
        intersection() {
            translate([0, SIZE / 2, -SIZE / 2]) {
                cube(size = [100, 30, 20], center = true);
            }
            getside(1);
        }
    }
*/
}

