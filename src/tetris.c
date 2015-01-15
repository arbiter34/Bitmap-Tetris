#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "tetris.h"

char ***game_matrix;
int matrix_width;
int matrix_height;

SDL_Window *win;
SDL_Renderer *renderer;
SDL_Texture *background;
SDL_Texture *red, *blue, *green, *light_blue, *orange, *yellow, *purple;
SDL_Texture **blocks;

void initTetris() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
        printf("ERROR\n");
        exit(EXIT_FAILURE);
    }

    win = SDL_CreateWindow("Tetris", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (win == NULL){
        printf("Error creating window!\n");
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    
    renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL){
        SDL_DestroyWindow(win);
        printf("Create renderer error\n");
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    background = loadTextureFromImage("background.bmp", renderer);

    yellow = loadTextureFromImage("tetris_block_yellow_32.bmp", renderer);
    light_blue = loadTextureFromImage("tetris_block_light_blue_32.bmp", renderer);
    orange = loadTextureFromImage("tetris_block_orange_32.bmp", renderer);
    blue = loadTextureFromImage("tetris_block_blue_32.bmp", renderer);
    red = loadTextureFromImage("tetris_block_red_32.bmp", renderer);
    green = loadTextureFromImage("tetris_block_green_32.bmp", renderer);
    purple = loadTextureFromImage("tetris_block_purple_32.bmp", renderer);

    blocks = (SDL_Texture **)malloc(sizeof(SDL_Texture*) * NUM_SHAPES);
    blocks[0] = yellow;
    blocks[1] = light_blue;
    blocks[2] = orange;
    blocks[3] = blue;
    blocks[4] = red;
    blocks[5] = green;
    blocks[6] = purple;

    if (background == NULL || red == NULL || blue == NULL || green == NULL){
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

void refreshGameScreen() {
    SDL_RenderClear(renderer);
    SDL_PollEvent(NULL);
    //printGameMatrix();
    renderBackground(renderer, background);
    renderGameMatrix(renderer, blocks);
    SDL_RenderPresent(renderer);
}

void clearTetrimino(tetrimino *t) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (8>>y & t->sh.structure[x]) {
                for (int i = 0; i < 4; i++) {                    
                    game_matrix[y+t->y][x+t->x][i] = 0;
                }
                t->written = 0;
            }
        }
    }
}


void drawTetrimino(tetrimino *t) {
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            if (8>>y & t->sh.structure[x]) {
                game_matrix[y+t->y][x+t->x][0] = 1;
                game_matrix[y+t->y][x+t->x][1] = t->color;
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
            printf("%i ", game_matrix[y][x][0]);
        }
        printf("\n");
    }
    printf("\n");
}

void initGameMatrix() {
    game_matrix = (char***)malloc(sizeof(char**)*matrix_height);
    for (int y = 0; y <= matrix_height; y++) {
        game_matrix[y] = (char**)malloc(sizeof(char*)*(matrix_width));
        for (int x = 0; x < matrix_width; x++) {
            game_matrix[y][x] = (char*)malloc(sizeof(char)*4);
            if (y == matrix_height) {
                game_matrix[y][x][0] = 1;
            } else {
                game_matrix[y][x][0] = 0;
            }
            for (int i = 1; i < 4; i++) {
                game_matrix[y][x][i] = 0;
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

            if ((x+t->x < 0 || x+t->x >= matrix_width)) {                
                continue;
            }

            if ((8>>y & t->sh.structure[x]) && (game_matrix[y+t->y+1][x+t->x][0] || game_matrix[y+t->y][x+t->x][0])) {
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
    int x_adj = (direction ? 1 : -1);

    int x_edge = (direction ? matrix_width : -1);
    int x_adj_index;
    for (int y = 3; y >= 0; y--) {
        for ( int x = 0; x < 4; x++) {

            if ((x+t->x < 0 || x+t->x > matrix_width)) {
                if (8>>y & t->sh.structure[x]) {
                    printf("pre=pass\n");
                    return 1;
                }
                continue;
            }

            x_adj_index = (x+t->x+x_adj < 0 || x+t->x+x_adj >= matrix_width) ? 0 : x_adj;
            
            if ((8>>y & t->sh.structure[x]) && (game_matrix[y+t->y][x+t->x+x_adj_index][0] 
                                                || ((x+t->x+x_adj) == x_edge))) {
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
        if (game_matrix[1][x][0]) {
            printf("x: %i  y: %i\n", x, 1);
            return 1;
        }
    }
    if (cleared) {
        drawTetrimino(t);
    }
    return 0;
}

void deleteRow(int row) {
    for (int y = row; y >= 0; y--) {
        for (int x = 0; x < matrix_width; x++) {
            if (y == 0) {
                for (int i = 0; i < 4; i++) {
                    game_matrix[y][x][i] = 0;
                }
            } else {
                for (int i = 0; i < 4; i++) {
                    game_matrix[y][x][i] = game_matrix[y-1][x][i];
                }
            }
        }
    }
}

void blinkRows(int *rows, size_t size) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < size; j++) {
            for (int x = 0; x < matrix_width; x++) {
                game_matrix[rows[j]][x][0] = !game_matrix[rows[j]][x][0];
            }
        }
        refreshGameScreen();
        SDL_Delay(100);
    }
}

void checkRowsForCompletion() {
    int *rows = (int*)malloc(sizeof(int)*1);
    int count = 0;
    for (int y = 0; y < matrix_height; y++) {
        if (game_matrix[y][0][0]) {
            for (int x = 0; x < matrix_width; x++) {
                if (!game_matrix[y][x][0]) {
                    break;
                }
                if (game_matrix[y][x][0] && x == (matrix_width-1)) {
                    count++;
                    rows = (int*)realloc(rows, sizeof(int)*(count+1));
                    rows[count-1] = y;
                }
            }
        }
    }
    if (count > 0) {
        blinkRows(rows, count);
        for (int i = 0; i < count; i++) {
            deleteRow(rows[i]);
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
    if (direction == down && checkCollisionDown(t)) {
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
    printf("checking down\n");
    while (checkCollisionDown(t)) {
        printf("count: %i\n", ++stuff);
        t->y--;
    }
    printf("checking left\n");
    int collided = 0;
    while (checkCollisionLeftRight(t, left)) {
        printf("left\n");
        collided = 1;
        t->x++;
    }
    printf("checking right\n");
    while (checkCollisionLeftRight(t, right)) {
        printf("right\n");
        collided = 1;
        t->x--;
    }
    if (collided) {
        t->x += (direction == left ? -1 : 1);
    }
    printf("passed checks\n");
    drawTetrimino(t);

}

void advanceTetrimino(tetrimino *t, int direction) {
    if (direction == down && checkCollisionDown(t)) {
        return;
    } else if ((direction == right || direction == left) && checkCollisionLeftRight(t, direction)) {
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


void renderGameMatrix(SDL_Renderer *renderer, SDL_Texture **texture) {
    int img_width, img_height;

    SDL_QueryTexture(texture[0], NULL, NULL, &img_width, &img_height);
    int color_index = 0;
    for (int y = 0; y < matrix_height-2; y++) {
        for (int x = 0; x < matrix_width; x++) {
            if (game_matrix[y+2][x][0]) {
                color_index = game_matrix[y+2][x][1];
                renderTexture(texture[color_index], renderer, x*img_width, y*img_height);
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
    printf("color_index: %i\n", tet->sh.structure[4]);
    tet->color = tet->sh.structure[4];

    return tet;
}

int main(int argc, char** argv){

    atexit(__exit);

    SDL_Event event;

    time_t t;
    /* Intializes random number generator */
    srand((unsigned) time(&t));

    initTetris();

    int img_width, img_height, speed_up_divisor = 1;

    SDL_QueryTexture(blocks[0], NULL, NULL, &img_width, &img_height);

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
                                advanceTetrimino(tet, left);                                
                                break;

                            case SDLK_RIGHT:
                                advanceTetrimino(tet, right);
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
                            case SDLK_q:
                                __exit(EXIT_SUCCESS);
                                break;
                            default:
                                break;
                        }
                        //printGameMatrix();
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

    __exit(EXIT_SUCCESS);
    return 0;
}

void __exit(void) {
    SDL_DestroyTexture(background);
    for ( int i = 0; i < NUM_SHAPES; i++) {
        SDL_DestroyTexture(blocks[i]);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();

    printf("SUCCESS!\n");
    return;
}