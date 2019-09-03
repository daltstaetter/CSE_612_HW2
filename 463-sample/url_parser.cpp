

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

void remove_scheme(char* paIn_url, uint16_t max_len)
{
    char* pTemp_str;
    errno_t status;
    
    pTemp_str = strstr(paIn_url, "http://");
    err_check(pTemp_str == NULL, "strstr()", __FUNCTION__, __LINE__ - 1);

    status = strcpy_s(paIn_url, max_len, (pTemp_str + strlen("http://")));
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);
    return;
}

char* set_hostname(char* paSub_url, char* paIn_url)
{
    // char_search point to an entry within paIn_url
    // if no match return null char location in string, i.e. end of string
    char* host_end = get_char(paIn_url, '#');

    if (host_end > get_char(paIn_url, '?'))
        host_end = get_char(paIn_url, '?');

    if (host_end > get_char(paIn_url, '/'))
        host_end = get_char(paIn_url, '/');

    if (host_end > get_char(paIn_url, ':'))
        host_end = get_char(paIn_url, ':');


    // Keep the latter substring for additional parsing
    errno_t status = strcpy_s(paSub_url, MAX_HOST_LEN * sizeof(char), (const char*) host_end);
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

    // null-term the match w/in pUrl_struct->host
    *host_end = 0;

    // points to remaining substring
    return paSub_url;
}

int32_t print_usage(void)
{
    // TODO: Print the correct usage info I want
    printf("\nUsage:\thw1.exe scheme://host[:port][/path][?query][#fragment]\n");
    return 0;
}

/*
*
*   Goal: Find substring index in  null-terminated string paSub_url
*         -if no match (search returns NULL), return location of null char
*/
char* get_char(char* paSub_url, const int8_t delimiter)
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
    char* pSub_str;
    char* host_end;

    // TODO check that malloc succeeded
    url_t* pUrl_struct = (url_t*) malloc(sizeof(url_t));
    err_check(pUrl_struct == NULL, "pUrl_struct malloc()", __FUNCTION__, __LINE__ - 1);

#ifdef DEBUG
//    printf("argv[1]:\t%s\n", paInput_url);
#endif // DEBUG

    errno_t status = strcpy_s(pIn_url, MAX_HOST_LEN*sizeof(char), (const char*)paInput_url);
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

    // Removes http:// from URL
    remove_scheme(pIn_url, MAX_HOST_LEN * sizeof(char));

#ifdef DEBUG
    printf("No scheme:\t%s\n", pIn_url);
#endif // DEBUG

    //-------------------------------------------------------------
    // Get host/IP
    // To get host/IP find the first occurence of any of [:, /, ?, #]
    //-------------------------------------------------------------
    status = strcpy_s(pUrl_struct->host, MAX_HOST_LEN * sizeof(char),(const char*)pIn_url);
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

    // char_search point to an entry within pUrl_struct->host
    // if no match return null char location in string
    host_end = find_hostname_end(pUrl_struct->host);

    // Keep the latter substring for additional parsing
    status = strcpy_s(pIn_url, MAX_HOST_LEN * sizeof(char), (const char*)host_end);
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

    // null-term the match w/in pUrl_struct->host
    *host_end = 0;

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



