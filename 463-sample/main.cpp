/* main.cpp
 * CSCE 463 Sample Code
 * by Dmitri Loguinov
 *
 * 8/29/2019
 * Modified by Dalton Altstaetter
 * for HW1-1
 */
#include "pch.h"

#define SINGLE_URL_INPUT 2


// function inside winsock.cpp
void winsock_test(void);

int32_t main(int32_t argc, char* argv[] )
{

    for (int i = 1; i < argc; i++)
    {
        url_t* url_struct = parse_url(argv[i]);
        printf("\n\n----------------------\n");
    }
    return 0;

    argc = SINGLE_URL_INPUT;
    if (argc == SINGLE_URL_INPUT)
    {
        url_t* url_struct = parse_url(argv[1]);
        
        if (url_struct->status == INVALID_URL)
        {
            print_usage();
        }
        else
        {
            // do_something();
        }


        free(url_struct);
    }
    else
    {
        print_usage();
        return 0;
    }

    // connect to a server; test basic winsock functionality
    winsock_test();

    printf("-----------------\n");
    
    
    return 0;
}
