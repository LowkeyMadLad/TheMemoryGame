#include <stdio.h>
#include <stdlib.h>
#include "RNG.h"

// Returns a random number between 1-3
int rand_num(){
    int num = (rand() % 3) + 1;
    return num;
}