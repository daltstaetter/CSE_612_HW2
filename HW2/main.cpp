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
    if (argc != VALID_NUM_ARGS || strlen(argv[1]) <= 0 || strlen(argv[2]) <= 0)
        return print_usage();

    Inputs_t inputs;

    if (err_check((gLog_buffer = (char*)malloc(sizeof(char) * gLog_buffer_size)) == NULL, "malloc() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(&inputs);

    if (set_inputs(&inputs, argv[1], argv[2]) != SUCCESS)
        return terminate_safely(&inputs);
        
    //if (isdigit(*pNum_threads) && atoi(pNum_threads) > 0)

    if (argc == VALID_NUM_ARGS)
        status = SUCCESS;
    else
        status = print_usage();

    terminate_safely(&inputs);

    return status;
}

int32_t terminate_safely(Inputs_t* pInputs)
{
    kill_pointer((void**) &gLog_buffer);
    kill_pointer((void**) &pInputs->hostname_ip_lookup);
    kill_pointer((void**) &pInputs->dns_server_ip);

    return FAIL;
}




