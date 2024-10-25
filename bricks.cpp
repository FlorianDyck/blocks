#include "bricks.h"
#include <algorithm>

void add_rotations(Brick brick, std::vector<Brick>& vec) {
    Brick rotated = brick;
    do {
        vec.push_back(rotated);
        rotated = rotated.rotate();
    } while(rotated != brick);
}

void add_all_versions(Brick brick, std::vector<Brick>& vec) {
    add_rotations(brick, vec);
    Brick flipped = brick.flip_horizontally();
    if (std::find(vec.begin(), vec.end(), flipped) != vec.end()){
        add_rotations(flipped, vec);
    }
}

std::vector<Brick> all_bricks() {
    std::vector<Brick> result;

    add_all_versions(Brick(0b0000'0001), result); // 1x1
    add_all_versions(Brick(0b0000'0011), result); // 1x2
    add_all_versions(Brick(0b0000'0111), result); // 1x3
    add_all_versions(Brick(0b0000'1111), result); // 1x4
    add_all_versions(Brick(0b0001'1111), result); // 1x5
    
    add_all_versions(Brick(0b0000'0011'0000'0011), result); // 2x2
    add_all_versions(Brick(0b0000'0111'0000'0111'0000'0111), result); // 3x3
    
    add_all_versions(Brick(0b0000'0001'0000'0011), result); // 2L2
    add_all_versions(Brick(0b0000'0001'0000'0111), result); // 3L2
    add_all_versions(Brick(0b0000'0001'0000'0001'0000'0111), result); // 3L3

    add_all_versions(Brick(0b0000'0010'0000'0111), result); // T
    add_all_versions(Brick(0b0000'0110'0000'0011), result); // Z

    return result;
}