#include "bricks.h"

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

struct EvaluatorPlacableParams
{
    int freeBlockWeight;
    int freeGradeWeight[5];
    int usedGradeWeight[5];
    int place3x3Weight;
    int place5x1Weight;
    inline float eval(Board b) {
        int freeBlocks = b.free_positions();
        int score = freeBlocks * freeBlockWeight;
        Grades grades = b.grades();
        for (int i = 0; i < 5; i++) {
            score += grades.free[i] * freeGradeWeight[i] + grades.used[i] * usedGradeWeight[i];
        }
        if (brick3x3.can_place(b)) score += place3x3Weight;
        if (brick5x1.can_place(b)) score += place5x1Weight;
        if (brick1x5.can_place(b)) score += place5x1Weight;
        return score;
    }
    inline auto evaluator() {
        return [this](Board b){ return eval(b); };
    }
};

struct EvaluatorPlacableParams2
{
    int freeBlockWeight;
    int freeGradeWeight[5];
    int usedGradeWeight[5];
    int place3x3Weight;
    int place5x1Weight;
    inline float eval(Board b) {
        int freeBlocks = b.free_positions();
        int score = freeBlocks * freeBlockWeight;
        Grades grades = b.grades();
        for (int i = 0; i < 5; i++) {
            score += grades.free[i] * freeGradeWeight[i] + grades.used[i] * usedGradeWeight[i];
        }
        {
            u64 placablePositions = brick3x3.placable_positions(b);
            if (placablePositions) score += place3x3Weight + NumberOfSetBits(placablePositions);
        }
        {
            u64 placablePositions = brick5x1.placable_positions(b);
            if (placablePositions) score += place5x1Weight + NumberOfSetBits(placablePositions);
        }
        {
            u64 placablePositions = brick1x5.placable_positions(b);
            if (placablePositions) score += place5x1Weight + NumberOfSetBits(placablePositions);
        }
        return score;
    }
    inline auto evaluator() {
        return [this](Board b){ return eval(b); };
    }
};