#ifndef OPEN_FIGHT_AI_AGENT_H
#define OPEN_FIGHT_AI_AGENT_H

#include "input.h"
#include "player.h"

enum class AIDifficulty {
    EASY,
    MEDIUM,
    HARD
};

class AIAgent {
private:
    bool keys[KEY_MAX];
    int actionTimer;
    int state; // 0: Idle, 1: Approach, 2: Retreat, 3: Attack
    int targetXOffset;
    AIDifficulty difficulty;

    void resetKeys();
public:
    AIAgent();
    ~AIAgent();

    void setDifficulty(AIDifficulty level);
    AIDifficulty getDifficulty() const;
    void update(Player* self, Player* opponent, bool* outKeys);
};

#endif
