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
char* winsock_test(url_t* paUrl_struct, const char* paRequest, const int32_t aRequest_size);

int32_t main(int32_t argc, char* argv[] )
{

    if (argc == SINGLE_URL_INPUT-1)
    {
        print_usage();
        exit(1);
    }

    for (int i = 1; i < argc; i++)
    {
        int32_t request_size;
        url_t* url_struct = parse_url(argv[i]);
        char* request = create_get_request(url_struct, &request_size);
        
        char* html_response = winsock_test(url_struct, (const char*) request, request_size);

        if (html_response != NULL)
        {
            parse_response((const char*)html_response, url_struct);
            free(html_response);
        }
        
        printf("\n\n----------------------\n");

        
        free(request);
        free(url_struct);
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
    //winsock_test();

    printf("-----------------\n");
    
    
    return 0;
}
