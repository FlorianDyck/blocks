#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <iterator>
#include <fstream>

#include "bricks.h"
#include "types.h"
#include "computation.h"

template<typename Iter, typename RandomGenerator>
Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device random_device;
    static std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, std::distance(start, end) - 1);
    std::advance(start, distribution(generator));
    return start;
}

template<typename T>
const T select_randomly(const std::vector<T> vec) {
    return *select_randomly(vec.begin(), vec.end());
}


float eval1(Board b) {
    int freeBlocks = b.free_positions();
    Grades grades = b.grades();
    int borderLength = grades.free[1] + 2 * grades.free[2] + 3 * grades.free[3] + 4 * grades.free[4];
    return 5 * freeBlocks - 2 * borderLength - grades.free[4] * 20 - grades.free[3] * 3 - grades.used[4] * 2 - grades.used[3];
}

int main()
{
    // std::vector<Brick> bricks { Brick(0x0F0F0F), Brick(0x0F0F03), Brick(0x0C0F0F) };
    std::ofstream myfile;
    myfile.open("log.txt");
    Board board;
    size_t cleared = 0;
    for(int i = 0; true; i++) {
        std::vector<Brick> bricks { 
            select_randomly(ALL_BRICKS),
            select_randomly(ALL_BRICKS),
            select_randomly(ALL_BRICKS)
        };
        myfile << "\niteration " << i << " cleared rows: " << cleared << " available bricks: " << bricks[0] << bricks[1] << bricks[2] << "\n";
        std::cout << "\niteration " << i << " cleared rows: " << cleared;
        moves_t moves = computeBest(board, bricks, eval1);
        if (moves.empty()) {
            break;
        }
        for (Move move: moves) {
            myfile << "\n+" << (move.brick << Position(move.x, move.y)) << "\n=" << move.result;
            cleared += move.cleared;
        }
        board = moves.back().result;
        std::cout << board;
    };
    myfile.close();
}

