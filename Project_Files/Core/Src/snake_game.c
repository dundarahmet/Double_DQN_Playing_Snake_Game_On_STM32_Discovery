/*
 * snake_game.c
 *
 *  Created on: Jun 2, 2021
 *      Author: ad
 */

#include "snake_game.h"


volatile uint8_t head_row = 0 ;
volatile uint8_t head_column = 0 ;

volatile uint8_t food_row = 0 ;
volatile uint8_t food_column = 0 ;

volatile uint8_t game_status = START ;
volatile uint8_t snake_length = 4 ;
volatile uint8_t move_status = MOVE_DIRECTION_RIGHT ;

volatile uint8_t holder = 0 ;
volatile uint8_t holder_row = 0 ;
volatile uint8_t holder_column = 0 ;

volatile uint32_t frame_counter = 0 ;
volatile uint32_t snake_life_restriction = 500 ;

volatile uint8_t env_output[12] ;

volatile uint8_t state[12][21] = {
		// 0th row LSB // 0
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		// 0th row MSB // 1
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

		// 1st row LSB // 2
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		// 1st row MSB // 3
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

		// 2st row LSB // 4
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		// 2st row MSB // 5
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

		// 3rd row LSB // 6
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		// 3rd row MSB // 7
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

		// 4th row LSB // 8
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		// 4th row MSB // 9
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

		// 5th row LSB // 10
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
		// 5th row MSB // 11
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
} ;

// 0: 0000 = No direction. No snake.
// 4: 0100	= Direction: right, Snake: Yes
// 5: 0101	= Direction: left,  Snake: Yes
// 6: 0110	= Direction: up,    Snake: Yes
// 7: 0111	= Direction: down,  Snake: Yes
// 8: 1000 = Food

volatile  uint8_t actions[4] = {MOVE_DIRECTION_RIGHT, MOVE_DIRECTION_LEFT, MOVE_DIRECTION_UP, MOVE_DIRECTION_DOWN} ;

void convert_state_to_env_output (void) {
	env_output[0] = (uint8_t)(food_row <= head_row) ;
	env_output[1] = (uint8_t)(food_column >= head_column) ;
	env_output[2] = (uint8_t)(food_row >= head_row) ;
	env_output[3] = (uint8_t)(food_column <= head_column) ;

	env_output[4] = (uint8_t)(head_row == 0) ;
	env_output[5] = (uint8_t)(head_column == 20) ;
	env_output[6] = (uint8_t)(head_row == 11) ;
	env_output[7] = (uint8_t)(head_column == 0) ;

	env_output[8] = (uint8_t)(state[head_row][head_column] == DIRECTION_UP) ;
	env_output[9] = (uint8_t)(state[head_row][head_column] == DIRECTION_RIGHT) ;
	env_output[10] = (uint8_t)(state[head_row][head_column] == DIRECTION_DOWN) ;
	env_output[11] = (uint8_t)(state[head_row][head_column] == DIRECTION_LEFT) ;
}

float calculate_distance_between_head_and_food (void) {
	volatile float first_part ;
	volatile float second_part ;

	first_part = ((float) head_row) - ((float) food_row) ;
	second_part = ((float) head_column) - ((float) food_column) ;

	if (first_part < 0) {
		first_part *= -1 ;
	}

	if (second_part < 0) {
		second_part *= -1 ;
	}

	return first_part * first_part + second_part * second_part ;
}

float step (uint8_t direction) {
	uint8_t old_snake_length = snake_length ;
	volatile float old_distance ;
	volatile float new_distance ;
	old_distance = calculate_distance_between_head_and_food() ;

	if (state[head_row][head_column] == DIRECTION_RIGHT && direction == MOVE_DIRECTION_LEFT) {
		direction = MOVE_DIRECTION_RIGHT ;
	} else if (state[head_row][head_column] == DIRECTION_LEFT && direction == MOVE_DIRECTION_RIGHT) {
		direction = MOVE_DIRECTION_LEFT ;
	} else if (state[head_row][head_column] == DIRECTION_UP && direction == MOVE_DIRECTION_DOWN) {
		direction = MOVE_DIRECTION_UP ;
	} else if (state[head_row][head_column] == DIRECTION_DOWN && direction == MOVE_DIRECTION_UP) {
		direction = MOVE_DIRECTION_DOWN ;
	}

	switch (direction) {
		case MOVE_DIRECTION_RIGHT:
			move_right() ;
			break ;
		case MOVE_DIRECTION_LEFT:
			move_left() ;
			break ;
		case MOVE_DIRECTION_UP:
			move_up() ;
			break ;
		case MOVE_DIRECTION_DOWN:
			move_down() ;
			break ;
	}

	new_distance = calculate_distance_between_head_and_food() ;

	frame_counter += 1 ;
	if (frame_counter >= snake_life_restriction) {
		frame_counter = 0 ;
		game_status = GAME_OVER ;
	}
	if (snake_length == 252){
		game_status = WIN ;
	}

	convert_state_to_env_output() ;

	if (game_status == GAME_OVER) {
		return -100. ;
	}
	if (old_snake_length < snake_length) {
		return 10. ;
	}
	if (new_distance < old_distance) {
		return -0.8 ;
	}
	return -1 ;
}

void move_snake_one_block_without_head (void) {
	uint8_t direction = state[head_row][head_column] ;
	uint8_t row = head_row ;
	uint8_t column = head_column ;

	for (uint8_t foo = 1; foo < snake_length; foo++) {
		switch (direction) {
			case DIRECTION_RIGHT:
				column-- ;
				state[row][column + 1] = direction ;
				direction = state[row][column] ;
				break ;
			case DIRECTION_LEFT:
				column++ ;
				state[row][column - 1] = direction ;
				direction = state[row][column] ;
				break ;
			case DIRECTION_UP:
				row++ ;
				state[row - 1][column] = direction ;
				direction = state[row][column] ;
				break ;
			case DIRECTION_DOWN:
				row-- ;
				state[row + 1][column] = direction ;
				direction = state[row][column] ;
				break ;
		}
	}
	state[row][column] = EMPTLY_BLOCK ;
}

void move_right (void) {
	if (head_column == 20) {
		// will hit the wall
		game_status = GAME_OVER ;
	} else {
		if (state[head_row][head_column + 1] == 0x0) {
			move_snake_one_block_without_head() ;
			head_column++ ;
			state[head_row][head_column] = DIRECTION_RIGHT ;
		} else if (state[head_row][head_column + 1] == FOOD_BLOCK) {
			// valid move
			snake_length++ ;
			head_column++ ;
			state[head_row][head_column] = DIRECTION_RIGHT ;
			set_new_food_position() ;
		} else {
			// snake hits its body.
			game_status = GAME_OVER ;
		}
	}
}
void move_left (void) {
	if (head_column == 0) {
			// will hit the wall
		game_status = GAME_OVER ;
	} else {
		if (state[head_row][head_column - 1] == 0x0) {
			move_snake_one_block_without_head() ;
			head_column-- ;
			state[head_row][head_column] = DIRECTION_LEFT ;
		} else if (state[head_row][head_column - 1] == FOOD_BLOCK) {
			// valid move
			head_column-- ;
			snake_length++ ;
			state[head_row][head_column] = DIRECTION_LEFT ;
			set_new_food_position() ;
		} else {
			// snake hits its body.
			game_status = GAME_OVER ;
		}
	}
}
void move_up (void) {
	if (head_row == 0) {
			// will hit the wall
		game_status = GAME_OVER ;
	} else {
		if (state[head_row - 1][head_column] == 0x0) {
			move_snake_one_block_without_head() ;
			head_row-- ;
			state[head_row][head_column] = DIRECTION_UP ;
		} else if (state[head_row - 1][head_column] == FOOD_BLOCK) {
			// valid move
			head_row-- ;
			snake_length++ ;
			state[head_row][head_column] = DIRECTION_UP ;
			set_new_food_position() ;
		} else {
			// snake hits its body.
			game_status = GAME_OVER ;
		}
	}
}
void move_down (void) {
	if (head_row == 11) {
				// will hit the wall
		game_status = GAME_OVER ;
	} else {
		if (state[head_row + 1][head_column] == 0x0) {
			move_snake_one_block_without_head() ;
			head_row++ ;
			state[head_row][head_column] = DIRECTION_DOWN ;
		} else if (state[head_row + 1][head_column] == FOOD_BLOCK) {
			// valid move
			head_row++ ;
			snake_length++ ;
			state[head_row][head_column] = DIRECTION_DOWN ;
			set_new_food_position() ;
		} else {
			// snake hits its body.
			game_status = GAME_OVER ;
		}
	}
}


void game_init (void) {
	game_status = START ;
	snake_length = 4 ;
	frame_counter = 0 ;

	for (uint8_t foo = 0; foo < 12; foo++) {
		for (uint8_t bar = 0; bar < 21; bar++) {
			state[foo][bar] = EMPTLY_BLOCK ;
		}
	}

	uint32_t holder = 0 ;
	while (!(RNG->SR & 0x1)) { }
	holder = RNG->DR ;

	head_row = (holder & 0xFF) % 12 ;
	head_column = (holder & 0xFF00) % 16 + 5 ;

	state[head_row][head_column] = DIRECTION_RIGHT ;
	state[head_row][head_column - 1] = DIRECTION_RIGHT ;
	state[head_row][head_column - 2] = DIRECTION_RIGHT ;
	state[head_row][head_column - 3] = DIRECTION_RIGHT ;

	set_new_food_position() ;
	convert_state_to_env_output() ;
}

void set_new_food_position (void) {
	// TODO: improve
	volatile uint8_t new_food_row = 0 ;
	volatile uint8_t new_food_column = 0 ;
	do {
		volatile uint32_t holder = 0 ;
		while (!(RNG->SR & 0x1)) { }
		holder = RNG->DR ;

		new_food_row = (holder & 0xFF) % 12 ;
		new_food_column = (holder & 0xFF00) % 21 ;


		if (state[new_food_row][new_food_column] == 0x0) {
			break ;
		}
	} while (1) ;


	food_row = new_food_row ;
	food_column = new_food_column ;
	state[food_row][food_column] = FOOD_BLOCK ;
}

void render (void) {
	LCD_Full_Eraser() ;
	uint8_t holder[504] = { 0 } ;

	for (uint8_t row = 0; row < 12; row++) {
		for (uint8_t column = 0; column < 21; column++) {
			if (row & 1) {
				// MSB
				if ((row == head_row && column == head_column) || (row == food_row && column == food_column)) {
					switch (state[row][column]) {
						case DIRECTION_RIGHT:
							// right
							holder[(row / 2) * 84 + column * 4 + 0] |=  0xF0;
							holder[(row / 2) * 84 + column * 4 + 1] |=  0xF0;
							holder[(row / 2) * 84 + column * 4 + 2] |=  0xF0;
							holder[(row / 2) * 84 + column * 4 + 3] |=  0x90;
							break ;
						case DIRECTION_LEFT:
							// left
							holder[(row / 2) * 84 + column * 4 + 0] |=  0x90;
							holder[(row / 2) * 84 + column * 4 + 1] |=  0xF0;
							holder[(row / 2) * 84 + column * 4 + 2] |=  0xF0;
							holder[(row / 2) * 84 + column * 4 + 3] |=  0xF0;
							break ;
						case DIRECTION_UP:
							// upper ;
							holder[(row / 2) * 84 + column * 4 + 0] |=  0xF0;
							holder[(row / 2) * 84 + column * 4 + 1] |=  0xE0;
							holder[(row / 2) * 84 + column * 4 + 2] |=  0xE0;
							holder[(row / 2) * 84 + column * 4 + 3] |=  0xF0;
							break ;
						case DIRECTION_DOWN:
							// down ;
							holder[(row / 2) * 84 + column * 4 + 0] |=  0xF0;
							holder[(row / 2) * 84 + column * 4 + 1] |=  0x70;
							holder[(row / 2) * 84 + column * 4 + 2] |=  0x70;
							holder[(row / 2) * 84 + column * 4 + 3] |=  0xF0;
							break ;
						case FOOD_BLOCK:
							holder[(row / 2) * 84 + column * 4 + 0] |=  0x60;
							holder[(row / 2) * 84 + column * 4 + 1] |=  0xF0;
							holder[(row / 2) * 84 + column * 4 + 2] |=  0xF0;
							holder[(row / 2) * 84 + column * 4 + 3] |=  0x60;
							break ;
					}
				} else {
					for (uint8_t i = 0; i < 4; i++) {
						if (state[row][column]) {
							holder[(row / 2) * 84 + column * 4 + i] |= 0xF0 ;
						}
					}
				}
			} else {
				// LSB
				if ((row == head_row && column == head_column) || (row == food_row && column == food_column)) {
					switch (state[row][column]) {
						case DIRECTION_RIGHT:
							// right
							holder[(row / 2) * 84 + column * 4 + 0] |=  0xF;
							holder[(row / 2) * 84 + column * 4 + 1] |=  0xF;
							holder[(row / 2) * 84 + column * 4 + 2] |=  0xF;
							holder[(row / 2) * 84 + column * 4 + 3] |=  0x9;
							break ;
						case DIRECTION_LEFT:
							// left
							holder[(row / 2) * 84 + column * 4 + 0] |=  0x9;
							holder[(row / 2) * 84 + column * 4 + 1] |=  0xF;
							holder[(row / 2) * 84 + column * 4 + 2] |=  0xF;
							holder[(row / 2) * 84 + column * 4 + 3] |=  0xF;
							break ;
						case DIRECTION_UP:
							// upper ;
							holder[(row / 2) * 84 + column * 4 + 0] |=  0xF;
							holder[(row / 2) * 84 + column * 4 + 1] |=  0xE;
							holder[(row / 2) * 84 + column * 4 + 2] |=  0xE;
							holder[(row / 2) * 84 + column * 4 + 3] |=  0xF;
							break ;
						case DIRECTION_DOWN:
							// down ;
							holder[(row / 2) * 84 + column * 4 + 0] |=  0xF;
							holder[(row / 2) * 84 + column * 4 + 1] |=  0x7;
							holder[(row / 2) * 84 + column * 4 + 2] |=  0x7;
							holder[(row / 2) * 84 + column * 4 + 3] |=  0xF;
							break ;
						case FOOD_BLOCK:
							holder[(row / 2) * 84 + column * 4 + 0] |=  0x6;
							holder[(row / 2) * 84 + column * 4 + 1] |=  0xF;
							holder[(row / 2) * 84 + column * 4 + 2] |=  0xF;
							holder[(row / 2) * 84 + column * 4 + 3] |=  0x6;
							break ;
					}

				} else {
					for (uint8_t i = 0; i < 4; i++) {
						if (state[row][column]) {
							holder[(row / 2) * 84 + column * 4 + i] |= 0xF ;
						}
					}
				}
			}
		}
	}

	SPI2_Send_8_bit_pData(holder, 504) ;
}


void win_graph (void) {
	LCD_Full_Eraser() ;
	SPI2_Send_8_bit_pData(win_layout, 504) ;

	LCD_Command_Enabled() ;
	SPI2_Send_8_bit_data(FUNCTION_SET_BEGINNER | PD_CHIP_IN_ACTIVE | H_VALUE_FOR_DISPALY_OR_Y_X_ADDRESS) ;
	for (int i = 0; i < 5; i++) {
		delay_ms(250) ;
		SPI2_Send_8_bit_data(SET_DISPLAY_INVERSE_VIDEO_MODE) ;
		delay_ms(250) ;
		SPI2_Send_8_bit_data(SET_DISPLAY_NORMAL_MODE) ;
	}

	LCD_Data_Enabled() ;
}


uint8_t win_layout [] = {
						0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0xc0, 0xc0, 0xc0,
						0xf8, 0xfc, 0xfe, 0xfe, 0xfe, 0xff, 0xff, 0xfe, 0xfe, 0xfe,
						0xfc, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0xf0, 0xf8, 0xfc, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe,
						0xfe, 0xfe, 0xfc, 0xf8, 0xc0, 0xc0, 0xc0, 0xc0, 0x80, 0x80,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x3f, 0x3f,
						0x7f, 0x7f, 0x7f, 0x7f, 0xff, 0xff, 0x7f, 0x7f, 0x7f, 0x7f,
						0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x7f, 0x7f,
						0x7f, 0x7f, 0x7f, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f,
						0x7f, 0x7f, 0x7f, 0x3f, 0x1f, 0x04, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x03, 0x04, 0x3c, 0x06, 0x03, 0x00, 0x0e,
						0x11, 0x21, 0x21, 0x21, 0x11, 0x0e, 0x00, 0x1f, 0x20, 0x20,
						0x20, 0x00, 0x1f, 0x00, 0x00, 0x01, 0x00, 0x3f, 0x01, 0x01,
						0x01, 0x0d, 0x32, 0x00, 0x3f, 0x25, 0x25, 0x25, 0x25, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x04, 0x04, 0xfc, 0x04, 0x04, 0x00, 0xfc, 0x10, 0x10,
						0x10, 0x10, 0xfc, 0x00, 0xfc, 0x94, 0x94, 0x94, 0x94, 0x00,
						0x00, 0x00, 0x00, 0x7c, 0x94, 0x94, 0x94, 0x14, 0x68, 0x00,
						0x7c, 0x94, 0x94, 0x94, 0x94, 0x00, 0x48, 0x1c, 0x94, 0x94,
						0xa4, 0x60, 0x04, 0x04, 0x04, 0x7c, 0x04, 0x04, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0xfc, 0xfc,
						0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe,
						0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe,
						0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfc, 0xfc, 0xf8, 0xf0,
						0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x03, 0x1f, 0x7f,
						0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x3f, 0x0f,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0x3f, 0x7f, 0xff, 0xff,
						0xff, 0xff, 0xff, 0xff, 0x7f, 0x7f, 0x1f, 0x03, 0x03, 0x03,
						0x03, 0x03, 0x01, 0x00 };
