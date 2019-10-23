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

extern char* gLog_buffer;
extern uint32_t gLog_buffer_size;
extern int32_t gNum_jumps;
extern int32_t gMax_num_jumps;

#define SUCCESS         0
#define FAIL            1
#define MAX_DNS_LEN     512
#define MAX_HOST_LEN    256
#define BASE_10         10
#define ERR_TRUNCATION  -1
#define MAX_ATTEMPTS    3
#define LOG_LINE_SIZE   256
#define TYPE_LEN        8
#define MAX_DNS_A_LEN   16    // strlen("255.255.255.255") + 1

#define DNS_PORT        53

// DNS query types 
#define DNS_A           1       // name -> IPv4 (forward lookup)
#define DNS_NS          2       // name server 
#define DNS_CNAME       5       // canonical name 
#define DNS_PTR         12      // IP -> name (reverse lookup)
#define DNS_HINFO       13      // host info/SOA 
#define DNS_MX          15      // mail exchange 
#define DNS_AAAA        28      // name -> IPv6 (forward lookup)
#define DNS_AXFR        252     // request for zone transfer 
#define DNS_ANY         255     // all records 

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

#define COMPRESSION_MASK    0xC0
#define RCODE_MASK          0x000F


#pragma pack(push,1)    // sets struct padding/alignment to 1 byte
typedef struct DNS_AnswerRR_Header {
    uint16_t    dns_type;
    uint16_t    dns_class;
    uint32_t    dns_ttl;
    uint16_t    rd_length;
} DNS_AnswerRR_Header_t;

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
    uint16_t dns_type;
    uint16_t tx_id;
    uint16_t dns_pkt_size;
    int16_t bytes_recv;
    uint16_t num_questions;
    char* query_string;

} Inputs_t;

#pragma pack(pop)       // restores old packing

// main public functions
int32_t set_inputs(Inputs_t* pInputs, const char* pHost_IP, const char* pDNS_server);
int32_t run_DNS_Lookup(Inputs_t* pInputs, char* pRecv_buff);
int32_t parse_DNS_response(Inputs_t* pInputs, char* pRecv_buff);

static int32_t parse_ResourceRecord(Inputs_t* pInputs, char* pRecv_buff, unsigned char** ppAnswerRR_name, uint16_t count, const char* pSection);

static int32_t parse_query_name(Inputs_t* pInputs, char* pRecv_buff, char* start_string, char** output_string, char* temp_buff);

static int32_t get_compressed_field(Inputs_t* pInputs, char* pRecv_buff, uint16_t byte_offset, char* pField, uint16_t* pData_length);

// auxillary functions
int32_t null_strlen(const char* str);
int32_t print_usage(void);
int32_t append_to_log(const char* pAppend);
void print_log();
void kill_pointer(void** ptr);

// helpers
int32_t err_check(int32_t aTest, const char* aMsg, const char* aFile, const char* aFunction, int32_t aLine_num);
void exit_process(void);
static int32_t starts_with(const char* start_string, const char* in_string);
static char* get_char(char paSub_url[MAX_DNS_LEN], const int8_t delimiter);

// private/local functions
static char* create_packet(Inputs_t* pInputs);
static int32_t set_query_string(Inputs_t* pInputs, char* pQuery_str, uint32_t aHost_len);
static int32_t send_query_and_get_response(Inputs_t* pInput, char* pPacket, char* pRecv_buff);
static int32_t recurse_string_for_commas(char* pIn_string, int32_t strlen);
static int32_t query_to_host_string(Inputs_t* pInputs, char* pQuery_str);
static int32_t get_record_type(DNS_AnswerRR_Header_t* pAnswerRR, char* val, uint32_t length);
static uint16_t set_query_type(Inputs_t* pInputs);
static int32_t update_jumps();
static int32_t check_response_string(Inputs_t* pInputs, char* pRecv_buff);
static int32_t getA_data(Inputs_t* pInputs, char* pRecv_buff, char* pData, char* pRecord_data, uint16_t aData_length);
static int32_t getNS_data(Inputs_t* pInputs, char* pRecv_buff, char* pData, char* pRecord_data, uint16_t aData_length);
static int32_t getCNAME_data(Inputs_t* pInputs, char* pRecv_buff, char* pData, char* pRecord_data, uint16_t aData_length);
static int32_t getPTR_data(Inputs_t* pInputs, char* pRecv_buff, char* pData, char* pRecord_data, uint16_t aData_length);

static int32_t check_num_records(uint16_t numA_CNAME, uint16_t numNS);



