//
// pch.h
// Standard system include files for the Registry library.
//

#pragma once

// Get the whole Win32 API, not just the phone stuff
#include <winapifamily.h>
#ifdef WINAPI_PARTITION_DESKTOP
#undef WINAPI_PARTITION_DESKTOP
#endif
#define WINAPI_PARTITION_DESKTOP 1

#include <windows.h>

#define ROOTRPC_REPLACEMENTS
#define NO_ROOTRPC_FILE_APIS
#define NO_ROOTRPC_DRIVE_APIS
#include "../../RootRpc/RootRpcClient/RootRpcClient.h"

#include <stdlib.h>
#include <malloc.h>
#include <vector>