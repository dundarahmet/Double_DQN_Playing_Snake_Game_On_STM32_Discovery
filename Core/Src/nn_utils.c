/*
 * network.c
 *
 *  Created on: 15 Haz 2021
 *      Author: m
 */

#include "nn_utils.h"
#include <stdlib.h>

volatile float learning_rate = 0.000025 ;

void copy_matrix (uint8_t width, uint8_t height, volatile float source_matrix[width][height],
				  volatile float destination_matrix[width][height]) {
	uint8_t row ;
	uint8_t column ;

	for (row = 0; row < width; row++) {
		for (column = 0; column < height; column++) {
			destination_matrix[row][column] = source_matrix[row][column] ;
		}
	}
}
void copy_array (uint8_t width, volatile float source_array[width], volatile float destination_array[width]) {
	uint8_t row ;
	for (row = 0; row < width; row++) {
		destination_array[row] = source_array[row] ;
	}
}
void leaky_relu_dense (uint8_t width, volatile float matrix[width], volatile float matrix_result[width]) {
	uint8_t row ;

	for (row = 0; row < width; row++) {
		if (matrix[row] < 0) {
			matrix_result[row] = matrix[row] * (float) (0.01) ;
		} else {
			matrix_result[row] = matrix[row] ;
		}
	}
}
void dense_parameters_initializer (uint8_t width, uint8_t height, volatile float weight[width][height]) {
	uint8_t row ;
	uint8_t column ;

	for (row = 0; row < width; row++) {
		for (column = 0; column < height; column++) {
			weight[row][column] = create_random_number() ;
		}
	}
}
void dense_layer (uint8_t width_1, volatile float matrix[width_1],
				  uint8_t width_2, uint8_t height_2, volatile float weight[width_2][height_2], volatile float bias[height_2],
				  uint8_t width_3, volatile float matrix_result[width_3]) {
	uint8_t column ;
	uint8_t row ;
	volatile float holder;

	for (column = 0; column < height_2; column++) {
		holder = 0 ;
		for (row = 0; row < width_2; row++) {
			holder += (matrix[row] * weight[row][column]) ;
		}
		matrix_result[column] = holder + bias[column];
	}
}
uint8_t argmax (uint8_t row, volatile float matrix[row]) {
	volatile float maximum = matrix[0] ;
	uint8_t max_index = 0 ;
	uint8_t index ;
	for (index = 1; index < row; index++) {
		if (maximum < matrix[index]) {
			maximum = matrix[index] ;
			max_index = index ;
		}
	}

	return max_index ;
}
float create_random_number (void) {
	return ((float) rand() / (float) (RAND_MAX / 2)) - (float) 1.;
}
float mseloss (uint8_t row, volatile float y_pred[row], volatile float y_true[row]) {
	volatile float result = 0. ;
	volatile float holder = 0. ;
	uint8_t rw = 0 ;

	for (rw = 0; rw < row; rw++) {
		holder = y_pred[rw] - y_true[rw] ;
		result += holder * holder ;
	}

	return result / (float) 4. ;
}


void initialize_bias(uint8_t width, volatile float bias[width]) {
	uint8_t row = 0;

	for (row = 0; row < width; row++) {
		bias[row] = create_random_number() ;
	}
}
