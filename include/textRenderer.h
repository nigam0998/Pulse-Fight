#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_opengl.h>
#include <string>

class TextRenderer {
private:
    TTF_Font* font;
    GLuint textureID;

public:
    TextRenderer();
    ~TextRenderer();
    void renderText(const std::string& text, SDL_Color color, float x, float y, float width, float height);
};

#endif // TEXT_RENDERER_H
