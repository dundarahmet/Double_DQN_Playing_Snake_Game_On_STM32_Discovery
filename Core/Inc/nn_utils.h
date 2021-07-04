/*
 * network.h
 *
 *  Created on: 15 Haz 2021
 *      Author: m
 */

#ifndef INC_NN_UTILS_H_
#define INC_NN_UTILS_H_

#include "stm32f407xx.h"

volatile float learning_rate;

void initialize_bias(uint8_t width, volatile float bias[width]) ;

void dense_parameters_initializer (uint8_t width, uint8_t height, volatile float weight[width][height]) ;

void leaky_relu_dense (uint8_t width, volatile float matrix[width], volatile float matrix_result[width]) ;

void dense_layer (uint8_t width_1, volatile float matrix[width_1],
				  uint8_t width_2, uint8_t height_2, volatile float weight[width_2][height_2], volatile float bias[height_2],
				  uint8_t width_3, volatile float matrix_result[width_3]);


uint8_t argmax (uint8_t row, volatile float matrix[row]) ;

float create_random_number (void) ;

float mseloss (uint8_t row, volatile float y_pred[row], volatile float y_true[row]) ;

void copy_matrix (uint8_t width, uint8_t height, volatile float source_matrix[width][height],
				  volatile float destination_matrix[width][height]) ;
void copy_array (uint8_t width, volatile float source_array[width], volatile float destination_array[width]) ;

#endif /* INC_NN_UTILS_H_ */
