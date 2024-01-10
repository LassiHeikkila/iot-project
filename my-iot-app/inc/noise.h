#ifndef MY_IOT_APP_NOISE_H
#define MY_IOT_APP_NOISE_H

#define TEMP_STDDEV 789.2f
#define PRES_STDDEV 99.2f

float generate_normal_random(float stddev);

float add_noise(float stddev);

#endif // MY_IOT_APP_NOISE_H