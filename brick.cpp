#include<string.h>
#include "brick.h"

Brick Brick::flip_vertically() {
    u64 flipped_positions = 0;
    for (u8 line = 0; line <= size.y_int(); line++) {
        int index = XY(0, size.y_int() - line).value;
        flipped_positions |= (((board.positions >> index) & LINE.positions) << XY(0, line)).positions;
    }
    return Brick(flipped_positions);
}

Brick Brick::flip_horizontally() {
    u64 flipped_positions = 0;
    for (u8 column = 0; column <= size.x_int(); column++) {
        int index = XY(size.x_int() - column, 0).value;
        flipped_positions |= (((board.positions >> index) & COLUMN.positions) << XY(column, 0)).positions;
    }
    return Brick(flipped_positions);
}

Brick Brick::rotate() {
    u64 rotated_positions = 0;
    for (u8 y = 0; y <= size.y_int(); y++) {
        for (u8 x = 0; x <= size.x_int(); x++) {
            if (board.position(XY(x, y))) {
                rotated_positions |= (1 << XY(size.y_int() - y, x)).positions;
            }
        }
    }
    return Brick(rotated_positions);
}

std::ostream &Brick::print(std::ostream &os) const
{
    return board.print_range(os, size);
}