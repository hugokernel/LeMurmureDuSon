
use <cube.scad>

SIZE = 76;

coeff = 1.4;
thickness = 4.2;
size = SIZE * coeff;

module cable() {
    // Cable
    translate([SIZE / 2, 0, 0]) {
        rotate([0, 90, 0]) {
            cylinder(r = thickness / 2 - thickness * 0.1, h = 20);
        }
    }
}

module usb() {
    length = 8;
    width = 7.2;
    thickness = 2.9;

    // Usb
    translate([size / 2 - width / 2 + 0.001, 0, 0]) {//-thickness / 2 - 0.01]) {
        cube(size = [width, length, thickness], center = true);
    }
}

module enveloppe(usb = true) {
    base_thickness = 0.5;
    gain = 3;
    sizee = size + gain;
    clear = 0;
    translate([0, 0, - thickness / 2 - base_thickness / 2]) {
        cube(size = [sizee, sizee, base_thickness], center = true);
    }

    translate([0, 0, 0]) {
        difference() {
            cube(size = [sizee, sizee, thickness], center = true);
            translate([0, 0, 0.1]) {
                cube(size = [size + clear, size + clear, thickness], center = true);
            }

            translate([gain / 2, 0, 0]) {
                if (usb) {
                    usb();
                } else {
                    cable();
                }
            }
        }
    }
}

module base(usb = true) {

    module plate() {
        difference() {
            cube(size = [size, size, thickness], center = true);

            //translate([0, 0, -thickness / 2 - 0.01]) {
            translate([0, 0, -3]) {
                coil(hole = false, height = thickness);
            }

            //translate([33, 0, -thickness / 2 - 0.01]) {
            translate([33, 0, -0.3]) {
                cube(size = [28, 25, thickness], center = true);
            }

            if (usb) {
                usb();
            } else {
                cable();
            }
        }
    }

    plate();
    //enveloppe();
}

intersection() {
    /*
    translate([50, 0, 0]) {
        cube([40, 40, 20], center = true);
    }
    */
    //base();
    enveloppe();
}

