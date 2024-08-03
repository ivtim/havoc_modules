#include <windows.h>
#include "beacon.h"

/* Havoc console output modes */
#define HAVOC_CONSOLE_GOOD 0x90
#define HAVOC_CONSOLE_INFO 0x91
#define HAVOC_CONSOLE_ERRO 0x92

DECLSPEC_IMPORT WINBASEAPI LPVOID   WINAPI KERNEL32$VirtualAlloc(LPVOID lpAddress, SIZE_T dwSize, DWORD flAllocationType, DWORD flProtect);
DECLSPEC_IMPORT WINBASEAPI BOOL     WINAPI KERNEL32$VirtualFree(LPVOID, SIZE_T, DWORD);
DECLSPEC_IMPORT WINADVAPI LONG      WINAPI ADVAPI32$RegOpenKeyExW(HKEY, LPCWSTR, DWORD, REGSAM, PHKEY);
WINADVAPI LONG                      WINAPI ADVAPI32$RegEnumKeyExW(HKEY hKey, DWORD dwIndex, LPWSTR lpName, LPDWORD lpcchName, LPDWORD lpReserved, LPSTR lpClass, LPDWORD lpcchClass, PFILETIME lpftLastWriteTime);
DECLSPEC_IMPORT WINADVAPI LONG      WINAPI ADVAPI32$RegCloseKey(HKEY);
WINADVAPI LONG                      WINAPI ADVAPI32$RegQueryValueExW(HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved, LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData);
WINBASEAPI wchar_t*                 __cdecl MSVCRT$wcscat(wchar_t* dest, const wchar_t* src);
WINBASEAPI DWORD                    WINAPI KERNEL32$GetLastError();


LPVOID _malloc(SIZE_T size)
{
    return KERNEL32$VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
}

void _free(LPVOID address)
{
    KERNEL32$VirtualFree(address, 0, MEM_RELEASE);
}

VOID go(PVOID Buffer, ULONG Length)
{
    HKEY hKey;
    LONG res;
    if ((res = ADVAPI32$RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall", 0, KEY_READ, &hKey)) != ERROR_SUCCESS)
    {
        BeaconPrintf(HAVOC_CONSOLE_ERRO, "Error RegOpenKeyExW: %lu\n", KERNEL32$GetLastError());
        return;
    }

    wchar_t* program_list_buffer = (wchar_t*)_malloc(MAX_PATH * 100);
    if (!program_list_buffer)
    {
        BeaconPrintf(HAVOC_CONSOLE_ERRO, "Error VirtualAlloc: %lu\n", KERNEL32$GetLastError());
        ADVAPI32$RegCloseKey(hKey);
        return;
    }

    DWORD index = 0;
    wchar_t key_buffer[MAX_PATH];
    DWORD key_buffer_len = MAX_PATH;
    while ((res = ADVAPI32$RegEnumKeyExW(hKey, index, key_buffer, &key_buffer_len, NULL, NULL, NULL, NULL)) != ERROR_NO_MORE_ITEMS)
    {
        if (res == ERROR_SUCCESS)
        {
            HKEY hItem;
            if (ADVAPI32$RegOpenKeyExW(hKey, key_buffer, 0, KEY_READ, &hItem) != ERROR_SUCCESS)
            {
                continue;
            }

            wchar_t display_name_buffer[MAX_PATH];
            DWORD dw_size = sizeof(display_name_buffer);
            DWORD d_type;

            if ((res = ADVAPI32$RegQueryValueExW(hItem, L"DisplayName", NULL, &d_type, (LPBYTE)&display_name_buffer, &dw_size)) == ERROR_SUCCESS)
            {
                BeaconPrintf(HAVOC_CONSOLE_INFO, "%S\n", display_name_buffer);
                MSVCRT$wcscat(program_list_buffer, display_name_buffer);
                MSVCRT$wcscat(program_list_buffer, L"\n");
            }

            ADVAPI32$RegCloseKey(hItem);
        }

        index++;
        key_buffer_len = MAX_PATH;
    }

    BeaconPrintf(CALLBACK_OUTPUT, "%S\n", program_list_buffer);

    _free(program_list_buffer);
    ADVAPI32$RegCloseKey(hKey);
    return;
}

