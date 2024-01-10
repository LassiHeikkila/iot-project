#include "temperature.h"
#include "pressure.h"
#include "noise.h"

#include <stdbool.h>

#include "lpsxxx.h"
#include "lpsxxx_params.h"

static lpsxxx_t dev;
static bool init_ok = false;

static int init_lpsxxx(void) {
    if (init_ok) {
        // already initialized
        return TEMP_OP_OK;
    }

    if (lpsxxx_init(&dev, &lpsxxx_params[0]) != LPSXXX_OK) {
        puts("Initialization failed");
        return TEMP_OP_NOK;
    }
    init_ok = true;
    return TEMP_OP_OK;
}

int init_temperature(void) {
    return init_lpsxxx();
}

int init_pressure(void) {
    return init_lpsxxx();
}

// returns temperature in centi-Celsius
int read_raw_temperature(int16_t *temp) {
    int16_t tmp = 0;
    // this returns garbage due to https://github.com/RIOT-OS/RIOT/issues/20093
    // but it doesn't matter in this demo app
    if (lpsxxx_read_temp(&dev, &tmp) != LPSXXX_OK) {
        return TEMP_OP_NOK;
    }
    tmp += add_noise(TEMP_STDDEV);
    *temp = tmp;
    return TEMP_OP_OK;
}

// returns pressure in Pa
int read_raw_pressure(uint32_t *pres) {
    uint16_t hpa = 0;
    if (lpsxxx_read_pres(&dev, &hpa) != LPSXXX_OK) {
        return PRESSURE_OP_NOK;
    }
    hpa += add_noise(PRES_STDDEV);
    *pres = (uint32_t)hpa * 100;
    return PRESSURE_OP_OK;
}
