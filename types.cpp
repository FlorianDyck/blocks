#include<string.h>
#include "types.h"

bool Board::position(i8 x, i8 y)
{
    return (positions >> (x.value + y.value * BOARD_WIDTH.value)) & 0x1;
}

std::ostream &print_range(u64 positions, std::ostream &os, i8 bound_x, i8 bound_y)
{
    os << "\n|" << std::string(bound_x.value, '=') << "|\n|";

    for (i8 y = 0; y < bound_y; y++)
    {
        for (i8 x = 0; x < bound_x; x++)
        {
            os << (positions & 0x1 ? "X" : ".");
            positions >>= 1;
        }
        os << "|\n|";
        positions >>= BOARD_WIDTH.value - bound_x.value;
    }

    os << std::string(bound_x.value, '=') << "|";

    return os;
}

std::ostream &Board::print(std::ostream &os) const
{
    return print_range(positions, os, BOARD_WIDTH, BOARD_HEIGHT);
}

i8 Board::rightmost_position() const
{
    for (i8 i : COLUMN_NUMBERS_REVERSED)
    {
        if ((COLUMN << Position(i, 0)) & positions)
        {
            return i;
        }
    }
    return -1;
}

i8 Board::hightest_position() const
{
    for (i8 i : LINE_NUMBERS_REVERSED)
    {
        if ((LINE << Position(0, i)) & positions)
        {
            return i;
        }
    }
    return -1;
}

std::ostream &Brick::print3(std::ostream &os) const
{
    return print_range(positions, os, width, height);
}