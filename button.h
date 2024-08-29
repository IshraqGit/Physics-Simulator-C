#include <stdlib.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "utils.h"

typedef void (*ButtonEventHandler)();

typedef struct Button {
    int x;
    int y;
    int width;
    int height;
    char text[100];
    int colorR, colorG, colorB;
    bool _isMouseDown, _isMouseInside;
    ButtonEventHandler handler;
} Button;


Button* newButton(int x, int y, int width, int height, char* text) {
    Button* button = (Button*)malloc(sizeof(struct Button));
    button->x = x;
    button->y = y;
    button->width = width;
    button->height = height;
    strcpy(button->text, text);
    button->colorR = 255;
    button->colorG = 255;
    button->colorB = 255;
    button->_isMouseDown = false;
    button->_isMouseInside = false;
    return button;
}

void renderButton(SDL_Renderer* renderer, Button* button, TTF_Font* font) {
    if(button->_isMouseInside) {
        if(button->_isMouseDown) {
            SetButtonColor(button, 255, 255, 0);
        } else {
            SetButtonColor(button, 255, 0, 0);
        }
    } else {
        SetButtonColor(button, 255, 255, 255);
    }

    SDL_SetRenderDrawColor(renderer, button->colorR, button->colorG, button->colorB, 255);
    SDL_Rect rect = {
        button->x,
        button->y,
        button->width,
        button->height
    };
    SDL_RenderFillRect(renderer, &rect);

    
    SDL_Color color = {0, 0, 0, 255};
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, button->text, color);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    //RenderText(renderer, font, "Hello", button->x, button->y);
    float centerX = button->x + button->width/2.0;
    float centerY = button->y + button->height/2.0;
    int x = centerX - textSurface->w/2;
    int y = centerY - textSurface->h/2;
    
    SDL_Rect rectText = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &rectText);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void SetButtonColor(Button* button, int r, int g, int b) {
    button->colorR = r;
    button->colorG = g;
    button->colorB = b;
}

void ButtonEvent(SDL_Event* ev, Button* button) {
    SDL_Rect rect = {
        button->x,
        button->y,
        button->width,
        button->height
    };

    SDL_Point p = {
        ev->button.x,
        ev->button.y
    };

    if(SDL_PointInRect(&p, &rect)) {
        if(ev->type == SDL_MOUSEBUTTONDOWN) {
            button->_isMouseDown = true;
        }
        button->_isMouseInside = true;
    } else {
        button->_isMouseInside = false;
    }

    if(ev->type == SDL_MOUSEBUTTONUP) {
        button->_isMouseDown = false;
        if(button->_isMouseInside)
            button->handler();
    }
}

void registerButtonEvent(Button* button, ButtonEventHandler func) {
    button->handler = func;
}