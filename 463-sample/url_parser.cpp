

#include "pch.h"
#include "url_parser.h"
#define DEBUG

void err_check(BOOL aTest, const char* aMsg, const char* aFunction, int32_t aLine_num)
{
    if (aTest)
    {
        printf("%s:%d: Error in %s\n...Exiting\n", aFunction, aLine_num, aMsg);
        exit(1);
    }
}

int32_t print_usage(void)
{
    printf("Incorrect Usage:\n");
    return 0;
}

/*
*
*   Goal: Find substring index in  null-terminated string paSub_url
*         -if no match (search returns NULL), return location of null char
*/
char* find_hostname_end(char* paSub_url, const int8_t delimiter)
{
    char* char_index;
    char_index = strchr(paSub_url, delimiter);

    if (char_index == NULL) // if missing return the full string
        return paSub_url + strlen(paSub_url);
    else
        return char_index;
}

/*
*
*   Goal: Break URL in format scheme://host[:port][/path][?query][#fragment]
*         into its components, e.g. http://128.194.135.72:80/courses/index.asp#location
*/
url_t* parse_url(char* paInput_url)
{
    char pIn_url[MAX_HOST_LEN];
    char* pTemp_str;
    char* char_search;

    // TODO check that malloc succeeded
    url_t* pUrl_struct = (url_t*) malloc(sizeof(url_t));
    err_check(pUrl_struct == NULL, "pUrl_struct malloc()", __FUNCTION__, __LINE__ - 1);

#ifdef DEBUG
//    printf("argv[1]:\t%s\n", paInput_url);
#endif // DEBUG

    errno_t status = strcpy_s(pIn_url, (const char*)paInput_url);
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

    //-------------------------------------------------------------
    // Remove scheme
    //-------------------------------------------------------------
    pTemp_str = strstr(pIn_url, "http://");
    err_check(pTemp_str == NULL, "strstr()", __FUNCTION__, __LINE__ - 1);

    status = strcpy_s(pIn_url, (const char*) (pTemp_str + strlen("http://")));
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

#ifdef DEBUG
    printf("No scheme:\t%s\n", pIn_url);
#endif // DEBUG

    //-------------------------------------------------------------
    // Get host/IP
    // To get host/IP find the first occurence of any of [:, /, ?, #]
    //-------------------------------------------------------------
    status = strcpy_s(pUrl_struct->host, (const char*)pIn_url);
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

    // char_search point to an entry within pUrl_struct->host
    // if no match return null char location in string
    char_search = find_hostname_end(pUrl_struct->host, '#');

    if (char_search > find_hostname_end(pUrl_struct->host, '?'))
        char_search = find_hostname_end(pUrl_struct->host, '?');

    if (char_search > find_hostname_end(pUrl_struct->host, '/'))
        char_search = find_hostname_end(pUrl_struct->host, '/');

    if (char_search > find_hostname_end(pUrl_struct->host, ':'))
        char_search = find_hostname_end(pUrl_struct->host, ':');

    // null-term the match w/in pUrl_struct->host
    *char_search = 0; 


#ifdef DEBUG
    printf("host/IP:\t%s\n", pUrl_struct->host);
#endif // DEBUG
    

 /*   pUrl_struct->host = (char*) "www.tamu.edu";
    pUrl_struct->port = 11223;
    pUrl_struct->path = (char*) "/";
    pUrl_struct->query = (char*) "/";
    pUrl_struct->fragment = NULL;*/





    return pUrl_struct;
}



