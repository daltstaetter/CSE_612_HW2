

#include "pch.h"



int32_t print_usage(void)
{
    printf("Incorrect Usage:\n");
    return 0;
}


url_t* parse_url(uint8_t* paInput_url)
{
    url_t* pUrl_struct = (url_t*) malloc(sizeof(url_t));

    pUrl_struct->host = (uint8_t*) "www.tamu.edu";
    pUrl_struct->port = 80;
    pUrl_struct->path = (uint8_t*) "/";
    pUrl_struct->query = (uint8_t*) "/";
    pUrl_struct->fragment = NULL;


    return pUrl_struct;
}