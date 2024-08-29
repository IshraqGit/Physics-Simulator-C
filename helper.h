float EuclideanDistance(float x1, float y1, float x2, float y2) {
    return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

void RenderFillCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    int startX = centerX - radius;
    int startY = centerY - radius;
    int endX = centerX + radius;
    int endY = centerY + radius;

    for(int x=startX; x<=endX; x++) {
        for(int y=startY; y<=endY; y++) {
            int dist = EuclideanDistance(centerX, centerY, x, y);
            if(dist <= radius) {
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }
    }
}

int area(int x1, int y1, int x2, int y2, int x3, int y3) {
    return abs((x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)));
}

bool isPointInTriangle(int c1, int c2, int x1, int y1, int x2, int y2, int x3, int y3){
    int s1 = area(c1, c2, x1, y1, x2, y2);
    int s2 = area(c1, c2, x2, y2, x3, y3);
    int s3 = area(c1, c2, x3, y3, x1, y1);
    int tot = area(x1, y1, x2, y2, x3, y3);
    bool cond = (abs(tot - (s1 + s2 + s3)) < 5);
    printf("%d %d %d %d\n", s1, s2, s3, tot);
    printf("Inside: %d\n", cond);
    return cond;
}

int dis(int x1, int y1, int x2, int y2){
    return ((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

int distanceFromPointToLine(int x1, int y1, int x2, int y2, int x3, int y3){
    int midx = (x2 + x3) / 2;
    int midy = (y2 + y3) / 2;

    return dis(x1, y1, midx, midy);
}

bool isCircleInTriangle(int c1, int c2, int radius, int x1, int y1, int x2, int y2, int x3, int y3){
    bool ret = isPointInTriangle(c1, c2, x1, y1, x2, y2, x3, y3);
    ret &= ((distanceFromPointToLine(c1, c2, x1, y1, x2, y2) - radius*radius) >= 0);
    ret &= ((distanceFromPointToLine(c1, c2, x2, y2, x3, y3) - radius*radius) >= 0);
    ret &= ((distanceFromPointToLine(c1, c2, x3, y3, x1, y1) - radius*radius) >= 0);
    //printf("Inside isCir...: %d %d\n", )
    return ret;
}

typedef struct point
{
    int x, y;
} point;

point findPointInTriangle(int c1, int c2, int radius, int x1, int y1, int x2, int y2, int x3, int y3){
    if(isCircleInTriangle(c1, c2, radius, x1, y1, x2, y2, x3, y3)) {
        point p;
        p.x = c1;
        p.y = c2;
        return p;
    }
    static int t = 5;
    point ret = {0,0};
    int lx = (x1 + x2 + x3) / 3, ly = (y1 + y2 + y3) / 3;
    int rx = c1, ry = c2;

    while(dis(lx, ly, rx, ry) > 1){
        int midx = (lx + rx) / 2, midy = (ly + ry) / 2;
        if(isCircleInTriangle(midx, midy, radius, x1, y1, x2, y2, x3, y3)){
            printf("i am here\n");
            ret.x = midx, ret.y = midy;
            lx = midx, ly = midy;
        }else{
            rx = midx, ry = midy;
        }
    }

    if(t--) {
        printf("%d %d, %d %d, %d %d\n", x1, y1, x2, y2, x3, y3);
        printf("%d, %d\n", ret.x, ret.y);
        //scanf("%d", &t);
    }
    return ret;
}