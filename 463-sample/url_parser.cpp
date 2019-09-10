/* HW1-1
 * url_parser.cpp: This parses the input URL to create the GET request
 *
 * Date: 8/29/2019
 * Author: Dalton Altstaetter
 */

#include "pch.h"
#include "url_parser.h"
#define FORMAT_SIZE 512
#define HTTP_PORT   80

void exit_process()
{
#ifndef NO_QUIT
    exit(1);
#endif // NO_QUIT
}

void err_check(int32_t aTest, const char* aMsg, const char* aFile, const char* aFunction, int32_t aLine_num)
{
    const char* fname = ((char*)aFile - 1);
    const char* exit_test;
    char* aFile_end = (char*)aFile + strlen(aFile);

    if (aTest)
    {
        do
        {
            fname = get_char((char*)fname + 1, '.');
            exit_test = get_char((char*)fname + 1, '.');
        } while (*exit_test != 0 && fname < aFile_end);

        while (*(fname - 1) != '\\' && fname >= aFile)
            fname--;

        if (fname < aFile || fname > aFile_end)
            fname = aFile; // something went wrong

        printf("%s:%s():%d: Error in %s...\n...Exiting\n", fname, aFunction, aLine_num, aMsg);
        exit_process();
    }
}

void remove_scheme(char paIn_url[MAX_URL_LEN])
{   // Removes 'http://' from URL
    char* pTemp_str;

    pTemp_str = strstr(paIn_url, "http://");

    if (pTemp_str) // it matched HTTP
        err_check((strcpy_s(paIn_url, MAX_URL_LEN *sizeof(char), (pTemp_str + strlen("http://")))) != SUCCESS, "strcpy()", __FILE__, __FUNCTION__, __LINE__);
    else
    {
        printf("failed with invalid scheme\n");
        exit_process();
    }
}

void print_usage(void)
{
    printf("\nUsage:\thw1-1.exe scheme://host[:port][/path][?query][#fragment]\n");
    exit_process();
}

void set_hostname(char paSub_url[MAX_URL_LEN], char* paHostname)
{
    // copy input url into paHostname
    char* host_end = get_char(paSub_url, '#');

    if (host_end > get_char(paSub_url, '?'))
        host_end = get_char(paSub_url, '?');

    if (host_end > get_char(paSub_url, '/'))
        host_end = get_char(paSub_url, '/');

    if (host_end > get_char(paSub_url, ':'))
        host_end = get_char(paSub_url, ':');

    if (host_end && (host_end - paSub_url) >= MAX_HOST_LEN*sizeof(char))
    {
        printf("hostname exceeds max host length\n");
        exit_process();
    }

    err_check((strcpy_s(paHostname, MAX_HOST_LEN * sizeof(char), (const char*)paSub_url)) != SUCCESS, "strcpy()", __FILE__, __FUNCTION__, __LINE__);

    // host_end points to where path ends in paHostname
    // if no match return null char location in string, i.e. end of string
    host_end = get_char(paHostname, '#');

    if (host_end > get_char(paHostname, '?'))
        host_end = get_char(paHostname, '?');

    if (host_end > get_char(paHostname, '/'))
        host_end = get_char(paHostname, '/');

    if (host_end > get_char(paHostname, ':'))
        host_end = get_char(paHostname, ':');

    // Keep the latter substring for additional parsing
    err_check((strcpy_s(paSub_url, MAX_HOST_LEN * sizeof(char), (const char*)host_end)) != SUCCESS, "strcpy()", __FILE__, __FUNCTION__, __LINE__);

    // null-term the match w/in pUrl_struct->host
    *host_end = 0;

    if (strlen(paHostname) == 0)
        print_usage();
}

void set_fragment(char paIn_url[MAX_URL_LEN], url_t* paUrl_struct)
{
    if (paIn_url[0] == '#') // path omitted
        err_check((strcpy_s(paUrl_struct->fragment, FRAG_SIZE, paIn_url)) != SUCCESS, "strcpy_s()", __FILE__, __FUNCTION__, __LINE__);
}

/*
*
*   Goal: Find substring index in  null-terminated string paSub_url
*         -if no match (search returns NULL), return location of null char
*/
char* get_char(char paSub_url[MAX_URL_LEN], const int8_t delimiter)
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
void set_port(char paSub_url[MAX_URL_LEN], uint16_t* pPort)
{
    uint32_t port; // used to check if port w/in range 1-65535

    // paSub_url should take form ':[port]'
    if (paSub_url[0] == ':')
    {
        if (strlen(paSub_url) >= 2)
        {
            if (isdigit(paSub_url[1]))
            {
                // keep in host byte order
                port = (uint32_t)atoi(&paSub_url[1]); // start
                
                // check port range to be 1 - 65535
                if (port == 0 || port > UINT16_MAX)
                {
                    printf("failed with invalid port\n");
                    exit_process();
                }
                *pPort = (uint16_t)port;
                
                // move along the string & find first non-numeric index
                uint32_t i = 1;
                while (isdigit(paSub_url[i]))
                    i++;
                
                err_check((strcpy_s(paSub_url, MAX_HOST_LEN * sizeof(char), paSub_url + sizeof(char) * i)) != SUCCESS,
                    "strcpy()", __FILE__, __FUNCTION__, __LINE__);
            }
            else if (paSub_url[1] == '-')
            {
                // check for negative numbers
                printf("failed with invalid port\n");
                exit_process();
            }
        }
    }
}

void set_path(char paSub_url[MAX_URL_LEN], char* pPath)
{
    if (paSub_url[0] == '/')
    {
        err_check((strcpy_s(pPath, MAX_HOST_LEN * sizeof(char), (const char*)paSub_url)) != SUCCESS,
                  "strcpy()", __FILE__, __FUNCTION__, __LINE__);

        char* char_search = get_char(pPath, '#');

        if (char_search > get_char(pPath, '?'))
            char_search = get_char(pPath, '?');

        // Keep the latter substring for additional parsing
        err_check((strcpy_s(paSub_url, MAX_HOST_LEN * sizeof(char), (const char*)char_search)) != SUCCESS,
                  "strcpy()", __FILE__, __FUNCTION__, __LINE__);

        // null-term the match w/in pPath
        *char_search = 0;
    }
    else if (paSub_url[0] == '?' || paSub_url[0] == '#' || paSub_url[0] == NULL) // path omitted, set to default value of '/'
        err_check((strcpy_s(pPath, MAX_REQUEST_LEN * sizeof(char), "/")) != SUCCESS, "strcpy()", __FILE__, __FUNCTION__, __LINE__);
    else // error: malformed input
        print_usage();
}

void set_query(char paSub_url[MAX_URL_LEN], char* pQuery)
{
    if (paSub_url[0] == '?')
    {
        err_check((strcpy_s(pQuery, MAX_HOST_LEN * sizeof(char), (const char*)paSub_url)) != SUCCESS,
            "strcpy()", __FILE__, __FUNCTION__, __LINE__);
        
        char* char_search = get_char(pQuery, '#');
        
        // Keep the latter substring for additional parsing
        err_check((strcpy_s(paSub_url, MAX_HOST_LEN * sizeof(char), (const char*)char_search)) != SUCCESS,
            "strcpy()", __FILE__, __FUNCTION__, __LINE__);
        
        // null-term the match w/in pQuery
        *char_search = 0;
    }

}

void set_default_params(url_t* paUrl_struct, char  paIn_url[MAX_URL_LEN], char* paInput_url)
{
    paUrl_struct->port = HTTP_PORT; // assume port 80 if not specified
    
    err_check((strcpy_s(paUrl_struct->path, MAX_URL_LEN * sizeof(char), "/")) != SUCCESS,
        "strcpy()", __FILE__, __FUNCTION__, __LINE__);
    
    paUrl_struct->query[0] = 0;
    paUrl_struct->fragment[0] = 0;
    
    err_check((strcpy_s(paIn_url, MAX_URL_LEN * sizeof(char), (const char*)paInput_url)) != SUCCESS,
        "strcpy()", __FILE__, __FUNCTION__, __LINE__);
}

/*
*
*   Goal: Break URL in format scheme://host[:port][/path][?query][#fragment]
*         into its components, e.g. http://128.194.135.72:80/courses/index.asp#location
*/
url_t* parse_url(char* paInput_url)
{
    if (strlen(paInput_url) >= MAX_URL_LEN)
    {
        printf("url exceeds max url length\n");
        exit_process();
    }

    url_t* pUrl_struct = (url_t*) malloc(sizeof(url_t));
    err_check(pUrl_struct == NULL, "pUrl_struct malloc()", __FILE__, __FUNCTION__, __LINE__ - 1);

    char pIn_url[MAX_URL_LEN];

    // Set default values in case parameters are omitted
    // fails if strlen(paInput_url) >= 256
    set_default_params(pUrl_struct, pIn_url, paInput_url);

    // Removes 'http://' from URL
    remove_scheme(pIn_url);

    //-------------------------------------------------------------
    // Set host/IP
    // To get host/IP find the first occurence of any of [:, /, ?, #]
    //-------------------------------------------------------------
    set_hostname(pIn_url, pUrl_struct->host);

    set_port(pIn_url, &(pUrl_struct->port));

    // ----------------------------------------------
    // Parse the path: could be explicit or omitted (set to '/')
    // ----------------------------------------------
    set_path(pIn_url, pUrl_struct->path);

    // ----------------------------------------------
    // Parse the query: could be explicit or omitted
    // ----------------------------------------------
    set_query(pIn_url, pUrl_struct->query);

    // ----------------------------------------------
    // Parse the fragment: Not used - set to NULL
    // ----------------------------------------------
    set_fragment(pIn_url, pUrl_struct);

    return pUrl_struct;
}



//TODO: Free every thing i did a malloc for
char* create_get_request(url_t* paUrl_struct, int32_t* paBytes_written)
{
    char* request = (char*)malloc(MAX_REQUEST_LEN * sizeof(char));
    char format_str[FORMAT_SIZE];

    char HTTP_ver[] = "HTTP/1.0";

    errno_t status = strcpy_s(format_str, FORMAT_SIZE, "GET %s%s %s\r\nUser-agent: myAgent/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n");
    err_check(status != SUCCESS, "strcpy_s()", __FILE__, __FUNCTION__, __LINE__ - 1);

    errno_t bytes_written = _snprintf_s(request,
        MAX_REQUEST_LEN * sizeof(char),
        _TRUNCATE,
        format_str,
        paUrl_struct->path, 
        paUrl_struct->query, 
        HTTP_ver, 
        paUrl_struct->host
    );

    err_check(bytes_written >= FORMAT_SIZE || bytes_written == -1, "_snprintf_s", __FILE__, __FUNCTION__, __LINE__ - 1);
    *paBytes_written = (int32_t)bytes_written + 1; // add null char

    return request;
}

void print_links(int32_t aLinks, char* paLink_buffer)
{
#ifdef DEBUG
    printf("Found %d links:\n", aLinks);

    // print each URL; these are NULL - separated C strings
    for (int i = 0; i < aLinks; i++)
    {
        printf("%s\n", paLink_buffer);
        paLink_buffer += strlen(paLink_buffer) + 1;
    }
#endif // DEBUG
}
int32_t parse_links(const char* paHtml_response, char* apBase_url)
{
    // create new parser object
    HTMLParserBase* parser = new HTMLParserBase;
    uint32_t len;

    //char baseUrl[] = "http://www.tamu.edu";		// where this page came from; needed for construction of relative links
    //char baseUrl[] = "http://www.washington.edu/";
    //apBase_url = baseUrl;
    
    err_check((len = (uint32_t)strlen(paHtml_response)) == 0, "strlen()", __FILE__, __FUNCTION__, __LINE__);

    int32_t nLinks;
    char* linkBuffer = parser->Parse((char*)paHtml_response, len, apBase_url, (int)strlen(apBase_url), &nLinks);

    // check for errors indicated by negative values
    if (nLinks < 0)
        nLinks = 0;

    print_links(nLinks, linkBuffer);

    delete parser;		// this internally deletes linkBuffer
    return nLinks;
}

void parse_response(const char* paResponse, url_t* paUrl_struct)
{
    char* pHeader = NULL;
    char* pBody = NULL;;
    char* pTmp = NULL;;
    int32_t http_status_code = -1;
    char base_url[MAX_HOST_LEN];

    printf("\tVerifying header... ");

    // Check for valid HTTP header
    if (strstr((char*)paResponse, "HTTP/1.0") || strstr((char*)paResponse, "HTTP/1.1"))
    {
        // check for HTTP header & split header-body
        pHeader = strstr((char*)paResponse, "\r\n\r\n");
        if (pHeader)
        {
            pBody = pHeader + strlen("\r\n\r\n");
            *pHeader = NULL;
            pHeader = (char*)paResponse;
        }
        else
        {
            printf("failed with invalid HTTP header");
            exit_process();
        }
    }
    else 
    {   
        printf("failed with non-HTTP header\n");
        exit_process();
    }

    // Print the links from the HTML parser
    // pTmp points to status code location
    pTmp = pHeader + strlen("HTTP/1.0 ");
    if (isdigit(*pTmp))
    {
        http_status_code = atoi(pTmp);
    }
    else
    {
        printf("failed parsing http response status\n");
        exit_process();
    }

    printf("status code %d\n", http_status_code);

    if (http_status_code >= 200 && http_status_code <= 299)
    {   // Success
        // Parse the links on the page
        printf("      + Parsing Page... ");
        clock_t time_start, time_stop;
        
        err_check((strcpy_s(base_url, MAX_HOST_LEN * sizeof(char), "http://"))          != SUCCESS, "strcpy()", __FILE__, __FUNCTION__, __LINE__);
        err_check((strcat_s(base_url, MAX_HOST_LEN * sizeof(char), paUrl_struct->host)) != SUCCESS, "strcat()", __FILE__, __FUNCTION__, __LINE__);
        err_check((strcat_s(base_url, MAX_HOST_LEN * sizeof(char), paUrl_struct->path)) != SUCCESS, "strcat()", __FILE__, __FUNCTION__, __LINE__);
        
        time_start = clock();
        int32_t num_links = parse_links(pBody, base_url);
        time_stop = clock();

        printf("done in %d ms with %d links\n", time_stop - time_start, num_links);
    }
    
    printf("\n----------------------------------------\n");
    printf("%s\n", pHeader);
}

