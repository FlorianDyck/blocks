#include<string.h>
#include "board.h"

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

std::ostream &print_range_func(u64 positions, std::ostream &os, XY bound)
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

std::ostream &Board::print_range(std::ostream &os, XY bound) const
{
    return print_range_func(positions, os, bound);
}

std::ostream &Board::print(std::ostream &os) const
{
    return print_range(os, XY(7,7));
}

