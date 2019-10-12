#pragma once

/* HW1-1
 * winsock.cpp: Handles the network/socket communication
 *
 * Date: 8/29/2019
 * Modified by Dalton Altstaetter with permission
 * CSE 612
 * Fall 2019
 */
#ifndef GLOBALS1
#define GLOBALS1

#include <unordered_set>
#include <string>
using namespace std;

#endif // !GLOBALS1

#ifndef GLOBALS2
#define GLOBALS2

// Global variables
int32_t gNum_cmdline_args;
int32_t gNum_init_threads;
unordered_set<DWORD> *gSeen_IPs;
unordered_set<string> *gSeen_hosts;

#endif // !GLOBALS1