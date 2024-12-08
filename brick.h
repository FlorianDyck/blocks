#pragma once

#include <inttypes.h>
#include <iostream>
#include <cassert>
#include <vector>

#include "xy.h"
#include "board.h"

class Brick
{
public:
    Board board {};
    u64 placable_positions_mask {};
    XY size {};
    // std::vector<XY> blocks {};

    constexpr Brick() = default;
    constexpr Brick(u64 positions)
        : board(positions),
          size(board.rightmost_position() + board.hightest_position())
    {
        assert(valid());
        for(auto position: XY(7,7) - size)
        {
            placable_positions_mask |= toBoard(position).positions;
        }
    }
    constexpr Brick(const Brick& other) = default;
    constexpr Brick(Brick&& other) = default;
    constexpr ~Brick() = default;
    constexpr Brick& operator=(const Brick& other) = default;

    constexpr bool valid() const
    {
        return (board.positions & LINE.positions)
            && (board.positions & (LINE << size.y()).positions) 
            && (board.positions & COLUMN.positions)
            && (board.positions & (COLUMN << size.x()).positions);
    }

    bool operator==(const Brick &other) const { return board.positions == other.board.positions; }

    constexpr u64 placable_positions(const Board board) const;
    constexpr inline bool can_place(const Board board) const { return placable_positions(board); }

    Brick flip_vertically();
    Brick flip_horizontally();
    Brick rotate();
    Board operator<<(XY pos) { return board.positions << pos; }

    std::ostream &print(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &os, const Brick &m) { return m.print(os); }
};

constexpr u64 Brick::placable_positions(const Board board) const
{
    u64 result = 0;
    for (auto position: size)
    {
        if ((this->board.positions >> position.value) & 1)
        {
            result |= board.positions >> position.value;
        }
    }
    return ~result & placable_positions_mask;
}

constexpr inline u64 placable_positions_3X3(const Board board)
{
    u64 result = board.positions; 
    result = result | (result >> Y1.value) | (result >> Y2.value);
    result = result | (result >> X1.value) | (result >> X2.value);
    return ~result & 0x00'00'00'1F'1F'1F'1F'1F;
}

constexpr inline u64 placable_positions_1X5(const Board board)
{
    u64 result = board.positions; 
    result = result | (result >> Y1.value) | (result >> Y2.value);
    result = result | (result >> Y2.value);
    return ~result & 0x00'00'00'00'00'FF'FF'FF;
}

constexpr inline u64 placable_positions_5X1(const Board board)
{
    u64 result = board.positions; 
    result = result | (result >> X1.value) | (result >> X2.value);
    result = result | (result >> X2.value);
    return ~result & 0x07'07'07'07'07'07'07'07;
}
