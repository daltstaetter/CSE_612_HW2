/* HW1-1
 * DNS.cpp: This parses the input URL to create the GET request
 *
 * Date: 8/29/2019
 * Author: Dalton Altstaetter
 * CSE 612
 * Fall 2019
 */

#include "pch.h"
//extern _CrtMemState s1, s2, s3, s4, s5, s6, s7, s8, s9, s10;

extern char* gLog_buffer;
extern uint32_t gLog_buffer_size;
extern int32_t gNum_jumps;
extern int32_t gMax_num_jumps;

void exit_process()
{
#ifndef NO_QUIT
    exit(1);
#endif // NO_QUIT
}

static int32_t starts_with(const char* start_string, const char* in_string)
{
    return !strncmp(start_string, in_string, strlen(start_string));
}

/*
 * Input: aTest
 * if aTest evaluates to true then log an error
 */
int32_t err_check(int32_t aTest, const char* aMsg, const char* aFile, const char* aFunction, int32_t aLine_num)
{
    if (aFile == NULL)
        aFile = "NULL";
    if (aFunction == NULL)
        aFunction = "NULL";

    char log_msg[LOG_LINE_SIZE];
    const char* fname = ((char*)aFile - 1);
    const char* exit_test;
    char* aFile_end = (char*)aFile + strlen(aFile) - 1;

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
        
        int32_t bytes_written = _snprintf_s(log_msg, LOG_LINE_SIZE * sizeof(char), "\nERROR:%s:%u:%s\n", fname, aLine_num, aMsg);
        append_to_log(log_msg);
        //printf("\nERROR:%s:%u:%s", fname, aLine_num, aMsg);
        return FAIL;
    }
    return SUCCESS;
}

/*
 * kill_pointer:
        - free pointer memory if not NULL
        - NULLS the pointer
*/
void kill_pointer(void** ptrptr)
{
    if (*ptrptr)
    {
        free(*ptrptr);
        *ptrptr = NULL;
    }
}

int32_t null_strlen(const char* str)
{
    return (int32_t)(strlen(str) + 1);
}

void print_log()
{
    printf("%s", gLog_buffer);
    kill_pointer((void**) &gLog_buffer);
    if (err_check((gLog_buffer = (char*)calloc(gLog_buffer_size, sizeof(char))) == NULL, "calloc() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        printf("gLoc calloc failed\n");
}

int32_t append_to_log(const char* pAppend)
{
    int32_t status = FAIL;
    
    if (pAppend)
    {
        if ((strcat_s(gLog_buffer, gLog_buffer_size * sizeof(char), pAppend)) != SUCCESS)
            printf("Need to realloc pLog_buffer. Error on log strcat");
        else
            status = SUCCESS;
    }

    return status;
}

// ERROR CHECK COMPLETED
int32_t print_usage(void)
{
    printf("\nUsage:\t1)  hw2.exe [hostname | IPADDR] DNS_Server_IP\n\n");
    printf("      \tForward Lookup (host->IP): hw2.exe www.cnn.com 8.8.8.8\n"); // exit_process();
    printf("      \tReverse Lookup (IP->host): hw2.exe 72.146.82.6 128.194.135.84\n"); // exit_process();

    return FAIL;
}

/*
*
*   Goal: Find substring index in null-terminated string paSub_url
*         -if no match (search returns NULL), return location of null char
*/
static char* get_char(char paSub_url[MAX_DNS_LEN], const int8_t delimiter)
{
    char* char_index = NULL;
    char_index = strchr(paSub_url, delimiter);

    if (char_index == NULL) // if missing return the full string
        return paSub_url + strlen(paSub_url);
    else
        return char_index;
}

int32_t set_port(char paSub_url[MAX_DNS_LEN], uint16_t* pPort)
{
    uint32_t port; // used to check if port w/in range 1-65535
    if (isdigit(paSub_url[1]))
    {
        // keep in host byte order
        port = (uint32_t)atoi(&paSub_url[1]); // start
                
        *pPort = (uint16_t)port;
                
        // move along the string & find first non-numeric index
        uint32_t i = 1;
        while (isdigit(paSub_url[i]))
            i++;
                
        if (err_check((strcpy_s(paSub_url, MAX_DNS_LEN * sizeof(char), paSub_url + sizeof(char) * i)) != SUCCESS,
            "strcpy()", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        {
            return FAIL;
        }
    }
    
    return SUCCESS;
}

int32_t set_inputs(Inputs_t* pInputs, const char* pHost_IP, const char* pDNS_server)
{
    assert(strlen(pHost_IP) > 0);
    assert(strlen(pDNS_server) > 0);

    const int host_length = null_strlen(pHost_IP);
    const int dns_length = null_strlen(pDNS_server);

    char* pLog_buffer = gLog_buffer;

    if (err_check((gLog_buffer = (char*)calloc(gLog_buffer_size, sizeof(char))) == NULL, "calloc() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(pInputs);
    
    append_to_log("Lookup   : ");
    append_to_log(pHost_IP);
    append_to_log("\n");

    if (err_check((pInputs->hostname_ip_lookup = (char*)calloc(host_length, sizeof(char))) == NULL, "calloc() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(pInputs);

    if (err_check((pInputs->dns_server_ip = (char*)calloc(dns_length, sizeof(char))) == NULL, "calloc() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(pInputs);

    if (err_check((strcpy_s(pInputs->hostname_ip_lookup, host_length, pHost_IP)) != SUCCESS, "strcpy() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(pInputs);

    if (err_check((strcpy_s(pInputs->dns_server_ip, dns_length, pDNS_server)) != SUCCESS, "strcpy() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return terminate_safely(pInputs);

    pInputs->tx_id = 0;
    pInputs->dns_type = 0;
    pInputs->dns_pkt_size = 0;
    pInputs->bytes_recv = -1;
    pInputs->query_string = NULL;
    pInputs->num_questions = 0;
    
    gNum_jumps = 0;
    gMax_num_jumps = 0;

    // check for valid inputs
    if (host_length > MAX_HOST_LEN || dns_length > MAX_HOST_LEN) // too long
    {
        print_usage();
        return FAIL;
    }
    if (set_query_type(pInputs) == DNS_A) // ensure it only contains [-a-zA-Z0-9] doesn't start with '-'
    {
        if (pHost_IP[0] == '-' || pHost_IP[strlen(pHost_IP)-1] == '-') // cannot start or end with '-'
        {
            print_usage();
            return FAIL;
        }

        for (int i = 0; i < (int)strlen(pHost_IP); i++)
        {
            if (isalnum(pHost_IP[i]) == 0 && (pHost_IP[i] != '-' && pHost_IP[i] != '.'))
            {
                print_usage();
                return FAIL;
            }
        }
    }

    return SUCCESS;
}

int32_t run_DNS_Lookup(Inputs_t* pInputs, char* pRecv_buff)
{
    int32_t status = SUCCESS;
    char* dns_pkt = NULL;
    char log_msg[LOG_LINE_SIZE];

    if ((dns_pkt = create_packet(pInputs)) == NULL)
        return FAIL;
    
    int32_t bytes_written = _snprintf_s(log_msg, LOG_LINE_SIZE * sizeof(char), 
                                        "Query    : %s, type %d, TXID 0x%04X\nServer   : %s\n********************************\n", 
                                        pInputs->hostname_ip_lookup, 
                                        pInputs->dns_type, 
                                        pInputs->tx_id,
                                        pInputs->dns_server_ip
                                        );
   
    if (err_check(bytes_written >= (LOG_LINE_SIZE * sizeof(char)) || bytes_written == ERR_TRUNCATION, "_snprintf_s() exceeded buffer size", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
    {
        kill_pointer((void**) &dns_pkt);
        return FAIL;
    }
    
    append_to_log(log_msg);
    if (send_query_and_get_response(pInputs, dns_pkt, pRecv_buff) != SUCCESS)
        status = FAIL;

    kill_pointer((void**) &dns_pkt);
    return status;
}

// https://tools.ietf.org/html/rfc1035 see section 4.1
/*
4.1.1. Header section format

    The header contains the following fields:

      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      ID                       |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |QR|   Opcode  |AA|TC|RD|RA|   Z    |   RCODE   |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    QDCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ANCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    NSCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                    ARCOUNT                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

ID              A 16 bit identifier assigned by the program that
                generates any kind of query.  This identifier is copied
                the corresponding reply and can be used by the requester
                to match up replies to outstanding queries.

QR              A one bit field that specifies whether this message is a
                query (0), or a response (1).

OPCODE          A four bit field that specifies kind of query in this
                message.  This value is set by the originator of a query
                and copied into the response.  The values are:

                0               a standard query (QUERY)

                1               an inverse query (IQUERY)

                2               a server status request (STATUS)

                3-15            reserved for future use

AA              Authoritative Answer - this bit is valid in responses,
                and specifies that the responding name server is an
                authority for the domain name in question section.

                Note that the contents of the answer section may have
                multiple owner names because of aliases.  The AA bit
                corresponds to the name which matches the query name, 
                or the first owner name in the answer section.

RCODE           Response code - this 4 bit field is set as part of
                responses.  The values have the following
                interpretation:

                0               No error condition

                1               Format error - The name server was
                                unable to interpret the query.

                2               Server failure - The name server was
                                unable to process this query due to a
                                problem with the name server.

                3               Name Error - Meaningful only for
                                responses from an authoritative name
                                server, this code signifies that the
                                domain name referenced in the query does
                                not exist.

                4               Not Implemented - The name server does
                                not support the requested kind of query.

                5               Refused - The name server refuses to
                                perform the specified operation for
                                policy reasons.  For example, a name
                                server may not wish to provide the
                                information to the particular requester,
                                or a name server may not wish to perform
                                a particular operation (e.g., zone 
                                transfer) for particular data.

                6-15            Reserved for future use.

QDCOUNT         an unsigned 16 bit integer specifying the number of
                entries in the question section.

ANCOUNT         an unsigned 16 bit integer specifying the number of
                resource records in the answer section.

NSCOUNT         an unsigned 16 bit integer specifying the number of name
                server resource records in the authority records
                section.

ARCOUNT         an unsigned 16 bit integer specifying the number of
                resource records in the additional records section.

4.1.3. Resource record format

The answer, authority, and additional sections all share the same
format: a variable number of resource records, where the number of
records is specified in the corresponding count field in the header.
Each resource record has the following format:

      0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                                               |
    /                                               /
    /                      NAME                     /
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      TYPE                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                     CLASS                     |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                      TTL                      |
    |                                               |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    |                   RDLENGTH                    |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--|
    /                     RDATA                     /
    /                                               /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

where:

NAME            a domain name to which this resource record pertains.

TYPE            two octets containing one of the RR type codes.  This
                field specifies the meaning of the data in the RDATA
                field.

CLASS           two octets which specify the class of the data in the
                RDATA field.

TTL             a 32 bit unsigned integer that specifies the time
                interval (in seconds) that the resource record may be
                cached before it should be discarded.  Zero values are
                interpreted to mean that the RR can only be used for the
                transaction in progress, and should not be cached.



RDLENGTH        an unsigned 16 bit integer that specifies the length in
                octets of the RDATA field.

RDATA           a variable length string of octets that describes the
                resource.  The format of this information varies
                according to the TYPE and CLASS of the resource record.
                For example, the if the TYPE is A and the CLASS is IN,
                the RDATA field is a 4 octet ARPA Internet address.


4.1.4. Message compression

In order to reduce the size of messages, the domain system utilizes a
compression scheme which eliminates the repetition of domain names in a
message.  In this scheme, an entire domain name or a list of labels at
the end of a domain name is replaced with a pointer to a prior occurance
of the same name.

The pointer takes the form of a two octet sequence:

    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    | 1  1|                OFFSET                   |
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

The first two bits are ones.  This allows a pointer to be distinguished
from a label, since the label must begin with two zero bits because
labels are restricted to 63 octets or less.  (The 10 and 01 combinations
are reserved for future use.)  The OFFSET field specifies an offset from
the start of the message (i.e., the first octet of the ID field in the
domain header).  A zero offset specifies the first byte of the ID field,
etc.

The compression scheme allows a domain name in a message to be
represented as either:

   - a sequence of labels ending in a zero octet

   - a pointer

   - a sequence of labels ending with a pointer

Pointers can only be used for occurances of a domain name where the
format is not class specific.  If this were not the case, a name server
or resolver would be required to know the format of all RRs it handled.
As yet, there are no such cases, but they may occur in future RDATA
formats.

If a domain name is contained in a part of the message subject to a
length field (such as the RDATA section of an RR), and compression is
used, the length of the compressed name is used in the length
calculation, rather than the length of the expanded name.

Programs are free to avoid using pointers in messages they generate,
although this will reduce datagram capacity, and may cause truncation.
However all programs are required to understand arriving messages that
contain pointers.

3.3.12. PTR RDATA format

    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+
    /                   PTRDNAME                    /
    +--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+--+

where:

PTRDNAME        A <domain-name> which points to some location in the
                domain name space.

PTR records cause no additional section processing.  These RRs are used
in special domains to point to some other location in the domain space.
These records are simple data, and don't imply any special processing
similar to that performed by CNAME, which identifies aliases.  See the
description of the IN-ADDR.ARPA domain for an example.

3.5. IN-ADDR.ARPA domain

The Internet uses a special domain to support gateway location and
Internet address to host mapping.  Other classes may employ a similar
strategy in other domains.  The intent of this domain is to provide a
guaranteed method to perform host address to host name mapping, and to
facilitate queries to locate all gateways on a particular network in the
Internet.

Note that both of these services are similar to functions that could be
performed by inverse queries; the difference is that this part of the
domain name space is structured according to address, and hence can
guarantee that the appropriate data can be located without an exhaustive
search of the domain space.

The domain begins at IN-ADDR.ARPA and has a substructure which follows
the Internet addressing structure.

Domain names in the IN-ADDR.ARPA domain are defined to have up to four
labels in addition to the IN-ADDR.ARPA suffix.  Each label represents
one octet of an Internet address, and is expressed as a character string
for a decimal value in the range 0-255 (with leading zeros omitted
except in the case of a zero octet which is represented by a single
zero).

Host addresses are represented by domain names that have all four labels
specified.  Thus data for Internet address 10.2.0.52 is located at
domain name 52.0.2.10.IN-ADDR.ARPA.  The reversal, though awkward to
read, allows zones to be delegated which are exactly one network of
address space.  For example, 10.IN-ADDR.ARPA can be a zone containing
data for the ARPANET, while 26.IN-ADDR.ARPA can be a separate zone for
MILNET.  Address nodes are used to hold pointers to primary host names
in the normal domain space.

Network numbers correspond to some non-terminal nodes at various depths
in the IN-ADDR.ARPA domain, since Internet network numbers are either 1,
2, or 3 octets.  Network nodes are used to hold pointers to the primary
host names of gateways attached to that network.  Since a gateway is, by
definition, on more than one network, it will typically have two or more
network nodes which point at it.  Gateways will also have host level
pointers at their fully qualified addresses.

Both the gateway pointers at network nodes and the normal host pointers
at full address nodes use the PTR RR to point back to the primary domain
names of the corresponding hosts.


*/
int32_t parse_DNS_response(Inputs_t* pInputs, char* pRecv_buff)
{
    Fixed_DNS_Header_t* dns_header = (Fixed_DNS_Header_t*) pRecv_buff;
    char* qry_str_response = pRecv_buff + sizeof(Fixed_DNS_Header_t);
    DNS_Query_Header_t* dns_query_hdr = (DNS_Query_Header_t*)(qry_str_response + null_strlen(qry_str_response));
    unsigned char* answerRR_name = (unsigned char*) ((unsigned char*)dns_query_hdr + sizeof(DNS_Query_Header_t));
    DNS_AnswerRR_Header_t* answerRR_hdr = NULL;
    
    char* current_spot = NULL;
    char log_msg[LOG_LINE_SIZE] = { 0 };
    char tmp[MAX_HOST_LEN] = { 0 };
    char name[MAX_HOST_LEN] = { 0 };
    char* qry_str_copy = tmp;
    unsigned char* ptr;

    //-------------------------------------------------
    int32_t  bytes_written = _snprintf_s(log_msg, LOG_LINE_SIZE * sizeof(char), "  TXID 0x%.4X flags 0x%.4X questions %u answers %u authority %u additional %u\n", 
                                        ntohs(dns_header->tx_id), 
                                        ntohs(dns_header->flags),
                                        ntohs(dns_header->num_questions),
                                        ntohs(dns_header->num_answers),
                                        ntohs(dns_header->num_authority),
                                        ntohs(dns_header->num_additional)
                                        );

    if (err_check(bytes_written >= (LOG_LINE_SIZE * sizeof(char)) || bytes_written == ERR_TRUNCATION, "_snprintf_s() exceeded buffer size", __FILE__, __FUNCTION__, __LINE__))
        return FAIL;
    append_to_log(log_msg);

    if (ntohs(dns_header->tx_id) != pInputs->tx_id)
    {
        append_to_log("  ++ invalid reply: TXID mismatch, sent 0x0871, received 0x0872");
    }
    if (pInputs->bytes_recv < sizeof(Fixed_DNS_Header_t))
    {
        append_to_log("  ++ invalid reply: smaller than fixed header");
    }

    // (ntohs(dns_header->flags) & RCODE_MASK) ? "failed" : "succeeded",
        //ntohs(dns_header->flags)& RCODE_MASK);

    //-------------------------------------------------

    if (ntohs(dns_header->flags) & RCODE_MASK) // ERROR in DNS Reply
        return FAIL;

    //if (err_check(pInputs->num_questions != htons(dns_header->num_questions), "Num questions doesn't match in response", __FILE__, __FUNCTION__, __LINE__))
    if (pInputs->num_questions != ntohs(dns_header->num_questions))
    {
        append_to_log("  ++ invalid reply: not enough records\n");
        return FAIL;
    }

    if (ntohs(dns_query_hdr->qry_class) != DNS_INET)
    {
        append_to_log("  ++ invalid reply: query class does not match\n");
        return FAIL;
    }

    if (ntohs(dns_query_hdr->qry_type) != DNS_A && ntohs(dns_query_hdr->qry_type) != DNS_PTR)
    {
        append_to_log("  ++ invalid reply: query type is not A or PTR\n");
        return FAIL;
    }

    //if (err_check(strcmp(pInputs->query_string, qry_str_response), "00:response qry string does not match\n", __FILE__, __FUNCTION__, __LINE__))
    //    return FAIL;

    if (err_check(strcpy_s(qry_str_copy, MAX_HOST_LEN, qry_str_response), "strcpy_s() fail\n", __FILE__, __FUNCTION__, __LINE__))
        return FAIL;
    
    append_to_log("  ------------ [questions] ----------\n");
    gMax_num_jumps = pInputs->bytes_recv; // used to check for if we get into a loop for reading compressed values
    
    if (parse_query_name(pInputs, pRecv_buff, qry_str_response, &qry_str_copy, name) != SUCCESS)
        return FAIL;

    
    //-------------------------------------------------
    bytes_written = _snprintf_s(log_msg, LOG_LINE_SIZE * sizeof(char), "        %s type %u class %u\n", qry_str_copy, ntohs(dns_query_hdr->qry_type), ntohs(dns_query_hdr->qry_class));
    if (err_check(bytes_written >= (LOG_LINE_SIZE * sizeof(char)) || bytes_written == ERR_TRUNCATION, "_snprintf_s() exceeded buffer size", __FILE__, __FUNCTION__, __LINE__))
        return FAIL;
    append_to_log(log_msg);
    //-------------------------------------------------
    
    append_to_log("  ------------ [answers] ------------\n");
    for (uint16_t i = 0; i < ntohs(dns_header->num_answers); i++)
    {
        memset((char*)name, 0, sizeof(char) * MAX_HOST_LEN);
        qry_str_copy = name;
        if (parse_query_name(pInputs, pRecv_buff, (char*)answerRR_name, &qry_str_copy, name) != SUCCESS)
            return FAIL;
        
        if (*answerRR_name == COMPRESSION_MASK)
        {
            answerRR_hdr = (DNS_AnswerRR_Header_t*)(answerRR_name + sizeof(int16_t));
        }
        else
        {
            for (ptr = answerRR_name; *ptr != NULL; ptr++)
                answerRR_hdr = (DNS_AnswerRR_Header_t*)(ptr + 1);
            
            // it stops at the null char (one byte short of header)
            answerRR_hdr = (DNS_AnswerRR_Header_t*)((char*) (answerRR_hdr) + 1);
        }

        if (ntohs(answerRR_hdr->dns_type) != DNS_A && ntohs(answerRR_hdr->dns_type) != DNS_PTR)
        {
            append_to_log("  ++ invalid reply: dns type is not A/NS/PTR/CNAME\n");
            return FAIL;
        }
        



//      Logging
        char record_string[TYPE_LEN] = { 0 };
        if (get_record_type(ntohs(answerRR_hdr->dns_class), record_string, TYPE_LEN) != SUCCESS)
            return FAIL;

        char record_data[MAX_DNS_A_LEN] = { 0 };
        char* data_ptr = (char*) (&answerRR_hdr->rd_length + 1); // pts to data string field

        if (strcmp(record_string, "A") == SUCCESS)          // get from data len and add '.'
        {
            if (getA_data(pInputs, pRecv_buff, data_ptr, record_data, ntohs(answerRR_hdr->rd_length)) != SUCCESS)
                return FAIL;
        }
        //else if (strcmp(record_string, "NS") == SUCCESS)    // get name
        //{
        //    if (getNS_data(pInputs, pRecv_buff, data_ptr, record_data) != SUCCESS)
        //        return FAIL;
        //}
        //else if (strcmp(record_string, "CNAME") == SUCCESS) // get name
        //{
        //    if (getCNAME_data(pInputs, pRecv_buff, data_ptr, record_data) != SUCCESS)
        //        return FAIL;
        //}
        //else if (strcmp(record_string, "PTR") == SUCCESS)   // get name (usually inline)
        //{
        //    if (getPTR_data(pInputs, pRecv_buff, data_ptr, record_data) != SUCCESS)
        //    return FAIL;
        //}

        bytes_written = _snprintf_s(log_msg, LOG_LINE_SIZE * sizeof(char), "        %s %s %s TTL = %u\n", qry_str_copy, record_string, record_data, ntohl (answerRR_hdr->dns_ttl));
        if (err_check(bytes_written >= (LOG_LINE_SIZE * sizeof(char)) || bytes_written == ERR_TRUNCATION, "_snprintf_s() exceeded buffer size", __FILE__, __FUNCTION__, __LINE__))
            return FAIL;
        
        append_to_log(log_msg);
        answerRR_name = (unsigned char*)(answerRR_hdr + 1) + ntohs(answerRR_hdr->rd_length);
    }
    for (uint16_t i = 0; i < ntohs(dns_header->num_authority); i++)
    {

    }
    for (uint16_t i = 0; i < ntohs(dns_header->num_additional); i++)
    {

    }
    return SUCCESS;
}

static int32_t parse_ResourceRecord(Inputs_t* pInputs, char* pRecv_buff)
{
    //if (parse_query_name(pInputs, pRecv_buff, answerRR_name, &qry_str_copy, name) != SUCCESS)
    //    return FAIL;

    return SUCCESS;
    
}

static int32_t parse_query_name(Inputs_t* pInputs, char* pRecv_buff, char* start_string, char** output_string, char* temp_buff)
{

    if ((start_string[0] & COMPRESSION_MASK) == COMPRESSION_MASK)
    {
        uint32_t next_offset = (uint16_t)(((uint8_t)start_string[0] << 10) + start_string[1]); //answer_name[0] & !COMPRESSION_MASK;

        if (get_compressed_field(pInputs, pRecv_buff, next_offset, temp_buff, MAX_HOST_LEN) != SUCCESS)
            return FAIL;

        *output_string = temp_buff;
    }
    if (query_to_host_string(pInputs, *output_string) != SUCCESS)
        return FAIL;
    (*output_string)++;

    return SUCCESS;
}

static int32_t get_compressed_field(Inputs_t* pInputs, char* pRecv_buff, uint16_t byte_offset, char* pField, uint16_t aData_length)
{
    if (update_jumps() != SUCCESS)
        return FAIL;

    int i, j;

    char* ptr = &pRecv_buff[byte_offset];
    // I first need to check every byte to see if it is a compressed value and points
    // to another field within the packet

    for (i = 0, j = 0; ptr[i] != NULL && j < aData_length; i++)
    {
        if ((ptr[i] & COMPRESSION_MASK) == COMPRESSION_MASK)
        {   // copy from ptr[0] to ptr[i-1] into buff
            //printf("compressed\n");
            //current_spot = answer_name;
            uint16_t new_offset = (((uint8_t)ptr[i] << 10) + ptr[i+1]);

            if (new_offset > pInputs->bytes_recv - 1)
            {
                append_to_log("  ++ invalid record: jump beyond packet boundary\n");
                return FAIL;
            }
            if (new_offset < sizeof(Fixed_DNS_Header_t))
            {
                append_to_log("  ++ invalid record: jump into fixed header\n");
                return FAIL;
            }

            ptr = &pRecv_buff[new_offset];
            if (update_jumps() != SUCCESS)
                return FAIL;

            i = -1; // reset index & account for loop update
        }
        else
        {
            pField[j++] = ptr[i];
        }

        if (j > aData_length)
            return FAIL;
    }
    
    return SUCCESS;
}

static int32_t update_jumps()
{
    gNum_jumps++;

    if (gNum_jumps > gMax_num_jumps)
    {
        append_to_log("  ++  invalid record: jump loop\n");
        return FAIL;
    }

    return SUCCESS;
}

static int32_t check_response_string(Inputs_t* pInputs, char* pQuery_string_response)
{
    char tmp[MAX_HOST_LEN];
    char* qry_str_copy = tmp;

    if (err_check(strcmp(pInputs->query_string, pQuery_string_response), "00:response qry string does not match\n", __FILE__, __FUNCTION__, __LINE__))
        return FAIL;

    if (err_check(strcpy_s(qry_str_copy, MAX_HOST_LEN, pQuery_string_response), "strcpy_s() fail\n", __FILE__, __FUNCTION__, __LINE__))
        return FAIL;

    return SUCCESS;
}

static char* create_packet(Inputs_t* pInputs)
{
    char* pPacket = NULL;
    /*
    //   DNS Transmit pkt
    //  __________________
    // | Fixed DNS HEADER | 12-bytes
    // |__________________|
    // |    Questions     | var_length
    // |__________________|
    // |     Answers      | var_length
    // |__________________|
    // |    Authority     | var_length
    // |__________________|
    // |   Extra ResRec   | var_length
    // |__________________|
    //     Subcomponents
    //  ------------------
    // |  TXID  |  FLAGS  | 4-bytes
    // | nQuest | nAnsRR  | 4-bytes
    // | nAuthRR| nXtraRR | 4-bytes
    // |------------------|
    // | host_IP_qry_str  | len = strlen(host_IP) + 2 (for first count and Null char)
    // | qType  | qClass  | 4-bytes
    //  ------------------
    */

    // ex: "www.yahoo.com" -> qry_str = "3www5yahoo3com"
    uint32_t host_len = null_strlen(pInputs->hostname_ip_lookup) + 1; // prepend one byte for the "3" in "3www5yahoo3com"
    uint32_t pkt_size = sizeof(Fixed_DNS_Header_t) + host_len + sizeof(DNS_Query_Header_t);

    if (err_check((pPacket = (char*)calloc(pkt_size, sizeof(char))) == NULL, "calloc() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return NULL;

    Fixed_DNS_Header_t* dns_fixed_hdr = (Fixed_DNS_Header_t*) pPacket;
    DNS_Query_Header_t* dns_query_hdr = (DNS_Query_Header_t*)(pPacket + pkt_size - sizeof(DNS_Query_Header_t));
    char* dns_query_str = pPacket + sizeof(Fixed_DNS_Header_t);

    // fixed field initialization
    dns_fixed_hdr->tx_id = htons((uint16_t)clock());
    dns_fixed_hdr->flags = htons(DNS_QUERY | DNS_RD | DNS_STDQUERY);
    dns_fixed_hdr->num_questions = htons(1);
    dns_fixed_hdr->num_answers = 0;
    dns_fixed_hdr->num_authority = 0;
    dns_fixed_hdr->num_additional = 0;

    // variable length query field initialization
    if (set_query_string(pInputs, dns_query_str, host_len) != SUCCESS)
    {
        kill_pointer((void**) &pPacket);
        return NULL;
    }
    dns_query_hdr->qry_type = htons(set_query_type(pInputs));
    dns_query_hdr->qry_class = htons(DNS_INET);

    // Update status
    pInputs->tx_id = ntohs(dns_fixed_hdr->tx_id);
    pInputs->dns_type = ntohs(dns_query_hdr->qry_type);
    pInputs->dns_pkt_size = pkt_size;
    pInputs->num_questions = ntohs(dns_fixed_hdr->num_questions);
    
    return pPacket;
}

static uint16_t set_query_type(Inputs_t* pInputs)
{
    unsigned long IP = inet_addr(pInputs->hostname_ip_lookup);

    if (IP == INADDR_NONE)  // we were given a hostname not an IP
        return DNS_A;       // forward lookup on hostname; Hostname -> IP
    else
        return DNS_PTR;     // reverse lookup on IP; IP -> Hostname
}

static int32_t set_query_string(Inputs_t* pInputs, char* pQuery_str, uint32_t aHost_len)
{
    if (strlen(pInputs->hostname_ip_lookup) <= 0 || aHost_len <= 0)
        return FAIL;

    if (err_check((strcpy_s(&pQuery_str[1], aHost_len, pInputs->hostname_ip_lookup)) != SUCCESS, "strcpy() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return FAIL;

    int32_t bytes_written = 0;
    for (char* next_token = &pQuery_str[1]; next_token[0] != NULL; )
    {
        char* current_token = strtok_s(next_token, ".", &next_token);

        // Very tricky bug discovered. if current_token is NULL and you test current_token - 1 >= pQuery_str
        // then current_token-1 wraps around and is >= pQuery_str and executes. By moving 1 to query string you avoid
        // the underflow error. Key design idea: Don't do math on ptrs more likely to be null or which change more frequently
        if (current_token >= (pQuery_str + 1) && current_token < (pQuery_str + aHost_len - 1))
        {
            //bytes_written = _snprintf_s(&current_token[-1], (size_t)null_strlen(current_token) + 1, _TRUNCATE, "%u%s", (unsigned int)strlen(current_token)-'0', current_token);
            current_token[-1] = (char) strlen(current_token);
            if (err_check(bytes_written >= null_strlen(current_token) + 1 || bytes_written == ERR_TRUNCATION, "_snprintf_s() exceeded buffer size", __FILE__, __FUNCTION__, __LINE__ - 1) != SUCCESS)
                return FAIL;
        }
    }

    pInputs->query_string = (char*) calloc(aHost_len, sizeof(char));
    
    if (err_check((strcpy_s(pInputs->query_string, aHost_len, pQuery_str)) != SUCCESS, "strcpy() failed", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
        return FAIL;

    return SUCCESS;
}

static int32_t send_query_and_get_response(Inputs_t* pInputs, char* pPacket, char* pRecv_buff)
{
    int32_t status = SUCCESS;
    sockaddr_in sender_addr;
    int sender_addr_len = sizeof(sender_addr);
    int32_t available = NULL;
    char log_msg[LOG_LINE_SIZE];
    int32_t bytes_written;
    clock_t time_start;
    clock_t time_stop;

    //Initialize WinSock; once per program run
    WSADATA wsaData;
    WORD wVersionRequested = MAKEWORD(2, 2);
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        printf("WSAStartup error %d\n", WSAGetLastError());
        WSACleanup();
        return FAIL;
    }

    // open a UDP socket
    SOCKET dns_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (dns_sock == INVALID_SOCKET)
    {
        printf("failed with %d on socket init\n", WSAGetLastError());
        WSACleanup();
        return FAIL;
    }

    // TODO: Try using different sender/receiver sockets
    // Are the sender and receiver sockets different??? I think they are
    // TODO: Add the null char at the end of the socket message I am sending
    // https://www.geeksforgeeks.org/udp-server-client-implementation-c/
    // handle errors
    struct sockaddr_in local;
    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = INADDR_ANY;
    local.sin_port = htons(0);
    if (bind(dns_sock, (struct sockaddr*) &local, sizeof(local)) == SOCKET_ERROR)
    {
        bytes_written = _snprintf_s(log_msg, LOG_LINE_SIZE * sizeof(char), "socket error: %d\n", WSAGetLastError());
        err_check(bytes_written >= (LOG_LINE_SIZE * sizeof(char)) || bytes_written == ERR_TRUNCATION, "_snprintf_s() exceeded buffer size", __FILE__, __FUNCTION__, __LINE__);
        append_to_log(log_msg);
        return FAIL;
    }

    struct sockaddr_in remote;
    memset(&remote, 0, sizeof(remote));
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = inet_addr(pInputs->dns_server_ip);      // Google DNS Server
    remote.sin_port = htons(DNS_PORT);                               // DNS port on server
    
    for (int attempts = 0; attempts < MAX_ATTEMPTS; attempts++)
    {
        bytes_written = _snprintf_s(log_msg, LOG_LINE_SIZE * sizeof(char),
                                    "Attempt %d with %d bytes... ",
                                    attempts,
                                    pInputs->dns_pkt_size
                                    );

        if (err_check(bytes_written >= (LOG_LINE_SIZE * sizeof(char)) || bytes_written == ERR_TRUNCATION, "_snprintf_s() exceeded buffer size", __FILE__, __FUNCTION__, __LINE__) != SUCCESS)
            status = FAIL;

        append_to_log(log_msg);
        if (sendto(dns_sock, pPacket, pInputs->dns_pkt_size, 0, (struct sockaddr*) &remote, sizeof(remote)) == SOCKET_ERROR)
        {
            bytes_written = _snprintf_s(log_msg, LOG_LINE_SIZE * sizeof(char), "socket error: %d\n", WSAGetLastError());
            err_check(bytes_written >= (LOG_LINE_SIZE * sizeof(char)) || bytes_written == ERR_TRUNCATION, "_snprintf_s() exceeded buffer size", __FILE__, __FUNCTION__, __LINE__);
            append_to_log(log_msg); 
            status = FAIL;
        }

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
        FD_SET(dns_sock, &fd_reader);
        FD_SET(dns_sock, &fd_exception);
        available = select(0, &fd_reader, 0, &fd_exception, &timeout);
        time_start = clock();
        
        if (available > 0)
        {
            pInputs->bytes_recv = recvfrom(dns_sock, pRecv_buff, MAX_DNS_LEN * sizeof(char), 0, (SOCKADDR*)& sender_addr, &sender_addr_len);
            if (pInputs->bytes_recv > 0)
            {
                time_stop = clock();
                bytes_written = _snprintf_s(log_msg, LOG_LINE_SIZE * sizeof(char), "response in %d ms with %d bytes", time_stop - time_start, pInputs->bytes_recv);
                err_check(bytes_written >= (LOG_LINE_SIZE * sizeof(char)) || bytes_written == ERR_TRUNCATION, "_snprintf_s() exceeded buffer size", __FILE__, __FUNCTION__, __LINE__);
                append_to_log(log_msg);
                status = SUCCESS;
                break;
            }
        }
        else    // timeout occured
        {
            time_stop = clock();
            bytes_written = _snprintf_s(log_msg, LOG_LINE_SIZE * sizeof(char), "timeout: %d\n", time_stop - time_start);
            err_check(bytes_written >= (LOG_LINE_SIZE * sizeof(char)) || bytes_written == ERR_TRUNCATION, "_snprintf_s() exceeded buffer size", __FILE__, __FUNCTION__, __LINE__);
            append_to_log(log_msg);
        }

        status = FAIL;
    }
    closesocket(dns_sock);
    
    return status;
}

static int32_t recurse_string_for_commas(char* pIn_string, int32_t strlen)
{
    int32_t status = SUCCESS;
    uint8_t offset = pIn_string[0] + 1;
    
    if (err_check(offset >= strlen, "error recursing\n", __FILE__, __FUNCTION__, __LINE__))
        return FAIL;
    
    if (pIn_string[offset] == NULL)
    {
        pIn_string[0] = '.';
        return SUCCESS;
    }
    
    status = recurse_string_for_commas(&pIn_string[offset], strlen - offset);
    if (status == SUCCESS)
        pIn_string[0] = '.';

    return status;
}

static int32_t query_to_host_string(Inputs_t* pInputs, char* pQuery_str)
{
    if (recurse_string_for_commas(pQuery_str, null_strlen(pQuery_str)) != SUCCESS)
        return FAIL;

    if (strcmp(pInputs->hostname_ip_lookup, pQuery_str + 1) != SUCCESS)
    {
        if (strstr(pInputs->hostname_ip_lookup, pQuery_str + 1) == NULL)
            append_to_log("  ++ invalid record: truncated name");
        return FAIL;
    }

    return SUCCESS;
}

static int32_t parse_answer_data(Inputs_t* pInputs, char* pRecv_buff, char* start_string, char* temp_buff, uint16_t aData_length)
{
    if (start_string < pRecv_buff)
        return FAIL;

    char* ptr = start_string;
    uint32_t next_offset = (ptr - pRecv_buff);

    if ((*ptr & COMPRESSION_MASK) == COMPRESSION_MASK)
        next_offset = (uint16_t)(((uint8_t)ptr[0] << 10) + ptr[1]); //answer_name[0] & !COMPRESSION_MASK;

    gNum_jumps++; // undo what will have happened
    if (get_compressed_field(pInputs, pRecv_buff, next_offset, temp_buff, aData_length) != SUCCESS)
        return FAIL;

    return SUCCESS;
}

static int32_t get_record_type(uint16_t record, char* val, uint32_t length)
{
    int32_t status = SUCCESS;

    if (record == DNS_A)
        status = strcpy_s(val, length, "A");
    else if (record == DNS_NS)
        status = strcpy_s(val, length, "NS");
    else if (record == DNS_CNAME)
        status = strcpy_s(val, length, "CNAME");
    else if (record == DNS_PTR)
        status = strcpy_s(val, length, "PTR");
    else if (record == DNS_HINFO)
        status = strcpy_s(val, length, "HINFO");
    else if (record == DNS_MX)
        status = strcpy_s(val, length, "MX");
    else if (record == DNS_AXFR)
        status = strcpy_s(val, length, "AXFR");
    else
        status = FAIL;
    
    return status;

}

static int32_t getA_data(Inputs_t* pInputs, char* pRecv_buff, char* pData, char* pRecord_data, uint16_t aData_length)
{
    unsigned char byte_IP[MAX_HOST_LEN] = { 0 };

    if (parse_answer_data(pInputs, pRecv_buff, pData, (char*)byte_IP, aData_length) != SUCCESS)
        return FAIL;

    if (strlen((char*)byte_IP) != aData_length)
        return FAIL;

    int32_t bytes_written = sprintf_s(pRecord_data, MAX_DNS_A_LEN * sizeof(char), "%u.%u.%u.%u", byte_IP[0], byte_IP[1], byte_IP[2], byte_IP[3]);
    if (err_check(bytes_written >= (LOG_LINE_SIZE * sizeof(char)) || bytes_written == ERR_TRUNCATION, "sprintf_s() exceeded buffer size", __FILE__, __FUNCTION__, __LINE__))
        return FAIL;

    return SUCCESS;
}

static int32_t getNS_data(Inputs_t* pInputs, char* pRecv_buff, char* pData, char* pRecord_data)
{

    return SUCCESS;
}

static int32_t getCNAME_data(Inputs_t* pInputs, char* pRecv_buff, char* pData, char* pRecord_data)
{

    return SUCCESS;
}

static int32_t getPTR_data(Inputs_t* pInputs, char* pRecv_buff, char* pData, char* pRecord_data)
{

    return SUCCESS;
}