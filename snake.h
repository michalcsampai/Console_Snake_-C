#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    #include <unistd.h> 
    #include <fcntl.h>
    #include <termios.h>    
    #define clear() system("clear");
#endif
#if defined(_WIN32)
    #include <conio.h> 
    #define clear() system("cls");
#endif


#define HEIGHT 15
#define WIDTH  25

#define NAMELENGT 21

#define WORLDBORDER  '#'
#define SNAKEHEAD    '@'
#define SNAKEBODY    '='
#define FRUIT        'O'


struct Vector2
{
    int posX;
    int posY;

}typedef Vector2_t;

struct player
{
    char name[NAMELENGT];
    __uint16_t score;

}typedef player_t;

struct world
{
    char *pixels;

}typedef world_t;

struct snake
{
    __uint16_t posX[WIDTH*HEIGHT];
    __uint16_t posY[WIDTH*HEIGHT];

    __uint16_t index;             // 0 = head, indexing from 1

    Vector2_t snakeVector;

}typedef snake_t;

//Inicialization
//=======================

void initPlayerInput(player_t *player);
void worldGen(world_t *world, snake_t *snake, Vector2_t *fruit);
void gameLoop(world_t *world, snake_t *snake, Vector2_t *fruit, player_t *player);

void randomFruitGen(world_t *world, Vector2_t *fruit);
void exitScreen(player_t player);
void update(world_t *world, snake_t *snake, Vector2_t *fruit, bool *game, player_t *player);
void draw(world_t * world, __uint16_t score);

Vector2_t input(Vector2_t  currentVector);