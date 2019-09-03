#pragma once

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

int32_t print_usage(void);
url_t* parse_url(char* paInput_url);
char* create_get_request(url_t* paUrl_struct);

void remove_scheme(char* paIn_url, uint16_t max_len);
char* set_hostname(char* paSub_url, char* paHostname);
errno_t set_port(char* paSub_url, uint16_t* pPort);
char* set_path(char* paSub_url, char* pPath);
char* set_query(char* paSub_url, char* pQuery);

// helpers
static char* get_char(char* paSub_url, const int8_t delimiter);
void err_check(BOOL aTest, const char* aMsg, const char* aFunction, int32_t aLine_num);

