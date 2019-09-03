

#include "pch.h"
#include "url_parser.h"
#define DEBUG
#define FORMAT_SIZE 512
#define HTTP_PORT   80


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

    if (pTemp_str == NULL)
    {
        print_usage();
        exit(1);
    }

    status = strcpy_s(paIn_url, max_len, (pTemp_str + strlen("http://")));
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);
    return;
}

char* set_hostname(char* paSub_url, char* paHostname)
{
    errno_t status = strcpy_s(paHostname, MAX_HOST_LEN * sizeof(char), (const char*)paSub_url);
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);


    // char_search point to an entry within paIn_url
    // if no match return null char location in string, i.e. end of string
    char* host_end = get_char(paHostname, '#');

    if (host_end > get_char(paHostname, '?'))
        host_end = get_char(paHostname, '?');

    if (host_end > get_char(paHostname, '/'))
        host_end = get_char(paHostname, '/');

    if (host_end > get_char(paHostname, ':'))
        host_end = get_char(paHostname, ':');


    // Keep the latter substring for additional parsing
    status = strcpy_s(paSub_url, MAX_HOST_LEN * sizeof(char), (const char*) host_end);
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
static char* get_char(char* paSub_url, const int8_t delimiter)
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
*  
*        
*/
errno_t set_port(char* paSub_url, uint16_t* pPort)
{
    if (':' == paSub_url[0])
    {
        if (strlen(paSub_url) >= 2)
        {
            if (isdigit(paSub_url[1]))
            {
                // keep in host byte order
                *pPort = atoi(&paSub_url[1]); // start
                
                // move along the string
                uint32_t i = 1;
                while (isdigit(paSub_url[i]))
                {
                    i++;
                }
                //paSub_url = &paSub_url[i];
                errno_t status = strcpy_s(paSub_url, MAX_HOST_LEN * sizeof(char), paSub_url + sizeof(char)*i);
                err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

                return SUCCESS;
            }
        }
    }

    return -1;
}

// pSub_str = set_path(pSub_str, pUrl_struct->path);
char* set_path(char* paSub_url, char* pPath)
{
    errno_t status = strcpy_s(pPath, MAX_HOST_LEN * sizeof(char), (const char*) paSub_url);
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

    char* char_search = get_char(pPath, '#');

    if (char_search > get_char(pPath, '?'))
        char_search = get_char(pPath, '?');

    // Keep the latter substring for additional parsing
    status = strcpy_s(paSub_url, MAX_HOST_LEN * sizeof(char), (const char*) char_search);
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

    // null-term the match w/in pPath
    *char_search = 0;

    // points to remaining substring
    return paSub_url;
}

char* set_query(char* paSub_url, char* pQuery)
{
    errno_t status = strcpy_s(pQuery, MAX_HOST_LEN * sizeof(char), (const char*) paSub_url);
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

    char* char_search = get_char(pQuery, '#');

    // Keep the latter substring for additional parsing
    status = strcpy_s(paSub_url, MAX_HOST_LEN * sizeof(char), (const char*) char_search);
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

    // null-term the match w/in pQuery
    *char_search = 0;

    // points to remaining substring
    return paSub_url;
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

    url_t* pUrl_struct = (url_t*) malloc(sizeof(url_t));
    err_check(pUrl_struct == NULL, "pUrl_struct malloc()", __FUNCTION__, __LINE__ - 1);

    // Set default values in case we return early
    pUrl_struct->port = HTTP_PORT; // assume port 80 if not specified
    
    errno_t status = strcpy_s(pUrl_struct->path, MAX_REQUEST_LEN * sizeof(char), "/");
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

    pUrl_struct->query[0] = 0;
    pUrl_struct->fragment[0] = 0;
#if 0
    printf("%s\n", pUrl_struct->path);
    printf("%d\n", pUrl_struct->port);
    printf("%s\n", pUrl_struct->query);
    printf("%s\n", pUrl_struct->fragment);
#endif

#ifdef DEBUG
//    printf("argv[1]:\t%s\n", paInput_url);
#endif // DEBUG

    status = strcpy_s(pIn_url, MAX_HOST_LEN*sizeof(char), (const char*)paInput_url);
    err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

    // Removes http:// from URL
    remove_scheme(pIn_url, MAX_HOST_LEN * sizeof(char));

#ifdef DEBUG
    printf("No scheme:\t%s\n", pIn_url);
#endif // DEBUG

    //-------------------------------------------------------------
    // Set host/IP
    // To get host/IP find the first occurence of any of [:, /, ?, #]
    //-------------------------------------------------------------
    set_hostname(pIn_url, pUrl_struct->host);

#ifdef DEBUG
    printf("host/IP:\t%s\n", pUrl_struct->host);
#endif // DEBUG

    if (strlen(pUrl_struct->host) == 0)
    {
        printf("Invalid host/IP\n");
        print_usage();
    }
    if (strlen(pIn_url) == 0) // nothing left to parse
        return pUrl_struct;

    // ----------------------------------------------
    // Parse the port
    // ----------------------------------------------
    if (pIn_url[0] == ':')
    {
        if( set_port(pIn_url, &(pUrl_struct->port)) == SUCCESS )
            printf("port:\t\t0x%X, %d\n", pUrl_struct->port, pUrl_struct->port);
    }
    if (strlen(pIn_url) == 0) // nothing left to parse
        return pUrl_struct;

    // ----------------------------------------------
    // Parse the path: could be explicit or omitted (set to '/')
    // ----------------------------------------------
    if (pIn_url[0] == '/')
    {
        set_path(pIn_url, pUrl_struct->path);
        printf("path:\t\t%s\n", pUrl_struct->path);
    }
    else if (pIn_url[0] == '?' || pIn_url[0] == '#') // path omitted
    {
        status = strcpy_s(pUrl_struct->path, MAX_REQUEST_LEN * sizeof(char), "/");
        err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

        printf("path:\t\t%s\n", pUrl_struct->path);
    }
    else
    {
#ifndef DEBUG
        err_check(true, "malformed URL", __FUNCTION__, __LINE__);
#else
        printf("malformed URL\nExiting...\n");
        printf("substring\t%s\n", pIn_url);
#endif // !DEBUG
    }
    if (strlen(pIn_url) == 0) // nothing left to parse
        return pUrl_struct;

    // ----------------------------------------------
    // Parse the query: could be explicit or omitted (set to '/')
    // ----------------------------------------------
    if (pIn_url[0] == '?')
    {
        set_query(pIn_url, pUrl_struct->query);
        printf("query:\t\t%s\n", pUrl_struct->query);
    }
    if (strlen(pIn_url) == 0) // nothing left to parse
        return pUrl_struct;

    // ----------------------------------------------
    // Parse the fragment: Not used
    // ----------------------------------------------
    if (pIn_url[0] == '#') // path omitted
    {
        status = strcpy_s(pUrl_struct->fragment, FRAG_SIZE, pIn_url);
        err_check(status != SUCCESS, "strcpy_s()", __FUNCTION__, __LINE__ - 1);
        printf("fragment:\t%s\n", pUrl_struct->fragment);
    }

    return pUrl_struct;
}

//TODO: Free every thing i did a malloc for
char* create_get_request(url_t* paUrl_struct, int32_t* paBytes_written)
{
    char* request = (char*)malloc(MAX_REQUEST_LEN * sizeof(char));
    char format_str[FORMAT_SIZE];

    char HTTP_ver[] = "HTTP/1.0";

    errno_t status = strcpy_s(format_str, FORMAT_SIZE, "GET %s%s %s\r\nUser-agent: myAgent/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n");
    err_check(status != SUCCESS, "strcpy_s()", __FUNCTION__, __LINE__ - 1);

    errno_t bytes_written = _snprintf_s(request,
                                         MAX_REQUEST_LEN * sizeof(char),
                                         _TRUNCATE,
                                         format_str,
                                         paUrl_struct->path, paUrl_struct->query, HTTP_ver, paUrl_struct->host
                                         );

    err_check(bytes_written >= FORMAT_SIZE || bytes_written == -1, "_snprintf_s", __FUNCTION__, __LINE__ - 1);
    *paBytes_written = (int32_t) bytes_written + 1; // add null char
#ifdef DEBUG
    printf("%s\n", request);
#endif // DEBUG

    return request;
}



// parse_links(response
void parse_links(const char* paHtml_response, char* apBase_url)
{
    // create new parser object
    HTMLParserBase* parser = new HTMLParserBase;

    // char baseUrl[] = "http://www.tamu.edu";		// where this page came from; needed for construction of relative links
    uint32_t len = strlen(paHtml_response);
    err_check(len == NULL, "strlen()", __FUNCTION__, __LINE__ - 1);

    int nLinks;
    char* linkBuffer = parser->Parse((char*)paHtml_response, len, apBase_url, (int)strlen(apBase_url), &nLinks);

    // check for errors indicated by negative values
    if (nLinks < 0)
        nLinks = 0;

    printf("Found %d links:\n", nLinks);

    // print each URL; these are NULL-separated C strings
    for (int i = 0; i < nLinks; i++)
    {
        printf("%s\n", linkBuffer);
        linkBuffer += strlen(linkBuffer) + 1;
    }

    delete parser;		// this internally deletes linkBuffer
}

int32_t parse_response(const char* paResponse, url_t* paUrl_struct)
{
    char* pHeader;
    char* pBody;
    char* pIs_valid;
    errno_t status;
    char base_url[MAX_HOST_LEN];
   
    // need to split response into header and body
    // look for /r/n/r/n

    pHeader = strstr((char*)paResponse, "\r\n\r\n<html>");
    pBody = pHeader + strlen("\r\n\r\n");

    *pHeader = 0;
    pHeader = (char*) paResponse;

    printf("%s\n", pHeader);
    
    if ( strstr(pHeader, "HTTP/1.0 2") != NULL || 
         strstr(pHeader, "HTTP/1.1 2") != NULL )
    {   // valid response 
        // print response
        // search links
        // print num links
        // print time
    
    }
    else //if (strstr(pHeader, "HTTP/1.0 4") != NULL || strstr(pHeader, "HTTP/1.1 4") != NULL)
    {   // invalid response
        // print response
        // omit search links
        // print time
    }

    
    //if (pIs_valid != NULL)
    {
        status = strcpy_s(base_url, MAX_HOST_LEN*sizeof(char), paUrl_struct->host);
        err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

        status = strcat_s(base_url, MAX_HOST_LEN * sizeof(char), paUrl_struct->path);
        err_check(status != SUCCESS, "strcpy()", __FUNCTION__, __LINE__ - 1);

        // run the HTML parser 
        printf("base_url: %s\n", base_url);
        parse_links(paResponse, base_url);
        // print everything else
    }


    // print HTTP_response

    return 0;
}

