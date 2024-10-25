#include <iostream>
#include <vector>
#include <string>
#include "types.h"
#include "computation.h"

float eval1(Board b) {
    int freeBlocks = b.free_positions();
    Grades grades = b.grades();
    int borderLength = grades.free[1] + 2 * grades.free[2] + 3 * grades.free[3] + 4 * grades.free[4];
    return freeBlocks - borderLength;
}

int main()
{
    std::vector<Brick> moves { Brick(0x0F0F0F), Brick(0x0F0F03), Brick(0x0C0F0F) };
    computeBest(Board(), moves, eval1);
    std::cout << "Hello World";
}

