#include "lineprotocol.h"

#include <stdio.h>
#include <stdbool.h>

#define line_proto_prefix_fmt "measurement,node=%s "
#define line_proto_temp_fmt   "temperature=%d.%d"
#define line_proto_pres_fmt   "pressure=%ldi"

#define abs(x) ((x) > 0 ? (x) : (-(x))) 

int into_lineprotocol(char *buf, size_t cap, char* node_id, int16_t* temperature, uint32_t* pressure) {
    // example of final output:
    // measurement,node=4efbf280-f563b1f1-ddb4dc9 temperature=16.79,pressure=103300i
    // length is 78 (including final null)

    int head = 0;
    bool need_comma = false;

    // check that node_id is non-null, and at least one of temperature or pressure is non-null
    if (node_id == NULL) {
        return -1;
    }

    if (temperature == NULL && pressure == NULL) {
        return -1;
    }

    // hundred bytes should be long enough for realistic values
    // TODO: calculate required space properly
    if (cap < 100) {
        return -1;
    }

    head += sprintf(buf, line_proto_prefix_fmt, node_id);
    if (temperature != NULL) {
        int16_t temp = *temperature;
        head += sprintf(&buf[head], line_proto_temp_fmt, (temp / 100), abs(temp % 100));
        need_comma = true;
    }
    if (pressure != NULL) {
        if (need_comma) {
            buf[head++] = ',';
        }
        head += sprintf(&buf[head], line_proto_pres_fmt, *pressure);
        need_comma = true;
    }

    return head;
}