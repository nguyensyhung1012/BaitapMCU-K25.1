/*
 * srec_parser.c
 *
 *  Created on: 20 thg 10, 2025
 *      Author: nguye
 */


#include "srec_parser.h"


unsigned char hex_to_byte(const char *hex)
{
    unsigned char high, low;
    char c;
    char result;

    /* change first character (4 high bit) */
    c = hex[0];
    if (c >= '0' && c <= '9')
        high = c - '0';
    else if (c >= 'A' && c <= 'F')
        high = c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        high = c - 'a' + 10;
    else
        return 0;

    /* change second character (4 low bit)*/
    c = hex[1];
    if (c >= '0' && c <= '9')
        low = c - '0';
    else if (c >= 'A' && c <= 'F')
        low = c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        low = c - 'a' + 10;
    else
        return 0;
    /*get 8 bit result*/
    result = (high << 4) | low ;

    return result;
}

int parse_srec_line(const char *line, srec_record_t *rec) {
    /*type of srec*/
    int type ;
    /*length of addr*/
    int addr_len ;
    /* byte count*/
    uint8_t count = hex_to_byte(line + 2);
    /*length of data*/
    int data_len ;
    /*position of srec*/
    int pos = 0 ;
    /* caclulate ByteCount + Address bytes + Data bytes + Checksum*/
    uint8_t sum = 0 ;
    /*1. check first character */
    if (line[0] != 'S') return -1;

    /*2. check type */
    type = line[1] - '0';
    rec->type = type;

    /*3. check length of address */
    switch (type) {
        case 0: case 1: case 5: case 9: addr_len = 2; break;
        case 2: case 8: addr_len = 3; break;
        case 3: case 7: addr_len = 4; break;
        default: return -2;
    }

    /*4. get address */
    rec->address = 0;
    for (int i = 0; i < addr_len; i++) {
        rec->address = (rec->address << 8) | hex_to_byte(line + 4 + i * 2);
    }

    /*5. get data byte */
    data_len = count - addr_len - 1;
    rec->data_len = data_len;

    /*6. read data byte */
    pos = 4 + addr_len * 2;
    for (int i = 0; i < data_len; i++) {
        rec->data[i] = hex_to_byte(line + pos + i * 2);
    }

    /* 7. read checksum */
    rec->checksum = hex_to_byte(line + pos + data_len * 2);

    /* 8. check checksum */
    sum = count;
    pos = 4;
    for (int i = 0; i < addr_len * 2; i += 2)
    {
        sum += hex_to_byte(line + pos + i);
    }
    for (int i = 0; i < data_len; i++)
    {
        sum += rec->data[i];
    }

    sum += rec->checksum;
    rec->valid = ((sum & 0xFF) == 0xFF);

    return 0;
}
