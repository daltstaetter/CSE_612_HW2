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