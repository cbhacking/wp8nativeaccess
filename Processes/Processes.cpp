/*
 * Processes\Processes.cpp
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.1.3
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

Array<uint32>^ NativeProcess::GetProcessIds ()
{
	int count = 64;
	DWORD *ids;
	DWORD bytes;

	while (true)
	{
		ids = new DWORD[count];
		bytes = 0;
		if (::EnumProcesses(ids, (count * sizeof(DWORD)), &bytes))
		{
			if ((bytes / sizeof(DWORD)) == count)
			{
				// We got as many items as the array could hold, try again...
				delete[] ids;
				count *= 2;
			}
			else
			{
				// We got a valid result
				count = (bytes / sizeof(DWORD));
				break;
			}
		}
		else
		{
			// An error occurred
			delete[] ids;
			return nullptr;
		}
	}

	// If we got here, the call succeeded.
	Array<uint32> ^ret = ref new Array<uint32>((uint32*)ids, count);
	delete[] ids;
	return ret;
}

Array<ProcessInfo>^ NativeProcess::GetProcesses ()
{
	int count = 64;
	DWORD *ids;
	DWORD bytes;

	// Get the process array
	while (true)
	{
		ids = new DWORD[count];
		bytes = 0;
		if (::EnumProcesses(ids, (count * sizeof(DWORD)), &bytes))
		{
			if ((bytes / sizeof(DWORD)) == count)
			{
				// We got as many items as the array could hold, try again...
				delete[] ids;
				count *= 2;
			}
			else
			{
				// We got a valid result
				count = (bytes / sizeof(DWORD));
				break;
			}
		}
		else
		{
			// An error occurred
			delete[] ids;
			return nullptr;
		}
	}

	// If we got here, the call succeeded.
	ProcessInfo *infos = (ProcessInfo*)::calloc(count, sizeof(ProcessInfo));
	for (int i = 0; i < count; i++)
	{
		if (!ids[i]) continue;
		HANDLE h = ::OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, ids[i]);
		if (NULL == h)
		{
			// An error occurred
			delete[] ids;
			// Clean up the strings allocated so far...
			for (i--; i >= 0; i--)
			{
				infos[i].ImageName = nullptr;
			}
			::free(infos);
			return nullptr;
		}
		WCHAR buffer[MAX_PATH];
		DWORD size = MAX_PATH;
		if (::QueryFullProcessImageName(h, 0, buffer, &size))
		{
			infos[i].ImageName = ref new String(buffer);
			infos[i].ProcessID = ids[i];
		}
		else
		{
			::CloseHandle(h);
			delete[] ids;
			// Clean up the strings allocated so far...
			for (i--; i >= 0; i--)
			{
				infos[i].ImageName = nullptr;
			}
			::free(infos);
			return nullptr;
		}
		::CloseHandle(h);
	}
	// If we got here, the array is populated
	Array<ProcessInfo> ^ret = ref new Array<ProcessInfo>(infos, count);
	// Clean up the string references before freeing the buffer
	for (int i = 0; i < count; i++) infos[i].ImageName = nullptr;
	::free(infos);
	delete[] ids;
	return ret;
}

Array<uint32>^ NativeProcess::SearchProcessIds ()
{
	std::vector<unsigned> vec;
	HANDLE proc = INVALID_HANDLE_VALUE;
	for (unsigned i = 4; i < 0xFFFF; i+=4)
	{
		::OpenProcessForQuery(FALSE, i, &proc);
		if ((NULL == proc) || (INVALID_HANDLE_VALUE == proc)) continue;
		// If we got here, found one!
		vec.push_back(i);
		::CloseHandle(proc);
	}
	return ref new Array<uint32>(vec.data(), vec.size());
}

Array<ProcessInfo>^ NativeProcess::SearchProcesses ()
{
	std::vector<ProcessInfo> vec;
	HANDLE proc = INVALID_HANDLE_VALUE;
	WCHAR buffer[MAX_PATH];
	DWORD size = MAX_PATH;
	for (unsigned i = 4; i < 0xFFFF; i+=4)
	{
		::OpenProcessForQuery(FALSE, i, &proc);
		if ((NULL == proc) || (INVALID_HANDLE_VALUE == proc)) continue;
		// If we got here, found one!
		ProcessInfo pi;
		pi.ProcessID = i;
		if (::QueryFullProcessImageName(proc, 0, buffer, &size))
		{
			pi.ImageName = ref new String(buffer);
		}
		size = MAX_PATH;
		vec.push_back(pi);
		::CloseHandle(proc);
	}
	return ref new Array<ProcessInfo>(vec.data(), vec.size());
}

uint32 NativeProcess::CreateProcInChamber (String ^sid, String ^commandLine)
{
	size_t len = commandLine->Length() + 1;
	PWSTR cmd = new WCHAR[len];
	::wcscpy_s(cmd, len, commandLine->Data());
	STARTUPINFO si;
	::memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;

	HRESULT hr = ::CreateProcessInChamber(
		sid->Data(), NULL, NULL, cmd, FALSE, 0x0, NULL, &si, &pi);

	delete[] cmd;

	if (SUCCEEDED(hr))
	{
		::CloseHandle(pi.hThread);
		::CloseHandle(pi.hProcess);
		return pi.dwProcessId;
	}
	::SetLastError(WIN32_FROM_HRESULT(hr));
	return 0x0;
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

uint32 Security::TryGetCurrentToken ()
{
	TOKEN_PRIVILEGES tp, tpOld;
	LUID luid;
	DWORD tplen = sizeof(tpOld);

	if ( !LookupPrivilegeValue( 
		NULL,            // lookup privilege on local system
		SE_BACKUP_NAME,   // privilege to lookup 
//		SE_CREATE_SYMBOLIC_LINK_NAME,   // privilege to lookup 
		&luid ) )        // receives LUID of privilege
	{
		return 0x0;
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Luid = luid;
//	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Attributes = 0;

	HANDLE hToken;
	if (!
		::OpenProcessToken(::GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken)
		)
	{
		return 0x0;
	}

	uint32 retval = (uint32)hToken;

	if ( !AdjustTokenPrivileges(
		hToken, 
		FALSE, 
		&tp, 
		sizeof(TOKEN_PRIVILEGES), 
		&tpOld, 
		&tplen) )
	{
		tplen = ::GetLastError();
		if (tplen == ERROR_NOT_ALL_ASSIGNED)
		{
			retval = 1;
		}
		else if (ERROR_ACCESS_DENIED == tplen)
		{
			retval = 55;
		}
		else if ((tplen == ERROR_SUCCESS) && (1 == tpOld.PrivilegeCount))
		{
			switch (tpOld.Privileges[0].Attributes & 0xF)
			{
			case 0:
				retval = 5;
				break;
			case SE_PRIVILEGE_ENABLED:
				retval = 7;
				break;
			case SE_PRIVILEGE_ENABLED_BY_DEFAULT:
				retval = 9;
				break;
			default:
				retval = 11;
			}
		}
		else
		{
			retval = 3;
		}
	}
	else
	{
		retval = 0;
	}

	::CloseHandle(hToken);
	return retval;
}

uint32 Security::LogonUser (String ^username, String ^password, uint32 type)
{
	HANDLE ret = NULL;
	const wchar_t* user = (username ? username->Data() : NULL);
	const wchar_t* pass = (password ? password->Data() : NULL);
	if (::LogonUserExExW((PWSTR)user, NULL, (PWSTR)pass, type, LOGON32_PROVIDER_DEFAULT, NULL, &ret, NULL, NULL, NULL, NULL))
	{
		return (uint32)ret;
	}
	return 0x0;
}

Array<TokenPrivilege>^ Security::GetPrivileges (uint32 processId)
{
	HANDLE hProcess = INVALID_HANDLE_VALUE;
	HANDLE hToken = INVALID_HANDLE_VALUE;
	Array<TokenPrivilege> ^retval = nullptr;
	DWORD bufsize = 0x0;
	PTOKEN_PRIVILEGES privsbuf = nullptr;
	LUID luid;
	WCHAR namebuf[1024];
	WCHAR purposebuf[1024];
	DWORD stuff;

	// First, get the token for the process
	if (processId)
	{
		::OpenProcessForQuery(FALSE, processId, &hProcess);
		if (!hProcess)
			goto Cleanup;
	}
	else
	{
		// Current process
		hProcess = ::GetCurrentProcess();
	}
	if (!::OpenProcessToken(hProcess, TOKEN_QUERY, &hToken))
	{
		// Failed to open the token
		goto Cleanup;
	}
	// Get the required length
	if (!(::GetTokenInformation(hToken, TokenPrivileges, NULL, 0, &bufsize)) &&
		(::GetLastError() != ERROR_INSUFFICIENT_BUFFER))
	{
		// Unable to get the required size of the privilege collection!
		goto Cleanup;
	}
	// Allocate the memory to receive the collection
	if (!(privsbuf = (PTOKEN_PRIVILEGES)(::malloc(bufsize))))
	{
		// Allocation failed
		::SetLastError(ERROR_OUTOFMEMORY);
		goto Cleanup;
	}
	// Actually retrieve the privileges
	if (!(::GetTokenInformation(hToken, TokenPrivileges, privsbuf, bufsize, &bufsize)))
	{
		// Unable to retrieve the privilege collection!
		goto Cleanup;
	}
	// Create the array of privileges to return
	if (!(retval = ref new Array<TokenPrivilege>(privsbuf->PrivilegeCount)))
	{
		// Allocation failed
		::SetLastError(ERROR_OUTOFMEMORY);
		goto Cleanup;
	}
	// Set each privilege
	for (uint32 i = 0; i < privsbuf->PrivilegeCount; i++)
	{
		TokenPrivilege priv = retval->get(i);
		// Get the LUID
		luid = privsbuf->Privileges[i].Luid;
		priv.Luid = (((int64)(luid.HighPart) << 32) | luid.LowPart);
		// Get the name
		bufsize = (sizeof(namebuf) / sizeof(namebuf[0]));
		if (!(::LookupPrivilegeName(NULL, &luid, namebuf, &bufsize)))
		{
			// Failed to get the name; eh, whatever
			priv.Name = ref new String(L"*ERROR RETRIEVING NAME*");
		}
		else
		{
			priv.Name = ref new String(namebuf);
		}
		// Get the display name that describes the privilege
		bufsize = (sizeof(purposebuf) / sizeof(purposebuf[0]));
		if (!(::LookupPrivilegeDisplayName(NULL, namebuf, purposebuf, &bufsize, &stuff)))
		{
			// Failed to get the display name; eh, whatever
			priv.Purpose = ref new String(L"*ERROR RETRIEVING DISPLAY NAME*");
		}
		else
		{
			priv.Purpose = ref new String(purposebuf);
		}
		// Get the privilege attributes
		stuff = privsbuf->Privileges[i].Attributes;
		priv.Enabled = !!(stuff & SE_PRIVILEGE_ENABLED);
		priv.Default = !!(stuff & SE_PRIVILEGE_ENABLED_BY_DEFAULT);
		priv.Removed = !!(stuff & SE_PRIVILEGE_REMOVED);
		priv.WasUsed = !!(stuff & SE_PRIVILEGE_USED_FOR_ACCESS);
		retval->set(i, priv);
	}

Cleanup:
	if (hProcess && hProcess != INVALID_HANDLE_VALUE)
		::CloseHandle(hProcess);
	if (hToken && hToken != INVALID_HANDLE_VALUE)
		::CloseHandle(hToken);
	if (privsbuf)
		::free(privsbuf);

	return retval;
}

String^ Security::GetChamberSid (String ^identifier)
{
	PWSTR sid = NULL;
	HRESULT hr = ::GetChamberSidFromId(identifier->Data(), &sid);
	if (!SUCCEEDED(hr))
	{
		::SetLastError(WIN32_FROM_HRESULT(hr));
		return nullptr;
	}
	String ^ret = ref new String(sid);
	::LocalFree(sid);
	return ret;
}