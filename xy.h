
#pragma once

#include <inttypes.h>
#include <cassert>
#include <iostream>

using u8 = uint_fast8_t;

constexpr const u8 BOARD_WIDTH {8};
constexpr const u8 BOARD_HEIGHT {8};

class XYwithin;

struct XY
{
    u8 value;

private:
    inline constexpr XY(u8 xy) : value(xy) {}

public:
    inline constexpr XY() : value(0) {}
    inline constexpr XY(u8 x, u8 y) : value((y << 3) + x)
    {
        assert(!((x | y) & ~0b000111));
    }
    inline constexpr bool valid() const { return !(value & ~0b111111); }

    inline constexpr XY x() const { return value & 0b000111; }
    inline constexpr XY y() const { return value & 0b111000; }

    inline constexpr u8 x_int() const { return value & 0b111; }
    inline constexpr u8 y_int() const { return value >> 3 & 0b111; }

    inline constexpr bool operator==(const XY other) const { return value == other.value; }
    inline constexpr XY operator++(int) { return value++; }
    inline constexpr XY operator+(XY other) const
    {
        assert(!(((x_int() + other.x_int()) | (y_int() + other.y_int())) & ~0b000111));
        return value + other.value;
    }
    inline constexpr XY operator-(XY other) const
    {
        assert(x_int() >= other.x_int() && y_int() >= other.y_int());
        return value - other.value;
    }

    inline constexpr const XYwithin begin() const;
    inline constexpr const XYwithin end() const;

    friend std::ostream &operator<<(std::ostream &os, XY xy)
    {
        return os << "(" << (int)xy.x_int() << ", " << (int)xy.y_int() << ")";
    }
};
struct X : public XY
{
    inline constexpr X(u8 x) : XY(x, 0) {}
};
struct Y : public XY
{
    inline constexpr Y(u8 y) : XY(0, y) {}
};

inline constexpr const X X0 = X(0), X1 = X(1), X2 = X(2), X3 = X(3), X4 = X(4), X5 = X(5), X6 = X(6), X7 = X(7);
inline constexpr const Y Y0 = Y(0), Y1 = Y(1), Y2 = Y(2), Y3 = Y(3), Y4 = Y(4), Y5 = Y(5), Y6 = Y(6), Y7 = Y(7);

#define XS(y) y + X0, y + X1, y + X2, y + X3, y + X4, y + X5, y + X6, y + X7
inline constexpr XY board_xys[] = {XS(Y0), XS(Y1), XS(Y2), XS(Y3), XS(Y4), XS(Y5), XS(Y6), XS(Y7)};
#undef XS

inline constexpr const X XS[] = {X(0), X(1), X(2), X(3), X(4), X(5), X(6), X(7)};
inline constexpr const Y YS[] = {Y(0), Y(1), Y(2), Y(3), Y(4), Y(5), Y(6), Y(7)};

inline constexpr const X XS_REV[] = {X(7), X(6), X(5), X(4), X(3), X(2), X(1), X(0)};
inline constexpr const Y YS_REV[] = {Y(7), Y(6), Y(5), Y(4), Y(3), Y(2), Y(1), Y(0)};

struct XYwithin
{
    XY position;
    XY bound;
    inline constexpr XYwithin(XY position, XY bound): position(position), bound(bound) {}
    inline constexpr XY operator*() { return position; }
    inline constexpr void operator++() 
    { 
        if (position.x() == bound.x())
        {
            position.value = position.y().value + Y1.value;
        }
        else 
        {
            position.value++;
        }
    }
    inline constexpr bool operator!=(const XYwithin other) const { return position.value != other.position.value || bound != other.bound; }
};

constexpr const XYwithin XY::begin() const
{
    return XYwithin(0, value);
}
constexpr const XYwithin XY::end() const
{
    return XYwithin(y().value + Y(1).value, value);
}
