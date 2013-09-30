
module m2_nut(height = 1.54) {
    cylinder(r = 2.6, h = height, center = true, $fn = 6);
}

module m2_hole(height = 10) {
    cylinder(r = 1.2, h = height, center = true);
}

module m3_nut(height = 2.34) {
    cylinder(r = 3.4, h = height, center = true, $fn = 6);
}

module m3_hole(height = 10) {
    cylinder(r = 1.9, h = height, center = true);
}

module m4_nut(height = 2.88) {
    cylinder(r = 4.3, h = height, center = true, $fn = 6);
}

module m4_hole(height = 10) {
    cylinder(r = 2.2, h = height, center = true);
}

