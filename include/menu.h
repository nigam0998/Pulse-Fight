#ifndef OPEN_FIGHT_MENU_H
#define OPEN_FIGHT_MENU_H

#include <GL/glew.h>
#include <SDL2/SDL_opengl.h>
#include "aiAgent.h"

class Menu {
private:
    int currentSelection;
    bool inSubMenu; // false = Main Menu, true = Difficulty Menu
    bool upPressed;
    bool downPressed;

public:
    Menu();
    ~Menu();

    void update(const bool* p1Keys);
    void draw();

    // 0: Start, 1: Modes, 2: Quit
    int getSelection() const;
    bool isInSubMenu() const;
    void enterSubMenu();
    void exitSubMenu();
    
    // Pass in the global difficulty to render it correctly
    AIDifficulty currentDifficulty;
};

#endif
