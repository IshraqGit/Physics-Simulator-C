
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "button.h"
#include "helper.h"
#include "linked_list.h"
#include "ball.h"
#include "slider.h"
//#include "utils.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define CONSTRAINT_MODE_CIRCLE 0
#define CONSTRAINT_MODE_SQUARE 1
#define CONSTRAINT_MODE_TRIANGLE 2

int constarintMode = CONSTRAINT_MODE_SQUARE;

Node* head = NULL;
Ball* ball;
Ball* lastBall;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

Node* linkHead = NULL;
int insertMode = BALL_TYPE_NORMAL;

TTF_Font* font;

Mix_Music* popEffect;
Mix_Chunk *soundEffect1, *soundEffect2, *soundEffect3, *soundEffect4;
// UI STUFFS
Slider* sliderRadius; 
Slider* sliderGravity;

Button* toggleButton;
Button* clearButton;
Button* modeButton;

bool insertedChain = false;

void ApplyCircleWall() {
    float centerX = SCREEN_WIDTH / 2;
    float centerY = SCREEN_HEIGHT / 2;
    float radius = 250;

    Node* tmp = head;

    while(tmp) {
        float ballX = ((Ball*)tmp->value)->x;
        float ballY = ((Ball*)tmp->value)->y;

        float dist = EuclideanDistance(ballX, ballY, centerX, centerY);
        float toX = ballX - centerX;
        float toY = ballY - centerY;
          
        float clamped_radius = radius - (float)(((Ball*)tmp->value)->radius);
        //printf("Radius: %f\n", clamped_radius);
        if(dist > clamped_radius) {
            toX = (toX / dist) * clamped_radius;
            toY = (toY / dist) * clamped_radius;

            ((Ball*)tmp->value)->x = centerX + toX;
            ((Ball*)tmp->value)->y = centerY + toY;

            //printf("Setting to X: %f, Y: %f\n", centerX + toX, centerY + toY);
        }
        tmp = tmp->next;
    }
}

void ApplySquareWall() {
    float centerX = SCREEN_WIDTH / 2;
    float centerY = SCREEN_HEIGHT / 2;
    float span = 250;

    Node* tmp = head;

    while(tmp) {
        float ballX = ((Ball*)tmp->value)->x;
        float ballY = ((Ball*)tmp->value)->y;

        Ball* ball = (Ball*)tmp->value;

        if(ballX < centerX - span + ball->radius) {
            ball->x = centerX - span + ball->radius;
        }
        if(ballX > centerX + span - ball->radius) {
            ball->x = centerX + span - ball->radius;
        }
        if(ballY > centerY + span - ball->radius) {
            ball->y = centerY + span - ball->radius;
        }
        if(ballY < centerY - span + ball->radius) {
            ball->y = centerY - span + ball->radius;
        }
        tmp = tmp->next;
    }
}

void ApplySquareTriangle() {
    float centerX = SCREEN_WIDTH / 2;
    float centerY = SCREEN_HEIGHT / 2;
    float span = 250;

    double x1 = centerX;
    double y1 = centerY - span;
    double x2 = centerX - span;
    double y2 = centerY + span;
    double x3 = centerX + span;
    double y3 = centerY + span;

    Node* tmp = head;

    while(tmp) {
        float ballX = ((Ball*)tmp->value)->x;
        float ballY = ((Ball*)tmp->value)->y;

        Ball* ball = (Ball*)tmp->value;

        point p;
        p = findPointInTriangle(ball->x, ball->y, ball->radius, x1, y1, x2, y2, x3, y3);

        ball->x = p.x;
        ball->y = p.y;

        tmp = tmp->next;
    }
}

void ApplyWall() {
    if(constarintMode == CONSTRAINT_MODE_CIRCLE) {
        ApplyCircleWall();
    } else if(constarintMode == CONSTRAINT_MODE_SQUARE) {
        ApplySquareWall();
    } else if(constarintMode == CONSTRAINT_MODE_TRIANGLE) {
        ApplySquareTriangle();
    }
}

void ApplyCollisions() {
    Node* tmpi = head;
    while(tmpi && tmpi->next) {
        Node* tmpj = tmpi->next;
        while(tmpj) {
            Ball* ball1 = (Ball*)tmpi->value;
            Ball* ball2 = (Ball*)tmpj->value;
            if(ball1->type == BALL_TYPE_LINK && ball2->type == BALL_TYPE_LINK) {
                tmpj = tmpj->next;
                continue;
            }

            float toX = ball2->x - ball1->x;
            float toY = ball2->y - ball1->y;

            float dist = EuclideanDistance(ball1->x, ball1->y, ball2->x, ball2->y);
            float delta = (ball1->radius + ball2->radius) - dist;

            float nX = toX / dist;
            float nY = toY / dist;

            if(dist < (ball1->radius + ball2->radius)) {
                ball1->x -= 0.5 * delta * nX;
                ball1->y -= 0.5 * delta * nY;

                ball2->x += 0.5 * delta * nX;
                ball2->y += 0.5 * delta * nY;
            }
            tmpj = tmpj->next;
        }
        tmpi = tmpi->next;
    }
}

void ApplyStaticConstraints() {
    Node* tmpi = head;
    while(tmpi) {
        Ball* ball = (Ball*)tmpi->value;
        if(ball->type != BALL_TYPE_STATIC)
        {
            tmpi = tmpi->next;
            continue;
        }
        ball->x = ball->initX;
        ball->y = ball->initY;
        tmpi = tmpi->next;
    }
}

void ApplyLinks() {
    Node* tmpi = linkHead;
    while(tmpi) {
        Link* link = (Link*)tmpi->value;
        applyLink(link);
        tmpi = tmpi->next;
    }
}

void CreateLink(Ball* ball1, Ball* ball2) {
    Link* link = (Link*)malloc(sizeof link);
    link->ball1 = ball1;
    link->ball2 = ball2;
    link->target_dist = 25;

    insertNode(&linkHead, link);
}

void InsertChain() {
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;
    CreateStaticBall(centerX - 170, centerY);
    for(int x = centerX - 150; x <= centerX + 150; x += 20) {
        CreateLinkedBall(x, centerY);
    }
    CreateLinkedStaticBall(centerX + 150, centerY);
}

void toggleButton_Click() {
    if(!insertedChain) {
        InsertChain();
        insertedChain = true;
        strcpy(toggleButton->text, "(Inserted)");
    }
}

void clearButton_Click() {
    deleteAllFromCurrent(&head);
    insertedChain = false;
    strcpy(toggleButton->text, "Insert Chain");
}

void modeButton_Click() {
    constarintMode = !constarintMode;
}

void InitializeUI() {
    sliderRadius = newSlider(SCREEN_WIDTH + 25, 75, 250, 0);
    sliderRadius->value = 5;

    sliderGravity = newSlider(SCREEN_WIDTH + 25, 175, 250, 0);
    sliderGravity->value = 70;

    toggleButton = newButton(SCREEN_WIDTH + 25, 275, 250, 70, "Insert Chain");
    registerButtonEvent(toggleButton, toggleButton_Click);

    clearButton = newButton(SCREEN_WIDTH + 25, 365, 250, 70, "Delete All");
    registerButtonEvent(clearButton, clearButton_Click);

    modeButton = newButton(SCREEN_WIDTH + 25, 455, 250, 70, "Square/Circle");
    registerButtonEvent(modeButton, modeButton_Click);
}

void InitializeFont() {
    if(TTF_Init() == -1) {
        printf("Couldn't initialize TTF.");
    }
    font = TTF_OpenFont("arial.ttf", 24);
    if(!font) {
        printf("Couldn't initialize font\n");
    }
}

void InitializeAudio() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        printf("Error: %s\n", Mix_GetError());
    }

    popEffect = Mix_LoadMUS("Music/pop.mp3");
    if(!popEffect) {
        printf("Couldn't load popEffect: %s\n", Mix_GetError());
    }

    soundEffect1 = Mix_LoadWAV("Music/Sound_Effect_01.wav");
    soundEffect2 = Mix_LoadWAV("Music/Sound_Effect_02.wav");
    soundEffect3 = Mix_LoadWAV("Music/Sound_Effect_03.wav");
    soundEffect4 = Mix_LoadWAV("Music/Sound_Effect_04.wav");
}

void Initialize() {
    InitializeUI();
    InitializeFont();
    InitializeAudio();
}

void update(double deltaTime) {
    int subStep = 4;
    double subDeltaTime = deltaTime / subStep;
    for(int i=0; i<subStep; i++) {
        Node* tmp = head;
        while(tmp) {
            BallUpdate(tmp->value, subDeltaTime);
            tmp = tmp->next;
        }
        ApplyLinks();
        ApplyWall();
        ApplyStaticConstraints();
        ApplyCollisions();
    }
}

void renderUI(SDL_Renderer* renderer) {
    // Slider for Radius
    renderSlider(renderer, sliderRadius);
    char radiusText[100];
    float radius = 5 + (sliderRadius->value/100.0) * 25;
    sprintf(radiusText, "Radius: %d", (int)radius);
    RenderText(renderer, font, radiusText, SCREEN_WIDTH + 25, 25);

    // Slider for gravity
    renderSlider(renderer, sliderGravity);
    char gravityText[100];
    sprintf(gravityText, "Gravity: %d", sliderGravity->value);
    RenderText(renderer, font, gravityText, SCREEN_WIDTH + 25, 125);

    // Button for toggling chain on / off
    renderButton(renderer, toggleButton, font);
    
    // Button for clearing the balls
    renderButton(renderer, clearButton, font);

    // Button for changing modes;
    renderButton(renderer, modeButton, font);
}

void renderBackground(SDL_Renderer* renderer) {
    if(constarintMode == CONSTRAINT_MODE_CIRCLE) {
        float centerX = SCREEN_WIDTH / 2;
        float centerY = SCREEN_HEIGHT / 2;
        float radius = 250;

        SDL_SetRenderDrawColor(renderer, 111, 220, 227, 255);
        RenderFillCircle(renderer, centerX, centerY, radius + 20);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        RenderFillCircle(renderer, centerX, centerY, radius);
    } else if(constarintMode == CONSTRAINT_MODE_SQUARE) {
        float centerX = SCREEN_WIDTH / 2;
        float centerY = SCREEN_HEIGHT / 2;
        float span = 250;

        SDL_SetRenderDrawColor(renderer, 111, 220, 227, 255);

        SDL_Rect* rect;
        rect->x = centerX - span - 20;
        rect->y = centerY - span - 20;
        rect->w = 2*span + 40;
        rect->h = 2*span + 40;
        SDL_RenderFillRect(renderer, rect);

        rect->x += 20;
        rect->y += 20;
        rect->w -= 40;
        rect->h -= 40;

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, rect);
    }
}

void render(double deltaTime) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    renderBackground(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    Node* tmp = head;
    while(tmp) {
        BallRenderDraw((Ball*)tmp->value, renderer);
        tmp = tmp->next;
    }

    //RenderFillCircle(renderer, 640/2, 480/2, 25);
    renderUI(renderer);
    SDL_RenderPresent(renderer);
}

void CreateNewBall(int x, int y) {
    float radius = 5 + (sliderRadius->value/100.0) * 25;
    Ball* newBall = BallCreate(x, y, radius, 0, sliderGravity->value * 10, BALL_TYPE_NORMAL);
    insertNode(&head, newBall);
}

void CreateStaticBall(int x, int y) {
    Ball* newBall = BallCreate(x, y, 10, 0, 200, BALL_TYPE_STATIC);
    insertNode(&head, newBall);
    lastBall = newBall;
}

void CreateLinkedBall(int x, int y) {
    Ball* newBall = BallCreate(x, y, 10, 0, 200, BALL_TYPE_LINK);
    insertNode(&head, newBall);

    CreateLink(lastBall, newBall);

    lastBall = newBall;
}

void CreateLinkedStaticBall(int x, int y) {
    Ball* newBall = BallCreate(x, y, 10, 0, 200, BALL_TYPE_STATIC);
    insertNode(&head, newBall);

    CreateLink(newBall, lastBall);

    lastBall = newBall;
}

void HandleUIEvents(SDL_Event* ev) {
    HandleEvent(sliderRadius, ev);
    HandleEvent(sliderGravity, ev);
    ButtonEvent(ev, toggleButton);
    ButtonEvent(ev, clearButton);
    ButtonEvent(ev, modeButton);
}

void HandlePopSound() {
    int choice = rand() % 4 + 1;
    switch(choice) {
        case 1:
            Mix_PlayChannel(-1, soundEffect1, 0);
            break;
        case 2:
            Mix_PlayChannel(-1, soundEffect2, 0);
            break;
        case 3:
            Mix_PlayChannel(-1, soundEffect3, 0);
            break;
        case 4:
            Mix_PlayChannel(-1, soundEffect4, 0);
            break;
    }
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_CreateWindowAndRenderer(SCREEN_WIDTH + 300, SCREEN_HEIGHT, 0, &window, &renderer);

    Initialize();

    bool isRunning = true;
    SDL_Event ev;
    int lastTick = SDL_GetTicks();
    double deltaTime;
    while(isRunning) {
        lastTick = SDL_GetTicks();
        while(SDL_PollEvent(&ev) != 0) {
            if(ev.type == SDL_QUIT) {
                isRunning = false;
            }
            if(ev.button.x >= SCREEN_WIDTH) {
                HandleUIEvents(&ev);
                continue;
            }
            if(ev.type == SDL_MOUSEBUTTONDOWN) {
                HandlePopSound();
                if(insertMode == BALL_TYPE_NORMAL) {
                    CreateNewBall(ev.button.x, ev.button.y);
                } else if(insertMode == BALL_TYPE_STATIC) {
                    CreateStaticBall(ev.button.x, ev.button.y);
                } else if(insertMode == BALL_TYPE_LINK) {
                    CreateLinkedBall(ev.button.x, ev.button.y);
                }
            }
            if(ev.type == SDL_KEYUP) {
                if(ev.key.keysym.sym == SDLK_m) {
                    if(insertMode == BALL_TYPE_NORMAL) {
                        insertMode = BALL_TYPE_STATIC;
                    } else if(insertMode == BALL_TYPE_STATIC) {
                        insertMode = BALL_TYPE_LINK;
                    } else if(insertMode == BALL_TYPE_LINK) {
                        insertMode = BALL_TYPE_NORMAL;
                    }
                }
            } 
        }
        update(deltaTime);
        render(deltaTime);
        deltaTime = (SDL_GetTicks() - lastTick) / 1000.0;
    }

    SDL_free(renderer);
    SDL_free(window);

    printf("%s \n", SDL_GetError());
    return 0;
}
