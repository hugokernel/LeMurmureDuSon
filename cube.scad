
$fn = 20;

SIZE = 76;
RIDGE_WIDTH = 10;

THICKNESS = 3;

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

module main() {
    hole_diameter = 2;
    hole_offset = RIDGE_WIDTH - hole_diameter * 1.2;

    nut_diameter = 5;
    nut_height = 1.9;

    difference() {
        cube(size = [SIZE, SIZE, SIZE], center = true);
        cube(size = [SIZE * 2, SIZE - RIDGE_WIDTH * 2, SIZE - RIDGE_WIDTH * 2], center = true);
        cube(size = [SIZE - RIDGE_WIDTH * 2, SIZE * 2, SIZE - RIDGE_WIDTH * 2], center = true);
        cube(size = [SIZE - RIDGE_WIDTH * 2, SIZE - RIDGE_WIDTH * 2, SIZE * 2], center = true);
        cube(size = [SIZE - THICKNESS * 2, SIZE - THICKNESS * 2, SIZE - THICKNESS * 2], center = true);

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
                        cylinder(r = hole_diameter / 2, h = 150, center = true);

                        translate([0, 0, SIZE / 2 - THICKNESS - nut_height * .3]) {
                            cylinder(r = nut_diameter / 2, h = nut_height, $fn = 6);
                        }

                        translate([0, 0, - SIZE / 2 + THICKNESS - nut_height + 0.5]) {
                            cylinder(r = nut_diameter / 2, h = nut_height, $fn = 6);
                        }
                    }
                }
            }
        }
    }
}

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

if (0) {
    all();
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

