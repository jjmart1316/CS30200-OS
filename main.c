#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

void printError(char* functionName);

typedef struct processor_data {
    int affinityMask;                /* affinity mask of this processor (just one bit set) */
    PROCESS_INFORMATION processInfo; /* process currently running on this processor */
    int running;                     /* 1 when this processor is running a task, 0 otherwise */
} ProcessorData;





int main(int argc, char **argv) {

    int processorCount = 0;         //The number of allocated processors
    ProcessorData *processorData;   //An array of ProcessorData structures
    HANDLE *processHandles;         //An array of handles to processes
    HANDLE hProcess = GetCurrentProcess();
    DWORD_PTR processAffinityMask;
    DWORD_PTR systemAffinityMask;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof(si));
    ZeroMemory( &pi, sizeof(pi));
    si.cb = sizeof(si);


    if (argc < 3) {
        fprintf(stderr, "usage, %s  SCHEDULE_TYPE  SECONDS...\n", argv[0]);
        fprintf(stderr, "Where: SCHEDULE_TYPE = 0 means \"first come first serve\"\n");
        fprintf(stderr, "       SCHEDULE_TYPE = 1 means \"shortest job first\"\n");
        fprintf(stderr, "       SCHEDULE_TYPE = 2 means \"longest job first\"\n");
        return 0;
    }

    //read the job duration times off the command-line
    int jobDurationTimes[argc - 1];
    for (int i = 1; i < argc; i++) {
        jobDurationTimes[ i - 1 ] = atoi(argv[i]);
    }

    //get the processor affinity mask for this process
    if (!GetProcessAffinityMask(hProcess, &processAffinityMask, &systemAffinityMask)) {
        printError("GetProcessAffinityMask");
        return 0;
    }

    //count the number of processors set in the affinity mask
    int bitPos = 0;
    do {
        if ( (1 << bitPos) & processAffinityMask ) {
            processorCount++;
            //printf("1"); //delete later
        } else {
            //printf("0"); //delete later
        }
        bitPos++;
    } while (bitPos < 32);

    //create, and then initialize, the processor pool data structure
    processorData = malloc( processorCount * sizeof(ProcessorData) );

    for (int i = 0; i < processorCount; i++) {
        processorData[i].affinityMask = processAffinityMask;
        processorData[i].running = 0;
    }

    //start the first group of processes
    for (int i = 0; i < processorCount; i++) {
        if (!CreateProcess( "main.exe", NULL, NULL, NULL, FALSE, CREATE_NEW_CONSOLE | CREATE_SUSPENDED, NULL, NULL, &si, &pi )) {
            printError("CreateProcess");
        }

        if (!SetProcessAffinityMask(hProcess, processAffinityMask)) {
            printError("SetProcessAffinityMask");
        }

        if (ResumeThread(pi.hThread) == -1) {
            printError("ResumeThread");
        }
        processorData[i].processInfo = pi;
        processorData[i].running = 1;

        }


    return 0;
}




void printError(char* functionName) {
    LPVOID lpMsgBuf;
    int error_no;
    error_no = GetLastError();
    FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            error_no,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* default language */
            (LPTSTR) &lpMsgBuf,
            0,
            NULL
    );
    /* Display the string. */
    fprintf(stderr, "\n%s failed on error %d: ", functionName, error_no);
    fprintf(stderr, (const char*)lpMsgBuf);
    /* Free the buffer. */
    LocalFree( lpMsgBuf );
}