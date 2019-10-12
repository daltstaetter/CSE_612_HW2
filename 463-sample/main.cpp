/* HW1-1
 * main.cpp
 * CSCE 463 Sample Code
 * by Dmitri Loguinov
 *
 * Date: 8/29/2019
 * Modified by Dalton Altstaetter
 * with permission for HW1-1
 * CSE 612
 * Fall 2019
 */

#include "pch.h"
#include "main.h"
#include "globals.h"
#define COUNT_URLS
Parameters* gParams = new Parameters();

//_CrtMemState s1, s2, s3, s4, s5, s6, s7, s8, s9, s10;




int32_t main(int32_t argc, char* argv[])
{
   // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF);

    int32_t status = SUCCESS;
    gNum_cmdline_args = argc;
    gNum_init_threads = -1;
    const char* pNum_threads = (const char*) argv[1];

    if (isdigit(*pNum_threads) && atoi(pNum_threads) > 0)
        gNum_init_threads = atoi(pNum_threads);

    if (argc == SINGLE_URL_INPUT && starts_with("http://", argv[1]))
        status = process_single_url_input(argv[1], NULL);
    else if (argc == URL_FILE_INPUT && isdigit(*pNum_threads) && (gNum_init_threads >= SINGLE_THREAD))
    {
        if (gNum_init_threads == SINGLE_THREAD)
            main_multithread(gNum_init_threads=1, argv[2]); //status = process_file_url_input(argv[1], argv[2]);
        else
            main_multithread(gNum_init_threads, argv[2]);
    }
    else
        status = print_usage();

    return status;
}

int32_t process_single_url_input(const char* paIn_url, HTMLParserBase* paParserBase)
{
    int32_t head_request_size = 0;;
    int32_t request_size = 0;
    
    url_t* url_struct = NULL;
    const char* request = NULL;
    const char* html_response1 = NULL;
    const char* html_response2 = NULL;
    const char* head_request = NULL;
    int32_t stat = SUCCESS;

    if (paIn_url == NULL || !starts_with("http://", paIn_url))
        return NULL; // invalid input, expecting a url

    if (gNum_init_threads <= SINGLE_THREAD)
        printf("URL: %s\n", paIn_url);

    if (gNum_init_threads <= SINGLE_THREAD)
        printf("\tParsing URL... ");
    
    url_struct = parse_url(paIn_url);
    
    

    if (url_struct != NULL)
    {

        //EnterCriticalSection(gParams->pCriticalSection);
        //_CrtMemCheckpoint(&s1);
        //_CrtMemDumpStatistics(&s1);

        if (gNum_cmdline_args == SINGLE_URL_INPUT)
        {
            if (gNum_init_threads <= SINGLE_THREAD)
                printf("host %s, port %d, request %s%s\n", url_struct->host, url_struct->port, url_struct->path, url_struct->query);

            request = create_get_request(url_struct, &request_size, "GET");
        }
        else // file input
        {
            if (gNum_init_threads <= SINGLE_THREAD)
                printf("host %s, port %d\n", url_struct->host, url_struct->port);
            head_request = create_head_request(url_struct, &head_request_size);
            //_CrtMemCheckpoint(&s5);
            //_CrtMemDumpStatistics(&s5);
            //{
            //    uint32_t memstat = _CrtMemDifference(&s6, &s4, &s5);
            //    if (memstat == 1)
            //        printf("# 1\n");
            //}

            request = create_get_request(url_struct, &request_size, "GET");

            //_CrtMemCheckpoint(&s5);
            //_CrtMemDumpStatistics(&s5);
            //{
            //    uint32_t memstat = _CrtMemDifference(&s6, &s4, &s5);
            //    if (memstat == 1)
            //        printf("## 2\n");
            //}

            //--------------------------
            // Robot Check
            //--------------------------
            //_CrtMemCheckpoint(&s7);
            html_response1 = send_request(url_struct, head_request, head_request_size, ROBOT);
            //_CrtMemCheckpoint(&s8);
            //_CrtMemDumpStatistics(&s8);
            //{
            //    uint32_t memstat = _CrtMemDifference(&s9, &s7, &s8);
            //    if (memstat == 1)
            //        printf("### 3\n");
            //}
            if (parse_response(html_response1, url_struct, ROBOT, paParserBase))
                stat = 1; // not unique or error occurred

            //_CrtMemCheckpoint(&s5);
            //_CrtMemDumpStatistics(&s5);
            //{
            //    uint32_t memstat = _CrtMemDifference(&s6, &s8, &s5);
            //    if (memstat == 1)
            //        printf("#### 4\n");
            //}
        }

        if (stat == SUCCESS)
        {
            //--------------------------
            // Web page check
            //--------------------------
            html_response2 = send_request(url_struct, request, request_size, 0);
            //_CrtMemCheckpoint(&s8);
            //_CrtMemDumpStatistics(&s8);
            //{
            //    uint32_t memstat = _CrtMemDifference(&s9, &s7, &s8);
            //    if (memstat == 1)
            //        printf("##### 5\n");
            //}

            parse_response(html_response2, url_struct, 0, paParserBase);
            //_CrtMemCheckpoint(&s6);
            //{
            //    uint32_t memstat = _CrtMemDifference(&s9, &s6, &s8);
            //    if (memstat == 1)
            //        printf("###### 6\n");
            //}
        }
        //_CrtMemCheckpoint(&s9);
        //_CrtMemDumpStatistics(&s9);
        //{ 
        //    uint32_t memstat = _CrtMemDifference(&s3, &s7, &s9); 
        //    if (memstat == 1)
        //        printf("####### 7\n");
        //}
        //_CrtMemDumpStatistics(&s3);
        kill_pointer((void**)& head_request);
        kill_pointer((void**)& html_response1);
        kill_pointer((void**)& html_response2);
        kill_pointer((void**)& request);

        //_CrtMemCheckpoint(&s2);
        //_CrtMemDumpStatistics(&s2);
        //uint32_t memstat = _CrtMemDifference(&s3, &s7, &s2);
        //_CrtMemDumpStatistics(&s3);
        //if (memstat == 1)
        //    printf("^\n");
        //LeaveCriticalSection(gParams->pCriticalSection);

        kill_pointer((void**)& url_struct);
    }
    return SUCCESS;
}

int32_t process_file_url_input(const char* paNum_threads, const char* paFilename)
{
    int32_t num_threads = -1;
    char fbuff[MAX_URL_LEN];
    long fsize;
    int32_t status;
    // char test[] = "http://www.washington.edu/";
    
    if (isdigit(*paNum_threads) && atoi(paNum_threads) > 0)
        num_threads = atoi(paNum_threads);

    if (num_threads != SINGLE_THREAD)
    {
        printf("Use only one thread\n");
        print_usage();
        return 1;
    }

    FILE* fstream;

    if (fopen_s(&fstream, paFilename, "r"))
    {
        printf("%s does not exists. Check the path\n", paFilename);
        return 1;
    }

    if (fstream == 0)
    {
        printf("\nfailed to read %s\n", paFilename);
        return 1;
    }
    
    // find the file size in bytes
    fseek(fstream, 0, SEEK_END);
    fsize = ftell(fstream);
    rewind(fstream);
    char* next_token = NULL;

    printf("Opened %s with size %lu\n", paFilename, fsize);

    uint32_t count = 1;
    while (fgets(fbuff, MAX_URL_LEN * sizeof(char), fstream))
    {
#ifdef COUNT_URLS
        printf("\t\t\tcount: \t%d\n", count++);
#endif // COUNT_URLS
        process_single_url_input((const char*) strtok_s(fbuff, "\n", &next_token), NULL);
    }

    if (status = ferror(fstream))
    {
        printf("\nfailed to read %s\n", paFilename);
        fclose(fstream);
        return 1;
    }

    fclose(fstream);
    return SUCCESS;
}

void print_stats()
{
    static uint32_t num_interrupts = 0;
    static clock_t t1 = 0;;

    num_interrupts++;                                            // TODO: find out why this thread is starting more than once every two seconds. I have hacked it.
    //if ((clock() - gParams->time_millis) < MILLIS_PER_INTERRUPT) // this could be a problem bc I think I am interrupting this at a much greater frequency than once every two seconds
    //    return;
    //gParams->time_millis = num_interrupts * SECONDS_PER_INTERRUPT * MILLIS_TO_SECONDS;
    printf("[%3u] %4d Q %6d E %7d H %6d D %6d I %5d R %5d C %5d L %4d\n", gParams->time_millis / MILLIS_TO_SECONDS,
                                                                          gParams->num_running_threads,
                                                                          (uint32_t) gParams->pQueue->size(),
                                                                          gParams->num_extracted_urls,
                                                                          (uint32_t) (*gSeen_hosts).size(),
                                                                          gParams->num_DSN_lookups,
                                                                          (uint32_t) (*gSeen_IPs).size(),
                                                                          gParams->num_passed_robot_checks,
                                                                          gParams->num_crawled_urls,
                                                                          gParams->num_links_parsed);

    printf("      *** crawling %.1f pps @ %.1f Mbps\n", ((double)gParams->num_crawled_urls * MILLIS_TO_SECONDS) / (double)gParams->time_millis
                                                      , ((double)gParams->num_bytes_read   * MILLIS_TO_SECONDS) / ((double)gParams->time_millis * BYTES_TO_MBYTES));      // bytes/ms * 1000 ms/s * (1 Mb/1e6 bytes) ->Mb/s
    t1 = clock();
}

int32_t WINAPI StatsRun(LPVOID lpPara)
{
    // shared parameters
    Parameters* p = ((Parameters*)lpPara);
    clock_t time_diff = clock();
    //int32_t tmp;
    
    while (WaitForSingleObject(p->eventQuit, MILLIS_PER_INTERRUPT) == WAIT_TIMEOUT)
    {            
        EnterCriticalSection(gParams->pCriticalSection);
        //tmp = clock() - time_diff;
        gParams->time_millis += (MILLIS_PER_INTERRUPT); // (clock() - time_diff);
        print_stats();
        if (p->pQueue->size() == 0 && p->num_running_threads == 0)
            SetEvent(p->eventQuit);
        //time_diff = clock();
        LeaveCriticalSection(gParams->pCriticalSection);
    }
        
    return 0;
}

int32_t fill_URL_queue(LPVOID lpPara, const char* paFilename)
{
    // shared parameters
    Parameters* p = (Parameters*)lpPara;
    char fbuff[MAX_URL_LEN];
    long fsize;
    int32_t status;
    FILE* fstream;
    char* input_url = NULL;
    
    if (fopen_s(&fstream, paFilename, "r"))
    {
        printf("%s does not exists. Check the path\n", paFilename);
        return 1;
    }

    if (fstream == 0)
    {
        printf("\nfailed to read %s\n", paFilename);
        return 1;
    }

    // find the file size
    fseek(fstream, 0, SEEK_END);
    fsize = ftell(fstream);
    rewind(fstream);
    char* next_token = NULL;

    printf("Opened %s with size %lu\n", paFilename, fsize);

    while (fgets(fbuff, MAX_URL_LEN * sizeof(char), fstream))
    {
        input_url = strtok_s(fbuff, "\n", &next_token);
        if(input_url)
            p->pQueue->push((string)input_url);
    }
    
#ifdef DEBUG    
    while (!p->pQueue->empty())
    {
        printf("%s\n", p->pQueue->front());
        p->pQueue->pop();
    }
#endif // DEBUG
        

    if (status = ferror(fstream))
    {
        printf("\nfailed to read %s\n", paFilename);
        fclose(fstream);
        return 1;
    }

    fclose(fstream);
    return SUCCESS;
}

int32_t main_multithread(int32_t aNum_threads, const char* paFilename)
{
    if (aNum_threads < 1 || paFilename == NULL)
        return 1;

    HANDLE *handles = new HANDLE[aNum_threads + 1];
    Parameters* p = gParams;
    gSeen_IPs = new unordered_set<DWORD>;
    gSeen_hosts = new unordered_set<string>;

    // create an object for accessing critical sections (including printf); initial state = not locked
    //p->mutex = CreateMutex(NULL, 0, NULL);

    p->pCriticalSection = new CRITICAL_SECTION;
    InitializeCriticalSection(p->pCriticalSection);
    //InitializeCriticalSectionAndSpinCount(p->pCriticalSection, 0);

    EnterCriticalSection(p->pCriticalSection);
    // initialize default values
    p->pQueue = new queue<string>;
    p->num_running_threads = 0;
    p->num_extracted_urls = 0;
    p->num_DSN_lookups = 0;
    p->num_passed_robot_checks = 0;
    p->num_crawled_urls = 0;
    p->http_codes->two_hundreds = 0;
    p->http_codes->three_hundreds = 0;
    p->http_codes->four_hundreds = 0;
    p->http_codes->five_hundreds = 0;
    p->http_codes->other = 0;
    p->num_links_parsed = 0;
    p->num_bytes_read = 0;
    p->time_millis = 1;
    p->num_tamu_links = 0;
    p->from_outside_tamu = 0;
    
    // create a quit event; manual reset, initial state = not signaled
    p->eventQuit = CreateEvent(NULL, true, false, NULL);

    fill_URL_queue(p, paFilename);
    LeaveCriticalSection(p->pCriticalSection);

    // structure p is the shared space between the threads
    handles[0] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StatsRun, p, 0, NULL);     // start threadA (instance #1) 
    
    for (int i = 1; i < aNum_threads + 1; i++)
        handles[i] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)thread_crawler, p, 0, NULL);     // start threadA (instance #2)


    // make sure this thread hangs here until the other three quit; otherwise, the program will terminate prematurely
    for (int i = 0; i < aNum_threads + 1; i++)
        WaitForSingleObject(handles[0], INFINITE);
    
    for (int i = 0; i < aNum_threads+1; i++)
        CloseHandle(handles[i]); 

    print_multithread_results();

    delete (void*)gSeen_IPs;
    delete (void*)gSeen_hosts;

    return SUCCESS;
}

int32_t WINAPI thread_crawler(LPVOID lpPara)
{
    Parameters* p = ((Parameters*)lpPara);
    HTMLParserBase* parser = NULL;
    char* pCurr_url = NULL;
    // = (char*)malloc(MAX_URL_LEN * sizeof(char));
//    string tmp_str;

    try { parser = new HTMLParserBase; }
    catch (std::bad_alloc e) { 
        WaitForSingleObject(p->eventQuit, INFINITE);
        return 0;
    }

    EnterCriticalSection(p->pCriticalSection);
    p->num_running_threads++;
    LeaveCriticalSection(p->pCriticalSection);

    if (gNum_init_threads == 1) gNum_init_threads = 2;// SINGLE_THREAD;

    while (true)
    {
        EnterCriticalSection(p->pCriticalSection);        // WaitForSingleObject(p->mutex, INFINITE); // lock mutex

        if (p->pQueue->size() == 0)                        // finished crawling?
        {
            p->num_running_threads--; // remove a running thread and wait for other threads to finish
            //kill_pointer((void**)& pCurr_url);
            if (parser) delete parser;
            LeaveCriticalSection(p->pCriticalSection);    // ReleaseMutex (p->mutex);    // release critical section
            break;
        }

        pCurr_url = (char*)malloc((p->pQueue->front().size() + 1) * sizeof(char));
        if (pCurr_url)
        {
            strcpy_s(pCurr_url, (p->pQueue->front().size() + 1) * sizeof(char), (char*)p->pQueue->front().c_str());
            //        tmp_str = p->pQueue->front();
            p->pQueue->pop();
            p->num_extracted_urls++;
            LeaveCriticalSection(p->pCriticalSection);        // ReleaseMutex (p->mutex);    // release critical section

            process_single_url_input((const char*)pCurr_url, parser);
            free(pCurr_url);
            pCurr_url = NULL;
        }
        else
            LeaveCriticalSection(p->pCriticalSection);        // ReleaseMutex (p->mutex);    // release critical section
    }

    // Wait for stats to decide when to break
    WaitForSingleObject(p->eventQuit, INFINITE);

    return SUCCESS;
}


void print_multithread_results(void)
{
    if (gNum_init_threads > SINGLE_THREAD)
    {
        EnterCriticalSection(gParams->pCriticalSection);
        printf("\n");
        printf("Extracted %u URLS @ %u/s\n", gParams->num_extracted_urls, (gParams->num_extracted_urls * MILLIS_TO_SECONDS) / gParams->time_millis);
        printf("Looked up %6u DNS names @ %u/s\n", gParams->num_DSN_lookups, (gParams->num_DSN_lookups * MILLIS_TO_SECONDS) / gParams->time_millis);
        printf("Downloaded %5u robots @ %u/s\n", gParams->num_passed_robot_checks, (gParams->num_passed_robot_checks * MILLIS_TO_SECONDS) / gParams->time_millis);
        printf("Crawled %5u pages @ %u/s (%4.2f MB)\n", gParams->num_crawled_urls, (gParams->num_crawled_urls * MILLIS_TO_SECONDS) / gParams->time_millis,
                                                     ((double) gParams->num_bytes_read / BYTES_TO_MBYTES));
        printf("Parsed %4d links @ %d/s\n", gParams->num_links_parsed, gParams->num_links_parsed  / (gParams->time_millis / MILLIS_TO_SECONDS));
        printf("\nHTTP codes: 2xx = %u, 3xx = %u, 4xx = %u, 5xx = %u, other = %u\n", gParams->http_codes->two_hundreds, 
                                                                                   gParams->http_codes->three_hundreds, 
                                                                                   gParams->http_codes->four_hundreds, 
                                                                                   gParams->http_codes->five_hundreds, 
                                                                                   gParams->http_codes->other);
        printf("TAMU links found: %u\n", gParams->num_tamu_links);
        printf("TAMU links outside domain: %u\n", gParams->from_outside_tamu);
        LeaveCriticalSection(gParams->pCriticalSection);
    }
}
