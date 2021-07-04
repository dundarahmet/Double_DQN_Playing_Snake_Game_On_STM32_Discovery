/*
 * neural_nerwork.h
 *
 *  Created on: 19 Haz 2021
 *      Author: m
 */

#ifndef INC_NEURAL_NETWORK_H_
#define INC_NEURAL_NETWORK_H_
#include "nn_utils.h"

const uint32_t replay_size ;

volatile float input_env[12] ;

volatile float model_dense_layer_1_weight[12][32] ;
volatile float model_dense_layer_1_bias[32] ;
volatile float model_dense_layer_2_weight[32][16] ;
volatile float model_dense_layer_2_bias[16] ;
volatile float model_dense_layer_3_weight[16][4] ;
volatile float model_dense_layer_3_bias[4] ;


volatile float model_derivative_dense_layer_1_bias[32] ;
volatile float model_derivative_dense_layer_2_bias[16] ;
volatile float model_derivative_dense_layer_3_bias[4] ;


volatile float model_dense_layer_1_output[32] ;
volatile float model_dense_layer_2_output[16] ;
volatile float model_output[4] ;


volatile uint8_t replay_env_output[3966][12] ;
volatile uint8_t replay_next_env_output[3966][12] ;
volatile uint8_t replay_action[3966] ;
volatile uint8_t replay_game_status[3966] ;
volatile float replay_reward[3966] ;


volatile float target_dense_layer_1_weight[12][32] ;
volatile float target_dense_layer_1_bias[32] ;
volatile float target_dense_layer_2_weight[32][16] ;
volatile float target_dense_layer_2_bias[16] ;
volatile float target_dense_layer_3_weight[16][4] ;
volatile float target_dense_layer_3_bias[4] ;


volatile float target_dense_layer_1_output[32] ;
volatile float target_dense_layer_2_output[16] ;
volatile float target_output[4] ;

volatile float backprop_input[4] ;


void reset_backprop_input (void) ;
void initialise_parameters (void) ;
void model_nn (void) ;
void target_nn (void) ;
void copy_parameters (void) ;
void backpropagation (void) ;
#endif
