#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void RenderText(SDL_Renderer* renderer, TTF_Font* font, char* text, int x, int y) {
    SDL_Color color = {255, 255, 255, 0};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect rect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &rect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}