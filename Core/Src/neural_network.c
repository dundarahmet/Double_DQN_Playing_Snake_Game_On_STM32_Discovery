/*
 * neural_network.c
 *
 *  Created on: 19 Haz 2021
 *      Author: m
 */


#include <neural_network.h>


const uint32_t replay_size = 3966 ;

volatile float input_env[12] ;

volatile float model_dense_layer_1_weight[12][32] ;
volatile float model_dense_layer_1_bias[32] = {0};
volatile float model_dense_layer_2_weight[32][16] ;
volatile float model_dense_layer_2_bias[16] = {0};
volatile float model_dense_layer_3_weight[16][4] ;
volatile float model_dense_layer_3_bias[4] = {0};


volatile float model_derivative_dense_layer_1_bias[32] = {0};
volatile float model_derivative_dense_layer_2_bias[16] = {0};
volatile float model_derivative_dense_layer_3_bias[4] = {0};


volatile float model_dense_layer_1_output[32] ;
volatile float model_dense_layer_2_output[16] ;
volatile float model_output[4] ;


volatile uint8_t replay_env_output[3966][12] ;
volatile uint8_t replay_next_env_output[3966][12] ;
volatile uint8_t replay_action[3966] ;
volatile uint8_t replay_game_status[3966] ;
volatile float replay_reward[3966] ;


volatile float target_dense_layer_1_weight[12][32] ;
volatile float target_dense_layer_1_bias[32] = {0};
volatile float target_dense_layer_2_weight[32][16] ;
volatile float target_dense_layer_2_bias[16] = {0};
volatile float target_dense_layer_3_weight[16][4] ;
volatile float target_dense_layer_3_bias[4] = {0};


volatile float target_dense_layer_1_output[32] ;
volatile float target_dense_layer_2_output[16] ;
volatile float target_output[4] ;
volatile float backprop_input[4] ;


void reset_backprop_input (void) {
	uint8_t row ;
	for (row = 0; row < 4; row++) {
		backprop_input[row] = 0 ;
	}
}

void initialise_parameters (void) {
	dense_parameters_initializer(12, 32, model_dense_layer_1_weight) ;
	initialize_bias(32, model_dense_layer_1_bias) ;
	dense_parameters_initializer(32, 16, model_dense_layer_2_weight) ;
	initialize_bias(16, model_dense_layer_2_bias) ;
	dense_parameters_initializer(16, 4, model_dense_layer_3_weight) ;
	initialize_bias(4, model_dense_layer_3_bias) ;

	copy_parameters() ;

}
void model_nn (void) {
	dense_layer(12, input_env, 12, 32, model_dense_layer_1_weight, model_dense_layer_1_bias, 32, model_dense_layer_1_output) ;
	leaky_relu_dense(32, model_dense_layer_1_output, model_dense_layer_1_output) ;

	dense_layer(32, model_dense_layer_1_output, 32, 16, model_dense_layer_2_weight, model_dense_layer_2_bias, 16, model_dense_layer_2_output) ;
	leaky_relu_dense(16, model_dense_layer_2_output, model_dense_layer_2_output) ;

	dense_layer(16, model_dense_layer_2_output, 16, 4, model_dense_layer_3_weight, model_dense_layer_3_bias, 4, model_output) ;
}
void target_nn (void) {
	dense_layer(12, input_env, 12, 32, target_dense_layer_1_weight, target_dense_layer_1_bias, 32, target_dense_layer_1_output) ;
	leaky_relu_dense(32, target_dense_layer_1_output, target_dense_layer_1_output) ;

	dense_layer(32, target_dense_layer_1_output, 32, 16, target_dense_layer_2_weight, target_dense_layer_2_bias, 16, target_dense_layer_2_output) ;
	leaky_relu_dense(16, target_dense_layer_2_output, target_dense_layer_2_output) ;

	dense_layer(16, target_dense_layer_2_output, 16, 4, target_dense_layer_3_weight, target_dense_layer_3_bias, 4, target_output) ;
}
void copy_parameters (void) {
	copy_matrix(12, 32, model_dense_layer_1_weight, target_dense_layer_1_weight) ;
	copy_array(32, model_dense_layer_1_bias, target_dense_layer_1_bias) ;

	copy_matrix(32, 16, model_dense_layer_2_weight, target_dense_layer_2_weight) ;
	copy_array(16, model_dense_layer_2_bias, target_dense_layer_2_bias) ;

	copy_matrix(16, 4, model_dense_layer_3_weight, target_dense_layer_3_weight) ;
	copy_array(4, model_dense_layer_3_bias, target_dense_layer_3_bias) ;
}
void backpropagation (void) {
	volatile float result ;
	uint8_t row ;
	uint8_t column ;

	for (row = 0; row < 4; row++) {
		//model_derivative_dense_layer_3_bias[row] = (model_output[row] - target_output[row]) / (float) 2.;
		model_derivative_dense_layer_3_bias[row] = backprop_input[row];
		model_dense_layer_3_bias[row] -= learning_rate * model_derivative_dense_layer_3_bias[row] ;
	}
	for (row = 0; row < 16; row++) {
		for (column = 0; column < 4; column++) {
			result = model_dense_layer_2_output[row] * model_derivative_dense_layer_3_bias[column] ;
			model_dense_layer_3_weight[row][column] -= learning_rate * result ;
		}
	}


	for (row = 0; row < 16; row++) {
		result = 0 ;
		for (column = 0; column < 4; column++) {
			result += (model_dense_layer_3_weight[row][column] * model_derivative_dense_layer_3_bias[column]) ;
		}
		if (model_dense_layer_2_output[row] < 0) {
			result *= (float)0.001 ;
		}
		model_derivative_dense_layer_2_bias[row] = result ;
	}
	for (row = 0; row < 32; row++) {
		for (column = 0; column < 16; column++) {
			result = model_dense_layer_1_output[row] * model_derivative_dense_layer_2_bias[column] ;
			model_dense_layer_2_weight[row][column] -= learning_rate * result ;
		}
	}


	for (row = 0; row < 32; row++) {
		result = 0 ;
		for (column = 0; column < 16; column++) {
			result += (model_dense_layer_2_weight[row][column] * model_derivative_dense_layer_2_bias[column]) ;
		}
		if (model_dense_layer_1_output[row] < 0) {
			result *= (float)0.001 ;
		}
		model_derivative_dense_layer_1_bias[row] = result ;
	}
	for (row = 0; row < 12; row++) {
		for (column = 0; column < 32; column++) {
			result = input_env[row] * model_derivative_dense_layer_1_bias[column] ;
			model_dense_layer_1_weight[row][column] -= learning_rate * result ;
		}
	}
}
