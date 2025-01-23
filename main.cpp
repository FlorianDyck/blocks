#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <iterator>
#include <fstream>
#include <functional>
#include <filesystem>
#include <format>

#include "xy.h"
#include "evaluators.h"
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
const T select_randomly(const std::vector<T>& vec) {
    // return vec[0];
    return *select_randomly(vec.begin(), vec.end());
}


float eval1(Board b) {
    int freeBlocks = b.free_positions();
    Grades grades = b.grades();
    int borderLength = grades.free[1] + 2 * grades.free[2] + 3 * grades.free[3] + 4 * grades.free[4];
    return 3 * freeBlocks - 2 * borderLength - grades.free[4] * 20 - grades.free[3] * 3 - grades.used[4] * 2 - grades.used[3];
}

struct Game{
    Board board {0};
    size_t cleared {0};
};

template <typename ostream1, typename ostream2>
Game log_game(ostream1 &full_log, ostream2& partial_log, std::function<float(Board)> evaluation) {
    Game game;
    for(int i = 0; true; i++) {
        std::vector<Brick> bricks { 
            select_randomly(ALL_BRICKS),
            select_randomly(ALL_BRICKS),
            select_randomly(ALL_BRICKS)
        };
        full_log << "\niteration " << i << " cleared rows: " << game.cleared << " available bricks: " << bricks[0] << bricks[1] << bricks[2] << "\n";
        partial_log << "\niteration " << i << " cleared rows: " << game.cleared;
        moves_t moves = computeBestParallel(game.board, bricks, evaluation);
        if (moves.empty()) {
            break;
        }
        for (Move move: moves) {
            full_log << "\n+" << (move.brick << move.position) << "\n=" << move.result;
            game.cleared += move.cleared;
        }
        game.board = moves.back().result;
        partial_log << game.board;
    };
    return game;
}

int main()
{
    GradeParams gp {{3, 2, 1, -2, -21}, {0, 0, 0, -1, -5}};
    PlacableParams pp {20, 10};
    LineParams lp = LineParams{}
        .addBitChangeCost([](int changes) { return -(changes * changes / 4); })
        .addSetBitCost([](int setBits) { return -(setBits * (10 - setBits)) / 4; })
        .addSetBitPositionCost([](int position) { return -(position * (8 - position)) / 4; })
    ;

    int logNum = 1;
    while (std::filesystem::exists(std::format("logs/log{}.txt", logNum)))
    {
        logNum += 1;
    }

    std::ofstream myfile;
    myfile.open(std::format("logs/log{}.txt", logNum));
    Game game = log_game(myfile, std::cout, [gp, pp, lp](Board b){
        return gp.eval(b) + pp.eval(b) + lp.eval(b);
    });
    myfile.close();

    std::ofstream resultfile;
    resultfile.open("logs/log.txt", std::ios_base::out | std::ios_base::app);
    resultfile << std::format("logs/log{}.txt", logNum) << " score: " << game.cleared << std::endl;
    resultfile.close();
}

