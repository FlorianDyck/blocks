#include "types.h"
#include <cmath>

// using evaluation = std::function<void()>;
// typedef std::function<void()> evaluation;

struct Move {
    Brick brick;
    i8 x;
    i8 y;
    Board result;
};
using moves_t = std::vector<Move>;

void computeBest(
    Board current,
    moves_t current_moves,
    std::vector<Brick> remaining_bricks,
    float &best_evaluation,
    moves_t &best_moves,
    float (&evaluation)(Board)
)
{
    for (size_t i = 0; i < remaining_bricks.size(); i++) {

        std::vector<Brick> now_remaining;
        for (size_t j = 0; j < remaining_bricks.size(); j++) {
            if (i != j) {
                now_remaining.push_back(remaining_bricks[j]);
            }
        }
        Brick current_brick = remaining_bricks[i];

        for (i8 y = 0; y + current_brick.height <= BOARD_HEIGHT; y++) {
            for (i8 x = 0; x + current_brick.width <= BOARD_WIDTH; x++) {
                Board shifted_brick = current_brick << Position(x, y);
                if (!current.can_combine(shifted_brick)) {
                    continue;
                }
                Board new_board = current.combine(shifted_brick);
                current_moves.push_back({current_brick, x, y, new_board});

                if (now_remaining.empty()) {
                    float new_score = evaluation(new_board);
                    if (new_score > best_evaluation) {
                        // for (Move move: current_moves) {
                        //     std::cout << "\n+" << (move.brick << Position(move.x, move.y)) << "\n=" << move.result;
                        // }
                        std::cout << new_board << "\n";
                        new_board.print_eval_stats();
                        std::cout << "\nevaluation = " << new_score << std::endl;
                        best_evaluation = new_score;
                        best_moves = current_moves;
                    }
                } else {
                    computeBest(new_board, current_moves, now_remaining, best_evaluation, best_moves, evaluation);
                }

                current_moves.pop_back();
            }
        }
    }
}

moves_t computeBest(
    Board current,
    std::vector<Brick> bricks,
    float (&evaluation)(Board)
) {
    float best_evaluation = -INFINITY;
    moves_t best_moves;
    computeBest(current, moves_t(), bricks, best_evaluation, best_moves, evaluation);
    return best_moves;
}
