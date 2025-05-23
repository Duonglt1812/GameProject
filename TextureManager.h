#pragma once
#pragma once
#include <SDL.h>
#include <SDL_image.h>

class TextureManager {
public:
    static SDL_Texture* LoadTexture(const char* fileName);
    static void Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest);
    static void DrawFlipped(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, bool flipHorizontal);
};
