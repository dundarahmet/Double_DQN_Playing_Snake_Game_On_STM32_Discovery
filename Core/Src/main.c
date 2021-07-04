#include <neural_network.h>
#include "snake_game.h"
#include "stdio.h"
#include <stdlib.h>
/*
 * 1. RST = Reset pin. = PB0
 * 2. CE = chip Enable = PB12
 * 3. D/C = Data/Command. = PB1
 * 4. DIN = Data in = PB15
 * 5. CLK = Clock = PB13
 * 6. VCC = PB5
 * 7. BL = PB7
 * 8. GND
*/

// SPI2 is connected to APB1
// max. sckl speed is 4Mhz.
// AF5 (SPI1/SPI2)
// PB12 SPI2_NSS
// PB13 SPI2_SCK
// PB15 SPI2_MOSI

volatile float loss_value = 0. ;


volatile uint32_t win_counter = 0. ;
volatile uint32_t loop_counter = 0. ;
volatile uint32_t decision_counter = 0 ;
volatile uint32_t total_epochs = 0 ;

volatile uint8_t old_env_output[12] ;
const uint8_t mini_batch_size = 32 ;

uint32_t generate_random_number (void) {
	uint32_t rng_holder ;
	rng_holder = rand() ;
	return rng_holder ;
}

void copy_state_to_replay (uint32_t at, volatile uint8_t action, volatile float reward) {
	uint8_t row ;
	for (row = 0; row < 12; row++) {
		replay_env_output[at][row] = old_env_output[row] ;
		replay_next_env_output[at][row] = env_output[row] ;
	}

	replay_action[at] = action ;
	replay_reward[at] = reward ;
	replay_game_status[at] = game_status ;
}

void copy_env_to_old_env (void) {
	uint8_t row ;
	for (row = 0; row < 12; row++) {
		old_env_output[row] = env_output[row] ;
	}
}

void copy_env_to_input_matrix (void) {
	uint8_t row = 0 ;
	for (row = 0; row < 12; row++) {
		input_env[row] = (float)(env_output[row]) ;
	}
}

volatile uint8_t generate_random_action (void) {

	return rand() % 4;
}



int main (void) {

	LED_Init() ;
	HSE_168MHZ_Init() ;
	systick_init() ;

	SPI2_Init() ;
	LCD_Pin_Init() ;
	initialise_parameters() ;

	volatile float reward = 0. ;
	volatile float exploration_explotation_rate = 1. ;
	volatile float gamma = .96 ;
	volatile float modification_value = 0. ;
	volatile float hold = 0. ;
	volatile uint8_t best_action = 0 ;

	volatile uint8_t action_ix = 0 ;
	volatile uint32_t rng_holder = 0 ;

	volatile uint8_t general_row = 0 ;
	volatile uint8_t mini_batch_counter = 0;
	volatile uint8_t stop = 1 ;

	uint8_t update_counter = 0 ;

	while (1) {
		game_init() ;
		render() ;
		loop_counter += 1 ;

		if (total_epochs >= 200000 && stop != 0) {
			stop = 0;

		}

		while (game_status == START) {
			update_counter += 1 ;

			// For state St
			rng_holder = generate_random_number() ;
			hold = (float)(rng_holder) / (float)(RAND_MAX) ;

			if (exploration_explotation_rate > hold) {
				action_ix = generate_random_action() ;
			} else {
				copy_env_to_input_matrix() ;
				model_nn() ;
				action_ix = argmax(4, model_output) ;
				decision_counter += 1 ;
			}


			copy_env_to_old_env() ;
			reward = step(actions[action_ix]) ;
			copy_state_to_replay(total_epochs % replay_size, action_ix, reward) ;
			if (total_epochs > replay_size) {
				//start learning
				// copy current state
				reset_backprop_input() ;
				for (mini_batch_counter = 0; mini_batch_counter < mini_batch_size; mini_batch_counter++) {
						rng_holder = generate_random_number() % replay_size ;

					// target
					for (general_row = 0; general_row < 12; general_row++) {
						input_env[general_row] = (float)replay_next_env_output[rng_holder][general_row] ;
					}

					model_nn() ;
					best_action = argmax(4, model_output) ;
					target_nn() ;

					if (replay_game_status[rng_holder] != START) {
						modification_value = replay_reward[rng_holder] ;
					} else {
						modification_value = replay_reward[rng_holder] + gamma * target_output[best_action] ;
					}


					// model
					for (general_row = 0; general_row < 12; general_row++) {
						input_env[general_row] = (float)replay_env_output[rng_holder][general_row] ;
					}
					model_nn() ;

//					for (general_row = 0; general_row < 4; general_row++) {
//						if (general_row == replay_action[rng_holder]) {
//							target_output[general_row] = modification_value ;
//						} else {
//							target_output[general_row] = model_output[general_row] ;
//						}
//					}
					for (general_row = 0; general_row < 4; general_row++) {
						if (general_row == replay_action[rng_holder]) {
							backprop_input[general_row] += ((model_output[general_row] - modification_value) / (float)2.) ;
						} else {
							backprop_input[general_row] += 0 ;
						}
					}
				}

				for (general_row = 0; general_row < 4; general_row++) {
					backprop_input[general_row] /= mini_batch_size ;
				}
				backpropagation() ;
			}

			if (update_counter == 10000) {
				copy_parameters() ;
				update_counter = 0 ;
			}

			render() ;

			loss_value = mseloss(4, model_output, target_output) ;
			total_epochs += 1 ;

			if (exploration_explotation_rate <= 0.001) {
				exploration_explotation_rate = 0.001 ;
			} else {
				exploration_explotation_rate *= 0.99973 ;
			}
		}

		if (game_status == WIN) {
			win_counter += 1 ;
			delay_ms(250) ;
			win_graph() ;
		}
	}
}
