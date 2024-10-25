#pragma once

#include <inttypes.h>
#include <iostream>
#include <cassert>
#include <vector>

using u64 = uint_fast64_t;
struct i8
{
    int_fast8_t value;
    inline constexpr i8(int value) : value(value)
    {
    }
    inline constexpr bool operator==(const i8 other) const { return value == other.value; }
    inline constexpr std::strong_ordering operator<=>(const i8 other) const { return value <=> other.value; }
    inline constexpr i8 operator+(const i8 other) const { return value + other.value; }
    inline constexpr i8 operator*(const i8 other) const { return value * other.value; }
    inline constexpr i8 operator++(int) { return value++; }

    friend std::ostream &operator<<(std::ostream &os, i8 num)
    {
        return os << (int)num.value;
    }
};

const i8 BOARD_WIDTH = 8;
const i8 BOARD_HEIGHT = 8;

const u64 LINE = 0b11111111;
const u64 COLUMN = 0x0101010101010101;

const i8 LINE_NUMBERS[] = {0, 1, 2, 3, 4, 5, 6, 7};
const i8 COLUMN_NUMBERS[] = {0, 1, 2, 3, 4, 5, 6, 7};

const i8 LINE_NUMBERS_REVERSED[] = {7, 6, 5, 4, 3, 2, 1, 0};
const i8 COLUMN_NUMBERS_REVERSED[] = {7, 6, 5, 4, 3, 2, 1, 0};

#define L(index) LINE << (index * BOARD_WIDTH.value)
const u64 LINES[] = { L(0), L(1), L(2), L(3), L(4), L(5), L(6), L(7) };
const u64 LINES_REVERSED[] = { L(7), L(6), L(5), L(4), L(3), L(2), L(1), L(0) };
#undef L

#define C(index) COLUMN << index
const u64 COLUMNS[] = { C(0), C(1), C(2), C(3), C(4), C(5), C(6), C(7) };
const u64 COLUMNS_REVERSED[] = { C(7), C(6), C(5), C(4), C(3), C(2), C(1), C(0) };
#undef C

// std::ostream &operator<<(std::ostream &os, i8 num)
// {
//     return os << (int) num;
// }

struct Position
{
    i8 x;
    i8 y;
    constexpr Position() : x(0), y(0) {}
    constexpr Position(i8 x, i8 y) : x(x), y(y)
    {
        assert(valid());
    }

    constexpr const bool valid() const
    {
        return 0 <= x && x < BOARD_WIDTH && 0 <= y && y < BOARD_HEIGHT;
    }

    int index() const {
        return x.value + y.value * BOARD_WIDTH.value;
    }

    friend const u64 operator<<(const u64 blocks, const Position position)
    {
        assert(position.valid());
        return blocks << (position.index());
    }

    friend std::ostream &operator<<(std::ostream &os, Position position)
    {
        return os << "(" << position.x << ", " << position.y << ")";
    }
};

#define POSITIONS_IN_LINE(line) Position(0, line), Position(1, line), Position(2, line), Position(3, line), Position(4, line), Position(5, line), Position(6, line), Position(7, line)
const Position board_positions[] = {POSITIONS_IN_LINE(0), POSITIONS_IN_LINE(1), POSITIONS_IN_LINE(2), POSITIONS_IN_LINE(3), POSITIONS_IN_LINE(4), POSITIONS_IN_LINE(5), POSITIONS_IN_LINE(6), POSITIONS_IN_LINE(7)};
#undef POSITIONS_IN_LINE

static int NumberOfSetBits(u64 i)
{
    i = i - ((i >> 1) & 0x5555555555555555UL);
    i = (i & 0x3333333333333333UL) + ((i >> 2) & 0x3333333333333333UL);
    return (int)((((i + (i >> 4)) & 0xF0F0F0F0F0F0F0FUL) * 0x101010101010101UL) >> 56);
}

struct Grades {
    int free[5] = { 0, 0, 0, 0, 0 };
    int used[5] = { 0, 0, 0, 0, 0 };
};

class Board
{
protected:
    u64 positions;

public:
    Board() : positions(0) {}
    Board(u64 positions) : positions(positions) {}
    bool operator==(const Board other) const {
        return positions == other.positions;
    }

    bool position(i8 x, i8 y);

    std::ostream &print(std::ostream &os) const;

    i8 rightmost_position() const;
    i8 hightest_position() const;

    bool can_combine(Board other) {
        return !(positions & other.positions);
    }

    Board combine(Board other) {
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

        return Board(combination);
    }

    int set_positions() {
        return NumberOfSetBits(positions);
    }

    int free_positions() {
        return BOARD_HEIGHT.value * BOARD_WIDTH.value - set_positions();
    }

    int differentBlocksAround(Position position) {
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

    Grades grades() {
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

    void print_eval_stats() {
        int freeBlocks = free_positions();
        Grades g = grades();
        int borderLength = g.free[1] + 2 * g.free[2] + 3 * g.free[3] + 4 * g.free[4];
        std::cout << freeBlocks << " free positions, border length " << borderLength;
    }

    friend std::ostream &operator<<(std::ostream &os, Board const &m)
    {
        return m.print(os);
    }
};

class Brick : Board
{
public:
    i8 width;
    i8 height;

public:
    Brick(u64 positions) : Board(positions), width(rightmost_position() + 1), height(hightest_position() + 1)
    {
        assert(valid());
    }

    bool valid()
    {
        return (positions & LINE) && (positions & (LINE << Position(0, height.value - 1))) && (positions & COLUMN) && (positions & (COLUMN << Position(width.value - 1, 0)));
    }

    Board operator<<(Position pos) {
        return positions << pos;
    }

    std::ostream &print3(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, Brick const &m)
    {
        return m.print3(os);
    }
};