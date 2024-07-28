#include <windows.h>
#include <evntrace.h>
#include "beacon.h"

/* Havoc console output modes */
#define HAVOC_CONSOLE_GOOD 0x90
#define HAVOC_CONSOLE_INFO 0x91
#define HAVOC_CONSOLE_ERRO 0x92

DECLSPEC_IMPORT WINBASEAPI LPVOID   WINAPI  KERNEL32$VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
DECLSPEC_IMPORT WINBASEAPI BOOL     WINAPI  KERNEL32$VirtualFree(LPVOID, SIZE_T, DWORD);
DECLSPEC_IMPORT DWORD	WINAPI KERNEL32$GetLastError(VOID);
DECLSPEC_IMPORT ULONG WINAPI ADVAPI32$QueryAllTracesA(PEVENT_TRACE_PROPERTIES* PropertyArray, ULONG PropertyArrayCount, PULONG LoggerCount);


const unsigned MAX_SESSION_NAME_LEN = 1024;
const unsigned MAX_LOGFILE_PATH_LEN = 1024;
const unsigned PropertiesSize = sizeof(EVENT_TRACE_PROPERTIES) + (MAX_SESSION_NAME_LEN * sizeof(char)) + (MAX_LOGFILE_PATH_LEN * sizeof(char));

VOID go(PVOID Buffer, ULONG Length)
{
    ULONG sessionCount;

    EVENT_TRACE_PROPERTIES** sessions = NULL;
    BYTE* buffer = NULL;

    sessionCount = 64;

    sessions = (EVENT_TRACE_PROPERTIES**)KERNEL32$VirtualAlloc(NULL, sessionCount * sizeof(EVENT_TRACE_PROPERTIES*), MEM_COMMIT, PAGE_READWRITE);
    if (!sessions)
    {
        BeaconPrintf(HAVOC_CONSOLE_ERRO, "Error VirtualAlloc: %lu", KERNEL32$GetLastError());
        goto CLEAR;
    }

    buffer = (BYTE*)KERNEL32$VirtualAlloc(NULL, PropertiesSize * sessionCount, MEM_COMMIT, PAGE_READWRITE);
    if (!buffer)
    {
        BeaconPrintf(HAVOC_CONSOLE_ERRO, "Error VirtualAlloc: %lu", KERNEL32$GetLastError());
        goto CLEAR;
    }

    for (size_t i = 0; i != sessionCount; i += 1)
    {
        sessions[i] = (EVENT_TRACE_PROPERTIES*)&buffer[i * PropertiesSize];
        sessions[i]->Wnode.BufferSize = PropertiesSize;
        sessions[i]->LoggerNameOffset = sizeof(EVENT_TRACE_PROPERTIES);
        sessions[i]->LogFileNameOffset = sizeof(EVENT_TRACE_PROPERTIES) + (MAX_SESSION_NAME_LEN * sizeof(char));
    }

    if (ADVAPI32$QueryAllTracesA(&sessions[0], sessionCount, &sessionCount) != ERROR_SUCCESS)
    {
        BeaconPrintf(HAVOC_CONSOLE_ERRO, "Error QueryAllTracesA: %lu\n", KERNEL32$GetLastError());
        goto CLEAR;
    }

    for (ULONG i = 0; i < sessionCount; i++)
    {
        GUID guid = sessions[i]->Wnode.Guid;

        BeaconPrintf(
            CALLBACK_OUTPUT,
            "Guid: %08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX\n"
            "Session ID: %llu\n"
            "Session name: %s\n"
            "Log file: %s\n",
            guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7],
            sessions[i]->Wnode.HistoricalContext,
            ((LPCBYTE)sessions[i] + sessions[i]->LoggerNameOffset),
            ((LPCBYTE)sessions[i] + sessions[i]->LogFileNameOffset)
        );
    }

CLEAR:
    if (buffer)
    {
        KERNEL32$VirtualFree(buffer, 0, MEM_RELEASE);
    }

    if (sessions)
    {
        KERNEL32$VirtualFree(sessions, 0, MEM_RELEASE);
    }

    return;
}
