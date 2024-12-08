#include "bricks.h"

struct FreeParams
{
    int freeBlockWeight;
    inline float eval(Board b) {
        return b.free_positions() * freeBlockWeight;
    }
};

struct GradeParams
{
    int freeGradeWeight[5];
    int usedGradeWeight[5];
    inline float eval(Board b) const {
        int score = 0;
        Grades grades = b.grades();
        for (int i = 0; i < 5; i++) {
            score += grades.free[i] * freeGradeWeight[i] + grades.used[i] * usedGradeWeight[i];
        }
        return score;
    }
};

struct PlacableParams
{
    int place3x3Weight;
    int place5x1Weight;
    inline float eval(Board b) const {
        int score = 0;
        if (placable_positions_3X3(b)) score += place3x3Weight;
        if (placable_positions_5X1(b)) score += place5x1Weight;
        if (placable_positions_1X5(b)) score += place5x1Weight;
        return score;
    }
};

struct LineParams
{
    int evaluations[256];
    inline float eval(Board b) const {
        int score = 0;
        Board f = b.flipDiagonally1();
        for (Y y: YS)
        {
            score += evaluations[0xFF & (b.positions >> y.value)];
            score += evaluations[0xFF & (f.positions >> y.value)];
        }
        return score;
    }
    LineParams& addSetBitCost(std::function<float(int)> cost)
    {
        for (size_t i = 0; i <= 255; ++i)
        {
            evaluations[i] += cost(NumberOfSetBits(i));
        }
        return *this;
    }
    LineParams& addBitChangeCost(std::function<float(int)> cost)
    {
        for (size_t i = 0; i <= 255; ++i)
        {
            uint_fast16_t changed = i;
            changed = (0x100 | changed) ^ ((changed << 1) | 0x1);
            evaluations[i] += cost(NumberOfSetBits(changed));
        }
        return *this;
    }
    LineParams& addSetBitPositionCost(std::function<float(int)> cost)
    {
        for (size_t i = 0; i <= 255; ++i)
        {
            for (int bit = 0; bit < 8; ++bit)
            {
                if (i & (0x1 << bit))
                {
                    evaluations[i] += cost(bit);
                }
            }
        }
        return *this;
    }
};

struct EvaluatorPlacableParams2
{
    int place3x3Weight;
    int place5x1Weight;
    inline float eval(Board b) const {
        int score = 0;
        {
            u64 placablePositions = placable_positions_3X3(b);
            if (placablePositions) score += place3x3Weight + NumberOfSetBits(placablePositions);
        }
        {
            u64 placablePositions = placable_positions_5X1(b);
            if (placablePositions) score += place5x1Weight + NumberOfSetBits(placablePositions);
        }
        {
            u64 placablePositions = placable_positions_1X5(b);
            if (placablePositions) score += place5x1Weight + NumberOfSetBits(placablePositions);
        }
        return score;
    }
};