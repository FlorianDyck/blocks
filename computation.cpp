#include <functional>
#include <cmath>
#include <future>
#include "computation.h"

struct evaluated_moves_t
{
    float evaluation = -INFINITY;
    moves_t moves;
};

void computeBest(
    Board current,
    moves_t current_moves,
    std::vector<Brick> remaining_bricks,
    evaluated_moves_t &best_moves,
    std::function<float(Board)> evaluation
)
{
    Brick current_brick = remaining_bricks.back();
    remaining_bricks.pop_back();
    for (size_t i = 0; i <= remaining_bricks.size(); i++) {
        for (XY xy: XY(7,7) - current_brick.size)
        {
            Board shifted_brick = current_brick << xy;
            if (!current.can_combine(shifted_brick)) {
                continue;
            }
            auto[new_board, cleared] = current.combine(shifted_brick);
            current_moves.push_back({current_brick, xy, new_board, cleared});

            if (remaining_bricks.empty()) {
                float new_score = evaluation(new_board);
                if (new_score > best_moves.evaluation) {
                    best_moves.evaluation = new_score;
                    best_moves.moves = current_moves;
                }
            } else {
                computeBest(new_board, current_moves, remaining_bricks, best_moves, evaluation);
            }

            current_moves.pop_back();
        }
        if (i < remaining_bricks.size())
        {
            std::swap(remaining_bricks[i], current_brick);
        }
    }
    remaining_bricks.push_back(current_brick);
}

moves_t computeBest(
    Board current,
    std::vector<Brick> bricks,
    std::function<float(Board)> evaluation
) {
    evaluated_moves_t best_moves;
    computeBest(current, moves_t(), bricks, best_moves, evaluation);
    return best_moves.moves;
}