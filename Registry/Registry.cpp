/*
 * Registry\Registry.cpp
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.1.1
 *
 * This file implements the WinRT-visible registry access functions.
 */

#include "pch.h"
#include "Registry.h"

using namespace Registry;
using namespace Platform;

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
	err = ::RegDeleteKeyExW((HKEY)hive, path->Data(), 0x0, 0x0);
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		return false;
	}
	return true;
}

uint32 NativeRegistry::GetError ()
{
	return ::GetLastError();
}


HKEY Registry::GetHKey (HKEY base, PCWSTR path, REGSAM permission)
{
	HKEY ret = nullptr;
	LONG err = ::RegOpenKeyExW(base, path, 0x0, permission, &ret);
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		ret = nullptr;
	}
	return ret;
}