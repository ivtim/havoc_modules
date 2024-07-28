#include <windows.h>
#include "beacon.h"


#define HAVOC_CONSOLE_GOOD 0x90
#define HAVOC_CONSOLE_INFO 0x91
#define HAVOC_CONSOLE_ERRO 0x92


WINBASEAPI DWORD WINAPI KERNEL32$GetLogicalDriveStringsA(
    DWORD nBufferLength,
    LPSTR lpBuffer
);

WINBASEAPI UINT WINAPI KERNEL32$GetDriveTypeA(
    LPCSTR lpRootPathName
);

WINBASEAPI DWORD   WINAPI KERNEL32$GetLastError();


#define DRIVER_NAME_SIZE 4
#define DRIVERS_MAX_SIZE 26
#define BUFFER_SIZE DRIVERS_MAX_SIZE * DRIVER_NAME_SIZE * sizeof(char)


VOID go(PVOID Buffer, ULONG Length)
{
    DWORD result = ERROR_SUCCESS;
    char drivers[BUFFER_SIZE];

    char* driver_types[] = {
        "DRIVE_UNKNOWN",
        "DRIVE_NO_ROOT_DIR",
        "DRIVE_REMOVABLE",
        "DRIVE_FIXED",
        "DRIVE_REMOTE",
        "DRIVE_CDROM",
        "DRIVE_RAMDISK"
    };

    result = KERNEL32$GetLogicalDriveStringsA(BUFFER_SIZE, drivers);

    if (!result)
    {
        BeaconPrintf(HAVOC_CONSOLE_ERRO, "Failed to enum logic drivers: %d", KERNEL32$GetLastError());
    }

    for (char* current_driver = drivers; *current_driver; current_driver += 4)
    {
        UINT driver_type = KERNEL32$GetDriveTypeA(current_driver);
        BeaconPrintf(CALLBACK_OUTPUT, "%s %s\n", current_driver, driver_types[driver_type]);
    }

    BeaconPrintf(HAVOC_CONSOLE_GOOD, "enumiration logic drivers successful executed");
}
