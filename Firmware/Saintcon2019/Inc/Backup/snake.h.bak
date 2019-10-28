/*
 * snake.h
 *
 *  Created on: Oct 13, 2019
 *      Author: bashNinja
 */
#include "stdint.h"
#include "main.h"

#define BOARD_SIZE_X 35
#define BOARD_SIZE_Y 12


typedef struct Point {
	uint8_t		x;
	uint8_t		y;
} Point;

struct Player {
    uint8_t				lives;
    uint16_t			score;
    uint16_t			bodySize;
    struct Point		location;
    struct Point		direction;
};

struct Board {
	int8_t			board[BOARD_SIZE_X][BOARD_SIZE_Y];
	struct Point	bounds;
};

struct SnakeGame {
	struct Board	*current_board;
    struct Player	*player;
    uint8_t			playing;
};

void snake();
void createFood(struct Board *gameBoard);
void movePlayer(struct Player *player, struct SnakeGame *game);
void drawBoard(struct Board *gameBoard);
void drawOutsideBoardItems(uint8_t lives, uint16_t score);
int checkHitBody(struct Point position, struct Board *gameBoard);
int checkOutsideBounds(struct Point bounds, struct Point position);
void lostLife(struct Player *player);
void drawGameOver(struct Player *player);
