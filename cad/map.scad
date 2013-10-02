
SIZE = 76;

module face(index, size) {
    stl_width = 473;
    //coeff = stl_width / size;
    thickness = 2;

    //size_map = [ 0, 0, 2, 2, 1, 1 ];
    size_map = [
        // Type 0
        [ size + thickness * 2, size + thickness * 2 ],
        [ size + thickness * 2, size + thickness * 2 ],

        // Type 2
        [ size, size + thickness * 2 ],
        [ size, size + thickness * 2 ],

        // Type 1
        [ size, size ],
        [ size, size ]
    ];

    coeff_w = stl_width / size_map[index][0] + .1;
    coeff_l = stl_width / size_map[index][1];

    translate([stl_width / coeff_w / 2, stl_width / coeff_l / 2, thickness / 2]) {
        //scale([1 / coeff, 1 / coeff, 1 / coeff]) {
        scale([1 / coeff_w, 1 / coeff_l, 1 / coeff_l]) {
            import(str("map/", index, ".stl"));
        }
    }
/*
    translate([0, 0, thickness / 2]) {
        color("RED") {
            cube(size = [size_map[index][0], size_map[index][1], 2], center = true);
        }
    }
*/
}

module faces(which = -1, offset = 30) {
    offset = offset + SIZE / 2;
    for (data = [
        [ 0, [0, 0, -offset],   [180, 0, 0], "RED" ],
        //[ 1, [0, 0, offset],    [0, 0, 0]   ],
        [ 2, [offset, 0, 0],    [0, 90, 0], "BLUE"  ],
        //[ 3, [-offset, 0, 0],   [0, -90, 0] ],
        //[ 4, [0, -offset, 0],   [90, 0, 0]  ],
        //[ 5, [0, offset, 0],    [-90, 0, 0] ]
    ]) {
        if (which == -1) {
            translate(data[1]) {
                rotate(data[2]) {
                    color(data[3])
                    face(data[0], SIZE);
                }
            }
        }
    }
}

//face(1, SIZE);

%cube(size = [SIZE, SIZE, SIZE], center = true);

faces(-1, 0);

//cube(size = [80, 80, 80]);

