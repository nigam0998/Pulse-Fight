// clang-format off
#include "animation.h"
#include "configuration.h"
#include "global.h"
#include "graphics.h"
#include "graphicsCore.h"
#include "input.h"
#include "playerAgent.h"
#include "playerBridge.h"
#include "sprite.h"
#include "utilities.h"
#include "aiAgent.h"
#include "menu.h"
#include "textRenderer.h"
#include <SDL2/SDL.h>
#include <string>
#include <list>
#include <vector>
// clang-format on

using namespace std;

#ifdef __cplusplus
extern "C"
#endif

    int
    main(int argc, char *argv[]) {
    cout << "SDL Version: " << SDL_GetCompiledVersion() << endl;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return -1;
    }

    SDL_Window *window = SDL_CreateWindow("Development of Street Fighter Game Using OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screen_width,
                                          screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

    if (!window) {
        SDL_Log("SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        SDL_Log("SDL_GL_CreateContext failed: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetSwapInterval(0);

    graphics->initialize(screen_width, screen_height);
    cout << "VideoSystem Init OK" << endl;

    Configuration configuration("data/config.yml");
    configuration.read();

    Input input;
    input.addPlayer(configuration.getConfigKeys(true), configuration.getConfigDevice(true));
    input.addPlayer(configuration.getConfigKeys(false), configuration.getConfigDevice(false));

    PlayerAgent player;
    player.initialize("data/ryu/ryu.yml", "data/ryu/moves.yml", true);

    PlayerAgent player2;
    player2.initialize("data/ryu/ryu.yml", "data/ryu/moves.yml", false);

    AIAgent ai_agent;
    Menu menu;
    TextRenderer textRenderer;

    enum class GameState { MENU, PLAYING, GAME_OVER };
    GameState state = GameState::MENU;
    std::string winnerText = "";

    auto p2 = player2.getPlayer().get();
    auto p1 = player.getPlayer().get();
    player.getPlayer()->setOpponent(p2);
    player2.getPlayer()->setOpponent(p1);

    Sprite background;
    GLuint texture = texture_manager->addTexture("data/background.png", false);
    background.setTexture(texture, 200, 100);

    PlayerBridge bridge;
    bridge.initialize(p1, p2);

    object_manager->add("0", p1);
    object_manager->add("1", p2);

    Uint32 lastTime = SDL_GetTicks();
    Uint32 lastStateChange = SDL_GetTicks();
    int frames = 0;

    bool running = true;
    bool enterPressed = false;
    float game_time = 0.0f;

    while (running && !input.quitGame()) {

        // ========== LOGIC (throttled) ==========
        if (updateGame(game_time)) {
            SDL_Event event = input.poll();

            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                int sw = event.window.data1;
                int sh = event.window.data2;
                if (sw > 0 && sh > 0) {
                    SDL_SetWindowSize(window, sw, sh);
                    graphics->resizeWindow(sw, sh);
                }
            }

            bool *keys = input.getKeys(0);

            if (state == GameState::MENU) {
                menu.update(keys);

                const Uint8 *kbState = SDL_GetKeyboardState(NULL);
                if (keys[KEY_A] || kbState[SDL_SCANCODE_RETURN] || kbState[SDL_SCANCODE_KP_ENTER]) {
                    if (!enterPressed && (SDL_GetTicks() - lastStateChange > 500)) {
                        enterPressed = true;
                        int sel = menu.getSelection();

                        if (!menu.isInSubMenu()) {
                            if (sel == 0) {
                                state = GameState::PLAYING;
                                lastStateChange = SDL_GetTicks();
                                p1->resetHp();
                                p2->resetHp();
                            } else if (sel == 1) {
                                menu.enterSubMenu();
                            } else if (sel == 2) {
                                running = false;
                            }
                        } else {
                            if (sel == 0) ai_agent.setDifficulty(AIDifficulty::EASY);
                            else if (sel == 1) ai_agent.setDifficulty(AIDifficulty::MEDIUM);
                            else if (sel == 2) ai_agent.setDifficulty(AIDifficulty::HARD);

                            menu.currentDifficulty = ai_agent.getDifficulty();
                            menu.exitSubMenu();
                        }
                    }
                } else {
                    enterPressed = false;
                }

                if (keys[KEY_B] && menu.isInSubMenu()) {
                    menu.exitSubMenu();
                }

            } else if (state == GameState::PLAYING) {
                player.update(keys);

                bool ai_keys[KEY_MAX];
                ai_agent.update(p2, p1, ai_keys);
                player2.update(ai_keys);

                string key = object_manager->first();
                while (key != "") {
                    bool done = false;
                    if (key != "0" && key != "1")
                        done = object_manager->get(key)->update();
                    if (done) {
                        object_manager->remove(key);
                        break;
                    }
                    key = object_manager->next();
                }

                bridge.update();

                if (p1->getHp() <= 0 || p2->getHp() <= 0) {
                    state = GameState::GAME_OVER;
                    if (p1->getHp() > p2->getHp())
                        winnerText = "You Won";
                    else if (p2->getHp() > p1->getHp())
                        winnerText = "AI Won";
                    else
                        winnerText = "STALEMATE!";
                }

            } else if (state == GameState::GAME_OVER) {
                const Uint8 *kbState = SDL_GetKeyboardState(NULL);
                if (keys[KEY_A] || kbState[SDL_SCANCODE_RETURN] || kbState[SDL_SCANCODE_KP_ENTER]) {
                    if (!enterPressed && (SDL_GetTicks() - lastStateChange > 500)) {
                        enterPressed = true;
                        state = GameState::MENU;
                        lastStateChange = SDL_GetTicks();
                        p1->resetHp();
                        p2->resetHp();

                        std::vector<std::string> keysToRemove;
                        std::string k = object_manager->first();
                        while (k != "") {
                            if (k != "0" && k != "1") keysToRemove.push_back(k);
                            k = object_manager->next();
                        }
                        for (size_t i = 0; i < keysToRemove.size(); i++) {
                            object_manager->remove(keysToRemove[i]);
                        }

                        p1->setCurrentState("start");
                        p1->setLocation(30, 0);
                        p1->inverted = false;
                        p1->state["INVERT"] = false;

                        p2->setCurrentState("start");
                        p2->setLocation(70, 0);
                        p2->inverted = true;
                        p2->state["INVERT"] = true;
                    }
                } else {
                    enterPressed = false;
                }
            }

            game_time = getGameTime();
        }

        // ========== RENDER (runs every loop iteration, never skipped) ==========
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        glTranslatef(-50.0f, -50.0f, -120.0f);

        if (state == GameState::MENU) {
            background.draw(50, 50, 1, false);
            menu.draw();

        } else if (state == GameState::PLAYING) {
            background.draw(50, 50, 1, false);

            list<Player *> objects = object_manager->getSortedList();
            for (auto i = objects.begin(); i != objects.end(); ++i)
                (*i)->draw();

            // Restore clean state for UI drawing
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // P1 Health Background (Red)
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
            glBegin(GL_QUADS);
            glVertex3i(2, 90, 0); glVertex3i(2, 95, 0);
            glVertex3i(48, 95, 0); glVertex3i(48, 90, 0);
            glEnd();

            // P1 Health Bar (Green)
            glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
            int p1Width = (int)(46.0f * ((float)p1->getHp() / 100.0f));
            glBegin(GL_QUADS);
            glVertex3i(2, 90, 0); glVertex3i(2, 95, 0);
            glVertex3i(2 + p1Width, 95, 0); glVertex3i(2 + p1Width, 90, 0);
            glEnd();

            // P2 Health Background (Red)
            glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
            glBegin(GL_QUADS);
            glVertex3i(98, 90, 0); glVertex3i(98, 95, 0);
            glVertex3i(52, 95, 0); glVertex3i(52, 90, 0);
            glEnd();

            // P2 Health Bar (Green)
            glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
            int p2Width = (int)(46.0f * ((float)p2->getHp() / 100.0f));
            glBegin(GL_QUADS);
            glVertex3i(98 - p2Width, 90, 0); glVertex3i(98 - p2Width, 95, 0);
            glVertex3i(98, 95, 0); glVertex3i(98, 90, 0);
            glEnd();

            // Reset state cleanly
            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glDisable(GL_BLEND);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_DEPTH_TEST);

        } else if (state == GameState::GAME_OVER) {
            background.draw(50, 50, 1, false);

            // Dim Background
            glDisable(GL_TEXTURE_2D);
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glColor4f(0.0f, 0.0f, 0.0f, 0.7f);
            glBegin(GL_QUADS);
            glVertex3i(0, 0, 0); glVertex3i(0, 100, 0);
            glVertex3i(100, 100, 0); glVertex3i(100, 0, 0);
            glEnd();
            glDisable(GL_BLEND);

            glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
            glEnable(GL_TEXTURE_2D);
            glEnable(GL_DEPTH_TEST);

            SDL_Color winColor = {255, 255, 0, 255};
            textRenderer.renderText(winnerText, winColor, 50.0f, 60.0f, 60.0f, 15.0f);

            SDL_Color promptColor = {255, 255, 255, 255};
            textRenderer.renderText("Press ENTER to return", promptColor, 50.0f, 40.0f, 40.0f, 6.0f);
        }

        SDL_GL_SwapWindow(window);

        frames++;
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime > lastTime + 1000) {
            lastTime = currentTime;
            frames = 0;
        }
    }

    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
