## Introduction
I create a project that learn how to play a snake game on the STM32F407VG Discovery Board. 

# Model Information: 
In the project, I used Double Deep Q learning with replay memory and mini batch. 
  1. Replay size is 3966 because of the limited RAM
  2. Mini-batch size is 32.
  3. MSE Loss function is used as Loss function
  4. SGD is used for optimizer. Optimizer settings are:
    I. learning rate: 0.000025
    II. momentum: 0
  5. Hard Update is used for updating Target network every 1e4 times.
  6. 

# Input Information
The input of the model is array with 12 elements. Each of the element represent special information.
These information is following:
  0. The Food is above the snake (1: Yes/True, 0: No/False)
  1. The Food is on the right size of the snake (1: Yes/True, 0: No/False)
  2. The Food is bottom the snake. (1: Yes/True, 0: No/False)
  3. The Food is on the left size of the snake (1: Yes/True, 0: No/False)
  4. The wall -obstacle- is just above the snake (1: Yes/True, 0: No/False)
  5. The wall -obstacle- is just on the right size of the snake (1: Yes/True, 0: No/False)
  6. The wall -obstacle- is just bottom the snake (1: Yes/True, 0: No/False)
  7. The wall -obstacle- is just on the left of the snake (1: Yes/True, 0: No/False)
  8. The snake's head will be moving to the upward (1: Yes/True, 0: No/False)
  9. The snake's head will be moving to the right  (1: Yes/True, 0: No/False)
  10. The snake's head will be moving to the downward (1: Yes/True, 0: No/False)
  11. The snake's head will be moving to the left (1: Yes/True, 0: No/False)

# Network Information
```
|‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|
|Dense Layer: Input=12, Output=32|
|________________________________|
              ||
              \/
|‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|
|    Leaky Relu: Max(0.01x, x)   |
|________________________________|
              ||
              \/
|‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|
|Dense Layer: Input=32, Output=16|
|________________________________|
              ||
              \/
|‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|
|    Leaky Relu: Max(0.01x, x)   |
|________________________________|
              ||
              \/
|‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|
|Dense Layer: Input=16, Output=4 |
|________________________________|
              ||
              \/
|‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|
|           Q-Values             |
|________________________________|
```
