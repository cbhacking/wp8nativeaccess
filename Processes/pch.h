//
// NativeAccess/Processes/pch.h
// Standard system include files for the Processes library
//

#pragma once

#include <vector>

// Get the whole Win32 API, not just the phone stuff
#include <winapifamily.h>
#ifdef WINAPI_PARTITION_DESKTOP
#undef WINAPI_PARTITION_DESKTOP
#endif
#define WINAPI_PARTITION_DESKTOP 1

#include <windows.h>
#include <psapi.h>

#include <malloc.h>
//#include <ntdef.h>
typedef __success(return >= 0) LONG NTSTATUS;
//#include <TlHelp32.h>

// Total hack based on
// http://blogs.msdn.com/b/oldnewthing/archive/2006/11/03/942851.aspx
// First result could have come from many values, just give the code
#define WIN32_FROM_HRESULT(hr) \
	((hr & 0xFFFF0000) == MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0)) \
		? HRESULT_CODE(hr) : ((hr == S_OK)? 0x0 : /* we got an impossible value */ (DWORD)hr)

extern "C"
{
BOOL WINAPI LogonUserExExW(
  _In_       LPWSTR lpszUsername,
  _In_opt_   LPWSTR lpszDomain,
  _In_opt_   LPWSTR lpszPassword,
  _In_       DWORD dwLogonType,
  _In_       DWORD dwLogonProvider,
  _In_opt_   PTOKEN_GROUPS pTokenGroups,
  _Out_opt_  PHANDLE phToken,
  _Out_opt_  PSID *ppLogonSid,
  _Out_opt_  PVOID *ppProfileBuffer,
  _Out_opt_  LPDWORD pdwProfileLength,
  _Out_opt_  PQUOTA_LIMITS pQuotaLimits
);
//
// Undocumented SecRuntime APIs
//

typedef enum {
	WP8_OBJ_MUTEX,
	WP8_OBJ_EVENT,
	WP8_OBJ_SEMAPHORE,
	WP8_OBJ_WAITABLETIMER,
	WP8_OBJ_JOBOBJECT,
	WP8_OBJ_FILEMAPPING,
	WP8_OBJ_NAMEDPIPE,
	WP8_OBJ_MAILSLOT,
	WP8_OBJ_ALPCPORT,
	WP8_OBJ_RPC,
	WP8_OBJ_WNF,
	WP8_OBJ_TEMPLATE,
	WP8_OBJ_PRIVATE
} WP_NT_SEC_RUNTIME_OBJECT;

HRESULT WINAPI QueryTransientObjectSecurityDescriptor(
	_In_ WP_NT_SEC_RUNTIME_OBJECT ObjectType,
	_In_ LPWSTR lpszObjectName,
    _Out_ PSECURITY_DESCRIPTOR *ppSecurityDescriptor
	);

HRESULT WINAPI FreeTransientObjectSecurityDescriptor(
    _In_ PSECURITY_DESCRIPTOR pSecurityDescriptor
	);

HRESULT WINAPI OpenProcessForQuery(
	_In_ BOOL bInheritHandle,
	_In_ DWORD dwProcessId,
	_Out_ PHANDLE phProcess
	);

HRESULT WINAPI OpenProcessTokenForQuery(
	_In_ HANDLE hProcess,
	_Out_ PHANDLE phToken
	);

NTSTATUS GetChamberSid(
	_In_opt_ HANDLE hClientToken,
	_In_ ULONG ulSidLength,
	_Out_writes_bytes_to_opt_(ulSidLength, *pReturnLength) PSID pSid,
	_Out_ PULONG pReturnLength,
	_Out_opt_ BOOLEAN *pfAppContainerSid
);

/**
 * Gets the SID for an app ProductId, built-in application, or service account
 * The output value of this function is allocated by the function itself.
 * It should be freed by calling LocalFree 
 */
HRESULT GetChamberSidFromId(
	_In_ LPCWSTR pszIdentifier,
	_Out_ LPWSTR *ppszChamberSid
);

HRESULT WINAPI CreateTokenFromChamberSid(
    _In_ LPCWSTR pszChamberSid,
    _In_ SECURITY_IMPERSONATION_LEVEL ImpersonationLevel,
    _Out_ HANDLE *phToken
);

HRESULT WINAPI CreateProcessInChamber(
	_In_opt_ LPCWSTR pszChamberSid,
	_In_opt_ LPCWSTR pszApplicationId,
	_In_opt_ LPCWSTR pszApplicationName,
	_Inout_opt_ LPWSTR pszCommandLine,
	_In_ BOOL bInheritHandles,
	_In_ DWORD dwCreationFlags,
	_In_opt_ LPCWSTR pszCurrentDirectory,
	_In_ LPSTARTUPINFO lpStartupInfo,
	_Out_ LPPROCESS_INFORMATION lpProcessInformation
	);
}