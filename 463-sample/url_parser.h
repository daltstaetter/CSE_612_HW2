#pragma once

/* HW1-1
 * url_parser.h: This parses the input URL to create the GET request
 *
 * Date: 8/29/2019
 * Author: Dalton Altstaetter
 * CSE 612
 * Fall 2019
 */

#include "pch.h"
//#include "globals.h"

#define INVALID_URL     -1
#define SUCCESS         0

#define FRAG_SIZE 64

typedef struct url {
    char host[MAX_HOST_LEN] = {0};
    uint16_t port;
    char path[MAX_REQUEST_LEN] = {0};
    char query[MAX_REQUEST_LEN] = {0};
    char fragment[FRAG_SIZE] = {0};
    int32_t status;
    int32_t isRobot_txt = FALSE;
} url_t;


extern int32_t gNum_cmdline_args;
extern int32_t gNum_init_threads;

int32_t print_usage(void);
void exit_process(void);
int32_t starts_with(const char* start_string, const char* in_string);
url_t* parse_url(const char* paInput_url);
int ends_with_tamu_domain(const char * paDomain);
int32_t parse_links(const char* paHtml_response, char* apBase_url, HTMLParserBase* paParser);

int32_t parse_response(const char* paResponse, url_t* paUrl_struct, int32_t robot_mode, HTMLParserBase* paParserBase);

int32_t set_default_params(url_t* paUrl_struct, char paIn_url[MAX_URL_LEN], const char* paInput_url);
int32_t remove_scheme(char paIn_url[MAX_URL_LEN]);
char* remove_scheme_v2(char* paIn_url);
int32_t set_hostname(char paSub_url[MAX_URL_LEN], char* paHostname);
int32_t set_port(char paSub_url[MAX_URL_LEN], uint16_t* pPort);
int32_t set_path(char paSub_url[MAX_URL_LEN], char* pPath);
int32_t set_query(char paSub_url[MAX_URL_LEN], char* pQuery);
int32_t set_fragment(char pIn_url[MAX_URL_LEN], char paFragment[FRAG_SIZE]);

// helpers
static char* get_char(char paSub_url[MAX_URL_LEN], const int8_t delimiter);
int32_t err_check(int32_t aTest, const char* aMsg, const char* aFile, const char* aFunction, int32_t aLine_num, int32_t err_quit);
void kill_pointer(void **ptr);


