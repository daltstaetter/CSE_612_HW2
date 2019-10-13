/* HW1-1
 * DNS.cpp: This parses the input URL to create the GET request
 *
 * Date: 8/29/2019
 * Author: Dalton Altstaetter
 * CSE 612
 * Fall 2019
 */

#include "pch.h"
//extern _CrtMemState s1, s2, s3, s4, s5, s6, s7, s8, s9, s10;

extern char* gLog_buffer;
extern uint32_t gLog_buffer_size;

void exit_process()
{
#ifndef NO_QUIT
    exit(1);
#endif // NO_QUIT
}

int32_t starts_with(const char* start_string, const char* in_string)
{
    return !strncmp(start_string, in_string, strlen(start_string));
}

/*
 * Input: aTest
 * if aTest evaluates to true then log an error
 */
int32_t err_check(int32_t aTest, const char* aMsg, const char* aFile, const char* aFunction, int32_t aLine_num)
{
    if (aFile == NULL || aFunction == NULL)
        return SUCCESS;

    const char* fname = ((char*)aFile - 1);
    const char* exit_test;
    char* aFile_end = (char*)aFile + strlen(aFile) - 1;

    if (aTest)
    {
        do
        {
            fname = get_char((char*)fname + 1, '.');
            exit_test = get_char((char*)fname + 1, '.');
        } while (*exit_test != 0 && fname < aFile_end);

        while (*(fname - 1) != '\\' && fname >= aFile)
            fname--;

        if (fname < aFile || fname > aFile_end)
            fname = aFile; // something went wrong
        
        printf("\nERROR:%s:%u:%s", fname, aLine_num, aMsg);
            return FAIL;
    }
    return SUCCESS;
}

/*
 * kill_pointer:
        - free pointer memory if not NULL
        - NULLS the pointer
*/
void kill_pointer(void** ptrptr)
{
    if (*ptrptr)
    {
        free(*ptrptr);
        *ptrptr = NULL;
    }
}

uint32_t null_strlen(const char* str)
{
    return (uint32_t)(strlen(str) + 1);
}

// ERROR CHECK COMPLETED
int32_t print_usage(void)
{
    printf("\nUsage:\t1)  hw2.exe [hostname | IPADDR] DNS_Server_IP\n\n");
    printf("      \tForward Lookup (host->IP): hw2.exe www.cnn.com 8.8.8.8\n"); // exit_process();
    printf("      \tReverse Lookup (IP->host): hw2.exe 72.146.82.6 128.194.135.84\n"); // exit_process();

    return FAIL;
}

/*
*
*   Goal: Find substring index in  null-terminated string paSub_url
*         -if no match (search returns NULL), return location of null char
*/
static char* get_char(char paSub_url[MAX_DNS_LEN], const int8_t delimiter)
{
    char* char_index = NULL;
    char_index = strchr(paSub_url, delimiter);

    if (char_index == NULL) // if missing return the full string
        return paSub_url + strlen(paSub_url);
    else
        return char_index;
}

int32_t set_port(char paSub_url[MAX_DNS_LEN], uint16_t* pPort)
{
    uint32_t port; // used to check if port w/in range 1-65535
    if (isdigit(paSub_url[1]))
    {
        // keep in host byte order
        port = (uint32_t)atoi(&paSub_url[1]); // start
                
        *pPort = (uint16_t)port;
                
        // move along the string & find first non-numeric index
        uint32_t i = 1;
        while (isdigit(paSub_url[i]))
            i++;
                
        if (err_check((strcpy_s(paSub_url, MAX_DNS_LEN * sizeof(char), paSub_url + sizeof(char) * i)) != SUCCESS,
            "strcpy()", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        {
            return FAIL;
        }
    }
    
    return SUCCESS;
}

int32_t set_inputs(Inputs_t* pInputs, char* pLog_buffer, const char* pHost_IP, const char* pDNS_server)
{
    assert(strlen(pHost_IP) > 0);
    assert(strlen(pDNS_server) > 0);

    if (err_check((pLog_buffer = (char*)calloc(gLog_buffer_size, sizeof(char))) == NULL, "malloc() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(pInputs);

    if (err_check((pInputs->hostname_ip_lookup = (char*)calloc(null_strlen(pHost_IP), sizeof(char))) == NULL, "malloc() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(pInputs);

    if (err_check((pInputs->dns_server_ip = (char*)calloc(null_strlen(pDNS_server), sizeof(char))) == NULL, "malloc() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(pInputs);

    if (err_check((strcpy_s(pInputs->hostname_ip_lookup, null_strlen(pHost_IP), pHost_IP)) != SUCCESS, "strcpy() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(pInputs);

    if (err_check((strcpy_s(pInputs->dns_server_ip, null_strlen(pDNS_server), pDNS_server)) != SUCCESS, "strcpy() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(pInputs);

    return SUCCESS;
}

int32_t run_DNS(Inputs_t* pInputs, char* pLog_buffer)
{
    ;
    DNS_Answer_Header_t* dns_answer_hdr;
    
    char* pkt;
    //   DNS Transmit pkt
    //  __________________
    // | Fixed DNS HEADER | 12-bytes
    // |__________________|
    // |    Questions     | var_length
    // |__________________|
    // |     Answers      | var_length
    // |__________________|
    // |    Authority     | var_length
    // |__________________|
    // |   Extra ResRec   | var_length
    // |__________________|

    //     Subcomponents
    //  ------------------
    // |  TXID  |  FLAGS  | 4-bytes
    // | nQuest | nAnsRR  | 4-bytes
    // | nAuthRR| nXtraRR | 4-bytes
    // |------------------|
    // | host_IP_qry_str  | len = strlen(host_IP) + 2 (for first count and Null char)
    // | qType  | qClass  | 4-bytes
    //  ------------------
    
    // ex: "www.yahoo.com" -> qry_str = "3www5yahoo3com"
    uint32_t host_len = strlen(pInputs->hostname_ip_lookup) + 2;
    uint32_t pkt_size = sizeof(Fixed_DNS_Header_t) + host_len + sizeof(DNS_Query_Header_t);
    
    if (err_check((pkt = (char*) calloc(pkt_size, sizeof(char))) == NULL, "malloc() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return FAIL;

    
    Fixed_DNS_Header_t* dns_fixed_hdr = (Fixed_DNS_Header_t*) pkt;
    DNS_Query_Header_t* dns_query_hdr = (DNS_Query_Header_t*) (pkt + pkt_size - sizeof(DNS_Query_Header_t));
    char* dns_query_str = pkt + sizeof(Fixed_DNS_Header_t);
    
    // fixed field initialization
    dns_fixed_hdr->flags = htons(DNS_QUERY | DNS_RD | DNS_STDQUERY);
    dns_fixed_hdr->num_questions = htons(1);
    dns_fixed_hdr->num_answers = 0;
    dns_fixed_hdr->num_authority = 0;
    dns_fixed_hdr->num_additional = 0;

    set_query_string(pInputs, dns_query_str, host_len);
    dns_query_hdr->qry_type = htons(set_query_type(pInputs));
    dns_query_hdr->qry_class = htons(DNS_INET);

    //DNS_Query_Header_t

    return SUCCESS;
}

uint16_t set_query_type(Inputs_t* pInputs)
{
    unsigned long IP = inet_addr(pInputs->hostname_ip_lookup);

    if (IP == INADDR_NONE)  // we were given a hostname
        return DNS_A;       // forward lookup
    else
        return DNS_PTR;     // reverse lookup
}

int32_t set_query_string(Inputs_t* pInputs, char* pQuery_str, uint32_t aHost_len)
{
    char* next_token = NULL;
    char* current_token = NULL;
    //char* tmp_str = (char*)calloc(null_strlen(pInputs->hostname_ip_lookup), sizeof(char));
    //if(err_check(strcpy_s(&pQuery_str[1], aHost_len, pInputs->hostname_ip_lookup) != SUCCESS, "strcpy_s fail", __FILE__, __FUNCTION__, __LINE__))
    if (err_check((strcpy_s(&pQuery_str[1], aHost_len, pInputs->hostname_ip_lookup)) != SUCCESS, "strcpy() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return FAIL;

    //next_token = &pQuery_str[1];
    //current_token = strtok_s(next_token, ".", &next_token);
    //if (current_token - 1 >= pQuery_str && current_token < (pQuery_str + aHost_len - 1))
    //    _snprintf_s(&current_token[-1], null_strlen(current_token) + 1, _TRUNCATE, "%u%s", strlen(current_token), current_token);
    ////_snprintf_s(pQuery_str, null_strlen(current_token)+1, _TRUNCATE, "%u%s", strlen(current_token), current_token);

    //
    //
    //current_token = strtok_s(next_token, ".", &next_token);
    //if (current_token - 1 >= pQuery_str && current_token < (pQuery_str + aHost_len - 1))
    //    _snprintf_s(&current_token[-1], null_strlen(current_token) + 1, _TRUNCATE, "%u%s", strlen(current_token), current_token);

    //current_token = strtok_s(next_token, ".", &next_token);
    //if (current_token - 1 >= pQuery_str && current_token < (pQuery_str + aHost_len - 1))
    //    _snprintf_s(&current_token[-1], null_strlen(current_token) + 1, _TRUNCATE, "%u%s", strlen(current_token), current_token);
    //
    int32_t bytes_written = 0;
    for (next_token = &pQuery_str[1]; next_token[0] != NULL; )
    {
        char* current_token = strtok_s(next_token, ".", &next_token);

        // Very tricky bug discovered. if current_token is NULL and you test current_token - 1 >= pQuery_str
        // then current_token-1 wraps around and is >= pQuery_str and executes. By moving 1 to query string you avoid
        // the underflow error. Key design idea: Don't do math on ptrs more likely to be null or which change more frequently
        if (current_token >= (pQuery_str + 1) && current_token < (pQuery_str + aHost_len - 1))
        {
            bytes_written = _snprintf_s(&current_token[-1], null_strlen(current_token) + 1, _TRUNCATE, "%u%s", strlen(current_token), current_token);

            if (err_check(bytes_written >= null_strlen(current_token) + 1 || bytes_written == ERR_TRUNCATION, "_snprintf_s() exceeded buffer size", __FILE__, __FUNCTION__, __LINE__ - 1) != SUCCESS)
                return FAIL;
        }
    }

}


