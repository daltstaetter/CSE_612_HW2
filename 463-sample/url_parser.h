#pragma once

#include "pch.h"

#define INVALID_URL     -1
#define SUCCESS         0


typedef struct url {

    // TODO: check host < MAX_HOST_LEN
    // TODO: check port within range
    // TODO: check request is < MAX_REQUEST_LEN
    char host[MAX_HOST_LEN] = {0};
    uint16_t port;
    char path[MAX_REQUEST_LEN] = {0};
    char query[MAX_REQUEST_LEN] = {0};
    char* fragment = NULL;
    int32_t status;
} url_t;

int32_t print_usage(void);
url_t* parse_url(char* paInput_url);
char* get_char(char* paSub_url, const int8_t delimiter);