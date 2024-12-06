#include "brick.h"
#include <vector>


inline Brick brick1x1 {0b0000'0001};
inline Brick brick2x2 {0b0000'0011'0000'0011};
inline Brick brick3x3 {0b0000'0111'0000'0111'0000'0111};

inline Brick brick2x1 {0b0000'0011};
inline Brick brick3x1 {0b0000'0111};
inline Brick brick4x1 {0b0000'1111};
inline Brick brick5x1 {0b0001'1111};

inline Brick brick2L2 {0b0000'0001'0000'0011};
inline Brick brick3L2 {0b0000'0001'0000'0111};
inline Brick brick3L3 {0b0000'0001'0000'0001'0000'0111};

inline Brick brickT {0b0000'0010'0000'0111};
inline Brick brickZ {0b0000'0110'0000'0011};

inline Brick brick1x2 {brick2x1.rotate()};
inline Brick brick1x3 {brick3x1.rotate()};
inline Brick brick1x4 {brick4x1.rotate()};
inline Brick brick1x5 {brick5x1.rotate()};

std::vector<Brick> all_bricks();
const std::vector<Brick> ALL_BRICKS = all_bricks();
