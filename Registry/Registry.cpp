// Registry.cpp
#include "pch.h"
#include "Registry.h"

using namespace Registry;
using namespace Platform;

NativeRegistry::NativeRegistry()
{
}

bool NativeRegistry::ReadDWORD (STDREGARGS, uint32 *data)
{
	DWORD bytes = 4;
	LSTATUS err = ::RegGetValueW(
		(HKEY)hive, path->Data(), value->Data(), RRF_RT_DWORD, NULL, data, &bytes);
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
	LSTATUS err = ::RegGetValueW(
		(HKEY)hive, path->Data(), value->Data(), RRF_RT_REG_SZ, NULL, NULL, &bytes);
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
	LSTATUS err = ::RegOpenKeyExW((HKEY)hive, path->Data(), 0x0, KEY_SET_VALUE, &hkey);
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		return false;
	}
	err = ::RegSetValueExW(hkey, value->Data(), 0x0, REG_DWORD, (PBYTE)(&data), sizeof(uint32));
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
	LSTATUS err = ::RegOpenKeyExW((HKEY)hive, path->Data(), 0x0, KEY_SET_VALUE, &hkey);
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		return false;
	}
	err = ::RegSetValueExW(
		hkey, value->Data(), 0x0, REG_SZ, (PBYTE)(data->Data()), ((data->Length() + 1) * (sizeof(WCHAR))));
	::RegCloseKey(hkey);
	if (ERROR_SUCCESS == err)
	{
		return true;
	}
	::SetLastError(err);
	return false;
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