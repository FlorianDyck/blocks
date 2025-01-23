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
    moves_t &current_moves,
    std::vector<Brick> &remaining_bricks,
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
                    best_moves = {new_score, current_moves};
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
    best_moves.moves.reserve(bricks.size());
    moves_t moves {};
    moves.reserve(bricks.size());
    computeBest(current, moves, bricks, best_moves, evaluation);
    return best_moves.moves;
}

moves_t computeBestParallel(
    Board current,
    std::vector<Brick> remaining_bricks,
    std::function<float(Board)> evaluation
)
{
    evaluated_moves_t best_moves;

    Brick current_brick = remaining_bricks.back();
    remaining_bricks.pop_back();
    for (size_t i = 0; i <= remaining_bricks.size(); i++) {
        std::vector<evaluated_moves_t> best_moves_vec(
            (8 - current_brick.size.x_int()) * (8 - current_brick.size.y_int()), 
            best_moves
        );

        #pragma omp parallel for shared(current_brick, best_moves_vec, remaining_bricks, evaluation)
        for (size_t i = 0; i < best_moves_vec.size(); ++i)
        {
            evaluated_moves_t &best_moves = best_moves_vec[i];
            XY xy (i % (8 - current_brick.size.x_int()), i / (8 - current_brick.size.x_int()));

            Board shifted_brick = current_brick << xy;
            if (!current.can_combine(shifted_brick)) {
                continue;
            }
            auto[new_board, cleared] = current.combine(shifted_brick);
            moves_t next_moves = {{current_brick, xy, new_board, cleared}};

            if (remaining_bricks.empty()) {
                float new_score = evaluation(new_board);
                if (new_score > best_moves.evaluation) {
                    best_moves = {new_score, next_moves};
                }
            } else {
                computeBest(new_board, next_moves, remaining_bricks, best_moves, evaluation);
            }
        }
        
        for (evaluated_moves_t moves: best_moves_vec)
        {
            if (moves.evaluation > best_moves.evaluation)
            {
                best_moves = moves;
            }
        }
        if (i < remaining_bricks.size())
        {
            std::swap(remaining_bricks[i], current_brick);
        }
    }
    remaining_bricks.push_back(current_brick);
    return best_moves.moves;
}