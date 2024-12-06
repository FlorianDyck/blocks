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

    add_all_versions(brick1x1, result);
    add_all_versions(brick2x2, result);
    add_all_versions(brick3x3, result);

    add_all_versions(brick2x1, result);
    add_all_versions(brick3x1, result);
    add_all_versions(brick4x1, result);
    add_all_versions(brick5x1, result);

    add_all_versions(brick2L2, result);
    add_all_versions(brick3L2, result);
    add_all_versions(brick3L3, result);

    add_all_versions(brickT, result);
    add_all_versions(brickZ, result);

    return result;
}