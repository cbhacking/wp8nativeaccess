//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <vector>

#include <windows.h>
#include <fileapi.h>

extern "C"
{
BOOLEAN WINAPI CreateSymbolicLinkW(
  _In_  LPWSTR lpSymlinkFileName,
  _In_  LPWSTR lpTargetFileName,
  _In_  DWORD dwFlags
);

WINBASEAPI
DWORD
WINAPI
GetLogicalDrives(
    VOID
    );


WINBASEAPI
DWORD
WINAPI
GetLogicalDriveStringsW(
    _In_ DWORD nBufferLength,
    _Out_writes_to_opt_(nBufferLength, return + 1) LPWSTR lpBuffer
    );


#ifdef UNICODE
#define GetLogicalDriveStrings  GetLogicalDriveStringsW
#endif
}
using namespace std;
