
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

#if defined(__GNUC__) | defined(__clang__)
constexpr static int NumberOfSetBits(u64 i)
{
    return __builtin_popcountll(i);
}
#else
constexpr static int NumberOfSetBits(u64 i)
{
    i = i - ((i >> 1) & 0x5555555555555555UL);
    i = (i & 0x3333333333333333UL) + ((i >> 2) & 0x3333333333333333UL);
    return (int)((((i + (i >> 4)) & 0xF0F0F0F0F0F0F0FUL) * 0x101010101010101UL) >> 56);
}
#endif

class Board
{
public:
    u64 positions;

    constexpr Board() = default;
    constexpr Board(u64 positions) : positions(positions) {}
    constexpr Board(const Board& other) = default;
    constexpr Board(Board&& other) = default;
    constexpr Board& operator=(const Board& other) = default;

    constexpr inline bool operator==(const Board other) const { return positions == other.positions; }

    constexpr inline Board operator|(const Board other) const { return positions | other.positions; }

    constexpr inline bool position(XY xy) const { return (positions >> xy.value) & 0x1; }
    constexpr inline bool operator[](XY xy) const { return position(xy); }

    constexpr X rightmost_position() const;
    constexpr Y hightest_position() const;

    constexpr bool can_combine(const Board other) const { return !(positions & other.positions); }

    constexpr std::pair<Board, u8> combine(Board other) const;

    constexpr int set_positions() const { return NumberOfSetBits(positions); }

    constexpr int free_positions() const
    {
        return BOARD_HEIGHT * BOARD_WIDTH - set_positions();
    }

    constexpr inline Grades grades();

    /// @brief  flips along - axis
    constexpr inline Board flipVertically();
    /// @brief  flips along | axis
    constexpr inline Board flipHorizontally();
    /// @brief  flips along \\ axis
    constexpr inline Board flipDiagonally1();
    /// @brief  flips along / axis
    constexpr inline Board flipDiagonally2();

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

constexpr std::pair<Board, u8> Board::combine(const Board other) const {
    assert(can_combine(other));
    u64 combination = positions | other.positions;
    
    u64 columns = combination;
    columns = columns & (columns >> Y1.value); // where 2 consecutive positions are set
    columns = columns & (columns >> Y2.value); // where 4 consecutive positions are set
    columns = columns & (columns >> Y4.value)  // where 8 consecutive positions are set
                & LINE.positions;              // keep only those corresponding to columns

#if defined(__GNUC__) | defined(__clang__)
    u8 cleared = __builtin_popcount(columns);  // count set bits int a 32 bit number (since only the last 8 can be set)
#else
    u8 cleared = columns;                                         // compute the number of set bits
    cleared = cleared - ((cleared >> 1) & (u8)0x55);              // 2 bits count the number of set bits from before
    cleared = (cleared & (u8)0x33) + ((cleared >> 2) & (u8)0x33); // sum for 4 bit groups
    cleared = (cleared + (cleared >> 4)) & 0x0F;                  // sum for 8 bit group
#endif

    u64 lines = combination;
    lines = lines & (lines >> X1.value); // where 2 consecutive positions are set
    lines = lines & (lines >> X2.value); // where 4 consecutive positions are set
    lines = lines & (lines >> X4.value)  // where 8 consecutive positions are set
                & COLUMN.positions;      // keep only those corresponding to lines

#if defined(__GNUC__) | defined(__clang__)
    cleared += __builtin_popcountll(columns);  // count set bits in a 64 bit number
#else
    cleared += (u8)((lines * 0x101010101010101UL) >> 56); //this is the number of set bits, because only the bits from COLUMN can be set
#endif

    u64 clearedLinesPositions   = lines   * LINE.positions;
    u64 clearedColumnsPositions = columns * COLUMN.positions;
    u64 clearedRowsPositions    = clearedLinesPositions | clearedColumnsPositions;
    combination &= ~clearedRowsPositions;

    return std::pair(Board(combination), cleared);
}

constexpr inline Grades Board::grades()
{
    Grades result;
    u64 left = positions ^ ((positions << X1.value) | COLUMN.positions);
    u64 right = positions ^ ((positions >> X1.value) | (COLUMN.positions << X7.value));
    u64 bottom = positions ^ ((positions << Y1.value) | LINE.positions);
    u64 top = positions ^ ((positions >> Y1.value) | (LINE.positions << Y7.value));
    for (XY xy: BOARD_XYS)
    {
        int differentBlocksAround = 0;
        u64 pos = ((u64) 0x1) << xy.value;
        if (left & pos) differentBlocksAround++;
        if (right & pos) differentBlocksAround++;
        if (top & pos) differentBlocksAround++;
        if (bottom & pos) differentBlocksAround++;

        if (positions & pos) 
        {
            result.used[differentBlocksAround]++;
        } else {
            result.free[differentBlocksAround]++;
        }
    }
    return result;
}

inline constexpr const Board operator|(const XY lhs, const XY rhs)
{
    return toBoard(lhs) | toBoard(rhs);
}

constexpr X Board::rightmost_position() const
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

constexpr Y Board::hightest_position() const
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

/// @brief  flips along - axis
constexpr inline Board Board::flipVertically()
{
    return ((positions & 0xFF'00'00'00'00'00'00'00) >> Y7.value)
         | ((positions & 0x00'FF'00'00'00'00'00'00) >> Y5.value)
         | ((positions & 0x00'00'FF'00'00'00'00'00) >> Y3.value)
         | ((positions & 0x00'00'00'FF'00'00'00'00) >> Y1.value)
         | ((positions & 0x00'00'00'00'FF'00'00'00) << Y1.value)
         | ((positions & 0x00'00'00'00'00'FF'00'00) << Y3.value)
         | ((positions & 0x00'00'00'00'00'00'FF'00) << Y5.value)
         | ((positions & 0x00'00'00'00'00'00'00'FF) << Y7.value);
}

/// @brief  flips along | axis
constexpr inline Board Board::flipHorizontally()
{
    return ((positions & 0x80'80'80'80'80'80'80'80) >> X7.value)
         | ((positions & 0x40'40'40'40'40'40'40'40) >> X5.value)
         | ((positions & 0x20'20'20'20'20'20'20'20) >> X3.value)
         | ((positions & 0x10'10'10'10'10'10'10'10) >> X1.value)
         | ((positions & 0x08'08'08'08'08'08'08'08) << X1.value)
         | ((positions & 0x04'04'04'04'04'04'04'04) << X3.value)
         | ((positions & 0x02'02'02'02'02'02'02'02) << X5.value)
         | ((positions & 0x01'01'01'01'01'01'01'01) << X7.value);
}

/// @brief  flips along \\ axis
constexpr inline Board Board::flipDiagonally1()
{
    return ((positions & 0x01'00'00'00'00'00'00'00) >> (Y7.value - X7.value))
         | ((positions & 0x02'01'00'00'00'00'00'00) >> (Y6.value - X6.value))
         | ((positions & 0x04'02'01'00'00'00'00'00) >> (Y5.value - X5.value))
         | ((positions & 0x08'04'02'01'00'00'00'00) >> (Y4.value - X4.value))
         | ((positions & 0x10'08'04'02'01'00'00'00) >> (Y3.value - X3.value))
         | ((positions & 0x20'10'08'04'02'01'00'00) >> (Y2.value - X2.value))
         | ((positions & 0x40'20'10'08'04'02'01'00) >> (Y1.value - X1.value))
         | ((positions & 0x80'40'20'10'08'04'02'01))
         | ((positions & 0x00'80'40'20'10'08'04'02) << (Y1.value - X1.value))
         | ((positions & 0x00'00'80'40'20'10'08'04) << (Y2.value - X2.value))
         | ((positions & 0x00'00'00'80'40'20'10'08) << (Y3.value - X3.value))
         | ((positions & 0x00'00'00'00'80'40'20'10) << (Y4.value - X4.value))
         | ((positions & 0x00'00'00'00'00'80'40'20) << (Y5.value - X5.value))
         | ((positions & 0x00'00'00'00'00'00'80'40) << (Y6.value - X6.value))
         | ((positions & 0x00'00'00'00'00'00'00'80) << (Y7.value - X7.value));
}

/// @brief  flips along / axis
constexpr inline Board Board::flipDiagonally2()
{
    return ((positions & 0x80'00'00'00'00'00'00'00) >> (Y7.value + X7.value))
         | ((positions & 0x40'80'00'00'00'00'00'00) >> (Y6.value + X6.value))
         | ((positions & 0x20'40'80'00'00'00'00'00) >> (Y5.value + X5.value))
         | ((positions & 0x10'20'40'80'00'00'00'00) >> (Y4.value + X4.value))
         | ((positions & 0x08'10'20'40'80'00'00'00) >> (Y3.value + X3.value))
         | ((positions & 0x04'08'10'20'40'80'00'00) >> (Y2.value + X2.value))
         | ((positions & 0x02'04'08'10'20'40'80'00) >> (Y1.value + X1.value))
         | ((positions & 0x01'02'04'08'10'20'40'80))
         | ((positions & 0x00'01'02'04'08'10'20'40) << (Y1.value + X1.value))
         | ((positions & 0x00'00'01'02'04'08'10'20) << (Y2.value + X2.value))
         | ((positions & 0x00'00'00'01'02'04'08'10) << (Y3.value + X3.value))
         | ((positions & 0x00'00'00'00'01'02'04'08) << (Y4.value + X4.value))
         | ((positions & 0x00'00'00'00'00'01'02'04) << (Y5.value + X5.value))
         | ((positions & 0x00'00'00'00'00'00'01'02) << (Y6.value + X6.value))
         | ((positions & 0x00'00'00'00'00'00'00'01) << (Y7.value + X7.value));
}