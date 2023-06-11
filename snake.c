#include "snake.h"

int main()
{
    srand(time(NULL));  
    player_t player;
    initPlayerInput(&player);

    snake_t snake;
    world_t world;
    Vector2_t fruit;
    world.pixels = malloc(sizeof(char) * HEIGHT * WIDTH);
    worldGen(&world, &snake, &fruit);


    gameLoop(&world, &snake, &fruit, &player);

    exitScreen(player);

    FILE *fp = fopen("score.txt", "w");
    fprintf(fp, "%s %d", player.name, player.score);
    fclose(fp);
    free(world.pixels);
    return 0;
}

//Functions
//=========
void initPlayerInput(player_t *player)
{
    fprintf(stdout, "Write your name: ");
    scanf(" %s", player->name);
    player->score = 0;
}

void worldGen(world_t *world, snake_t *snake, Vector2_t *fruit)
{
    //Random Player Position
    Vector2_t randomVec;
    randomVec.posX = 2 + rand() % WIDTH;
    randomVec.posY = 2 + rand() % HEIGHT;
    fprintf(stdout, "%d %d\n", randomVec.posX, randomVec.posY);

    //Set World
    for (__uint8_t y = 0; y < HEIGHT; y++)
    {
        for (__uint8_t x = 0; x < WIDTH; x++)
        {
            if (x == WIDTH - 1)
            {
                world->pixels[y * WIDTH + x] = '\n';
            }
            else if (x == 0 || x == WIDTH - 2 || y == 0 || y == HEIGHT - 1)
            {
                world->pixels[y * WIDTH + x] = WORLDBORDER;
            }
            else if (x == randomVec.posX && y == randomVec.posY)
            {
                world->pixels[y * WIDTH + x] = SNAKEHEAD;
                snake->posX[0] = x;
                snake->posY[0] = y;
                snake->snakeVector.posX = 1;
                snake->snakeVector.posY = 0;
                snake->index = 0;
            }
            else world->pixels[y * WIDTH + x] = ' ';
        }
        
    }
    //Gen Fruit
    randomFruitGen(world, fruit);
}

void gameLoop(world_t *world, snake_t *snake, Vector2_t *fruit, player_t *player)
{
    bool game = true;
    time_t timeStamp;
    timeStamp = time(NULL);

    draw(world, 0);

    while (game)
    {
        time_t currTimeStamp;
        currTimeStamp = time(NULL);

        if (currTimeStamp - timeStamp >= 1)
        {
            draw(world, player->score);
            update(world, snake, fruit, &game, player);
            timeStamp = currTimeStamp;
        }
    }
}
void exitScreen(player_t player)
{
    fprintf(stdout, "Final Score: %d\nPress any key to exit.\n", player.score);
}

//Utilities
void update(world_t *world, snake_t *snake, Vector2_t *fruit, bool *game, player_t *player)
{
    //Input
    snake->snakeVector = input(snake->snakeVector);

    //Collision Checking
    //==================
    //No Collision
    if (world->pixels[(snake->posY[0]-snake->snakeVector.posY) * WIDTH + (snake->posX[0] + snake->snakeVector.posX)] == ' ')
    {   
        if (snake->index == 0)
        {
            //Update World
            world->pixels[(snake->posY[0]-snake->snakeVector.posY) * WIDTH + (snake->posX[0] + snake->snakeVector.posX)] = SNAKEHEAD;
            world->pixels[snake->posY[0] * WIDTH + snake->posX[0]] = ' ';

            //Update snake head position
            snake->posX[0] = snake->posX[0] + snake->snakeVector.posX;
            snake->posY[0] = snake->posY[0] - snake->snakeVector.posY;
        }
        else if (snake->index != 0)
        {
            //Update World
            world->pixels[(snake->posY[0]-snake->snakeVector.posY) * WIDTH + (snake->posX[0] + snake->snakeVector.posX)] = SNAKEHEAD;   //Head
            world->pixels[snake->posY[snake->index] * WIDTH + snake->posX[snake->index]] = ' ';                                         //Tail                   
            //Update tail position
            for (int i = snake->index; i > 0; i--)
            {
                snake->posX[i] = snake->posX[i -1];
                snake->posY[i] = snake->posY[i -1];
            }

            world->pixels[snake->posY[0] * WIDTH + snake->posX[0]] = SNAKEBODY;

            //Update snake head position
            snake->posX[0] = snake->posX[0] + snake->snakeVector.posX;
            snake->posY[0] = snake->posY[0] - snake->snakeVector.posY;
        }
    }
    //Fruit Collision
    else if (world->pixels[(snake->posY[0]-snake->snakeVector.posY) * WIDTH + (snake->posX[0] + snake->snakeVector.posX)] == FRUIT)
    {
        if (snake->index == 0)
        {                   
            snake->index++;
            //Update World
            world->pixels[(snake->posY[0]-snake->snakeVector.posY) * WIDTH + (snake->posX[0] + snake->snakeVector.posX)] = SNAKEHEAD;
            world->pixels[snake->posY[0] * WIDTH + snake->posX[0]] = SNAKEBODY;
            //Update snake head position
            snake->posX[1] = snake->posX[0];
            snake->posY[1] = snake->posY[0];

            snake->posX[0] = snake->posX[0] + snake->snakeVector.posX;
            snake->posY[0] = snake->posY[0] - snake->snakeVector.posY;
        }
        else if (snake->index != 0)
        {
            snake->index++;
            //Update World
            world->pixels[(snake->posY[0]-snake->snakeVector.posY) * WIDTH + (snake->posX[0] + snake->snakeVector.posX)] = SNAKEHEAD;   //Head     
            world->pixels[snake->posY[0] * WIDTH + snake->posX[0]] = SNAKEBODY;                                                         //Tail            
            //Update tail position
            for (int i = snake->index; i > 0; i--)
            {
                snake->posX[i] = snake->posX[i -1];
                snake->posY[i] = snake->posY[i -1];
            }
            
            //Update snake head position
            snake->posX[0] = snake->posX[0] + snake->snakeVector.posX;
            snake->posY[0] = snake->posY[0] - snake->snakeVector.posY;
        }
        player->score++;
        randomFruitGen(world, fruit);
    }
    //End Game Collisions
    else
    {
        *game = false;
    }
}

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    Vector2_t input(Vector2_t  currentVector)
    {
        Vector2_t newVector = currentVector;
        char c = {0};

        static struct termios oldt, newt;
        //Get Parameters of the terminal
        tcgetattr( STDIN_FILENO, &oldt);
        //Copy old settings
        newt = oldt;
    
        //Set New Setings
        newt.c_lflag &= ~(ICANON | ECHO);          
        tcsetattr( STDIN_FILENO, TCSANOW, &newt);
        //Set Non bocking read
        fcntl(0, F_SETFL, O_NONBLOCK);
        //Get Input and set movemet vector by it
        read(0,&c,1);
        switch (c)
        {
        case 'w':
            newVector.posX = 0;
            newVector.posY = 1;
            break;
        case 'a':
            newVector.posX = -1;
            newVector.posY = 0;
            break;
        case 'd':
            newVector.posX = 1;
            newVector.posY = 0;
            break;
        case 's':
            newVector.posX = 0;
            newVector.posY = -1;
            break;
        default:
            break;
        }

        //restore the old settings
        tcsetattr( STDIN_FILENO, TCSANOW, &oldt);

        if (newVector.posX == currentVector.posX && newVector.posY == currentVector.posY)
        {
            return currentVector;
        }
        else return newVector;
    }
#endif
#if defined(_WIN32)
    Vector2_t input(Vector2_t  currentVector)
    {
        Vector2_t newVector = currentVector;
        char c = {0};
        getch(c);
        switch (c)
        {
        case 'w':
            newVector.posX = 0;
            newVector.posY = 1;
            break;
        case 'a':
            newVector.posX = -1;
            newVector.posY = 0;
            break;
        case 'd':
            newVector.posX = 1;
            newVector.posY = 0;
            break;
        case 's':
            newVector.posX = 0;
            newVector.posY = -1;
            break;
        default:
            break;
        }
        if (newVector.posX == currentVector.posX && newVector.posY == currentVector.posY)
        {
            return currentVector;
        }
        else return newVector;
    }
#endif 

void randomFruitGen(world_t *world, Vector2_t *fruit)
{
    bool isGenerated = false;

    while (isGenerated == false)
    {
        int randomX = 2 + rand() % WIDTH;
        int randomY = 2 + rand() % HEIGHT;

        if (world->pixels[randomY*WIDTH+randomX] == ' ')
        {
            isGenerated = true;
            fruit->posX = randomX;
            fruit->posY = randomY;
            world->pixels[randomY*WIDTH+randomX] = FRUIT;
        }
    }
}

void draw(world_t *world, __uint16_t score)
{
    clear();

	for (__uint8_t y = 0; y < HEIGHT; y++)
	{
		for (__uint8_t x = 0; x < WIDTH; x++)
		{
			fprintf(stdout,"%c", world->pixels[y*WIDTH+x]);
		}   
	}
    fprintf(stdout, "Score: %d\n", score);
}