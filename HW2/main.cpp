/* HW1-1
 * main.cpp
 * CSCE 463 Sample Code
 * by Dmitri Loguinov
 *
 * Date: 8/29/2019
 * Modified by Dalton Altstaetter
 * with permission for HW1-1
 * CSE 612
 * Fall 2019
 */

#include "pch.h"
#include "main.h"
#include "globals.h"

char stack_overflow_auth[] = "\x00\xD1\x85\x00\x00\x01\x00\x04\x00\x04\x00\x00\x0D\x73\x74\x61\x63\x6B\x6F\x76\x65\x72\x66\x6C\x6F\x77\x03\x63\x6F\x6D\x00\x00\x01\x00\x01\xC0\x0C\x00\x01\x00\x01\x00\x00\x01\x2C\x00\x04\x97\x65\x01\x45\xC0\x0C\x00\x01\x00\x01\x00\x00\x01\x2C\x00\x04\x97\x65\x81\x45\xC0\x0C\x00\x01\x00\x01\x00\x00\x01\x2C\x00\x04\x97\x65\xC1\x45\xC0\x0C\x00\x01\x00\x01\x00\x00\x01\x2C\x00\x04\x97\x65\x41\x45\xC0\x0C\x00\x02\x00\x01\x00\x02\xA3\x00\x00\x17\x07\x6E\x73\x2D\x31\x30\x33\x33\x09\x61\x77\x73\x64\x6E\x73\x2D\x30\x31\x03\x6F\x72\x67\x00\xC0\x0C\x00\x02\x00\x01\x00\x02\xA3\x00\x00\x13\x06\x6E\x73\x2D\x33\x35\x38\x09\x61\x77\x73\x64\x6E\x73\x2D\x34\x34\xC0\x1A\xC0\x0C\x00\x02\x00\x01\x00\x02\xA3\x00\x00\x1C\x0B\x6E\x73\x2D\x63\x6C\x6F\x75\x64\x2D\x65\x31\x0D\x67\x6F\x6F\x67\x6C\x65\x64\x6F\x6D\x61\x69\x6E\x73\xC0\x1A\xC0\x0C\x00\x02\x00\x01\x00\x02\xA3\x00\x00\x0E\x0B\x6E\x73\x2D\x63\x6C\x6F\x75\x64\x2D\x65\x32\xC0\xBD";

//_CrtMemState s1, s2, s3, s4, s5, s6, s7, s8, s9, s10;

extern char* gLog_buffer;
extern uint32_t gLog_buffer_size;

int32_t main(int32_t argc, char* argv[])
{
   // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF);

    int32_t status = SUCCESS;
    char recv_buff[MAX_DNS_LEN] = { 0 };
    if (argc != VALID_NUM_ARGS || strlen(argv[1]) <= 0 || strlen(argv[2]) <= 0 || inet_addr(argv[2]) == INADDR_NONE)
        return print_usage();

    Inputs_t inputs;

    if (set_inputs(&inputs, argv[1], argv[2]) != SUCCESS)
        return terminate_safely(&inputs);
        
    // TODO: print out the keys values/inputs
    status = run_DNS_Lookup(&inputs, recv_buff);

    if (status == SUCCESS)
        status = parse_DNS_response(&inputs, recv_buff);
    append_to_log("\n");
    print_log();
    
    if (status != SUCCESS)
        return FAIL;
    
   for (int i = 0; i < inputs.bytes_recv; i++)
    {
        if (i == 6)
            printf("\n");
        else if ((i-6) % (16) == 0 && i)
            printf("\n");
        else if ((i-6) % (8) == 0)
            printf("\t");

        //if((recv_buff[i] < 'z' && recv_buff[i] > 'a') || (recv_buff[i] < 'Z' && recv_buff[i] > 'A') || recv_buff[i] == '.')
        //    printf(" %c ", recv_buff[i]);
        //else
            printf("%02X ", (uint8_t)(recv_buff[i]));
        
    }

    //printf("\nnum_bytes = %d\n\n", inputs.bytes_recv);
    //for (int i = 2; i < inputs.bytes_recv; i++)
    //{
    //    //printf("\\x%02X", (uint8_t)(recv_buff[i]));
    //    if (recv_buff[i] != stack_overflow_auth[i])
    //        printf("buff cmpr issue\n");
    //}
    
    terminate_safely(&inputs);

    return status;
}

int32_t terminate_safely(Inputs_t* pInputs)
{
    kill_pointer((void**) &gLog_buffer);
    kill_pointer((void**) &pInputs->hostname_ip_lookup);
    kill_pointer((void**) &pInputs->dns_server_ip);
    kill_pointer((void**) &pInputs->query_string);

    return FAIL;
}




