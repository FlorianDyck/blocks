#include<string.h>
#include "types.h"

bool Board::position(i8 x, i8 y)
{
    return (positions >> (x.value + y.value * BOARD_WIDTH.value)) & 0x1;
}

std::pair<Board, size_t> Board::combine(Board other) {
    assert(this->can_combine(other));
    u64 combination = positions | other.positions;
    
    std::vector<i8> lines;
    for (i8 line: LINE_NUMBERS) {
        if ((combination & (LINE << Position(0, line))) == (LINE << Position(0, line))) {
            lines.push_back(line);
        }
    }
    
    std::vector<i8> columns;
    for (i8 column: COLUMN_NUMBERS) {
        if ((combination & (COLUMN << Position(column, 0))) == (COLUMN << Position(column, 0))) {
            columns.push_back(column);
        }
    }

    for(i8 line: lines) {
        combination &= ~(LINE << Position(0, line));
    }
    for(i8 column: columns) {
        combination &= ~(COLUMN << Position(column, 0));
    }

    return std::pair(Board(combination), lines.size() + columns.size());
}

int Board::differentBlocksAround(Position position) {
    int index = position.index();
    bool isSet = positions & (1ul << index);
    auto isDifferent = [this, index, position, isSet](int offset) { return isSet != (bool) (positions & (1ul << (index + offset))); };
    int result = 0;
    if ((index >= BOARD_WIDTH.value) ? isDifferent(-BOARD_WIDTH.value) : !isSet ) result++;
    if (((index % BOARD_WIDTH.value) != 0) ? isDifferent(-1) : !isSet ) result++;
    if (((index % BOARD_WIDTH.value) != (BOARD_WIDTH.value - 1)) ? isDifferent(1) : !isSet ) result++;
    if ((index < (BOARD_HEIGHT.value * BOARD_WIDTH.value - BOARD_WIDTH.value)) ? isDifferent(BOARD_WIDTH.value) : !isSet ) result++;
    return result;
}

Grades Board::grades() {
    Grades result;
    for (i8 y: LINE_NUMBERS) {
        for (i8 x: COLUMN_NUMBERS) {
            Position position(x, y);
            if (positions & (1ul << position)) {
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
    for (i8 line = 0; line < height; line++) {
        int index = Position(0, height.value - line.value - 1).index();
        flipped_positions |= ((positions >> index) & LINE) << Position(0, line);
    }
    return Brick(flipped_positions);
}

Brick Brick::flip_horizontally() {
    u64 flipped_positions = 0;
    for (i8 column = 0; column < width; column++) {
        int index = Position(width.value - column.value - 1, 0).index();
        flipped_positions |= ((positions >> index) & COLUMN) << Position(column, 0);
    }
    return Brick(flipped_positions);
}

Brick Brick::rotate() {
    u64 rotated_positions = 0;
    for (i8 y = 0; y < height; y++) {
        for (i8 x = 0; x < width; x++) {
            if (position(x, y)) {
                rotated_positions |= 1 << Position(height.value - 1 - y.value, x);
            }
        }
    }
    return Brick(rotated_positions);
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

std::ostream &Brick::print(std::ostream &os) const
{
    return print_range(positions, os, width, height);
}