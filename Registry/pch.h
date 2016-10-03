//
// NativeAccess/Registry/pch.h
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

#ifdef USE_ROOTRPC
#define ROOTRPC_REPLACEMENTS
#define ROOTRPC_REG_APIS
#include "../../RootRpc/RootRpcClient/RootRpcClient.h"
#endif

#include <stdlib.h>
#include <malloc.h>
#include <vector>