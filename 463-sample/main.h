#pragma once


#ifndef __MAIN__
#define __MAIN__

/* HW1-1
 * winsock.cpp: Handles the network/socket communication
 *
 * Date: 8/29/2019
 * Modified by Dalton Altstaetter with permission
 * CSE 612
 * Fall 2019
 */

#define SINGLE_THREAD           1
#define FORMAT_SIZE             512
#define ROBOT                   1
#define MILLIS_PER_INTERRUPT    2000
#define MILLIS_TO_SECONDS       1000
#define SECONDS_PER_INTERRUPT   (MILLIS_PER_INTERRUPT/MILLIS_TO_SECONDS)
#define BYTES_TO_MBYTES          1000000


int32_t main(int32_t argc, char* argv[]);

int32_t process_single_url_input(const char* paIn_url, HTMLParserBase* paParserBase);
int32_t process_file_url_input(const char* paNum_threads, const char* paFilename);

// functions inside winsock.cpp
const char* send_request(url_t* paUrl_struct, const char* paRequest, const int32_t aRequest_size, int32_t robot_mode);
char* read_socket(SOCKET* paSocket, uint32_t* paCurr_pos, const uint32_t aMax_recv_len);

int32_t check_IP_set(DWORD aIP);
int32_t check_hostname_set(string host_str);

const char* create_head_request(url_t* paUrl_struct, int32_t* paBytes_written);
const char* create_get_request(url_t* paUrl_struct, int32_t* paBytes_written, const char* cmd);
int32_t main_multithread(int32_t aNum_threads, const char* paFilename);
void print_multithread_results(void);

int32_t WINAPI thread_crawler(LPVOID lpPara);

class http_codes_t {

public:
    uint32_t two_hundreds;
    uint32_t three_hundreds;
    uint32_t four_hundreds;
    uint32_t five_hundreds;
    uint32_t other;
};

// this class is passed to all threads, acts as shared memory
class Parameters {
public:
    // HANDLE    mutex;
    LPCRITICAL_SECTION pCriticalSection = new CRITICAL_SECTION; // MSDN Documentation Example: https://docs.microsoft.com/en-us/windows/win32/sync/using-critical-section-objects
    //CRITICAL_SECTION* pCriticalSection;//
                                         //HANDLE    finished;
    HANDLE    eventQuit = NULL;

    /* Key Counts to measure:
        1. num urls left in queue                   // accounted for
        2. num currently running threads            // accounted for
        3. num urls extracted from queue            // accounted for
        4. num unique hostnames    (gSeen_hosts)    // accounted for
        5. num successful DNS lookups               // accounted for
        6. num IPs that are unique (gSeen_hosts)    // accounted for
        7. http return code count                   // accounted for
        8. total links found                        // accounted for
        9. total num bytes read                     // accounted for
        10. num_pages_with_robots.txt               // accounted for
    */

    queue<string>* pQueue = NULL;
    uint32_t num_running_threads = 0;       // accounted for
    uint32_t num_extracted_urls = 0;        // accounted for
    uint32_t num_DSN_lookups = 0;           // accounted for
    uint32_t num_passed_robot_checks = 0;
    uint32_t num_crawled_urls = 0;
    http_codes_t* http_codes = new http_codes_t();
    uint32_t num_links_parsed = 0;
    uint64_t num_bytes_read = 0;
    uint32_t time_millis = 0;
    uint32_t num_tamu_links = 0;
    uint32_t from_outside_tamu = 0;
};


#endif __MAIN__





