#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <psapi.h>
#include "beacon.h"

/* Havoc console output modes */
#define HAVOC_CONSOLE_GOOD 0x90
#define HAVOC_CONSOLE_INFO 0x91
#define HAVOC_CONSOLE_ERRO 0x92

DECLSPEC_IMPORT WINBASEAPI LPVOID   WINAPI  KERNEL32$VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
DECLSPEC_IMPORT WINBASEAPI BOOL     WINAPI  KERNEL32$VirtualFree(LPVOID, SIZE_T, DWORD);
DECLSPEC_IMPORT WINBASEAPI LPSTR    WINAPI MSVCRT$strcat(char* strDestination, const char* strSource);
DECLSPEC_IMPORT WINBASEAPI BOOL     WINAPI KERNEL32$K32EnumDeviceDrivers(LPVOID*, DWORD, LPDWORD);
DECLSPEC_IMPORT WINBASEAPI DWORD    WINAPI KERNEL32$K32GetDeviceDriverBaseNameA(LPVOID, LPSTR, DWORD);


#define ARRAY_SIZE 256

VOID go(PVOID Buffer, ULONG Length)
{
    LPVOID drivers[ARRAY_SIZE];
    DWORD cbNeeded;
    int cDrivers;

    if (KERNEL32$K32EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded) && cbNeeded < sizeof(drivers))
    {
        char szDriver[ARRAY_SIZE];
        cDrivers = cbNeeded / sizeof(drivers[0]);

        if (!cDrivers)
        {
            return;
        }

        char* result = (char*)KERNEL32$VirtualAlloc(NULL, cbNeeded * MAX_PATH, MEM_COMMIT, PAGE_READWRITE);
        if (!result)
        {
            return;
        }

        for (int i = 0; i < cDrivers; i++)
        {
            if (KERNEL32$K32GetDeviceDriverBaseNameA(drivers[i], szDriver, sizeof(szDriver) / sizeof(szDriver[0])))
            {
                MSVCRT$strcat(result, szDriver);
                MSVCRT$strcat(result, "\n");
            }
        }

        BeaconPrintf(CALLBACK_OUTPUT, "%s\n", result);
        KERNEL32$VirtualFree(result, 0, MEM_RELEASE);
    }
    else
    {
        BeaconPrintf(HAVOC_CONSOLE_ERRO, "EnumDeviceDrivers failed; array size needed is %zu\n", cbNeeded / sizeof(LPVOID));
        return;
    }

    return;
}
