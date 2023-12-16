#ifndef MY_APP_PRESSURE_H
#define MY_APP_PRESSURE_H

#include <stdint.h>

#define PRESSURE_OP_OK 0
#define PRESSURE_OP_NOK 1

int init_pressure(void);

// stores pressure value in Pa into *pres
int read_raw_pressure(uint32_t *pres);

#endif // MY_APP_PRESSURE_H