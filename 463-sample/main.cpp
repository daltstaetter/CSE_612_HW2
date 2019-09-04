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
char* send_request(url_t* paUrl_struct, const char* paRequest, const int32_t aRequest_size);

int32_t main(int32_t argc, char* argv[] )
{
    if (argc == SINGLE_URL_INPUT)
    {
        int32_t request_size;

        printf("URL: %s\n", argv[1]);
        printf("\tParsing URL... ");
        url_t* url_struct = parse_url(argv[1]);
        printf("host %s, port %d, request %s%s\n", url_struct->host, url_struct->port, url_struct->path, url_struct->query);

        char* request = create_get_request(url_struct, &request_size);
        char* html_response = send_request(url_struct, (const char*)request, request_size);

        if (html_response)
        {
            parse_response((const char*)html_response, url_struct);
            free(html_response);
        }

        free(request);
        free(url_struct);
    }
    else
    {
        print_usage();
    }
    
    return 0;
}
