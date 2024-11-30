
#pragma once

#include <inttypes.h>
#include <cassert>
#include <iostream>
#include <vector>

#include "xy.h"

struct Grades
{
    int free[5] = {0, 0, 0, 0, 0};
    int used[5] = {0, 0, 0, 0, 0};
};


using u64 = uint_fast64_t;

constexpr const static int NumberOfSetBits(u64 i)
{
    i = i - ((i >> 1) & 0x5555555555555555UL);
    i = (i & 0x3333333333333333UL) + ((i >> 2) & 0x3333333333333333UL);
    return (int)((((i + (i >> 4)) & 0xF0F0F0F0F0F0F0FUL) * 0x101010101010101UL) >> 56);
}

class Board
{
public:
    u64 positions;

    constexpr Board() = default;
    constexpr Board(u64 positions) : positions(positions) {}
    constexpr Board(const Board& other) = default;
    constexpr Board(Board&& other) = default;
    constexpr Board& operator=(const Board& other) = default;
    constexpr bool operator==(const Board other) const { return positions == other.positions; }
    constexpr Board operator|(const Board other) const { return positions | other.positions; }

    constexpr bool position(XY xy) const;

    constexpr const X rightmost_position() const;
    constexpr const Y hightest_position() const;

    constexpr bool can_combine(const Board other) const { return !(positions & other.positions); }

    constexpr const std::pair<Board, size_t> combine(Board other) const;

    constexpr const int set_positions() const { return NumberOfSetBits(positions); }

    constexpr const int free_positions() const
    {
        return BOARD_HEIGHT * BOARD_WIDTH - set_positions();
    }

    constexpr const int differentBlocksAround(const XY position) const;

    Grades grades();

    std::ostream &print_range(std::ostream &os, XY bound) const;
    std::ostream &print(std::ostream &os) const;
    void print_eval_stats()
    {
        int freeBlocks = free_positions();
        Grades g = grades();
        int borderLength = g.free[1] + 2 * g.free[2] + 3 * g.free[3] + 4 * g.free[4];
        std::cout << freeBlocks << " free positions, border length " << borderLength;
    }

    friend std::ostream &operator<<(std::ostream &os, Board const &m) { return m.print(os); }
};

constexpr const Board operator<<(const Board blocks, const XY xy)
{
    assert(xy.valid());
    return blocks.positions << xy.value;
}

constexpr const Board LINE {0b11111111};
constexpr const Board COLUMN {0x0101010101010101};

#define L(index) LINE << Y(index)
constexpr const Board LINES[] = {L(0), L(1), L(2), L(3), L(4), L(5), L(6), L(7)};
constexpr const Board LINES_REVERSED[] = {L(7), L(6), L(5), L(4), L(3), L(2), L(1), L(0)};
#undef L

#define C(index) COLUMN << X(index)
constexpr const Board COLUMNS[] = {C(0), C(1), C(2), C(3), C(4), C(5), C(6), C(7)};
constexpr const Board COLUMNS_REVERSED[] = {C(7), C(6), C(5), C(4), C(3), C(2), C(1), C(0)};
#undef C

inline constexpr const Board toBoard(XY xy) 
{
    return ((u64)1) << xy.value;
};

constexpr const std::pair<Board, size_t> Board::combine(const Board other) const {
    assert(this->can_combine(other));
    u64 combination = positions | other.positions;
    
    std::vector<XY> lines;
    for (XY y: YS) {
        if ((combination & (LINE << y).positions) == (LINE << y)) {
            lines.push_back(y);
        }
    }
    
    std::vector<XY> columns;
    for (XY x: XS) {
        if ((combination & (COLUMN << x).positions) == (COLUMN << x)) {
            columns.push_back(x);
        }
    }

    for(XY y: lines) {
        combination &= ~(LINE << y).positions;
    }
    for(XY x: columns) {
        combination &= ~(COLUMN << x).positions;
    }

    return std::pair(Board(combination), lines.size() + columns.size());
}

constexpr bool Board::position(XY xy) const
{
    return (positions >> xy.value) & 0x1;
}

constexpr const int Board::differentBlocksAround(const XY position) const
{
    int index = position.value;
    bool isSet = positions & (1ul << index);
    auto isDifferent = [this, index, position, isSet](int offset) { return isSet != (bool) (positions & (1ul << (index + offset))); };
    int result = 0;
    if ((index >= BOARD_WIDTH) ? isDifferent(-BOARD_WIDTH) : !isSet ) result++;
    if (((index % BOARD_WIDTH) != 0) ? isDifferent(-1) : !isSet ) result++;
    if (((index % BOARD_WIDTH) != (BOARD_WIDTH - 1)) ? isDifferent(1) : !isSet ) result++;
    if ((index < (BOARD_HEIGHT * BOARD_WIDTH - BOARD_WIDTH)) ? isDifferent(BOARD_WIDTH) : !isSet ) result++;
    return result;
}

inline constexpr const Board operator|(const XY lhs, const XY rhs)
{
    return toBoard(lhs) | toBoard(rhs);
}

constexpr const X Board::rightmost_position() const
{
    for (X x : XS_REV)
    {
        if ((COLUMN << x).positions & positions)
        {
            return x;
        }
    }
    return -1;
}

constexpr const Y Board::hightest_position() const
{
    for (Y y : YS_REV)
    {
        if ((LINE << y).positions & positions)
        {
            return y;
        }
    }
    return -1;
}