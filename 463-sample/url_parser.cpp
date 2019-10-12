/* HW1-1
 * url_parser.cpp: This parses the input URL to create the GET request
 *
 * Date: 8/29/2019
 * Author: Dalton Altstaetter
 * CSE 612
 * Fall 2019
 */

#include "pch.h"
#define HTTP_PORT   80
 //extern Parameters();
extern Parameters* gParams;
//extern _CrtMemState s1, s2, s3, s4, s5, s6, s7, s8, s9, s10;

void exit_process()
{
#ifndef NO_QUIT
    exit(1);
#endif // NO_QUIT
}

int32_t starts_with(const char* start_string, const char* in_string)
{
    return !strncmp(start_string, in_string, strlen(start_string));
}

/*
 * Input: aTest
 * if aTest evaluates to true then log an error
 */
int32_t err_check(int32_t aTest, const char* aMsg, const char* aFile, const char* aFunction, int32_t aLine_num, int32_t err_quit)
{
    return SUCCESS;
    if (aFile == NULL || aFunction == NULL)
        return SUCCESS;
    const char* fname = ((char*)aFile - 1);
    const char* exit_test;
    char* aFile_end = (char*)aFile + strlen(aFile) - 1;

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

        if (gNum_init_threads <= SINGLE_THREAD)
            printf("%s:%s():%d: Error %s...\n", fname, aFunction, aLine_num, aMsg);
        
        return 1;
        //if (err_quit)
        //{
        //    printf("...Exiting\n");
        //    exit_process();
        //}
    }
    return SUCCESS;
}

/*
 * kill_pointer:
        - free pointer memory if not NULL
        - NULLS the pointer
*/
void kill_pointer(void** ptrptr)
{
    if (*ptrptr)
    {
        free(*ptrptr);
        *ptrptr = NULL;
    }
}

uint32_t null_strlen(const char* str)
{
    return (strlen(str) + 1);
}

// ERROR CHECK COMPLETED
int32_t remove_scheme(char paIn_url[MAX_URL_LEN])
{   // Removes 'http://' from URL
    char* pTemp_str = NULL;
    int32_t status = SUCCESS;

    pTemp_str = strstr(paIn_url, "http://");

    if (pTemp_str) // it matched HTTP
    {
        status = err_check((strcpy_s(paIn_url, MAX_URL_LEN * sizeof(char), (pTemp_str + strlen("http://")))) != SUCCESS, "strcpy()", __FILE__, __FUNCTION__, __LINE__, 1);
    }
    else
    {
        if (gNum_init_threads <= SINGLE_THREAD)
            printf("failed with invalid scheme\n"); //exit_process();
        status = 1;
    }
    
    return status;
}

char* remove_scheme_v2(char* paIn_url)
{   // Removes 'http://' from URL
    char* pTemp_str = strstr(paIn_url, "http://");

    if (pTemp_str) // it matched HTTP
        paIn_url += strlen("http://");

    return paIn_url;
}

// ERROR CHECK COMPLETED
int32_t print_usage(void)
{
    printf("\nUsage:\t1) hw1.exe scheme://host[:port][/path][?query][#fragment]");
    printf("\n      \t2) hw1.exe threads input_file\n"); // exit_process();

    return 1;
}

// ERROR CHECK COMPLETED
int32_t set_hostname(char paSub_url[MAX_URL_LEN], char* paHostname)
{
    char tmp_str[MAX_URL_LEN];
    
    if (err_check((strcpy_s(tmp_str, MAX_URL_LEN * sizeof(char), paSub_url)) != SUCCESS,
        "strcpy()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
    {
        return 1;
    }

    // copy input url into paHostname
    char* host_end = get_char(tmp_str, '#');

    if (host_end > get_char(tmp_str, '?'))
        host_end = get_char(tmp_str, '?');

    if (host_end > get_char(tmp_str, '/'))
        host_end = get_char(tmp_str, '/');

    if (host_end > get_char(tmp_str, ':'))
        host_end = get_char(tmp_str, ':');

    *host_end = 0;


    if (host_end && (host_end - tmp_str) >= MAX_HOST_LEN * sizeof(char))
    {
        if (gNum_init_threads <= SINGLE_THREAD)
            printf("hostname exceeds max host length: %ld\n", (long int) (host_end - tmp_str)); // exit_process();
        return 1;
    }

    if (err_check((strcpy_s(paHostname, MAX_HOST_LEN * sizeof(char), (const char*)tmp_str)) != SUCCESS,
        "strcpy()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
    {
        return 1;
    }

    if (strlen(paHostname) == 0)
    {
        print_usage();
        return 1;
    }

    // host_end points to where path ends in paHostname
    // if no match return null char location in string, i.e. end of string
    if (err_check((strcpy_s(tmp_str, MAX_URL_LEN * sizeof(char), paSub_url)) != SUCCESS,
        "strcpy()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
    {
        return 1;
    }

    host_end = get_char(tmp_str, '#');

    if (host_end > get_char(tmp_str, '?'))
        host_end = get_char(tmp_str, '?');

    if (host_end > get_char(tmp_str, '/'))
        host_end = get_char(tmp_str, '/');

    if (host_end > get_char(tmp_str, ':'))
        host_end = get_char(tmp_str, ':');

    // Keep the latter substring for additional parsing
    if (err_check((strcpy_s(paSub_url, MAX_URL_LEN * sizeof(char), (const char*)host_end)) != SUCCESS, 
        "strcpy()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
    {
        return 1;
    }

    if (paSub_url[0] == NULL || paSub_url[0] == ':' || paSub_url[0] == '/' || paSub_url[0] == '?')
        return SUCCESS;
    else
        return 1;
}

// ERROR CHECK COMPLETED
int32_t set_fragment(char paIn_url[MAX_URL_LEN], char paFragment[FRAG_SIZE])
{
    if (paIn_url[0] == '#') // path omitted
    {
        if (err_check((strcpy_s(paFragment, FRAG_SIZE, paIn_url)) != SUCCESS,
            "strcpy_s()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
        {
            return 1;
        }
    }
    return SUCCESS;
}

/*
*
*   Goal: Find substring index in  null-terminated string paSub_url
*         -if no match (search returns NULL), return location of null char
*/
static char* get_char(char paSub_url[MAX_URL_LEN], const int8_t delimiter)
{
    char* char_index = NULL;
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
// ERROR CHECK COMPLETED
int32_t set_port(char paSub_url[MAX_URL_LEN], uint16_t* pPort)
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
                    if (gNum_init_threads <= SINGLE_THREAD)
                        printf("failed with invalid port\n"); // exit_process();
                    return 1;
                }
                *pPort = (uint16_t)port;
                
                // move along the string & find first non-numeric index
                uint32_t i = 1;
                while (isdigit(paSub_url[i]))
                    i++;
                
                if (err_check((strcpy_s(paSub_url, MAX_URL_LEN * sizeof(char), paSub_url + sizeof(char) * i)) != SUCCESS,
                    "strcpy()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
                {
                    return 1;
                }
            }
            else if (paSub_url[1] == '-')
            {
                // check for negative numbers
                if (gNum_init_threads <= SINGLE_THREAD)
                    printf("failed with invalid port\n"); // exit_process();
                return 1;
            }
        }
    }
    
    return SUCCESS;
}

// ERROR CHECK COMPLETED
int32_t set_path(char paSub_url[MAX_URL_LEN], char* pPath)
{
    if (paSub_url[0] == '/')
    {
        if (err_check((strcpy_s(pPath, MAX_URL_LEN * sizeof(char), (const char*)paSub_url)) != SUCCESS,
            "strcpy()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
        {
            return 1;
        }

        char* char_search = get_char(pPath, '#');

        if (char_search > get_char(pPath, '?'))
            char_search = get_char(pPath, '?');

        // Keep the latter substring for additional parsing
        if (err_check((strcpy_s(paSub_url, MAX_URL_LEN * sizeof(char), (const char*)char_search)) != SUCCESS,
                  "strcpy()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
        {
            return 1;
        }

        // null-term the match w/in pPath
        *char_search = 0;
    }
    else if (paSub_url[0] == '?' || paSub_url[0] == '#' || paSub_url[0] == NULL) // path omitted, set to default value of '/'
    {
        if (err_check((strcpy_s(pPath, MAX_REQUEST_LEN * sizeof(char), "/")) != SUCCESS, 
            "strcpy()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
        {
            return 1;
        }
    }
    else // error: malformed input
    {
        print_usage();
        return 1;
    }

    return SUCCESS;
}

// ERROR CHECK COMPLETED
int32_t set_query(char paSub_url[MAX_URL_LEN], char* pQuery)
{
    if (paSub_url[0] == '?')
    {
        if (err_check((strcpy_s(pQuery, MAX_URL_LEN * sizeof(char), (const char*)paSub_url)) != SUCCESS,
            "strcpy()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
        {
            return 1;
        }
        
        char* char_search = get_char(pQuery, '#');
        
        // Keep the latter substring for additional parsing
        if (err_check((strcpy_s(paSub_url, MAX_URL_LEN * sizeof(char), (const char*)char_search)) != SUCCESS,
            "strcpy()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
        {
            return 1;
        }
        
        // null-term the match w/in pQuery
        *char_search = 0;
    }

    return SUCCESS;
}

// ERROR CHECK COMPLETED
int32_t set_default_params(url_t* paUrl_struct, char  paIn_url[MAX_URL_LEN], const char* paInput_url)
{
    int32_t status = SUCCESS;
    paUrl_struct->port = HTTP_PORT; // assume port 80 if not specified
    
    if (err_check((strcpy_s(paUrl_struct->path, MAX_URL_LEN * sizeof(char), "/")) != SUCCESS,
        "strcpy()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
    {
        status = 1;
    }
    
    paUrl_struct->query[0] = 0;
    paUrl_struct->fragment[0] = 0;
    
    if (err_check((strcpy_s(paIn_url, MAX_URL_LEN * sizeof(char), paInput_url)) != SUCCESS,
        "strcpy()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
    {
        status = 1;
    }

    paUrl_struct->isRobot_txt = FALSE;

    return status;
}

/*
*
*   Goal: Break URL in format scheme://host[:port][/path][?query][#fragment]
*         into its components, e.g. http://128.194.135.72:80/courses/index.asp#location
*/
// ERROR CHECK COMLETED
url_t* parse_url(const char* paInput_url)
{
    int32_t status = SUCCESS;
    
    if (strlen(paInput_url) >= MAX_URL_LEN - 1)
    {
        if (gNum_init_threads <= SINGLE_THREAD)
            printf("url exceeds max url length\n");
        return NULL; // exit_process();
    }

    url_t* pUrl_struct = (url_t*) malloc(sizeof(url_t));
    
    if (pUrl_struct == NULL)
    {
        if (gNum_init_threads <= SINGLE_THREAD)
            printf("unable to malloc url_struct\n");
        return NULL;
    }
    
    char pIn_url[MAX_URL_LEN];

    // Set default values in case parameters are omitted
    // fails if strlen(paInput_url) >= 256
    if (status == SUCCESS && set_default_params(pUrl_struct, pIn_url, paInput_url) != SUCCESS)
        status = ~SUCCESS;

    // Removes 'http://' from URL
    if (status == SUCCESS && remove_scheme(pIn_url) != SUCCESS)
        status = ~SUCCESS;

    //-------------------------------------------------------------
    // Set host/IP
    // To get host/IP find the first occurence of any of [:, /, ?, #]
    //-------------------------------------------------------------
    if (status == SUCCESS && set_hostname(pIn_url, pUrl_struct->host) != SUCCESS)
        status = ~SUCCESS;
    
    if (status == SUCCESS && set_port(pIn_url, &(pUrl_struct->port)) != SUCCESS)
        status = ~SUCCESS;

    // ----------------------------------------------
    // Parse the path: could be explicit or omitted (set to '/')
    // ----------------------------------------------
    if (status == SUCCESS && set_path(pIn_url, pUrl_struct->path) != SUCCESS)
        status = ~SUCCESS;

    // ----------------------------------------------
    // Parse the query: could be explicit or omitted
    // ----------------------------------------------
    if (status == SUCCESS && set_query(pIn_url, pUrl_struct->query) != SUCCESS)
        status = ~SUCCESS;

    // ----------------------------------------------
    // Parse the fragment: Not used - set to NULL
    // ----------------------------------------------
    if (status == SUCCESS && set_fragment(pIn_url, pUrl_struct->fragment) != SUCCESS)
        status = ~SUCCESS;

    if (status != SUCCESS)
    {
        kill_pointer((void**)& pUrl_struct);
        return NULL;
    }

    return pUrl_struct;
}

void print_links(int32_t aLinks, char* paLink_buffer, char* base_url, int32_t base_len)
{
//#ifdef DEBUG
    //printf("Found %d links:\n", aLinks);

    if (gNum_init_threads > SINGLE_THREAD)
    {
        char* char_index = NULL;
        char* tamu_str_start = NULL;
        char* not_tamu_base_url = NULL;
        char* tmp = NULL;
        uint32_t has_tamu_link = 0;
        
        // check strings for tamu.edu hostname
        for (int i = 0; i < aLinks; i++)
        {
            paLink_buffer = remove_scheme_v2(paLink_buffer);
            char_index = strchr(paLink_buffer, '/');
            tamu_str_start = strstr(paLink_buffer, "tamu.edu");

            if (tamu_str_start)
            {
                if (char_index)
                {
                    if (tamu_str_start < char_index)
                    {   // valid tamu domain with '/'
                        //printf("%s\n", paLink_buffer);
                        EnterCriticalSection(gParams->pCriticalSection);
                        has_tamu_link = 1;
                        gParams->num_tamu_links++;
                        LeaveCriticalSection(gParams->pCriticalSection);
                    }
                }
                else
                {   // valid tamu domain without '/'
                    //printf("%s\n", paLink_buffer);
                    EnterCriticalSection(gParams->pCriticalSection);
                    has_tamu_link = 1;
                    gParams->num_tamu_links++;
                    LeaveCriticalSection(gParams->pCriticalSection);
                }
            }

            //printf("%s\n", paLink_buffer);
            paLink_buffer += strlen(paLink_buffer) + 1;
        }

        if (has_tamu_link)
        {
            //char test1[] = "http://tamu.edu.cn/";
            //char test2[] = "http://www.x.com/tamu.edu/";
            //char test3[] = "http://irl.cse.tamu.edu";
            tmp = NULL;
            char_index = NULL;
            int32_t len = base_len + 1;
            tamu_str_start = NULL;
            not_tamu_base_url = (char*)malloc(len * sizeof(char));

            if (not_tamu_base_url == NULL || (strcpy_s(not_tamu_base_url, len * sizeof(char), base_url) != SUCCESS) )
            {
                free(not_tamu_base_url);
                not_tamu_base_url = NULL;
            }
            else
            {
                tmp = remove_scheme_v2(not_tamu_base_url);
                char_index = strchr(tmp, '/');
                tamu_str_start = strstr(tmp, "tamu.edu");

                if (char_index != NULL)
                    *char_index = 0;
                char_index = tmp;

                // need to ensure that the host does not end in tamu.edu
                if (ends_with_tamu_domain(char_index) != SUCCESS)
                {
                    EnterCriticalSection(gParams->pCriticalSection);
                    gParams->from_outside_tamu++;
                    LeaveCriticalSection(gParams->pCriticalSection);
                }
                
                tmp = NULL;
                char_index = NULL;
                tamu_str_start = NULL;
                free(not_tamu_base_url);
                not_tamu_base_url = NULL;
            }
        }
    }
//#endif // DEBUG
}

int32_t ends_with_tamu_domain(const char * paDomain)
{
    int32_t ret_val = -1;
    paDomain = strstr(paDomain, "tamu.edu");

    if (paDomain)
        ret_val = strcmp(paDomain, "tamu.edu");
    
    return ret_val;
}

int32_t parse_links(const char* paHtml_response, char* apBase_url, HTMLParserBase* paParser)
{
    HTMLParserBase* parser;
    int32_t should_delete; // if in single thread mode
    char* linkBuffer = NULL;

    // this is used for multithread mode
    // HTMLParserBase is not multithread safe so we create a new object for each thread
    // and pass it down to this function. If it exists it uses it, otherwise it falls back
    // to single-thread mode where it creates it when needed.
    if (paParser == NULL)
    {
        parser = new HTMLParserBase;
        should_delete = TRUE;
    }
    else
    {
        parser = paParser;
        should_delete = FALSE;
    }

    uint32_t len;

    //char baseUrl[] = "http://www.tamu.edu";        // where this page came from; needed for construction of relative links
    //char baseUrl[] = "http://www.washington.edu/";
    //apBase_url = baseUrl;

    err_check((len = (uint32_t)strlen(paHtml_response)) == 0, "strlen()", __FILE__, __FUNCTION__, __LINE__, 1);

    int32_t nLinks = 0;
    //int32_t sumLinks = 0;
    //if (len > 262144-1) // break it up if its too big
    //{
    //    uint32_t step = 262144-1;
    //    while (len > 0)
    //    { 
    //        nLinks = 0;
    //        linkBuffer = parser->Parse((char*)paHtml_response, step, apBase_url, (int)strlen(apBase_url), &nLinks);
    //        sumLinks += nLinks;
    //        paHtml_response += (step + 1);
    //        len -= step;
    //        if (step > len)
    //            step = len;
    //    }
    //}
    //else
    //{
        linkBuffer = parser->Parse((char*)paHtml_response, len, apBase_url, (int)strlen(apBase_url), &nLinks);
    //}

    // check for errors indicated by negative values
    if (nLinks < 0)
        nLinks = 0;

    print_links(nLinks, linkBuffer, apBase_url, (int)strlen(apBase_url));
    if (should_delete == TRUE)
        delete parser;        // this internally deletes linkBuffer
    return nLinks;
}

// ERROR CHECK COMPLETED
int32_t parse_response(const char* paResponse, url_t* paUrl_struct, int32_t robot_mode, HTMLParserBase* paParserBase)
{
    char* pHeader = NULL;
    char* pBody = NULL;
    char* pTmp = NULL;
    int32_t http_status_code = -1;
    char base_url[MAX_URL_LEN];

    if (paUrl_struct == NULL || paResponse == NULL)
        return 1;
    if (gNum_init_threads <= SINGLE_THREAD)
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
            if (gNum_init_threads <= SINGLE_THREAD)
                printf("failed with invalid HTTP header"); // exit_process();
        }
    }
    else 
    {
        if (gNum_init_threads <= SINGLE_THREAD)
            printf("failed with non-HTTP header\n"); // exit_process();
    }

    if (pHeader == NULL)
        return 1;

    // Print the links from the HTML parser
    // pTmp points to status code location
    pTmp = pHeader + strlen("HTTP/1.0 ");
    if (isdigit(*pTmp))
    {
        http_status_code = atoi(pTmp);
    }
    else
    {
        if (gNum_init_threads <= SINGLE_THREAD)
            printf("failed parsing http response status\n"); // exit_process();
        return 1;
    }

    if (gNum_init_threads <= SINGLE_THREAD)
        printf("status code %d\n", http_status_code);

    if (http_status_code >= 200 && http_status_code <= 299 && !robot_mode)
    {   // Success
        // Parse the links on the page
        // only parse the links if we are searching the page
        if (gNum_init_threads <= SINGLE_THREAD)
            printf("      + Parsing Page... ");
        clock_t time_start, time_stop;
        
        if (err_check((strcpy_s(base_url, MAX_URL_LEN * sizeof(char), "http://")) != SUCCESS, "strcpy()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
            return -1;
        if (err_check((strcat_s(base_url, MAX_URL_LEN * sizeof(char), paUrl_struct->host)) != SUCCESS, "strcat()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
            return -1;
        if (err_check((strcat_s(base_url, MAX_URL_LEN * sizeof(char), paUrl_struct->path)) != SUCCESS, "strcat()", __FILE__, __FUNCTION__, __LINE__, 1) != SUCCESS)
            return -1;
        
        int32_t num_links;

        time_start = clock();
        num_links = parse_links(pBody, base_url, paParserBase);
        time_stop = clock();

        if (gNum_init_threads > SINGLE_THREAD)
        {
            EnterCriticalSection(gParams->pCriticalSection);
            gParams->num_links_parsed += num_links;
            gParams->num_crawled_urls++;
            gParams->http_codes->two_hundreds++;
            LeaveCriticalSection(gParams->pCriticalSection);
        }

        if (gNum_init_threads <= SINGLE_THREAD)
            printf("done in %d ms with %d links\n", time_stop - time_start, num_links);
    }
    else if (http_status_code >= 300 && http_status_code <= 399 && !robot_mode)
    {
        if (gNum_init_threads > SINGLE_THREAD)
        {
            EnterCriticalSection(gParams->pCriticalSection);
            gParams->num_crawled_urls++;
            gParams->http_codes->three_hundreds++;
            LeaveCriticalSection(gParams->pCriticalSection);
        }
    }
    else if (http_status_code >= 400 && http_status_code <= 499 && !robot_mode)
    {
        if (gNum_init_threads > SINGLE_THREAD)
        {
            EnterCriticalSection(gParams->pCriticalSection);
            gParams->num_crawled_urls++;
            gParams->http_codes->four_hundreds++;
            LeaveCriticalSection(gParams->pCriticalSection);
        }
    }
    else if (http_status_code >= 500 && http_status_code <= 599 && !robot_mode)
    {
        if (gNum_init_threads > SINGLE_THREAD)
        {
            EnterCriticalSection(gParams->pCriticalSection);
            gParams->num_crawled_urls++;
            gParams->http_codes->five_hundreds++;
            LeaveCriticalSection(gParams->pCriticalSection);
        }
    }
    else
    {
        if (gNum_init_threads > SINGLE_THREAD && !robot_mode)
        {
            EnterCriticalSection(gParams->pCriticalSection);
            gParams->num_crawled_urls++;
            gParams->http_codes->other++;
            LeaveCriticalSection(gParams->pCriticalSection);
        }
    }
    
    if (robot_mode)
    {
        if (!(http_status_code >= 400 && http_status_code <= 499))
        {
            paUrl_struct->isRobot_txt = TRUE;
        }
        else 
        {   
            if (gNum_init_threads > SINGLE_THREAD)
            {
                // page has a robot.txt, increment count and skip crawling URL
                EnterCriticalSection(gParams->pCriticalSection);
                gParams->num_passed_robot_checks++;
                LeaveCriticalSection(gParams->pCriticalSection);
            }
        }
    }
    
    if (gNum_cmdline_args == SINGLE_URL_INPUT && gNum_init_threads <= SINGLE_THREAD)
        printf("\n----------------------------------------\n%s\n", pHeader);

    return SUCCESS;
}

