#include <inttypes.h>
#include <iostream>
#include <cassert>

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
    inline constexpr i8 operator-(const i8 other) const { return value - other.value; }
    inline constexpr i8 operator*(const i8 other) const { return value * other.value; }
    inline constexpr i8 operator++(int) { return value++; }
    inline constexpr i8 operator--(int) { return value--; }

    friend std::ostream &operator<<(std::ostream &os, i8 num)
    {
        return os << (int)num.value;
    }
};

const i8 BOARD_WIDTH = 8;
const i8 BOARD_HEIGHT = 8;

const u64 line = 0b11111111;
const u64 column = 0x0101010101010101;

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

    friend const u64 operator<<(const u64 blocks, const Position position)
    {
        assert(position.valid());
        return blocks << (position.x.value + position.y.value * BOARD_WIDTH.value);
    }

    friend std::ostream &operator<<(std::ostream &os, Position position)
    {
        return os << "(" << position.x << ", " << position.y << ")";
    }
};

#define POSITIONS_IN_LINE(line) Position(0, line), Position(1, line), Position(2, line), Position(3, line), Position(4, line), Position(5, line), Position(6, line), Position(7, line)
const Position board_positions[] = {POSITIONS_IN_LINE(0), POSITIONS_IN_LINE(1), POSITIONS_IN_LINE(2), POSITIONS_IN_LINE(3), POSITIONS_IN_LINE(4), POSITIONS_IN_LINE(5), POSITIONS_IN_LINE(6), POSITIONS_IN_LINE(7)};
#undef POSITIONS_IN_LINE

class Board
{
protected:
    u64 positions;

public:
    Board(u64 positions) : positions(positions) {}

    bool position(i8 x, i8 y);

    std::ostream &print(std::ostream &os) const;

    i8 rightmost_position() const;
    i8 hightest_position() const;

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
        return (positions & line) && (positions & (line << Position(0, height - 1))) && (positions & column) && (positions & (column << Position(width - 1, 0)));
    }

    std::ostream &print3(std::ostream &os) const;

    friend std::ostream &operator<<(std::ostream &os, Brick const &m)
    {
        return m.print3(os);
    }
};