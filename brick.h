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
    XY size {};
    std::vector<XY> blocks {};

    constexpr Brick() = default;
    constexpr Brick(u64 positions)
        : board(positions),
          size(board.rightmost_position() + board.hightest_position())
    {
        blocks.reserve(board.set_positions());
        assert(valid());
    }
    constexpr Brick(const Brick& other) = default;
    constexpr Brick(Brick&& other) = default;
    constexpr ~Brick() = default;
    constexpr Brick& operator=(const Brick& other) = default;

    constexpr const bool valid() const
    {
        return (board.positions & LINE.positions)
            && (board.positions & (LINE << size.y()).positions) 
            && (board.positions & COLUMN.positions)
            && (board.positions & (COLUMN << size.x()).positions);
    }

    bool operator==(const Brick &other) const { return board.positions == other.board.positions; }

    constexpr const u64 placable_positions(const Board board) const;
    constexpr bool can_place(const Board board) const { return placable_positions(board); }

    Brick flip_vertically();
    Brick flip_horizontally();
    Brick rotate();
    Board operator<<(XY pos) { return board.positions << pos; }

    std::ostream &print(std::ostream &os) const;
    friend std::ostream &operator<<(std::ostream &os, const Brick &m) { return m.print(os); }
};

constexpr const u64 Brick::placable_positions(const Board board) const
{
    u64 result = 0;
    for (auto position: blocks)
    {

    }
    return ~result;
}
