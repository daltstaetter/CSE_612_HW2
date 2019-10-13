/* winsock.cpp
 * CPSC 463 Sample Code 
 * by Dmitri Loguinov
 */

/* HW1-1
 * winsock.cpp: Handles the network/socket communication
 *
 * Date: 8/29/2019
 * Modified by Dalton Altstaetter with permission
 * CSE 612
 * Fall 2019
 */


#include "pch.h"
//#include "main.h"
 //#include "globals.h"

//extern _CrtMemState s1, s2, s3, s4, s5, s6, s7, s8, s9, s10;


#pragma comment(lib, "ws2_32.lib")

#define SEND_TIMEOUT        5000
#define RECV_TIMEOUT        10000
#define RECV_BUFF_SIZE      8192
#define CONNECTION_CLOSED   0
#define ERR_BUFF_SIZE       32
#define TEN_SECONDS         10000
#define MILLIS_PER_SEC      1000

#define ROBOT_BUFF_SIZE     16384   // 16 KB
#define MAX_BUFF_SIZE       2097152 //  2 MB

int32_t sock_check(int32_t aTest, const char* aFile, const char* aFunction, int32_t aLine_num)
{
    char sock_err_buff[ERR_BUFF_SIZE];
    errno_t bytes_written = _snprintf_s(sock_err_buff, _countof(sock_err_buff), _TRUNCATE, "%d", WSAGetLastError());
    if (err_check(aTest, sock_err_buff, aFile, aFunction, aLine_num) != SUCCESS)
        return FAIL;

    return SUCCESS;
}

/*
 * max_recv_len is to determine whether to exit early depending on if robots.txt exists
 * if not max_recv len is larger
*/
char* read_socket(SOCKET* paSocket, uint32_t* paCurr_pos, const uint32_t aMax_recv_len)
{
    //return NULL;
    int32_t recv_bytes;
    char* small_buff = NULL;
    bool isData_available;
    clock_t time_start;
    char* pRecv_buff = NULL;
    uint32_t recv_buff_size = RECV_BUFF_SIZE;
    uint32_t threshold = recv_buff_size / 2;
    *paCurr_pos = 0;

    if ( (pRecv_buff = (char*)malloc(recv_buff_size * sizeof(char))) == NULL)
        return NULL;
    
    //---------------------------------------
    // Used for the select function
    //---------------------------------------

    // check socket for data
    TIMEVAL timeout; // timeout(s) = tv_sec + tv_usec*(10^-6)
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    fd_set fd_reader; // monitors an array of sockets. Need to init and then set my socket array for when it has data(read)
    fd_set fd_exception; // check for errors on socket

    // init monitors
    FD_ZERO(&fd_reader);
    FD_ZERO(&fd_exception);

    // bind monitors to socket
    FD_SET(*paSocket, &fd_reader);
    FD_SET(*paSocket, &fd_exception);

    //--------------------------------------f-
    time_start = clock();
    
    while (true)
    {
        if ( (clock() - time_start) > TEN_SECONDS)
        {
                printf("failed with slow download (> %d seconds)\n", TEN_SECONDS/MILLIS_PER_SEC);
            kill_pointer((void**)& pRecv_buff);
            break;
        }

        isData_available = (bool) (select(0, &fd_reader, 0, &fd_exception, &timeout) > 0 );
        if (isData_available) // new data available; read the next segment
        {
            recv_bytes = recv(*paSocket, pRecv_buff + *paCurr_pos, recv_buff_size - *paCurr_pos, 0);
            
            if (recv_bytes == SOCKET_ERROR)
            {
                    printf("failed with %d on recv\n", WSAGetLastError());
                kill_pointer((void**)&pRecv_buff);
                break;
            }
            else if (recv_bytes == CONNECTION_CLOSED) // connection closed
            {
                pRecv_buff[*paCurr_pos] = 0; // Null Terminate recv_buff
                return pRecv_buff; // SUCCESS - normal completion
            }
            else // received valid response
            {
                // update position in buffer
                *paCurr_pos += recv_bytes;

                // check if its within the max allowed size
                if (*paCurr_pos > aMax_recv_len)
                {
                        printf("failed with exceeding max %d\n", aMax_recv_len);
                    kill_pointer((void**)&pRecv_buff);
                    break;
                }

                // check if I need to realloc more space 
                if (recv_buff_size - *paCurr_pos < min(threshold, RECV_BUFF_SIZE))
                {
                    small_buff = pRecv_buff;
                    pRecv_buff = (char*)malloc(recv_buff_size * sizeof(char) * 2);
                    if (!pRecv_buff)
                    {
                        kill_pointer((void**)&small_buff);
                        return NULL;
                    }
                    
                    // update buffer sizes
                    threshold = recv_buff_size * sizeof(char);
                    recv_buff_size = threshold * 2;
                    
                    // copy data to new buffer, free the old buff, and Null the old buff ptr
                    memcpy_s(pRecv_buff, recv_buff_size, small_buff, threshold);
                    kill_pointer((void**)& small_buff);
                    //_CrtMemCheckpoint(&s7);
                }
            }
        }
        else // TIMEOUT occurred 
        {
                printf("failed with timeout on recv\n");
            kill_pointer((void**)&pRecv_buff);
            break;
        }
    }

    return NULL;
}

const char* send_request(const char* paRequest, const int32_t aRequest_size, int32_t robot_mode)
{
    // string pointing to an HTTP server (DNS name or IP)
    //char str [] = "www.tamu.edu";
    //char str [] = "128.194.135.72";

    WSADATA wsaData;
    int32_t status;
    clock_t time_start;
    clock_t time_stop;
    uint32_t curr_pos = 0;
    const char* recv_buff = NULL;
   

    //Initialize WinSock; once per program run
    WORD wVersionRequested = MAKEWORD(2, 2);
    if (WSAStartup(wVersionRequested, &wsaData) != 0) 
    {
            printf("WSAStartup error %d\n", WSAGetLastError());
        
        WSACleanup();
        return NULL;
    }

    // open a TCP socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
            printf("failed with %d on socket init\n", WSAGetLastError());
        WSACleanup();
        return NULL;
    }

    // https://docs.microsoft.com/en-us/windows/win32/winsock/ipproto-tcp-socket-options
    //int32_t connect_timeout = 5; //CONNECT_TIMEOUT;
    //setsockopt(sock, IPPROTO_TCP, TCP_MAXRT , (const char*)& connect_timeout, sizeof(connect_timeout));

    int32_t send_timeout_millis = SEND_TIMEOUT;
    if (sock_check((setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)& send_timeout_millis, sizeof(send_timeout_millis))) != SUCCESS, 
                    __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
    {
            printf("error setting SO_SNDTIMEO sockopt()\n");
    }

    int32_t recv_timeout_millis = RECV_TIMEOUT;
    if (sock_check((setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)& recv_timeout_millis, sizeof(recv_timeout_millis))) != SUCCESS,
                    __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
    {
            printf("error setting SO_RCVTIMEO sockopt()\n");
    }

    // Need to dynamically resize if larger than recv_buff
    //int32_t recv_buff_size = RECV_BUFF_SIZE;
    //setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char*)& recv_buff_size, sizeof(recv_buff_size));

    // structure used in DNS lookups
    struct hostent* remote;

    // structure for connecting to server
    struct sockaddr_in server;

    // first assume that the string is an IP address
    DWORD IP = NULL;// = inet_addr(paUrl_struct->host);

    printf("\tDoing DNS... ");
    
    if (IP == INADDR_NONE) // if it is a hostname and not an IP address, do a DNS lookup
    {
        time_start = clock();
        remote = NULL;
        //remote = gethostbyname(paUrl_struct->host); // if not a valid IP, then do a DNS lookup
        time_stop = clock();

        if (remote == NULL)
        {
                printf("failed with %d\n", WSAGetLastError());
            return NULL;
        }
        else // take the first IP address and copy into sin_addr
        {
            memcpy((char*) & (server.sin_addr), remote->h_addr, remote->h_length);
        }
    }
    else
    {
        // if a valid IP, directly drop its binary version into sin_addr
        server.sin_addr.S_un.S_addr = IP;
        time_start = time_stop = 0;
    }
   
    
    // setup the port # and protocol type
    server.sin_family = AF_INET;
    //server.sin_port = htons(paUrl_struct->port);        // host-to-network flips the byte order

    time_start = clock();
    status = connect(sock, (struct sockaddr*) & server, sizeof(struct sockaddr_in));
    time_stop = clock();

    if (status == SOCKET_ERROR)
    {
            printf("failed with %d\n", WSAGetLastError());
        return NULL;
    }
    
    // send HTTP requests here
    status = send(sock, paRequest, aRequest_size, 0);
    if (sock_check(status == SOCKET_ERROR || status > aRequest_size, __FILE__, __FUNCTION__, __LINE__ - 1) != SUCCESS)
        return NULL;
    
    
    //time_start = clock();
    //if (paUrl_struct->isRobot_txt) // robots.txt exists
    //    recv_buff = read_socket(&sock, &curr_pos, ROBOT_BUFF_SIZE);
    //else // robots.txt does NOT exist
    //    recv_buff = read_socket(&sock, &curr_pos, MAX_BUFF_SIZE);
    //time_stop = clock();
    
    if (recv_buff)
            printf("done in %d ms with %lu bytes\n", time_stop - time_start, (unsigned long)curr_pos);

    // close the socket to this server; open again for the next one
    closesocket (sock);

    // call cleanup when done with everything and ready to exit program
    WSACleanup ();

    return recv_buff;
}

/*
const char* create_get_request(int32_t* paBytes_written, const char* cmd)
{
    *paBytes_written = 0;
    if (paUrl_struct == NULL)
        return NULL;

    char* request = (char*)malloc(MAX_REQUEST_LEN * sizeof(char));
    //_CrtMemCheckpoint(&s4);
    //return request;
    char format_str[FORMAT_SIZE];

    char HTTP_ver[] = "HTTP/1.0";

    errno_t status = strcpy_s(format_str, FORMAT_SIZE, "%s %s%s %s\r\nUser-agent: myAgent/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n");
    if (err_check(status != SUCCESS, "strcpy_s()", __FILE__, __FUNCTION__, __LINE__ - 1, 1) != SUCCESS)
    {
        kill_pointer((void**)& request);
        return NULL;
    }

    errno_t bytes_written = _snprintf_s(request,
        MAX_REQUEST_LEN * sizeof(char),
        _TRUNCATE,
        format_str,
        cmd,
        paUrl_struct->path,
        paUrl_struct->query,
        HTTP_ver,
        paUrl_struct->host
    );

    *paBytes_written = (int32_t)bytes_written;
    if (err_check(bytes_written >= FORMAT_SIZE || bytes_written == -1, "_snprintf_s", __FILE__, __FUNCTION__, __LINE__ - 1, 1) != SUCCESS)
    {
        if (gNum_init_threads <= SINGLE_THREAD)
            printf("failed while creating request\n");
        kill_pointer((void**)& request);
        return NULL;
    }

    *paBytes_written = *paBytes_written + 1; // increment for null char
    return request;
}
*/
