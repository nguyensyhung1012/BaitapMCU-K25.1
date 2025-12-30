/*
 * srec_parsr.h
 *
 *  Created on: 20 thg 10, 2025
 *      Author: nguye
 */

#ifndef SREC_PARSER_H_
#define SREC_PARSER_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdint.h>
/**
 * @brief type of srec
 *
 */
typedef enum {
    SREC_S0 = 0,
    SREC_S1,
    SREC_S2,
    SREC_S3,
    SREC_S5,
    SREC_S7,
    SREC_S8,
    SREC_S9,
    SREC_UNKNOWN
} srec_type_t;

/**
 * @brief srec
 *
 */
typedef struct {
    srec_type_t type;
    uint32_t address;
    uint8_t data[256];
    uint8_t data_len;
    uint8_t checksum;
    uint8_t valid;
} srec_record_t;

/**
 * @brief hex to byte conversion function
 *
 * @param hex
 * @return * unsigned char
 */
unsigned char hex_to_byte(const char *hex);
/**
 * @brief srec data check function
 *
 * @param line
 * @param rec
 * @return int
 */
int parse_srec_line(const char *line, srec_record_t *rec);

#ifdef __cplusplus
}   // extern "C"
#endif


#endif /* INCLUDE_SREC_PARSER_H_ */
