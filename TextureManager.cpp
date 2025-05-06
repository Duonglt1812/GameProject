#include "TextureManager.h"
#include "game.hpp"
#include <iostream>

SDL_Texture* TextureManager::LoadTexture(const char* fileName) {
    SDL_Surface* tempSurface = IMG_Load(fileName);
    if (!tempSurface) {
        std::cout << "Failed to load image: " << fileName << ", Error: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
    if (!tex) {
        std::cout << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
    }
    return tex;
}

void TextureManager::Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest) {
    SDL_RenderCopy(Game::renderer, tex, &src, &dest);
}

void TextureManager::DrawFlipped(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest, bool flipHorizontal) {
    SDL_RendererFlip flip = flipHorizontal ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(Game::renderer, tex, &src, &dest, 0.0, nullptr, flip);
}
