CC=gcc

CFLAGS= -std=gnu99 -Wall  -pedantic -lrt -lpthread -g 
 

snake: snake.c
	$(CC) snake.c $(CFLAGS) -o snake 
	
