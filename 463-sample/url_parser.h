#pragma once

/* HW1-1
 * url_parser.h: This parses the input URL to create the GET request
 *
 * Date: 8/29/2019
 * Author: Dalton Altstaetter
 */

#include "pch.h"

#define INVALID_URL     -1
#define SUCCESS         0

#define FRAG_SIZE 64

typedef struct url {

    // TODO: check host < MAX_HOST_LEN
    // TODO: check port within range
    // TODO: check request is < MAX_REQUEST_LEN
    char host[MAX_HOST_LEN] = {0};
    uint16_t port;
    char path[MAX_REQUEST_LEN] = {0};
    char query[MAX_REQUEST_LEN] = {0};
    char fragment[FRAG_SIZE] = {0};
    int32_t status;
} url_t;

void print_usage(void);
void set_defaults(url_t* pUrl_struct, char  pIn_url[256], char* paInput_url);
url_t* parse_url(char* paInput_url);

char* create_get_request(url_t* paUrl_struct, int32_t* paBytes_written);
void parse_response(const char* paResponse, url_t* paUrl_struct);

void set_default_params(url_t* paUrl_struct, char paIn_url[MAX_URL_LEN], char* paInput_url);
void remove_scheme(char paIn_url[MAX_URL_LEN]);
void set_hostname(char paSub_url[MAX_URL_LEN], char* paHostname);
void set_port(char paSub_url[MAX_URL_LEN], uint16_t* pPort);
void set_path(char paSub_url[MAX_URL_LEN], char* pPath);
void set_query(char paSub_url[MAX_URL_LEN], char* pQuery);
void set_fragment(char  pIn_url[MAX_URL_LEN], url_t* pUrl_struct);

// helpers
char* get_char(char paSub_url[MAX_URL_LEN], const int8_t delimiter);
void err_check(BOOL aTest, const char* aMsg, const char* aFile, const char* aFunction, int32_t aLine_num);

