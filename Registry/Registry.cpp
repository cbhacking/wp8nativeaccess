﻿/*
 * Registry\Registry.cpp
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.2.3
 *
 * This file implements the WinRT-visible registry access functions.
 */

#include "pch.h"
#include "Registry.h"

using namespace Registry;
using namespace Platform;

PCWSTR Registry::REG_ROOTS[] = {
		L"HKEY_CLASSES_ROOT",
		L"HKEY_CURRENT_USER",
		L"HKEY_LOCAL_MACHINE",
		L"HKEY_USERS",
		L"HKEY_PERFORMANCE_DATA",
		L"HKEY_CURRENT_CONFIG"};

NativeRegistry::NativeRegistry()
{
}

bool NativeRegistry::ReadDWORD (STDREGARGS, uint32 *data)
{
	DWORD bytes = sizeof(uint32);
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : NULL;
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = ::RegGetValueW(
		(HKEY)hive, key, val, RRF_RT_DWORD, NULL, data, &bytes);
	if (ERROR_SUCCESS == err)
	{
		return true;
	}
	::SetLastError(err);
	*data = 0x0;
	return false;
}

bool NativeRegistry::ReadString (STDREGARGS, String ^*data)
{
	DWORD bytes = 0;
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : NULL;
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = ::RegGetValueW(
		(HKEY)hive, key, val, RRF_RT_REG_SZ, NULL, NULL, &bytes);
	if (ERROR_SUCCESS == err)
	{
		// Got the length...
		PWSTR str = new WCHAR[bytes];
		err = ::RegGetValueW(
			(HKEY)hive, path->Data(), value->Data(), RRF_RT_REG_SZ, NULL, str, &bytes);
		if (ERROR_SUCCESS == err)
		{
			*data = ref new String(str);
			delete[] str;
			return true;
		}
		else
		{
			delete[] str;
		}
	}
	// Can't get here without something going wrong...
	::SetLastError(err);
	*data = nullptr;
	return false;
}

bool NativeRegistry::WriteDWORD (STDREGARGS, uint32 data)
{
/*	Sadly, this API is not available on WP8 apparently...
	err = RegSetKeyValueW(
		(HKEY)hive, path->Data(), value->Data(), REG_DWORD, &data, sizeof(uint32));*/
	HKEY hkey = NULL;
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : L"";
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = ::RegOpenKeyExW((HKEY)hive, key, 0x0, KEY_SET_VALUE, &hkey);
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		return false;
	}
	err = ::RegSetValueExW(hkey, val, 0x0, REG_DWORD, (PBYTE)(&data), sizeof(uint32));
	::RegCloseKey(hkey);
	if (ERROR_SUCCESS == err)
	{
		return true;
	}
	::SetLastError(err);
	return false;
}

bool NativeRegistry::WriteString (STDREGARGS, String ^data)
{
/*	Sadly, this API is not available on WP8 apparently...
	LSTATUS err = RegSetKeyValueW(
		(HKEY)hive, path->Data(), value->Data(), REG_SZ, data->Data(), ((data->Length() + 1) * (sizeof(WCHAR))));*/
	HKEY hkey = NULL;
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : L"";
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = ::RegOpenKeyExW((HKEY)hive, key, 0x0, KEY_SET_VALUE, &hkey);
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		return false;
	}
	err = ::RegSetValueExW(hkey, val, 0x0, REG_SZ, (PBYTE)(data->Data()), ((data->Length() + 1) * (sizeof(WCHAR))));
	::RegCloseKey(hkey);
	if (ERROR_SUCCESS == err)
	{
		return true;
	}
	::SetLastError(err);
	return false;
}

bool NativeRegistry::DeleteValue (STDREGARGS)
{
	// Key or value name can be null; in that case, use the default value and/or the specified key
	HKEY hkey = (HKEY)hive;
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err;
	if ((nullptr != path) && (path->Length() > 0))
	{
		// Need to open a sub-key
		err = ::RegOpenKeyExW((HKEY)hive, path->Data(), 0x0, KEY_SET_VALUE, &hkey);
		if (err != ERROR_SUCCESS)
		{
			::SetLastError(err);
			return false;
		}
	}
	err = ::RegDeleteValueW(hkey, val);
	if (path && path->Length())
	{
		::RegCloseKey(hkey);
	}
	if (ERROR_SUCCESS == err)
	{
		return true;
	}
	::SetLastError(err);
	return false;
}

bool NativeRegistry::DeleteKey (RegistryHive hive, String ^path, bool recursive)
{
	LSTATUS err;
	if (recursive)
	{
		err = ::RegDeleteTreeW((HKEY)hive, path->Data());
		if (err != ERROR_SUCCESS)
		{
			::SetLastError(err);
			return false;
		}
	}
	err = ::RegDeleteKeyW((HKEY)hive, path->Data());
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		return false;
	}
	return true;
}

bool NativeRegistry::CreateKey (RegistryHive hive, String ^path)
{
	HKEY hk = NULL;
	LSTATUS err = ::RegCreateKeyEx((HKEY)hive, path->Data(), 0x0, NULL, 0x0, KEY_READ | KEY_WRITE, NULL, &hk, NULL);
	if (ERROR_SUCCESS != err)
	{
		::SetLastError(err);
		return false;
	}
	::RegCloseKey(hk);
	return true;
}

bool NativeRegistry::GetSubKeyNames (RegistryHive hive, String ^path, Array<String^> ^*names)
{
	DWORD count = 0x0;
	DWORD maxlen = 0x0;
	HKEY hk = (HKEY)hive;
	LSTATUS err = ERROR_SUCCESS;
	PWSTR *pnames = NULL;
	*names = nullptr;
	int i;

	// Get the key we're querying on
	if ((nullptr != path) && (!path->IsEmpty()))
	{
		hk = GetHKey(hk, path->Data(), KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE, RCOOK_OPEN_EXISTING);
	}
	if (nullptr != hk)
	{
		// Get the info needed for the enumeration
		err = ::RegQueryInfoKey(hk, NULL, NULL, NULL, &count, &maxlen, NULL, NULL, NULL, NULL, NULL, NULL);
		maxlen++;
	}
	if (ERROR_SUCCESS != err)
	{
		::SetLastError(err);
		goto Cleanup;
	}
	
	// Create an array of C wstrings to hold the names
	pnames = new PWSTR[count];
	if (nullptr == pnames)
	{
		::SetLastError(ERROR_OUTOFMEMORY);
		goto Cleanup;
	}
	// Populate the array of names
	for (i = 0; i < count; i++)
	{
		pnames[i] = new WCHAR[maxlen];
		if (nullptr == pnames[i])
		{
			::SetLastError(ERROR_OUTOFMEMORY);
			goto Cleanup;
		}
	}
	if (!EnumSubKeys(hk, pnames, count, maxlen))
	{
		goto Cleanup;
	}

	//	OK, should be ready to create the return value
	*names = ref new Array<String^>(count);
	for (int j = 0; j < count; j++)
	{
		(*names)->set(j, ref new String(pnames[j]));
	}

Cleanup:
	if (pnames)
	{
		// Free all allocations so far
		for (i--; i >= 0; i--)
		{
			delete[] pnames[i];
		}
		delete[] pnames;
	}
	if (hk && (hk != (HKEY)hive))
	{
		::RegCloseKey(hk);
	}

	return (*names != nullptr);
}

bool NativeRegistry::GetValues (RegistryHive hive, String ^path, Array<ValueInfo> ^*values)
{
	DWORD count = 0x0;
	DWORD maxlen = 0x0;
	DWORD length = 0x0;
	PWSTR name = NULL;
	HKEY hk = (HKEY)hive;
	LSTATUS err = ERROR_SUCCESS;
	ValueInfo *vals = NULL;
	*values = nullptr;
	bool unexpected = false;
	int i;

	// Get the key we're querying on
	if ((nullptr != path) && (!path->IsEmpty()))
	{
		hk = GetHKey(hk, path->Data(), KEY_QUERY_VALUE, RCOOK_OPEN_EXISTING);
	}
	if (nullptr != hk)
	{
		// Get the info needed for the enumeration
		err = ::RegQueryInfoKey(hk, NULL, NULL, NULL, NULL, NULL, NULL, &count, &maxlen, NULL, NULL, NULL);
		maxlen++;	// For the NULL character
	}
	if (ERROR_SUCCESS != err)
	{
		::SetLastError(err);
		goto Cleanup;
	}
	
	// Create a C array of values
	vals = (ValueInfo*) malloc(sizeof(ValueInfo) * count);
	if (nullptr == vals)
	{
		::SetLastError(ERROR_OUTOFMEMORY);
		goto Cleanup;
	}
	// Populate the values
	name = new WCHAR[maxlen];
	if (nullptr == name)
	{
		::SetLastError(ERROR_OUTOFMEMORY);
		goto Cleanup;
	}
	for (i = 0; i < count; i++)
	{
		// Get the actual value
		length = maxlen;
		err = ::RegEnumValue(hk, i, name, &length, NULL, (LPDWORD)&(vals[i].Type), NULL, (LPDWORD)&(vals[i].Length));
		if (err != ERROR_SUCCESS)
		{
			if (ERROR_NO_MORE_ITEMS == err)
			{
				// Unexpected, but handle it by shortening the returned array
				count = i;
				break;
			}
			else if (ERROR_MORE_DATA == err && !unexpected)
			{
				// name length too short?
				maxlen = length + 1;
				delete[] name;
				name = new WCHAR[maxlen];
				i--;
				unexpected = true;
			}
			else
			{
				::SetLastError(err);
				goto Cleanup;
			}
		}
		else
		{
			// Enum for this val succeeded
			unexpected = false;
			vals[i].Name = ref new String(name);
		}
	}

	*values = ref new Array<ValueInfo>(vals, count);

Cleanup:
	if (name) delete[] name;
	if (vals) ::free(vals);
	if (hk && (hk != (HKEY)hive))
	{
		::RegCloseKey(hk);
	}

	return (values != nullptr);
}

uint32 NativeRegistry::GetError ()
{
	return ::GetLastError();
}


HKEY Registry::GetHKey (HKEY base, PCWSTR path, REGSAM permission, RegCreateOrOpenKey disposition)
{
	HKEY ret = nullptr;
	LSTATUS err;
	DWORD disp = 0x0;
	if (RCOOK_OPEN_EXISTING == disposition)
	{
		err = ::RegOpenKeyExW(base, path, 0x0, permission, &ret);
	}
	else
	{
		err = ::RegCreateKeyEx(base, path, 0x0, NULL, 0x0, permission, NULL, &ret, (PDWORD)&disp);
	}
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		ret = nullptr;
	}
	if ((RCOOK_CREATE_NEW == disposition) && (REG_CREATED_NEW_KEY != disp))
	{
		::SetLastError(ERROR_FILE_EXISTS);
		ret = nullptr;
	}
	return ret;
}

bool Registry::EnumSubKeys (HKEY key, PWSTR *names, DWORD count, DWORD maxlen)
{
	LSTATUS err = ERROR_SUCCESS;
	DWORD length = maxlen;
	for (DWORD i = 0x0; i < count; i++)
	{
		err = ::RegEnumKeyEx(key, i, names[i], &length, NULL, NULL, NULL, NULL);
		if (err != ERROR_SUCCESS)
		{
			if (ERROR_NO_MORE_ITEMS == err)
			{
				// End of the key... might be unexpected, but handle anyhow
				for (; i < count; i++)
				{
					// empty-string the rest of the values
					names[i][0] = L'\0';
				}
				break;
			}
			else
			{
				// An actual error occurred
				::SetLastError(err);
				return false;
			}
		}
		length = maxlen;
	}
	return true;
}
