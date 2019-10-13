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
    printf("      \tEx: hw2.exe www.cnn.com 8.8.8.8\n"); // exit_process();
    printf("      \tEx: hw2.exe 72.146.82.6 128.194.135.84\n"); // exit_process();

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

int32_t set_inputs(Inputs_t* pInputs, const char* pHost_IP, const char* pDNS_server)
{
    assert(strlen(pHost_IP) > 0);
    assert(strlen(pDNS_server) > 0);

    if (err_check((pInputs->hostname_ip_lookup = (char*)malloc(sizeof(char) * null_strlen(pHost_IP))) == NULL, "malloc() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(pInputs);

    if (err_check((pInputs->dns_server_ip = (char*)malloc(sizeof(char) * null_strlen(pDNS_server))) == NULL, "malloc() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(pInputs);

    if (err_check((strcpy_s(pInputs->hostname_ip_lookup, null_strlen(pHost_IP), pHost_IP)) != SUCCESS, "strcpy() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(pInputs);

    if (err_check((strcpy_s(pInputs->dns_server_ip, null_strlen(pDNS_server), pDNS_server)) != SUCCESS, "strcpy() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(pInputs);

    return SUCCESS;
}


