//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <windows.h>

extern "C"
{
BOOLEAN WINAPI CreateSymbolicLinkW(
  _In_  LPWSTR lpSymlinkFileName,
  _In_  LPWSTR lpTargetFileName,
  _In_  DWORD dwFlags
);
}
using namespace std;
#include <vector>