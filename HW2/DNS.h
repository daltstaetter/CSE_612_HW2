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
#define BASE_10         10
#define ERR_TRUNCATION  -1
#define MAX_ATTEMPTS    3

// DNS query types 
#define DNS_A 1         // name -> IP (forward lookup)
#define DNS_NS 2        // name server 
#define DNS_CNAME 5     // canonical name 
#define DNS_PTR 12      // IP -> name (reverse lookup)
#define DNS_HINFO 13    // host info/SOA 
#define DNS_MX 15       // mail exchange 
#define DNS_AXFR 252    // request for zone transfer 
#define DNS_ANY 255     // all records 

// query classes 
#define DNS_INET 1

// flags - see bitfields
// ( Query_Resp[15] | opcode[14->11] | AuthAns[10]    | TruncResp[9] |
//      RecurDes[8] | RecurAvail[7]  | reserved[6->4] | result[3->0] )
#define DNS_QUERY       (0 << 15)       
#define DNS_RESPONSE    (1 << 15)       

#define DNS_STDQUERY    (0 << 11)       // opcode - 4 bits 

#define DNS_AA          (1 << 10)       // Authoritative answer
#define DNS_TC          (1 << 9)        // truncated
#define DNS_RD          (1 << 8)        // recursion desired 
#define DNS_RA          (1 << 7)        // recursion available 



#pragma pack(push,1)    // sets struct padding/alignment to 1 byte
typedef struct DNS_Answer_Header {
    uint16_t dns_type;
    uint16_t dns_class;
    uint32_t dns_ttl;
    uint16_t dns_length;

} DNS_Answer_Header_t;

typedef struct DNS_Query_Header {
    uint16_t qry_type;
    uint16_t qry_class;
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
int32_t null_strlen(const char* str);
int32_t print_usage(void);
void exit_process(void);
int32_t starts_with(const char* start_string, const char* in_string);
int32_t run_DNS(Inputs_t* pInputs, char* pLog_buffer);
int32_t create_packet(char** ppPacket, Inputs_t* pInputs, uint32_t* pPacket_size);
uint16_t set_query_type(Inputs_t* pInputs);

int32_t set_query_string(Inputs_t* pInputs, char* pQuery_str, uint32_t aHost_len);

int32_t send_query(char* pPacket, uint32_t aPacket_size);
int32_t append_to_log(const char* pAppend);
void print_log(const char* pLog_buffer);



