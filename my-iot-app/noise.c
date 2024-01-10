#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "noise.h"

float generate_normal_random(float stddev) {
    float M_PI = 3.1415926535;

    // Box-Muller transform to generate random numbers with normal distribution
    float u1 = rand() / (float)RAND_MAX;
    float u2 = rand() / (float)RAND_MAX;
    float z = sqrt(-2 * log(u1)) * cos(2 * M_PI * u2);
    
    return stddev * z;
}

float add_noise(float stddev) {
    int num;
    float noise_val = 0;
    
    num = rand() % 100 + 1; // use rand() function to get the random number
    if (num >= 50) {
        // Generate a random number with normal distribution based on a stddev
        noise_val = generate_normal_random(stddev);
    }
    return noise_val;
}