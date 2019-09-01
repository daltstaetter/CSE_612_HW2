#pragma once

#include "pch.h"

#define INVALID_URL     -1


typedef struct url {

    // TODO: check host < MAX_HOST_LEN
    // TODO: check port within range
    // TODO: check request is < MAX_REQUEST_LEN
    uint8_t* host;
    uint16_t port;
    uint8_t* path;
    uint8_t* query;
    uint8_t* fragment;
    int32_t status;
} url_t;

int32_t print_usage(void);
url_t* parse_url(uint8_t* paInput_url);