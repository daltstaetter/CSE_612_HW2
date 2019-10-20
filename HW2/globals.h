#pragma once

/* HW1-1
 * winsock.cpp: Handles the network/socket communication
 *
 * Date: 8/29/2019
 * Modified by Dalton Altstaetter with permission
 * CSE 612
 * Fall 2019
 */

#ifndef GLOBALS2
#define GLOBALS2

#define LOG_INIT_SIZE   (1 << 14) // 2**14

// Global variables
char* gLog_buffer;
uint32_t gLog_buffer_size = LOG_INIT_SIZE;
int32_t gNum_jumps = 0;
int32_t gMax_num_jumps = 0;

#endif // !GLOBALS1