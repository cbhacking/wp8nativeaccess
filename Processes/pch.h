//
// pch.h
// Standard system include files for the Processes library
//

#pragma once

#include <vector>

#include <windows.h>
#include <winbase.h>
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
//HANDLE WINAPI CreateToolhelp32Snapshot(
//  _In_  DWORD dwFlags,
//  _In_  DWORD th32ProcessID
//);

#define QueryFullProcessImageName QueryFullProcessImageNameW

BOOL WINAPI QueryFullProcessImageName(
  _In_     HANDLE hProcess,
  _In_     DWORD dwFlags,
  _Out_    LPTSTR lpExeName,
  _Inout_  PDWORD lpdwSize
);

// LookupPrivilegeValue
WINADVAPI
BOOL
WINAPI
LookupPrivilegeValueW(
    _In_opt_ LPCWSTR lpSystemName,
    _In_     LPCWSTR lpName,
    _Out_    PLUID   lpLuid
    );
#ifdef UNICODE
#define LookupPrivilegeValue  LookupPrivilegeValueW
#else
#define LookupPrivilegeValue  LookupPrivilegeValueA
#endif // !UNICODE

WINADVAPI
_Success_(return != FALSE) BOOL
WINAPI
LookupPrivilegeNameA(
    _In_opt_ LPCSTR lpSystemName,
    _In_     PLUID   lpLuid,
    _Out_writes_to_opt_(*cchName, *cchName + 1) LPSTR lpName,
    _Inout_  LPDWORD cchName
    );
WINADVAPI
_Success_(return != FALSE) BOOL
WINAPI
LookupPrivilegeNameW(
    _In_opt_ LPCWSTR lpSystemName,
    _In_     PLUID   lpLuid,
    _Out_writes_to_opt_(*cchName, *cchName + 1) LPWSTR lpName,
    _Inout_  LPDWORD cchName
    );
#ifdef UNICODE
#define LookupPrivilegeName  LookupPrivilegeNameW
#else
#define LookupPrivilegeName  LookupPrivilegeNameA
#endif // !UNICODE

WINADVAPI
_Success_(return != FALSE) BOOL
WINAPI
LookupPrivilegeDisplayNameA(
    _In_opt_ LPCSTR lpSystemName,
    _In_     LPCSTR lpName,
    _Out_writes_to_opt_(*cchDisplayName, *cchDisplayName + 1) LPSTR lpDisplayName,
    _Inout_  LPDWORD cchDisplayName,
    _Out_    LPDWORD lpLanguageId
    );
WINADVAPI
_Success_(return != FALSE) BOOL
WINAPI
LookupPrivilegeDisplayNameW(
    _In_opt_ LPCWSTR lpSystemName,
    _In_     LPCWSTR lpName,
    _Out_writes_to_opt_(*cchDisplayName, *cchDisplayName + 1) LPWSTR lpDisplayName,
    _Inout_  LPDWORD cchDisplayName,
    _Out_    LPDWORD lpLanguageId
    );
#ifdef UNICODE
#define LookupPrivilegeDisplayName  LookupPrivilegeDisplayNameW
#else
#define LookupPrivilegeDisplayName  LookupPrivilegeDisplayNameA
#endif // !UNICODE
WINADVAPI
BOOL
WINAPI
AdjustTokenPrivileges(
    _In_ HANDLE TokenHandle,
    _In_ BOOL DisableAllPrivileges,
    _In_opt_ PTOKEN_PRIVILEGES NewState,
    _In_ DWORD BufferLength,
    _Out_writes_bytes_to_opt_(BufferLength, *ReturnLength) PTOKEN_PRIVILEGES PreviousState,
    _Out_opt_ PDWORD ReturnLength
    );

WINADVAPI
BOOL
WINAPI
GetTokenInformation(
    _In_ HANDLE TokenHandle,
    _In_ TOKEN_INFORMATION_CLASS TokenInformationClass,
    _Out_writes_bytes_to_opt_(TokenInformationLength, *ReturnLength) LPVOID TokenInformation,
    _In_ DWORD TokenInformationLength,
    _Out_ PDWORD ReturnLength
    );

//
// Logon Support APIs
//

#define LOGON32_LOGON_INTERACTIVE       2
#define LOGON32_LOGON_NETWORK           3
#define LOGON32_LOGON_BATCH             4
#define LOGON32_LOGON_SERVICE           5
#define LOGON32_LOGON_UNLOCK            7
#define LOGON32_LOGON_NETWORK_CLEARTEXT 8
#define LOGON32_LOGON_NEW_CREDENTIALS   9

#define LOGON32_PROVIDER_DEFAULT    0
#define LOGON32_PROVIDER_WINNT35    1
#define LOGON32_PROVIDER_WINNT40    2
#define LOGON32_PROVIDER_WINNT50    3
#define LOGON32_PROVIDER_VIRTUAL    4

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

HLOCAL WINAPI LocalFree(
  _In_  HLOCAL hMem
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