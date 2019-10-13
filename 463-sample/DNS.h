#pragma once

/* HW1-1
 * DNS.h: This parses the input URL to create the GET request
 *
 * Date: 8/29/2019
 * Author: Dalton Altstaetter
 * CSE 612
 * Fall 2019
 */

#include "pch.h"
//#include "globals.h"

#define SUCCESS         0
#define MAX_DNS_LEN     512

// helpers
static char* get_char(char paSub_url[MAX_URL_LEN], const int8_t delimiter);
int32_t err_check(int32_t aTest, const char* aMsg, const char* aFile, const char* aFunction, int32_t aLine_num, int32_t err_quit);
void kill_pointer(void **ptr);
uint32_t null_strlen(const char* str);
int32_t print_usage(void);
void exit_process(void);
int32_t starts_with(const char* start_string, const char* in_string);

typedef struct DNS_Answer_Header {

    uint16_t dns_type;
    uint16_t dns_class;
    uint32_t dns_ttl;
    uint16_t dns_length;

} DNS_Answer_Header_t;

typedef struct DNS_Query_Header {

    uint16_t dns_type;
    uint16_t dns_class;
    uint32_t dns_ttl;
    uint16_t dns_length;

} DNS_Query_Header_t;



