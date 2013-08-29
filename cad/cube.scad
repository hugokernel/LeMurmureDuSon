
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

/*
module electronic_support() {
    width = 48;
    thickness = 3;
    offset = 15;

    module gy27_pos(offset = 0) {
        translate([-15 + offset, -23, 0]) {
            rotate([90, 45, 90]) {
                child(0);
            }
        }
    }

    difference() {
        cube(size = [width, 100, thickness], center = true);

        translate([0, offset, 0]) {
            cube(size = [ELEC_WIDTH, ELEC_LENGTH, 10], center = true);
        }

        for(pos = [
            [-5, -15, -5],
            [-5, -35, -5],
            [5, -35, -5],
            [5, -15, -5],
            [15, -15, -5],
            [15, -35, -5]
        ]) {
            translate(pos) {
                cylinder(r = 2, h = 10);
            }
        }

        gy27_pos() {
            cube(size = [GY27_WIDTH, GY27_LENGTH, GY27_HEIGHT], center = true);
            %gy27();
        }
    }

    gy27_pos(2.6) {
        gy27();
    }

    for(pos = [
        [ELEC_SUPPORT_WIDTH / 2, ELEC_SUPPORT_LENGTH / 2 + offset, 0],
        [-ELEC_SUPPORT_WIDTH / 2, ELEC_SUPPORT_LENGTH / 2 + offset, 0],
        [ELEC_SUPPORT_WIDTH / 2, -ELEC_SUPPORT_LENGTH / 2 + offset, 0],
        [-ELEC_SUPPORT_WIDTH / 2, -ELEC_SUPPORT_LENGTH / 2 + offset, 0],
    ]) {
        translate(pos) {
            difference() {
                //cylinder(r = 3.7, h = thickness, center = true);
                cylinder(r = 6.7, h = thickness, center = true, $fn = 6);

                translate([0, 0, -5]) {
                    cylinder(r = ELEC_HOLE_DIAMETER / 2, h = 10);
                }
            }
        }
    }
}

module holes() {
    hole_offset = RIDGE_WIDTH - 2.4;

    for (rot = [
        [0, 0, 0],
        [0, 90, 0],
        [90, 0, 0]
    ]) {
        rotate(rot) {
            for(pos = [
                [SIZE / 2 - hole_offset, SIZE / 2 - hole_offset, 0],
                [-SIZE / 2 + hole_offset, SIZE / 2 - hole_offset, 0],
                [SIZE / 2 - hole_offset, -SIZE / 2 + hole_offset, 0],
                [-SIZE / 2 + hole_offset, -SIZE / 2 + hole_offset, 0],
            ]) {
                translate(pos) {
                    cylinder(r = HOLE_DIAMETER / 2, h = 150, center = true);

                    for (i = [0 : $children - 1]) {
                        child(i);
                    }
                }
            }
        }
    }
}

module main() {
    nut_diameter = 5;
    nut_height = 1.9;

    difference() {
        cube(size = [SIZE, SIZE, SIZE], center = true);
        cube(size = [SIZE * 2, SIZE - RIDGE_WIDTH * 2, SIZE - RIDGE_WIDTH * 2], center = true);
        cube(size = [SIZE - RIDGE_WIDTH * 2, SIZE * 2, SIZE - RIDGE_WIDTH * 2], center = true);
        cube(size = [SIZE - RIDGE_WIDTH * 2, SIZE - RIDGE_WIDTH * 2, SIZE * 2], center = true);
        cube(size = [SIZE - THICKNESS * 2, SIZE - THICKNESS * 2, SIZE - THICKNESS * 2], center = true);

        holes() {
            translate([0, 0, SIZE / 2 - THICKNESS - nut_height * .3]) {
                cylinder(r = nut_diameter / 2, h = nut_height, $fn = 6);
            }

            translate([0, 0, - SIZE / 2 + THICKNESS - nut_height + 0.5]) {
                cylinder(r = nut_diameter / 2, h = nut_height, $fn = 6);
            }
        }
    }
}
*/

REFLECTOR_HEIGHT = 12;

module front(index, connection) {
    edge_size = SIZE / 2 * sqrt(2);

    module plot(add, substract = true) {
        width = 4;
        length = 10;
        height = 8;

        clear = 0.2;
        clear_height = 6;

        for (data = [
            [0, SIZE / 2 - RIDGE_WIDTH / 2, 4.5],
            [20, SIZE / 2 - RIDGE_WIDTH / 2, 4.5],
            [-20, SIZE / 2 - RIDGE_WIDTH / 2, 4.5],
        ]) {
            translate(data) {
                if (add && !substract) {
                    cube(size = [length, width, height], center = true);
                    cube(size = [length - 5, width + 2, height], center = true);
                } else if (substract) {
                    rotate([90, 0, 0]) {
                        cube(size = [length + clear, width + clear, height + clear_height], center = true);
                        cube(size = [length - 5 + clear, width + 2 + clear, height + clear_height], center = true);
                    }
                }
            }
        }
    }

    module blocker(connection, substract = false) {
        data = [0, 90, 180, 270];
        for (i = [0 : 3]) {
            rotate([0, 0, 45 + data[i]]) {
                plot(connection[i], substract);
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
                rotate([0, 0, 45]) {
                    cylinder(r1 = 5, r2 = size, h = REFLECTOR_HEIGHT, $fn = 4, center = true);
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
                    %cylinder(r = HOLE_DIAMETER / 2 - holes_clear, h = 150, center = true);
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
            rotate([0, 0, 90]) {
                holes_test();
            }
        }
    }

    // Todo: faire une marque sur chaque face pour la reconnaitre basé sur le index
}

module mainv2() {

    offset = 10;
    //cube(size = [SIZE, SIZE, SIZE], center = true);

    for (data = [
        [0, [0, 0, -SIZE / 2 - offset], [0, 0, 45], [false, false, false, false]], // Bottom
        [1, [0, 0, SIZE / 2 + offset], [180, 0, 45], [true, true, true, true]],    // Top
        [2, [0, SIZE / 2 + offset, 0], [90, 45, 0], [false, false, true, false]],
        [3, [0, -SIZE / 2 - offset, 0], [-90, 45, 0], [false, false, false, true]],
        [4, [-SIZE / 2 - offset, 0, 0], [90, 45, 90], [false, true, true, true]],
        [5, [SIZE / 2 + offset, 0, 0], [90, 45, -90], [false, true, true, true]],
    ]) {
        translate(data[1]) {
            rotate(data[2]) {
                front(data[0], data[3]);
            }
        }
    }

/*
    translate([0, 0, - SIZE / 2]) {
        rotate([0, 0, 45]) {
            cylinder(r1 = edge_size, r2 = edge_size - RIDGE_WIDTH * 2, h = RIDGE_WIDTH, $fn = 4);
        }
    }
*/
}

/*
module all() {
    main();

    intersection() {
        cube(size = [SIZE, SIZE, SIZE], center = true);
        rotate([90, 90, 45]) {
            electronic_support();
            //%electronic();
        }
    }
}

module side(type = 0, text = "", text_rot = [0, 0, 0], text_pos = [0, 0, 0]) {
    nut_head_diameter = 4.6; // Before : 4: serré, 5: too large
    nut_head_height = 1.3;
    thickness = 2;
    text_deep = 1;
    difference() {
        translate([0, 0, SIZE / 2 + thickness / 2]) {
            // Biggest type
            if (type == 0) {
                cube(size = [SIZE + thickness * 2, SIZE + thickness * 2, 2], center = true);
            // Medium type
            } else if (type == 1) {
                cube(size = [SIZE, SIZE + thickness * 2, 2], center = true);
            // Smallest type
            } else if (type == 2) {
                cube(size = [SIZE, SIZE, 2], center = true);
            }
        }

        translate([0, 0, SIZE / 2 - nut_head_height + thickness]) {
            holes() {
                cylinder(r = nut_head_diameter / 2, h = 2);
            }
        }

        translate([-10, -15, SIZE / 2 + thickness - text_deep]) {
            scale([3.5, 3.5, 1]) {
                rotate(text_rot) {
                    translate(text_pos) {
                        linear_extrude(height = 20) {
                            write(text);
                        }
                    }
                }
            }
        }
    }
}

module reflector() {
    clear = 0.1;
    edge_size = 2;
    edge_thickness = 0.2;
    hole_width = 5.8;
    size = ((SIZE - RIDGE_WIDTH * 2) / 2) * sqrt(2) - clear;
    difference() {
        union() {
            rotate([0, 0, 45]) {
                cylinder(r1 = 5, r2 = size, h = REFLECTOR_HEIGHT, $fn = 4, center = true);
            }

            translate([0, 0, REFLECTOR_HEIGHT / 2 + edge_thickness / 2]) {
                cube(size = [SIZE - RIDGE_WIDTH * 2 + edge_size, SIZE - RIDGE_WIDTH * 2 + edge_size, edge_thickness], center = true);
            }
        }

        translate([0, 0, 1.3]) {
            rotate([0, 0, 45]) {
                cylinder(r1 = hole_width / sqrt(2), r2 = SIZE / 2, h = REFLECTOR_HEIGHT + 1.01, $fn = 4, center = true);
            }
        }

        translate([0, 0, 1]) {
            cube(size = [hole_width, hole_width, 50], center = true);
        }
    }
}
*/

module sides(which = -1, offset = 30) {
    for (data = [
        [[0, 0, offset],    [0, 0, 0],      0, "1", [0, 0, 90],     [1.5, -7.5, 0]],
        [[0, 0, -offset],   [180, 0, 0],    0, "6"],
        [[offset, 0, 0],    [0, 90, 0],     1, "2", [0, 0, 90],     [1.5, -7.5, 0]],
        [[-offset, 0, 0],   [0, -90, 0],    1, "5", [0, 0, -90],    [-7, -2, 0]],
        [[0, -offset, 0],   [90, 0, 0],     2, "4"],
        [[0, offset, 0],    [-90, 0, 0],    2, "3", [0, 0, 180],    [-6, -9, 0]]
    ]) {
        if (which == -1 || which == data[3]) {
            translate(data[0]) {
                rotate(data[1]) {
                    side(data[2], data[3], data[4], data[5]);

                    child(0);
                }
            }
        }
    }
}

if (1) {

    intersection() {
        translate([0, SIZE / 2, -SIZE / 2]) {
            //cube(size = [100, 30, 10], center = true);
        }
        mainv2();
    }

    //reflector();
/*
    all();

    //reflector();

    //sides("4");
    //%sides(which = "1", offset = 0) {
    sides(offset = 0) {
        translate([0, 0, SIZE / 2 - REFLECTOR_HEIGHT / 2]) {
            reflector();
        }
    }
*/
} else {
    difference() {
        rotate([45, 0, 0]) {
            all();
        }

        translate([0, 0, 75]) {
            cube(size = [150, 150, 150], center = true);
        }
    }
}

