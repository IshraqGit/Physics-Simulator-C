#include <SDL2/SDL.h>
#include <stdlib.h>

#define BALL_TYPE_NORMAL 0
#define BALL_TYPE_STATIC 1
#define BALL_TYPE_LINK 2

typedef struct Ball {
    float x;
    float y;
    int radius;
    float oldX;
    float oldY;
    float aX;
    float aY;
    int colR, colG, colB;
    float initX, initY;
    int type;
} Ball;

Ball* BallCreate(int x, int y, int radius, float aX, float aY, int type) {
    Ball* newBall = (Ball*)malloc(sizeof (struct Ball));
    newBall->x = x;
    newBall->y = y;
    newBall->oldX = x;
    newBall->oldY = y;
    newBall->aX = aX;
    newBall->aY = aY;
    newBall->colR = rand() % 256;
    newBall->colG = rand() % 256;
    newBall->colB = rand() % 256;
    newBall->type = type;
    newBall->initX = x;
    newBall->initY = y;

    newBall->radius = radius;
    return newBall;
}

void BallUpdate(Ball* ball, double deltaTime) {
    float vX = ball->x - ball->oldX;
    float vY = ball->y - ball->oldY;

    ball->oldX = ball->x;
    ball->oldY = ball->y;

    ball->x += vX + ball->aX * deltaTime*deltaTime;
    ball->y += vY + ball->aY * deltaTime*deltaTime;
}

void BallRenderDraw(Ball* ball, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, ball->colR, ball->colG, ball->colB, 255);
    RenderFillCircle(renderer, ball->x, ball->y, ball->radius);
}

typedef struct Link {
    Ball* ball1;
    Ball* ball2;
    float target_dist;
} Link;

void applyLink(Link* link) {
    static int t = 10;
    float axisX = link->ball1->x - link->ball2->x;
    float axisY = link->ball1->y - link->ball2->y;
    float dist = EuclideanDistance(link->ball1->x, link->ball1->y,
            link->ball2->x, link->ball2->y);
    float nX = axisX / dist;
    float nY = axisY / dist;
    //printf("%f %f\n", nX, nY);
    //scanf("%d", &t);
    float delta = link->target_dist - dist;
    link->ball1->x += 0.1 * delta * nX;
    link->ball1->y += 0.1 * delta * nY;
    link->ball2->x -= 0.1 * delta * nX;
    link->ball2->y -= 0.1 * delta * nY;
}