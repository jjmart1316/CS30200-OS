/*
Course: CS 30200
Assignment: 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

//Provides the user with a selection of 5 programs. returns the number of the program selected
int userInput();
//binds the executable program's address to a variable
void programPath(char *arr, int num);
//Prints error if createprocess fails
void printError(char* functionName);

int main(VOID) {

    int programNumber;
    char lpCommandLine[256];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory( &si, sizeof si );
    ZeroMemory( &pi, sizeof pi );
    si.cb = sizeof si;
    si.lpTitle = "What is your command?";
    si.dwFlags = STARTF_USEFILLATTRIBUTE | STARTF_USEPOSITION;
    si.dwFillAttribute = FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY;
    _putenv("PROMPT=Speak to me$G");

    do {
        programNumber = userInput();
        //if programNumber == 0, the program exits
        if ( programNumber != 0 ) {

            programPath( lpCommandLine, programNumber );
            
            if ( !CreateProcess( NULL, lpCommandLine, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi ) ) {    
                printError("CreateProcess");
            } else {
                //prints the process ID
                printf("Started program %d with pid = %d\n\n", programNumber, (int)pi.dwProcessId);

                //waits for CMD to exit and prints the termination status
                if (programNumber == 3) {
                    printf("  waiting for program %d to terminate..\n", programNumber);
                    WaitForSingleObject(pi.hProcess, INFINITE);
                    DWORD exitCode = 0;
                    if ( GetExitCodeProcess(pi.hProcess, &exitCode) ) {
                        printf("  Program %d exited with return value %ld\n\n", programNumber, exitCode);
                    }
                }
                CloseHandle( pi.hProcess );
                CloseHandle( pi.hThread );
            }
        }
    } while( programNumber != 0 );
    return 0;
}

int userInput() {
    const char *programList[] = {"Quit", "Run Notepad", "Run Wordpad", "Run cmd shell", "Run Calculator", "Run Explorer"};
    int input = 0;
    printf("Please make a choice from the following list.\n");
    for (int i = 0; i < 6; i++) {
        if (i == 3) {
            printf(" *%1d: %s\n", i, programList[i]);
        } else {
            printf(" %2d: %s\n", i, programList[i]);
        }
    }
    printf("Enter your choice now:  ");
    scanf("%d", &input);
    return input;
}

void printError(char* functionName) {
    LPVOID lpMsgBuf;
    int error_no;
    error_no = GetLastError();
    FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            error_no,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            (LPTSTR) &lpMsgBuf,
            0,
            NULL
    );
    // Display the string.
    fprintf(stderr, "\n%s failed on error %d: ", functionName, error_no);
    fprintf(stderr, (char*)lpMsgBuf);
    LocalFree( lpMsgBuf );
}

void programPath(char *arr, int num) {
    if (num == 1) {
        sprintf(arr, "%s\\notepad.exe", getenv("windir"));
    } else if (num == 2) {
        sprintf(arr, "%s\\Windows NT\\Accessories\\wordpad.exe", getenv("ProgramFiles"));
    } else if (num == 3) {
        sprintf(arr, "%s", getenv("ComSpec"));
    } else if (num == 4) {
        sprintf(arr, "%s\\System32\\calc.exe", getenv("windir"));
    } else if (num == 5) {
        sprintf(arr, "%s\\explorer.exe", getenv("windir"));
    } else {
        //if user types a non-valid number, the binding variable receives a nul character
        *arr = 0;
    }
}
