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




int32_t main(int32_t argc, char* argv[])
{
   // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF);

    int32_t status = SUCCESS;

    //if (isdigit(*pNum_threads) && atoi(pNum_threads) > 0)

    if (argc == VALID_NUM_ARGS)
        status = SUCCESS;
    else
        status = print_usage();

    return status;
}




