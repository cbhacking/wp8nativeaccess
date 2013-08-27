/*
 * Processes\Processes.cpp
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.1.0
 *
 * This file implements the WinRT-visible static native process access functions.
 * It also implements the WinRT-visible static native current process functions.
 */

#include "pch.h"
#include "Processes.h"

using namespace Processes;
using namespace Platform;

NativeProcess::NativeProcess ()
{
}

uint32 NativeProcess::CreateProc (String ^commandLine)
{
	size_t len = commandLine->Length() + 1;
	PWSTR cmd = new WCHAR[len];
	::wcscpy_s(cmd, len, commandLine->Data());
	STARTUPINFO si;
	::memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;

	BOOL ret = ::CreateProcessW(NULL, cmd, NULL, NULL, FALSE, 0x0, NULL, NULL, &si, &pi);

	delete[] cmd;
	if (ret)
	{
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
		return pi.dwProcessId;
	}
	else
	{
		return 0x0;
	}
}

bool NativeProcess::KillProc (uint32 processID)
{
	HANDLE proc = ::OpenProcess(PROCESS_TERMINATE, FALSE, processID);
	BOOL ret = FALSE;
	if (NULL != proc)
	{
		ret = ::TerminateProcess(proc, 0x0);
		::CloseHandle(proc);
	}
	return !!ret;
}

uint32 NativeProcess::GetError ()
{
	return ::GetLastError();
}

uint32 CurrentProcess::ProcessId::get ()
{
	return ::GetCurrentProcessId();
}

String^ CurrentProcess::CommandLine::get ()
{
	return ref new String(::GetCommandLine());
}

String^ CurrentProcess::GetEnvironmentVar (String ^name)
{
	PCWSTR n = name->Data();
	DWORD size = 0x0;
	DWORD size2 = 0x1;
	PWSTR buf = NULL;
	while (size2 >= size)
	{
		if (buf != NULL)
		{
			delete[] buf;
		}
		size = ::GetEnvironmentVariable(n, NULL, 0);
		if (size > 0)
		{
			buf = new WCHAR[size];
			DWORD size2 = ::GetEnvironmentVariable(n, buf, size);
		}
	}
	if (size2 > 0)
	{
		// Function succeeded
		String ^ret = ref new String(buf);
		delete[] buf;
		return ret;
	}
	else
	{
		delete[] buf;
		return nullptr;
	}
}

bool CurrentProcess::SetEnvironmentVar (String ^name, String ^value)
{
	return !!::SetEnvironmentVariable(name->Data(), value->Data());
}