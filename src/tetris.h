#ifndef _TETRIS_H_
#define _TETRIS_H_

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 704
#define INTERVAL 1000
#define NUM_SHAPES 7
#define DEBUG 1

typedef struct shape {
    char structure[5];
} shape;

typedef struct tetrimino { 
    int x;
    int y;
    shape sh;
    int written;
    int color;
} tetrimino;

enum Direction {
    left,
    right,
    down
};

extern const float CLOCKS_PER_MSEC = 1000.0F/(float)CLOCKS_PER_SEC;

//least significant bit is at the bottom
extern const shape shapes[] = {  
    {{0, 3, 3, 0, 0}}, //square //yellow
    {{0, 15, 0, 0, 1}}, //line //light blue
    {{0, 7, 1, 0, 2}}, //l //orange
    {{0, 1, 7, 0, 3}}, //j //blue
    {{0, 2, 3, 1, 4}}, //z //red
    {{0, 1, 3, 2, 5}}, //s //green
    {{0, 8, 12, 8, 6}} //short t //purple
};

void __exit();

void initTetris();

void clearTetrimino(tetrimino *t);

void drawTetrimino(tetrimino *t);

void printTetrimino(tetrimino *t);

void printGameMatrix();

void initGameMatrix();

int checkCollisionDown(tetrimino *t);

int checkCollisionLeftRight(tetrimino *t, int direction);

int checkTetriminoCollision(tetrimino *t);

int checkGameOver(tetrimino *t);

void deleteRow(int row);

void blinkRows(int *rows, size_t size);

void checkRowsForCompletion();

void printMatrix(char matrix[4][4]);

void rotateTetrimino(tetrimino *t, int direction);

void advanceTetrimino(tetrimino *t, int direction);

void logSDLError(const char *str);

SDL_Texture* loadTextureFromImage(const char *file, SDL_Renderer *renderer);

void renderTexture(SDL_Texture *tex, SDL_Renderer *renderer, int x, int y);

void renderGameMatrix(SDL_Renderer *renderer, SDL_Texture **texture);

void renderBackground(SDL_Renderer *renderer, SDL_Texture *background);

tetrimino *createTetrimino();

void refreshGameScreen();

#endif