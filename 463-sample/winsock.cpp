/* winsock.cpp
 * CPSC 463 Sample Code 
 * by Dmitri Loguinov
 */
#include "pch.h"
#pragma comment(lib, "ws2_32.lib")


#define SEND_TIMEOUT 5000
#define RECV_TIMEOUT 10000
#define RECV_BUFF_SIZE 8192

void sock_check(BOOL aTest, const char* aFile, const char* aFunction, int32_t aLine_num)
{
    const char* fname = ((char*)aFile - 1);
    const char* exit_test;
    char* aFile_end = (char*)aFile + strlen(aFile);

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

        printf("%s:%s:%d: Connection error %d\n...Exiting\n", fname, aFunction, aLine_num, WSAGetLastError());
    }
}

char* read_socket(SOCKET* paSocket, char* paRecv_buff, uint32_t aRecv_buff_size, uint32_t* paCurr_pos, uint32_t threshold)
{
    int32_t recv_bytes;
    char* small_buff;
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

    //---------------------------------------
    while (true)
    {
        if (select(0, &fd_reader, 0, &fd_exception, &timeout) > 0) // new data available; read the next segment
        {
            recv_bytes = recv(*paSocket, paRecv_buff + *paCurr_pos, aRecv_buff_size - *paCurr_pos, 0);
            
            if (recv_bytes == SOCKET_ERROR)
            {
                printf("failed with %d on recv\n", WSAGetLastError());
                break;
            }
            else if (strstr((char*)paRecv_buff, "HTTP/1.") == NULL)
            {   // check if response is a valid HTTP response
                printf("failed with non-HTTP header\n");
                break;
            }

            if (recv_bytes == 0) // connection closed
            {
                paRecv_buff[*paCurr_pos] = 0; // Null Terminate recv_buff
                return paRecv_buff;
                //return SUCCESS; // normal completion
            }
            
            // update position
            *paCurr_pos += recv_bytes;
            
            // adjust where the next recv goes
            if (aRecv_buff_size - *paCurr_pos < min(threshold, RECV_BUFF_SIZE))
            {
                small_buff = paRecv_buff;
                paRecv_buff = (char*) malloc(aRecv_buff_size*sizeof(char) * 2);
                
                aRecv_buff_size = aRecv_buff_size * sizeof(char) * 2;
                threshold = aRecv_buff_size / 2;

                memcpy_s(paRecv_buff, aRecv_buff_size, small_buff, aRecv_buff_size / 2);
                
                free(small_buff);
            }
        }
        else // TIMEOUT or SOCKET_ERROR occurred 
        {
            printf("failed with %d on recv\n", WSAGetLastError());
            break;
        }
    }

    return NULL;
}

char* send_request(url_t* paUrl_struct, const char* paRequest, const int32_t aRequest_size)
{
    // string pointing to an HTTP server (DNS name or IP)
    //char str [] = "www.tamu.edu";
    //char str [] = "128.194.135.72";

    WSADATA wsaData;
    int32_t status;
    uint32_t realloc_thresh;
    clock_t time_start;
    clock_t time_stop;
    uint32_t curr_pos = 0;
    char* recv_buff = (char*) malloc(RECV_BUFF_SIZE * sizeof(char));

    //Initialize WinSock; once per program run
    WORD wVersionRequested = MAKEWORD(2, 2);
    if (WSAStartup(wVersionRequested, &wsaData) != 0) {
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
    sock_check((setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)& send_timeout_millis, sizeof(send_timeout_millis))) != SUCCESS, 
                __FILE__, __FUNCTION__, __LINE__);

    int32_t recv_timeout_millis = RECV_TIMEOUT;
    sock_check((setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)& recv_timeout_millis, sizeof(recv_timeout_millis))) != SUCCESS, 
                __FILE__, __FUNCTION__, __LINE__);

    // Need to dynamically resize if larger than recv_buff
    //int32_t recv_buff_size = RECV_BUFF_SIZE;
    //setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char*)& recv_buff_size, sizeof(recv_buff_size));

    // structure used in DNS lookups
    struct hostent* remote;

    // structure for connecting to server
    struct sockaddr_in server;

    // first assume that the string is an IP address
    DWORD IP = inet_addr(paUrl_struct->host);
    printf("\tDoing DNS... ");
    if (IP == INADDR_NONE)
    {
        time_start = clock();
        remote = gethostbyname(paUrl_struct->host); // if not a valid IP, then do a DNS lookup
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
        time_start = clock();
        time_stop = time_start;
    }
    printf("done in %d ms, found %s\n", time_stop - time_start, inet_ntoa (server.sin_addr));

    // setup the port # and protocol type
    server.sin_family = AF_INET;
    server.sin_port = htons(paUrl_struct->port);		// host-to-network flips the byte order

    // connect to the server on specified port
    printf("      * Connecting on page... ");
    time_start = clock();
    status = connect(sock, (struct sockaddr*) & server, sizeof(struct sockaddr_in));
    time_stop = clock();

    if (status == SOCKET_ERROR)
    {
        printf("failed with %d\n", WSAGetLastError());
        return NULL;
    }
    printf("done in %d ms\n", time_stop - time_start);
    
    // send HTTP requests here
    status = send(sock, paRequest, aRequest_size, 0);
    sock_check(status == SOCKET_ERROR || status > aRequest_size, __FILE__, __FUNCTION__, __LINE__ - 1);
    
    // receive HTTP response
    printf("\tLoading... ");
    realloc_thresh = RECV_BUFF_SIZE / 2;
    time_start = clock();
    recv_buff = read_socket(&sock, recv_buff, RECV_BUFF_SIZE, &curr_pos, realloc_thresh);
    time_stop = clock();

    if (recv_buff != NULL)
        printf("done in %d ms with %lu bytes\n", time_stop - time_start, (unsigned long)curr_pos);

	// close the socket to this server; open again for the next one
	closesocket (sock);

	// call cleanup when done with everything and ready to exit program
	WSACleanup ();

    return recv_buff;
}