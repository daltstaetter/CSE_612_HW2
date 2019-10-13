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
#define FAIL            1
#define MAX_DNS_LEN     512

#pragma pack(push,1)    // sets struct padding/alignment to 1 byte
typedef struct DNS_Answer_Header {
    uint16_t dns_type;
    uint16_t dns_class;
    uint32_t dns_ttl;
    uint16_t dns_length;

} DNS_Answer_Header_t;

typedef struct DNS_Query_Header {
    uint16_t dns_name;
    uint16_t dns_value;
    uint16_t dns_type;
    uint32_t dns_ttl;
    uint16_t dns_length;

} DNS_Query_Header_t;

typedef struct Fixed_DNS_Header {
    uint16_t tx_id;
    uint16_t flags;
    uint16_t num_questions;
    uint16_t num_answers;
    uint16_t num_authority;
    uint16_t num_additional;
} Fixed_DNS_Header_t;

typedef struct Inputs {
    char* hostname_ip_lookup;
    char* dns_server_ip;
} Inputs_t;

#pragma pack(pop)       // restores old packing


// helpers
static char* get_char(char paSub_url[MAX_DNS_LEN], const int8_t delimiter);
int32_t set_inputs(Inputs_t* pInputs, char* pLog_buffer, const char* pHost_IP, const char* pDNS_server);
int32_t err_check(int32_t aTest, const char* aMsg, const char* aFile, const char* aFunction, int32_t aLine_num);
void kill_pointer(void** ptr);
uint32_t null_strlen(const char* str);
int32_t print_usage(void);
void exit_process(void);
int32_t starts_with(const char* start_string, const char* in_string);
int32_t run_DNS(Inputs_t* pInputs, char* pLog_buffer);



