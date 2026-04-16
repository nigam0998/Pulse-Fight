#include "aiAgent.h"
#include <stdlib.h>
#include <time.h>
#include <cmath>

AIAgent::AIAgent() {
    resetKeys();
    actionTimer = 0;
    state = 0;
    targetXOffset = 0;
    difficulty = AIDifficulty::MEDIUM;
    srand(time(NULL));
}

AIAgent::~AIAgent() {}

void AIAgent::setDifficulty(AIDifficulty level) {
    difficulty = level;
}

AIDifficulty AIAgent::getDifficulty() const {
    return difficulty;
}

void AIAgent::resetKeys() {
    for (int i = 0; i < KEY_MAX; i++) {
        keys[i] = false;
    }
}

void AIAgent::update(Player* self, Player* opponent, bool* outKeys) {
    resetKeys();
    
    if (self == nullptr || opponent == nullptr) {
        for(int i=0; i<KEY_MAX; i++) outKeys[i] = keys[i];
        return;
    }

    float myX = self->getXpos();
    float opX = opponent->getXpos();
    float dist = std::abs(myX - opX);

    // AI state machine
    if (actionTimer <= 0) {
        // Decide next action probabilities based on difficulty
        int approachProb, attackProb, retreatProb; // idle is the remainder
        int minTimer, maxTimer;
        
        switch(difficulty) {
            case AIDifficulty::EASY:
                approachProb = 60; // Less aggressive approach
                attackProb = 20;   // Rarely attacks
                retreatProb = 40;  // Often retreats
                minTimer = 30;     // Slow reaction
                maxTimer = 80;
                break;
            case AIDifficulty::MEDIUM:
                approachProb = 80; 
                attackProb = 50;   
                retreatProb = 30;  
                minTimer = 15;     
                maxTimer = 45;
                break;
            case AIDifficulty::HARD:
                approachProb = 95; // Always rushing in
                attackProb = 85;   // Very aggressive
                retreatProb = 10;  // Rarely retreats
                minTimer = 5;      // Lightning fast
                maxTimer = 20;
                break;
        }

        int r = rand() % 100;
        
        if (dist > 15.0f) {
            // Far away mechanics
            if (r < approachProb) state = 1; // Approach
            else state = 0; // Idle
        } else {
            // Close up mechanics
            if (r < attackProb) state = 3; // Attack
            else if (r < attackProb + retreatProb) state = 2; // Retreat
            else state = 0; // Idle
        }
        
        // Duration for the action
        actionTimer = minTimer + rand() % (maxTimer - minTimer + 1);
        
        // Pick a preferred distance
        targetXOffset = 5 + rand() % 5;
    }
    actionTimer--;

    // Execute state
    switch(state) {
        case 0: // Idle
            // Do nothing, maybe crouch occasionally
            if (rand() % 100 < 5) keys[KEY_DOWN] = true;
            break;
            
        case 1: // Approach
            if (myX < opX - targetXOffset) {
                keys[KEY_RIGHT] = true;
            } else if (myX > opX + targetXOffset) {
                keys[KEY_LEFT] = true;
            }
            break;
            
        case 2: // Retreat
            if (myX < opX) {
                keys[KEY_LEFT] = true;
            } else {
                keys[KEY_RIGHT] = true;
            }
            break;
            
        case 3: // Attack
            // Hold direction towards opponent
            if (myX < opX) keys[KEY_RIGHT] = true;
            else keys[KEY_LEFT] = true;
            
            // Randomly push attack buttons
            int attackR = rand() % 100;
            if (attackR < 15) keys[KEY_A] = true;
            else if (attackR < 30) keys[KEY_B] = true;
            else if (attackR < 45) keys[KEY_C] = true;
            else if (attackR < 60) keys[KEY_X] = true;
            else if (attackR < 75) keys[KEY_Y] = true;
            else if (attackR < 90) keys[KEY_Z] = true;
            break;
    }

    // Pass inputs back out
    for(int i=0; i<KEY_MAX; i++) {
        outKeys[i] = keys[i];
    }
}
