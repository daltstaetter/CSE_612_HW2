/* winsock.cpp
 * CPSC 463 Sample Code 
 * by Dmitri Loguinov
 */
#include "pch.h"
#pragma comment(lib, "ws2_32.lib")


#define SEND_TIMEOUT 5000
#define RECV_TIMEOUT 10000
#define RECV_BUFF_SIZE 8192

void sock_check(BOOL aTest, const char* aFunction, int32_t aLine_num)
{
    if (aTest)
    {
        printf("%s:%d: Connection error %d\n...Exiting\n", aFunction, aLine_num, WSAGetLastError());
    }
}

char* read_socket(SOCKET* paSocket, char* paRecv_buff, int32_t aRecv_buff_size, uint32_t* paCurr_pos, int32_t threshold)
{
    errno_t status;
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
        status = select(0, &fd_reader, 0, &fd_exception, &timeout);

        if (status > 0) // new data available; read the next segment
        {
            recv_bytes = recv(*paSocket, paRecv_buff + *paCurr_pos, aRecv_buff_size - *paCurr_pos, 0);
            if (recv_bytes == SOCKET_ERROR)
            {
                printf("%s:%d: RECV error %d\n", __FUNCTION__, __LINE__, WSAGetLastError());
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
            if (aRecv_buff_size - *paCurr_pos < threshold)
            {
                small_buff = paRecv_buff;
                paRecv_buff = (char*) malloc(aRecv_buff_size*sizeof(char) * 2);
                
                aRecv_buff_size = aRecv_buff_size * sizeof(char) * 2;
                threshold = aRecv_buff_size / 2;

                memcpy_s(paRecv_buff, aRecv_buff_size, small_buff, aRecv_buff_size / 2);
                
                free(small_buff);
            }
        }
        else if (status == 0) // TIMEOUT occured
        {
            printf("Recv timeout occured\n");
            break;
        }
        else // SOCKET_ERROR occurred 
        {
            printf("%s:%d: RECV error %d\n", __FUNCTION__, __LINE__, WSAGetLastError());
            break;
        }
    }


    return NULL;
}


char* winsock_test(url_t* paUrl_struct, const char* paRequest, const int32_t aRequest_size)
{
    // string pointing to an HTTP server (DNS name or IP)
    //char str [] = "www.tamu.edu";
    //char str [] = "128.194.135.72";

    WSADATA wsaData;
    int32_t status;
    uint32_t realloc_thresh;
    uint32_t curr_pos = 0;
    uint32_t recv_buff_size = 8192;
    char* recv_buff = (char*) malloc(recv_buff_size * sizeof(char));

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
        printf("socket() generated error %d\n", WSAGetLastError());
        WSACleanup();
        return NULL;
    }

    // https://docs.microsoft.com/en-us/windows/win32/winsock/ipproto-tcp-socket-options
    //int32_t connect_timeout = 5; //CONNECT_TIMEOUT;
    //setsockopt(sock, IPPROTO_TCP, TCP_MAXRT , (const char*)& connect_timeout, sizeof(connect_timeout));

    int32_t send_timeout_millis = SEND_TIMEOUT;
    status =  setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char*)& send_timeout_millis, sizeof(send_timeout_millis));
    sock_check(status != SUCCESS, __FUNCTION__, __LINE__ - 1);

    int32_t recv_timeout_millis = RECV_TIMEOUT;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)& recv_timeout_millis, sizeof(recv_timeout_millis));
    sock_check(status != SUCCESS, __FUNCTION__, __LINE__ - 1);

    // Need to dynamically resize if larger than recv_buff
    //int32_t recv_buff_size = RECV_BUFF_SIZE;
    //setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (const char*)& recv_buff_size, sizeof(recv_buff_size));

    // structure used in DNS lookups
    struct hostent* remote;

    // structure for connecting to server
    struct sockaddr_in server;

    // first assume that the string is an IP address
    DWORD IP = inet_addr(paUrl_struct->host);
    if (IP == INADDR_NONE)
    {
        // if not a valid IP, then do a DNS lookup
        if ((remote = gethostbyname(paUrl_struct->host)) == NULL)
        {
            printf("Invalid string: neither FQDN, nor IP address\n");
            return NULL;
        }
        else // take the first IP address and copy into sin_addr
            memcpy((char*) & (server.sin_addr), remote->h_addr, remote->h_length);
    }
    else
    {
        // if a valid IP, directly drop its binary version into sin_addr
        server.sin_addr.S_un.S_addr = IP;
    }

    // setup the port # and protocol type
    server.sin_family = AF_INET;
    server.sin_port = htons(paUrl_struct->port);		// host-to-network flips the byte order

    // connect to the server on specified port
    if (connect(sock, (struct sockaddr*) & server, sizeof(struct sockaddr_in)) == SOCKET_ERROR)
    {
        printf("Connection error: %d\n", WSAGetLastError());
        return NULL;
    }

    printf ("Successfully connected to %s (%s) on port %d\n", paUrl_struct->host, inet_ntoa (server.sin_addr), htons(server.sin_port));

    // send HTTP requests here
    status = send(sock, paRequest, aRequest_size, 0);
    sock_check(status == SOCKET_ERROR || status > aRequest_size, __FUNCTION__, __LINE__ - 1);

    
    // receive HTTP response
    realloc_thresh = recv_buff_size / 2;
    recv_buff = read_socket(&sock, recv_buff, recv_buff_size, &curr_pos, realloc_thresh);
    sock_check(recv_buff == NULL, __FUNCTION__, __LINE__ - 1);
    printf("%s\n", recv_buff);
    
    //recv(sock, recv_buff, recv_buff_size, 0);

	// close the socket to this server; open again for the next one
	closesocket (sock);

	// call cleanup when done with everything and ready to exit program
	WSACleanup ();

    return recv_buff;
}