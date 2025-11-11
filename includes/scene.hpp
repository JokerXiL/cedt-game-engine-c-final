#pragma once

struct Transition {
    TransitionKind kind;
    TransitionVariants variants;
};

enum TransitionKind {
    MAIN_GAME,
};


union TransitionVariants {
    ScoreScreenParams score_screen;
};

struct ScoreScreenParams {
    // todo
};

Transition main_title();
Transition main_game();
Transition score_screen(ScoreScreenParams);