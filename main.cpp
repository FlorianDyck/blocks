#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <iterator>
#include <fstream>
#include <functional>

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
    return 3 * freeBlocks - 2 * borderLength - grades.free[4] * 20 - grades.free[3] * 3 - grades.used[4] * 2 - grades.used[3];
}

struct EvaluatorParams
{
    int freeBlockWeight;
    int freeGradeWeight[5];
    int usedGradeWeight[5];
    float eval(Board b) {
        int freeBlocks = b.free_positions();
        int score = freeBlocks * freeBlockWeight;
        Grades grades = b.grades();
        for (int i = 0; i < 5; i++) {
            score += grades.free[i] * freeGradeWeight[i] + grades.used[i] * usedGradeWeight[i];
        }
        return score;
    }
    auto evaluator() {
        return [this](Board b){ return this->eval(b); };
    }
};

void test(float (&evaluation)(Board)) {}
int test2(std::function<float(Board)>) {return 0;}


int main()
{
    // std::vector<Brick> bricks { Brick(0x0F0F0F), Brick(0x0F0F03), Brick(0x0C0F0F) };
    std::ofstream myfile;
    myfile.open("log.txt");
    Board board;
    size_t cleared = 0;
    EvaluatorParams e1 {3, {0, -1, -3, -6, -24}, {0, 0, 0, -1, -2}};
    test2(e1.evaluator());
    for(int i = 0; true; i++) {
        std::vector<Brick> bricks { 
            select_randomly(ALL_BRICKS),
            select_randomly(ALL_BRICKS),
            select_randomly(ALL_BRICKS)
        };
        myfile << "\niteration " << i << " cleared rows: " << cleared << " available bricks: " << bricks[0] << bricks[1] << bricks[2] << "\n";
        std::cout << "\niteration " << i << " cleared rows: " << cleared;
        moves_t moves = computeBest(board, bricks, e1.evaluator());
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

