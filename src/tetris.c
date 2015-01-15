#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "tetris.h"

char **game_matrix;
int matrix_width;
int matrix_height;

SDL_Window *win;
SDL_Renderer *renderer;
SDL_Texture *background;
SDL_Texture *image;

void initTetris() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("ERROR\n");
        return 1;
    }

    win = SDL_CreateWindow("Tetris", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (win == NULL){
        printf("Error creating window!\n");
        SDL_Quit();
        return 1;
    }
    
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL){
        SDL_DestroyWindow(win);
        printf("Create renderer error\n");
        SDL_Quit();
        return 1;
    }

    background = loadTextureFromImage("background.bmp", renderer);
    image = loadTextureFromImage("tetris_block_green.bmp", renderer);
    if (background == NULL || image == NULL){
        SDL_Quit();
        return 1;
    }
}

void refreshGameScreen() {
    SDL_RenderClear(renderer);
    SDL_PollEvent(NULL);
    printGameMatrix();
    renderBackground(renderer, background);
    renderGameMatrix(renderer, image);
    SDL_RenderPresent(renderer);
}

void clearTetrimino(tetrimino *t) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (8>>y & t->sh.structure[x]) {
                game_matrix[y+t->y][x+t->x] = 0;
                t->written = 0;
            }
        }
    }
}


void drawTetrimino(tetrimino *t) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (8>>y & t->sh.structure[x]) {
                game_matrix[y+t->y][x+t->x] = 1;
                t->written = 1;
            }
        }
    }
}

void printTetrimino(tetrimino *t) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (8>>y & t->sh.structure[x]) {
                printf("1 ");
            } else {
                printf("0 ");
            }
        }
        printf("\n");
    }
    printf("\n");
}

void printGameMatrix() {
    for (int y = 0; y <= matrix_height; y++) {
        for (int x = 0; x < matrix_width; x++) {
            printf("%i ", game_matrix[y][x]);
        }
        printf("\n");
    }
    printf("\n");
}

void initGameMatrix() {
    game_matrix = (char**)malloc(sizeof(char*)*matrix_height);
    for (int y = 0; y <= matrix_height; y++) {
        game_matrix[y] = (char*)malloc(sizeof(char)*(matrix_width));
        for (int x = 0; x < matrix_width; x++) {
            if (y == matrix_height) {
                game_matrix[y][x] = 1;
            } else {
                game_matrix[y][x] = 0;
            }
        }
    }
    printGameMatrix();
}


int checkCollisionDown(tetrimino *t) {
    if (t == NULL) {
        return 0;
    }
    int cleared = 0;
    if (t->written) {
        clearTetrimino(t);
        cleared = 1;
    }
    for (int y = 3; y >= 0; y--) {
        for (int x = 0; x < 4; x++) {
            if ((8>>y & t->sh.structure[x]) && (game_matrix[y+t->y+1][x+t->x] || game_matrix[y+t->y][x+t->x])) {
                if (cleared) {
                    drawTetrimino(t);
                }
                return 1;
            }
        }
    }
    if (cleared) {
        drawTetrimino(t);
    }
    return 0;
}

int checkCollisionLeftRight(tetrimino *t, int direction) {
    if (t == NULL) {
        return 0;
    }
    int cleared = 0;
    if (t->written) {
        clearTetrimino(t);
        cleared = 1;
    }
    for (int y = 3; y >= 0; y--) {
        for (int x = 0; x < 4; x++) {
            if ((8>>y & t->sh.structure[x]) && (game_matrix[y+t->y][x+t->x-(direction ? -1 : 1)])) {
                if (cleared) {
                    drawTetrimino(t);
                }
                return 1;
            }
        }
    }
    if (cleared) {
        drawTetrimino(t);
    }
    return 0;
}

int checkTetriminoCollision(tetrimino *t) {
    if (t->y + 3 == matrix_height) {
        return 1;
    }
    return 0;
}

int checkGameOver(tetrimino *t) {
    if (t == NULL) {
        return 0;
    }
    int cleared = 0;
    if (t->written) {
        clearTetrimino(t);
        cleared = 1;
    }
    for (int x = 0; x < matrix_width; x++) {
        if (game_matrix[1][x]) {
            printf("x: %i  y: %i\n", x, 1);
            return 1;
        }
    }
    if (cleared) {
        drawTetrimino(t);
    }
    return 0;
}

void deleteRow(int r) {
    for (int y = r; y >= 0; y--) {
        for (int x = 0; x < matrix_width; x++) {
            if (y == 0) {
                game_matrix[y][x] = 0;
            } else {
                game_matrix[y][x] = game_matrix[y-1][x];
            }
        }
    }
}

void blinkRow(int r) {
    for (int i = 0; i < 9; i++) {
        for (int x = 0; x < matrix_width; x++) {
            game_matrix[r][x] = !game_matrix[r][x];
        }
        refreshGameScreen();
        SDL_Delay(200);
    }
}

void checkRowsForCompletion() {
    for (int y = 0; y < matrix_height; y++) {
        if (game_matrix[y][0]) {
            for (int x = 0; x < matrix_width; x++) {
                if (!game_matrix[y][x]) {
                    break;
                }
                if (game_matrix[y][x] && x == (matrix_width-1)) {
                    blinkRow(y);
                    deleteRow(y);
                }
            }
        }
    }
}

void printMatrix(char matrix[4][4]) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            printf("%i ", matrix[y][x]);
        }
        printf("\n");
    }
    printf("\n\n");
}

void rotateTetrimino(tetrimino *t, int direction) {
    if (checkCollisionDown(t)) {
        return;
    }
    tetrimino *temp = (tetrimino *)malloc(sizeof(tetrimino)*1);
    *temp = *t;
    //temp 4x4 for transpose
    char temp_char[4][4];
    
    //temp 4x4 for swap
    char temp_char2[4][4];
    

    //transpose 4x4 array
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            temp_char[x][y] = (t->sh.structure[x] & 8>>y) == 0 ? 0 : 1;
        }
    }
    printMatrix(temp_char);
    //transpose only rows or columns for final rotate step
    for (int y = 0;y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (direction) {    //rotate right on 1
                temp_char2[y][3-x] = temp_char[y][x];
            } else {            //rotate left on 0
                temp_char2[3-y][x] = temp_char[y][x];              
            }
        }
    }
    printMatrix(temp_char2);
    //clear old tetrimino from the board
    clearTetrimino(t);


    //rebuild tetrimino(bitmap)
    int val = 0;
    for (int x = 0; x < 4; x++) {
        val = 0;
        for (int y = 0; y < 4; y++) {
            if (temp_char2[y][x]) {
                val += 8>>y;
            }
        }
        printf("Val: %i\n", val);
        t->sh.structure[x] = val;
    }
    printf("pre check bottom\n");
    int stuff = 0;
    while (checkCollisionDown(t)) {
        printf("count: %i\n", ++stuff);
        t->y--;
    }
    while(checkTetriminoCollision(t)) {
        t->y--;
    }
    drawTetrimino(t);

}

void advanceTetrimino(tetrimino *t, int direction) {
    if (checkCollisionDown(t)) {
        return;
    }
    clearTetrimino(t);
    if (direction == right) {
        t->x++;
    } else if (direction == down) {
        t->y++;
    } else {        //0 or others - move left
        t->x--;
    }
    drawTetrimino(t);
}

void logSDLError(const char *str) {
    printf("%s", str);
}

/**
* Loads a BMP image into a texture on the rendering device
* @param file The BMP image file to load
* @param rendererThe renderer to load the texture onto
* @return the loaded texture, or nullptr if something went wrong.
*/
SDL_Texture* loadTextureFromImage(const char *file, SDL_Renderer *renderer) {
    //Initialize to nullptr to avoid dangling pointer issues
    SDL_Texture *texture = NULL;
    //Load the image
    SDL_Surface *loadedImage = SDL_LoadBMP(file);
    //If the loading went ok, convert to texture and return the texture
    if (loadedImage != NULL){
        texture = SDL_CreateTextureFromSurface(renderer, loadedImage);
        SDL_FreeSurface(loadedImage);
        //Make sure converting went ok too
        if (texture == NULL){
            logSDLError("CreateTextureFromSurface");
        }
    }
    else {
        logSDLError("LoadBMP");
    }
    return texture;
}

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
* the texture's width and height
* @param tex The source texture we want to draw
* @param renderer The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *renderer, int x, int y) {
    //Setup the destination rectangle to be at the position we want
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    //Query the texture to get its width and height to use
    SDL_QueryTexture(tex, NULL, NULL, &dst.w, &dst.h);
    SDL_RenderCopy(renderer, tex, NULL, &dst);
}


void renderGameMatrix(SDL_Renderer *renderer, SDL_Texture *texture) {
    int img_width, img_height;

    SDL_QueryTexture(texture, NULL, NULL, &img_width, &img_height);

    for (int y = 0; y < matrix_height-2; y++) {
        for (int x = 0; x < matrix_width; x++) {
            if (game_matrix[y+2][x]) {
                renderTexture(texture, renderer, x*img_width, y*img_height);
            }
        }
    }
}

void renderBackground(SDL_Renderer *renderer, SDL_Texture *background) {
    int bg_width, bg_height;
    SDL_QueryTexture(background, NULL, NULL, &bg_width, &bg_height);
    for (int i = 0; i < (SCREEN_HEIGHT/bg_height)+1; i++) {
        for (int y = 0; y < (SCREEN_WIDTH/bg_width)+1; y++) {
            renderTexture(background, renderer, bg_width*y, bg_height*i);
        }
    }
}

tetrimino *createTetrimino() {
    //pick random shape from 
    int random_number = rand()%NUM_SHAPES;
    printf("%i\n", random_number);
    tetrimino *tet = (tetrimino *)malloc(sizeof(tetrimino)*1);
    tet->sh = shapes[random_number];

    //pick random X for drop spot
    tet->x = rand()%(matrix_width-3);
    tet->y = 0;
    tet->written = 0;

    return tet;
}

int main(int argc, char** argv){

    SDL_Event event;

    time_t t;
    /* Intializes random number generator */
    srand((unsigned) time(&t));

    initTetris();

    int img_width, img_height, speed_up_divisor = 1;

    SDL_QueryTexture(image, NULL, NULL, &img_width, &img_height);

    matrix_width = ((int)SCREEN_WIDTH/img_width);
    matrix_height = ((int)SCREEN_HEIGHT/img_height)+2;

    printf("width: %i  height: %i\n", matrix_width, matrix_height);

    initGameMatrix();

    tetrimino *tet = NULL;

    clock_t start = clock(), diff, last;
    last = start;

    printf("cps: %i  start: %d  last: %d  interval: %i\n", CLOCKS_PER_SEC, start, last, INTERVAL/speed_up_divisor);

    while (!checkGameOver(tet)) {
        while (SDL_PollEvent( &event )) {
            switch (event.type) {
                /* Keyboard event */
                /* Pass the event data onto PrintKeyInfo() */
                case SDL_KEYUP:
                    if (1) {
                        switch (event.key.keysym.sym) {
                            case SDLK_LEFT:
                                if (!checkCollisionLeftRight(tet, left)) {
                                    advanceTetrimino(tet, left);
                                }
                                break;

                            case SDLK_RIGHT:
                                if (!checkCollisionLeftRight(tet, right)) {
                                    advanceTetrimino(tet, right);
                                }
                                break;

                            case SDLK_DOWN:
                                advanceTetrimino(tet, down);
                                break;
                            case SDLK_r:
                                rotateTetrimino(tet, right);
                                break;
                            case SDLK_e:
                                rotateTetrimino(tet, left);
                                break;
                            default:
                                break;
                        }
                        printGameMatrix();
                        refreshGameScreen();
                    }
                    break;

                /* SDL_QUIT event (window close) */
                case SDL_QUIT:
                    return 0;
                    break;

                default:
                    break;
            }
        }
        if (checkCollisionDown(tet) || tet == NULL) {
            checkRowsForCompletion();
            if (tet != NULL) {
                free(tet);
            }

            tet = malloc(sizeof(tetrimino)*1);


            //generate new tetrimino
            tet = createTetrimino();

            //draw tetrimino to game matrix
            drawTetrimino(tet);


            refreshGameScreen();  
        }


        if (((float)(clock()-last))*CLOCKS_PER_MSEC > INTERVAL/speed_up_divisor) {

            last = clock();
            advanceTetrimino(tet, down);
            refreshGameScreen();
        }
    }

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(image);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    SDL_Quit();
    printf("SUCCESS!\n");
    return 0;
}