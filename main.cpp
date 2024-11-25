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
    inline float eval(Board b) {
        int freeBlocks = b.free_positions();
        int score = freeBlocks * freeBlockWeight;
        Grades grades = b.grades();
        for (int i = 0; i < 5; i++) {
            score += grades.free[i] * freeGradeWeight[i] + grades.used[i] * usedGradeWeight[i];
        }
        return score;
    }
    inline auto evaluator() {
        return [this](Board b){ return eval(b); };
    }
};

struct Game{
    Board board;
    size_t cleared {0};
};

template <typename ostream1, typename ostream2>
void log_game(ostream1 &full_log, ostream2& partial_log, std::function<float(Board)> evaluation) {
    Game game;
    for(int i = 0; true; i++) {
        std::vector<Brick> bricks { 
            select_randomly(ALL_BRICKS),
            select_randomly(ALL_BRICKS),
            select_randomly(ALL_BRICKS)
        };
        full_log << "\niteration " << i << " cleared rows: " << game.cleared << " available bricks: " << bricks[0] << bricks[1] << bricks[2] << "\n";
        partial_log << "\niteration " << i << " cleared rows: " << game.cleared;
        moves_t moves = computeBest(game.board, bricks, evaluation);
        if (moves.empty()) {
            break;
        }
        for (Move move: moves) {
            full_log << "\n+" << (move.brick << Position(move.x, move.y)) << "\n=" << move.result;
            game.cleared += move.cleared;
        }
        game.board = moves.back().result;
        partial_log << game.board;
    };
}

int main()
{
    EvaluatorParams e1 {3, {0, -1, -3, -6, -24}, {0, 0, 0, -2, -10}};
    EvaluatorParams e2 {3, {0, -1, -2, -5, -24}, {0, 0, 0, -5, -4}};

    std::ofstream myfile;
    myfile.open("log.txt");
    log_game(myfile, std::cout, e2.evaluator());
    myfile.close();
}

