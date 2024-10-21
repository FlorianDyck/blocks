#include "types.h"

bool Board::position(i8 x, i8 y)
{
    return (positions >> (x + y * BOARD_WIDTH)) & 0x1;
}

std::ostream &print_range(u64 positions, std::ostream &os, i8 bound_x, i8 bound_y)
{
    os << std::endl
       << "|";
    for (i8 x = 0; x < BOARD_WIDTH; x++)
        os << "-";
    os << "|";

    for (i8 y = 0; y < BOARD_HEIGHT; y++)
    {
        os << std::endl
           << "|";
        for (i8 x = 0; x < BOARD_WIDTH; x++)
        {
            os << (positions & 0x1 ? "X" : ".");
            positions >>= 1;
        }
        os << "|";
    }

    os << std::endl
       << "|";
    for (i8 x = 0; x < BOARD_WIDTH; x++)
        os << "-";
    os << "|";

    return os;
}

std::ostream &Board::print(std::ostream &os) const
{
    return print_range(positions, os, BOARD_WIDTH, BOARD_HEIGHT);
}

i8 Board::rightmost_position() const
{
    for (i8 i = BOARD_WIDTH - 1; i >= 0; i--)
    {
        if ((column << i) & positions)
        {
            return i;
        }
    }
    std::cout << "rightmost: -1" << std::endl;
    return -1;
}

i8 Board::hightest_position() const
{
    for (i8 i = BOARD_HEIGHT - 1; i >= 0; i--)
    {
        if ((row << (i * BOARD_WIDTH)) & positions)
        {
            return i;
        }
    }
    std::cout << "highest: -1" << std::endl;
    return -1;
}

std::ostream &Brick::print3(std::ostream &os) const
{
    return print_range(positions, os, width, height);
}