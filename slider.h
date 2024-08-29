#include <stdlib.h>
#include <SDL2/SDL.h>

typedef struct Slider {
    int x;
    int y;
    int width;
    int value;
    int colorR, colorG, colorB;
    bool _isMouseDown;
} Slider;

Slider* newSlider(int x, int y, int width, int value) {
    Slider* slider = (Slider*)malloc(sizeof(struct Slider));
    slider->x = x;
    slider->y = y;
    slider->width = width;
    slider->value = value;
    slider->colorR = 255;
    slider->colorG = 255;
    slider->colorB = 255;
    slider->_isMouseDown = false;
    return slider;
}

void renderSlider(SDL_Renderer* renderer, Slider* slider) {
    if(!slider->_isMouseDown)
        SDL_SetRenderDrawColor(renderer, slider->colorR, slider->colorG, slider->colorB, 255);
    else
        SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
    SDL_RenderDrawLine(renderer, slider->x, slider->y, 
                        slider->x + slider->width, slider->y);
    float startX = slider->x + ((float)slider->value/100) * slider->width;
    SDL_Rect rect = {
        startX - 10,
        slider->y - 10,
        20,
        20
    };
    SDL_RenderFillRect(renderer, &rect);
}

void UpdateKnobPosition(Slider* slider, SDL_Event* ev) {
    float deltaX = ev->button.x - slider->x;
    float value = (deltaX / slider->width) * 100;
    if(value < 0) value = 0;
    if(value > 100) value = 100;
    slider->value = value;
}

void HandleEvent(Slider* slider, SDL_Event* ev) {
    if(ev->type == SDL_MOUSEBUTTONUP) {
        slider->_isMouseDown = false;
    }
    float startX = slider->x + ((float)slider->value/100) * slider->width;
    SDL_Rect rect = {
        startX - 10,
        slider->y - 10,
        20,
        20
    };

    SDL_Point p;
    p.x = ev->button.x;
    p.y = ev->button.y;

    if(SDL_PointInRect(&p, &rect)) {
        if(ev->type == SDL_MOUSEBUTTONDOWN) {
            slider->_isMouseDown = true;
        }
    } else {
        slider->colorB = 255;
    }

    if(ev->type == SDL_MOUSEMOTION) {
        if(slider->_isMouseDown)
            UpdateKnobPosition(slider, ev);
    }
}