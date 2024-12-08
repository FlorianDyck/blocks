#include <functional>
#include "brick.h"

// using evaluation = std::function<void()>;
// typedef std::function<void()> evaluation;

struct Move {
    Brick brick;
    XY position;
    Board result;
    size_t cleared;
};
using moves_t = std::vector<Move>;

moves_t computeBest(
    Board current,
    std::vector<Brick> bricks,
    std::function<float(Board)> evaluation
);

moves_t computeBestParallel(
    Board current,
    std::vector<Brick> bricks,
    std::function<float(Board)> evaluation
);