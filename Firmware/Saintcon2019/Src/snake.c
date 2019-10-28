/*
 * snake.c
 *
 *  Created on: Oct 13, 2019
 *      Author: bashNinja
 */

#include "keyboard.h"
#include "lampboard.h"
#include "matrix.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "minibadge.h"
#include "snake.h"
#include "snake_images.h"

extern volatile uint8_t nextFrame;

void snake(){
	lampboard_clear();
	lampboard_setLetter('W', COLOR_RED);
	lampboard_setLetter('A', COLOR_RED);
	lampboard_setLetter('S', COLOR_RED);
	lampboard_setLetter('D', COLOR_RED);
	lampboard_setLetter('K', COLOR_RED);
	lampboard_update();

	uint8_t loop = 1;

	struct Player player1 = {
			3,
			0,
			1,
			(Point){5, 7},
			(Point){1, 0}
	};

	struct Board current_board = {{}, (Point){BOARD_SIZE_X, BOARD_SIZE_Y}};
	struct SnakeGame snakeGame = { &current_board, &player1, 1};
	uint32_t seed;
	seed = HAL_GetTick(); //Time since boot
	srand(seed);
	createFood(snakeGame.current_board);

	int snakeFrameDelay = -1;

	while (loop){
		  if(nextFrame == 1){ //Set by Timer16 - 30FPS
			  nextFrame = 0;
			  UpdateMinibadges(1);

			  //Button Test Code
			  uint32_t buttons = 0;
			  buttons = keyboardUpdate();
			  if(buttons>>22 & 1){ //W
				  if(snakeGame.player->direction.x != 0 && snakeGame.player->direction.y != -1)
					  snakeGame.player->direction = (Point){0,1};
			  }
			  if(buttons>>0 & 1){ //A
				  if(snakeGame.player->direction.x != 1 && snakeGame.player->direction.y != 0)
					  snakeGame.player->direction = (Point){-1,0};
			  }
			  if(buttons>>18 & 1){ //S
				  if(snakeGame.player->direction.x != 0 && snakeGame.player->direction.y != 1)
					  snakeGame.player->direction = (Point){0,-1};
			  }
			  if(buttons>>3 & 1){//D
				  if(snakeGame.player->direction.x != -1 && snakeGame.player->direction.y != 0)
					  snakeGame.player->direction = (Point){1,0};
			  }
			  if(buttons>>10 & 1) { //K
				  loop = 0;
			  }



			  if(snakeFrameDelay < 0){
				  snakeFrameDelay = 5;

				  //update actions
				  if(snakeGame.playing){
					  movePlayer(snakeGame.player, &snakeGame);

				  }

				  //update screen
				  matrix_clear();
				  drawOutsideBoardItems(snakeGame.player->lives, snakeGame.player->score);
				  drawBoard(snakeGame.current_board);
				  if(!snakeGame.playing)
					  drawGameOver(snakeGame.player);

				  matrix_update();

			  } else {
				  snakeFrameDelay--;
			  }

		  }
	}
}

void createFood(struct Board *gameBoard){
	uint8_t x = rand() % BOARD_SIZE_X;
	uint8_t y = rand() % BOARD_SIZE_Y;
	while(gameBoard->board[x][y] != 0){
		x = rand() % BOARD_SIZE_X;
		y = rand() % BOARD_SIZE_Y;
	}
	gameBoard->board[x][y] = -1;
}

void movePlayer(struct Player *player, struct SnakeGame *game){
	player->location.x += player->direction.x;
	player->location.y += player->direction.y;

	// check eat food
	if(game->current_board->board[player->location.x][player->location.y] < 0){
		// if food, eat and add size.
		createFood(game->current_board);
		player->score++;
		player->bodySize++;
	}else if(checkHitBody(game->player->location, game->current_board) || checkOutsideBounds(game->current_board->bounds, game->player->location)){
		// if no food, check if hit body or wall
		  lostLife(game->player);
		  if(game->player->lives < 1)
			  game->playing = 0;

		  // re-setup board
		  memset(game->current_board->board, 0, sizeof(game->current_board->board[0][0]) * BOARD_SIZE_X * BOARD_SIZE_Y); // empty board
		  createFood(game->current_board);
		  game->player->location = (Point){5, 7};
		  game->player->direction = (Point){1, 0};
		  game->player->bodySize = 1;
		  game->current_board->board[5][6];

	} else {
		// if nothing else, move
		for(int x = 0; x < BOARD_SIZE_X; x++){
			for(int y = 0; y < BOARD_SIZE_Y; y++){
				if(game->current_board->board[x][y] > 0)
					game->current_board->board[x][y]--;
			}
		}
	}
	if(checkOutsideBounds(game->current_board->bounds, game->player->location)){
		player->location.x -= player->direction.x;
		player->location.y -= player->direction.y;
	}
	game->current_board->board[player->location.x][player->location.y] = player->bodySize;
}

void drawBoard(struct Board *gameBoard){
	for(int x = 0; x < BOARD_SIZE_X; x++){
		for(int y = 0; y < BOARD_SIZE_Y; y++){
			if(gameBoard->board[x][y] < 0) // draw Food
				matrix_drawPixel(x + 2, 15 - y - 2, COLOR_RED);
			if(gameBoard->board[x][y] > 0) // draw player body
				matrix_drawPixel(x + 2, 15 - y - 2, COLOR_WHITE);

		}
	}
}

void drawOutsideBoardItems(uint8_t lives, uint16_t score){
	matrix_drawHorizontalLine(1, 1, 37, COLOR_YELLOW);
	matrix_drawHorizontalLine(1, 14, 37, COLOR_YELLOW);
	matrix_drawVerticalLine(1, 2, 13, COLOR_YELLOW);
	matrix_drawVerticalLine(37, 2, 13, COLOR_YELLOW);

	char s[4];
	sprintf(s,"%d", score);
	matrix_drawChar(s, 43, 0, COLOR_GREEN);

	for(int i = 0; i < lives; i++){
		matrix_drawImage(image_data_snake, 39 + (i * 10), 8, 6, 7);
	}
}

int checkHitBody(struct Point position, struct Board *gameBoard){
	if(gameBoard->board[position.x][position.y] > 0)
		return 1;
	else
		return 0;
}

int checkOutsideBounds(struct Point bounds, struct Point position){
	if(position.x >= bounds.x || position.x < 0 || position.y >= bounds.y || position.y < 0)
		return 1;
	else
		return 0;
}

void lostLife(struct Player *player){
	  player->lives--;
	  player->location = (Point) { 5,  7 };
	  player->direction = (Point) { 1,  0 };
}

void drawGameOver(struct Player *player){
	matrix_drawCharCentered("GAME OVER", 0, 0, COLOR_RED);
	char s[4];
	sprintf(s,"%d", player->score);
	matrix_drawCharCentered(s, 0, 8, COLOR_RED);
}
