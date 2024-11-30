#include<string.h>
#include "brick.h"

// XY X(u8 x) { return XY(x, 0); }
// // XY Y(u8 y) { return XY(0, y); }

// const XY X0 = X(0), X1 = X(1), X2 = X(2), X3 = X(3), X4 = X(4), X5 = X(5), X6 = X(6), X7 = X(7);
// const XY Y0 = Y(0), Y1 = Y(1), Y2 = Y(2), Y3 = Y(3), Y4 = Y(4), Y5 = Y(5), Y6 = Y(6), Y7 = Y(7);

Grades Board::grades() {
    Grades result;
    for (XY y: YS) {
        for (XY x: XS) {
            XY position = x + y;
            if (positions & (1ul << position).positions) {
                result.used[differentBlocksAround(position)]++;
            } else {
                result.free[differentBlocksAround(position)]++;
            }
        }
    }
    return result;
}

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

std::ostream &print_range(u64 positions, std::ostream &os, XY bound)
{
    os << "\n|" << std::string(bound.x_int() + 1, '=') << "|\n|";

    for (u8 y = 0; y <= bound.y_int(); y++)
    {
        for (u8 x = 0; x <= bound.x_int(); x++)
        {
            os << (positions & 0x1 ? "X" : ".");
            positions >>= 1;
        }
        os << "|\n|";
        positions >>= BOARD_WIDTH - bound.x_int() - 1;
    }

    os << std::string(bound.x_int() + 1, '=') << "|";

    return os;
}

std::ostream &Board::print(std::ostream &os) const
{
    return print_range(positions, os, XY(7,7));
}

std::ostream &Brick::print(std::ostream &os) const
{
    return print_range(board.positions, os, size);
}