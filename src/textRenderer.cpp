#include "textRenderer.h"
#include <iostream>

TextRenderer::TextRenderer() : font(nullptr), textureID(0) {
    if (!TTF_WasInit() && TTF_Init() == -1) {
        std::cerr << "TTF_Init: " << TTF_GetError() << std::endl;
    }
    font = TTF_OpenFont("data/arial.ttf", 24);
    if (!font) {
        std::cerr << "TTF_OpenFont: " << TTF_GetError() << std::endl;
    }
}

TextRenderer::~TextRenderer() {
    if (textureID) glDeleteTextures(1, &textureID);
    if (font) TTF_CloseFont(font);
}

void TextRenderer::renderText(const std::string& text, SDL_Color color, float x, float y, float width, float height) {
    if (!font) return;

    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;

    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    SDL_FreeSurface(surface);
    if (!formattedSurface) return;

    if (textureID) {
        glDeleteTextures(1, &textureID);
        textureID = 0;
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, formattedSurface->w, formattedSurface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, formattedSurface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    SDL_FreeSurface(formattedSurface);
    
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Set OpenGL color based on SDL_Color
    glColor4f(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
    
    // Draw quad with texture coordinates
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(x - width/2, y - height/2, 0.0f);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(x - width/2, y + height/2, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(x + width/2, y + height/2, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(x + width/2, y - height/2, 0.0f);
    glEnd();
    
    // Reset OpenGL color so other drawing calls aren't tainted
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    
    glDisable(GL_TEXTURE_2D);
}
