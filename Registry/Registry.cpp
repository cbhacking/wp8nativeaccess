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
	LSTATUS err = RegGetValueW(
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
	LSTATUS err = RegGetValueW(
		(HKEY)hive, path->Data(), value->Data(), RRF_RT_REG_SZ, NULL, NULL, &bytes);
	if (ERROR_SUCCESS == err)
	{
		// Got the length...
		PWSTR str = new WCHAR[bytes];
		err = RegGetValueW(
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
	return false;
}

bool NativeRegistry::WriteString (STDREGARGS, String ^data)
{
	return false;
}

uint32 NativeRegistry::GetError ()
{
	return ::GetLastError();
}


HKEY Registry::GetHKey (HKEY base, PCWSTR path)
{
	HKEY ret = nullptr;
	LONG err = ::RegOpenKeyExW(base, path, 0, KEY_ALL_ACCESS, &ret);
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		ret = nullptr;
	}
	return ret;
}