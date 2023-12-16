#ifndef MY_APP_TEMPERATURE_H
#define MY_APP_TEMPERATURE_H

#include <stdint.h>

#define TEMP_OP_OK 0
#define TEMP_OP_NOK 1

int init_temperature(void);

int read_raw_temperature(int16_t *temp);

#endif // MY_APP_TEMPERATURE_H
