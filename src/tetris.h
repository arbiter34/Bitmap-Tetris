#ifndef _TETRIS_H_
#define _TETRIS_H_

#define SCREEN_WIDTH 320        //window width
#define SCREEN_HEIGHT 704       //window height
#define INTERVAL 1000           //interval for falling
#define NUM_SHAPES 7            //Number of shapes used, for dynamic generation
#define DEBUG 1
#define SPEED_UP_INTERVAL 30000 //interval for the speed to increase in MS

typedef struct shape {          //shape struct, first four char's are the shape bitmap, fifth is color
    char structure[5];
} shape;

/* shape to hold a tetrimino, this includes it's x,y coords
 * shape sh - current shape bitmapped(this inherently means it's rotation)
 * written - bool for whether the tet is written to the screen matrix
 * color - this was in place before each shape had only one color
 */
typedef struct tetrimino {      
    int x;                      
    int y;
    shape sh;
    int written;
    int color;
} tetrimino;

// enum for easy direction checking
enum Direction {
    left,
    right,
    down
};

//clocks per ms factor
const float CLOCKS_PER_MSEC = 1000.0F/(float)CLOCKS_PER_SEC;

//least significant bit is at the bottom
const shape shapes[] = {  
    {{0, 3, 3, 0, 0}}, //square //yellow
    {{0, 15, 0, 0, 1}}, //line //light blue
    {{0, 7, 1, 0, 2}}, //l //orange
    {{0, 1, 7, 0, 3}}, //j //blue
    {{0, 2, 3, 1, 4}}, //z //red
    {{0, 1, 3, 2, 5}}, //s //green
    {{0, 8, 12, 8, 6}} //short t //purple
};

//atexit() cleanup function
void __exit();

//init functions
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