#include "menu.h"
#include "graphicsCore.h"
#include <iostream>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "textRenderer.h"

static TextRenderer* textRenderer = nullptr;


Menu::Menu() {
    currentSelection = 0;
    inSubMenu = false;
    upPressed = false;
    downPressed = false;
    currentDifficulty = AIDifficulty::MEDIUM;
    if(!textRenderer) {
        textRenderer = new TextRenderer();
    }
}

Menu::~Menu() {
}

void Menu::update(const bool* p1Keys) {
    // Handle Up
    if (p1Keys[KEY_UP]) {
        if (!upPressed) {
            currentSelection--;
            if (currentSelection < 0) currentSelection = 2; // Wrap around
            upPressed = true;
        }
    } else {
        upPressed = false;
    }

    // Handle Down
    if (p1Keys[KEY_DOWN]) {
        if (!downPressed) {
            currentSelection++;
            if (currentSelection > 2) currentSelection = 0; // Wrap around
            downPressed = true;
        }
    } else {
        downPressed = false;
    }
}

void Menu::draw() {
    glPushMatrix();
    
    // Position menu in the same space as the main game background (centered around 50, 50)
    glTranslatef(camera->getXposition() + 50.0f, camera->getYposition() + 50.0f, camera->getZposition());
    
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST); // UI needs to draw over everything
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glColor4f(0.0f, 0.0f, 0.0f, 0.8f);
    glBegin(GL_QUADS);
    glVertex3i(0, 0, 0);
    glVertex3i(0, 100, 0);
    glVertex3i(100, 100, 0);
    glVertex3i(100, 0, 0);
    glEnd();

    // Draw buttons centered horizontally
    int centerX = -20; // Move left towards the center

    for (int i = 0; i < 3; i++) {
        // Highlight logic
        if (i == currentSelection) {
            glColor4f(1.0f, 0.8f, 0.0f, 1.0f); // Yellow selection
        } else {
            glColor4f(0.3f, 0.3f, 0.3f, 1.0f); // Gray unselected
        }

        int btnY = -20 + (i * 20); // Spaced down to center it more vertically

        glBegin(GL_QUADS);
        glVertex3i(centerX - 30, btnY - 8, 0);
        glVertex3i(centerX - 30, btnY + 8, 0);
        glVertex3i(centerX + 30, btnY + 8, 0);
        glVertex3i(centerX + 30, btnY - 8, 0);
        glEnd();
        
        // Draw Text
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Set to white so texture color multiplies cleanly
        std::string btnText = "";
        
        if (!inSubMenu) {
            if (i == 0) btnText = "Start";
            if (i == 1) btnText = "Modes";
            if (i == 2) btnText = "Quit";
        } else {
            if (i == 0) btnText = "Easy";
            if (i == 1) btnText = "Medium";
            if (i == 2) btnText = "Hard";
            
            if ((i == 0 && currentDifficulty == AIDifficulty::EASY) ||
                (i == 1 && currentDifficulty == AIDifficulty::MEDIUM) ||
                (i == 2 && currentDifficulty == AIDifficulty::HARD)) {
                btnText += "  [x]"; // Indicate selection
            }
        }
        
        SDL_Color textColor = {255, 255, 255, 255}; 
        if (i == currentSelection) textColor = {0, 0, 0, 255}; // Black text on yellow background

        if(textRenderer) {
            textRenderer->renderText(btnText, textColor, centerX, btnY, 20.0f, 10.0f);
        }
    }

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
    glPopMatrix();
}

int Menu::getSelection() const {
    return currentSelection;
}

bool Menu::isInSubMenu() const {
    return inSubMenu;
}

void Menu::enterSubMenu() {
    inSubMenu = true;
    currentSelection = 0; // Reset to top
}

void Menu::exitSubMenu() {
    inSubMenu = false;
    currentSelection = 0; // Reset to top
}
