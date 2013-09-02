
use <lib/Spiff.scad>;

$fn = 20;

SIZE = 76;
RIDGE_WIDTH = 8;

THICKNESS = 3;

HOLE_DIAMETER = 2 + .5;

ELEC_WIDTH = 40;
ELEC_LENGTH = 42.3;
ELEC_THICKNESS = 0.8;

ELEC_SUPPORT_WIDTH = 35;
ELEC_SUPPORT_LENGTH = 37;
ELEC_HOLE_DIAMETER = 3;

GY27_WIDTH = 15.55;
GY27_LENGTH = 32.55;
GY27_THICKNESS = 1.6;
GY27_HEIGHT = 3.8;
GY27_HOLE_DIAMETER = 3;

REFLECTOR_HEIGHT = 12;

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

module electronic() {
    difference() {
        cube(size = [ELEC_WIDTH, ELEC_LENGTH, ELEC_THICKNESS], center = true);

        for(pos = [
            [ELEC_SUPPORT_WIDTH / 2, ELEC_SUPPORT_LENGTH / 2, -5],
            [-ELEC_SUPPORT_WIDTH / 2, ELEC_SUPPORT_LENGTH / 2, -5],
            [ELEC_SUPPORT_WIDTH / 2, -ELEC_SUPPORT_LENGTH / 2, -5],
            [-ELEC_SUPPORT_WIDTH / 2, -ELEC_SUPPORT_LENGTH / 2, -5],
        ]) {
            translate(pos) {
                cylinder(r = ELEC_HOLE_DIAMETER / 2, h = 10);
            }
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

module main(side = -1) {

    offset = 10;

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
        main(side);
    }
}

if (1) {

    main();

} else {

    getside(5);
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

