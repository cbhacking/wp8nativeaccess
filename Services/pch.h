/*
 * Services\pch.h
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.0.2
 *
 * This file project-wide includes for the Services NativeAccess library, which enables Service Control Manager access.
 */

#pragma once

 // Get the whole Win32 API, not just the phone stuff
#include <winapifamily.h>
#ifdef WINAPI_PARTITION_DESKTOP
#undef WINAPI_PARTITION_DESKTOP
#endif
#define WINAPI_PARTITION_DESKTOP 1

#include <windows.h>