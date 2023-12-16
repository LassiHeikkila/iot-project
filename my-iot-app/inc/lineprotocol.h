#ifndef MY_APP_LINEPROTOCOL_H
#define MY_APP_LINEPROTOCOL_H

#include <stddef.h>
#include <stdint.h>


/// @brief Serialize temperature and pressure measurement into Influx lineprotocol. 
///   If both temperature and pressure are NULL, -1 is returned. 
///   -1 is also returned node_id is NULL.
/// @param buf : pointer to string buffer
/// @param cap : capacity of string buffer
/// @param node_id : pointer to string representing ID of the node. Mandatory parameter.
/// @param temperature : pointer to temperature value. Omitted from output if NULL.
/// @param pressure : pointer to pressure value. Omitted from output if NULL.
/// @return Length of the final string, or -1 to indicate an error.
int into_lineprotocol(char *buf, size_t cap, char* node_id, int16_t* temperature, uint32_t* pressure);

#endif // MY_APP_LINEPROTOCOL_H