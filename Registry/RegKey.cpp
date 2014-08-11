/*
 * Registry\RegKey.cpp
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.4.3
 *
 * This file implements the WinRT-visible RegistryKey class.
 */

#include "pch.h"
#include "Registry.h"

using namespace Registry;
using namespace Platform;


RegistryKey^ RegistryKey::HKCR = nullptr;
RegistryKey^ RegistryKey::HKCU = nullptr;
RegistryKey^ RegistryKey::HKLM = nullptr;
RegistryKey^ RegistryKey::HKU = nullptr;
RegistryKey^ RegistryKey::HKPD = nullptr;
RegistryKey^ RegistryKey::HKCC = nullptr;
RegistryKey^ RegistryKey::HKDD = nullptr;
RegistryKey^ RegistryKey::HKCULS = nullptr;

RegistryKey^ RegistryKey::HKeyClassesRoot::get ()
{
	if (!HKCR)
	{
		HKCR = ref new RegistryKey(HKEY_CLASSES_ROOT, nullptr, nullptr);
	}
	return HKCR;
}

RegistryKey^ RegistryKey::HKeyCurrentUser::get ()
{
	if (!HKCU)
	{
		HKCU = ref new RegistryKey(HKEY_CURRENT_USER, nullptr, nullptr);
	}
	return HKCU;
}

RegistryKey^ RegistryKey::HKeyLocalMachine::get ()
{
	if (!HKLM)
	{
		HKLM = ref new RegistryKey(HKEY_LOCAL_MACHINE, nullptr, nullptr);
	}
	return HKLM;
}

RegistryKey^ RegistryKey::HKeyUsers::get ()
{
	if (!HKU)
	{
		HKU = ref new RegistryKey(HKEY_USERS, nullptr, nullptr);
	}
	return HKU;
}

RegistryKey^ RegistryKey::HKeyPerformanceData::get ()
{
	if (!HKPD)
	{
		HKPD = ref new RegistryKey(HKEY_PERFORMANCE_DATA, nullptr, nullptr);
	}
	return HKPD;
}

RegistryKey^ RegistryKey::HKeyCurrentConfig::get ()
{
	if (!HKCC)
	{
		HKCC = ref new RegistryKey(HKEY_CURRENT_CONFIG, nullptr, nullptr);
	}
	return HKCC;
}

RegistryKey^ RegistryKey::HKeyDynData::get ()
{
	if (!HKDD)
	{
		HKDD = ref new RegistryKey(HKEY_DYN_DATA, nullptr, nullptr);
	}
	return HKDD;
}

RegistryKey^ RegistryKey::HKeyCurrentUserLocalSettings::get ()
{
	if (!HKCULS)
	{
		HKCULS = ref new RegistryKey(HKEY_CURRENT_USER_LOCAL_SETTINGS, nullptr, nullptr);
	}
	return HKCULS;
}

RegistryKey^ RegistryKey::GetRootKey (RegistryHive hive)
{
	switch (hive)
	{
	case RegistryHive::HKCR:
		return HKeyClassesRoot;
	case RegistryHive::HKCU:
		return HKeyCurrentUser;
	case RegistryHive::HKLM:
		return HKeyLocalMachine;
	case RegistryHive::HKU:
		return HKeyUsers;
	case RegistryHive::HKPD:
		return HKeyPerformanceData;
	case RegistryHive::HKCC:
		return HKeyCurrentConfig;
	case RegistryHive::HKDD:
		return HKeyDynData;
	case RegistryHive::HKCULS:
		return HKeyCurrentUserLocalSettings;
	default:
		throw ref new InvalidArgumentException();
	}
}

RegistryKey::RegistryKey (HKEY hkey, String ^subpath, String ^name)
	: _root(hkey)
{
	if (!name || !name->Length())
	{
		if (((UINT)hkey < (UINT)HKEY_MIN) || (UINT)hkey > (UINT)HKEY_MAX)
		{
			// Not a valid key for the convenience constructors
			throw ref new InvalidArgumentException(ref new String(L"RegistryKey requires non-null name!"));
		}
		else
		{
			_name = this->HiveName;
			_path = nullptr;
		}
	}
	else
	{
		// First, we will find the actual name
		WCHAR data[MAX_PATH];
		wcscpy_s(data, name->Data());
		// First trim the end of the name, starting with the last character...
		int i = (int)(name->Length()) - 1;
		while (('\\' == data[i] || iswspace(data[i])) && i > -1)
		{
			i--;
		}
		data[i+1] = L'\0';
		// Now trim the front of the name, assuming there's anything left
		for (i = 0; data[i]; i++)
		{
			if (!iswspace(data[i]) && L'\\' != data[i])
			{
				// It's a real character
				break;
			}
		}
		if (!data[i]) throw ref new InvalidArgumentException(ref new String(L"RegistryKey requires real name!"));
		// Check for a "name' that's actually a path fragment
		PCWSTR n = wcsrchr(data + i, L'\\');
		if (n)
		{
			// There was a separator, so take the last part
			_name = ref new String(n + 1);
		}
		else
		{
			// No separator, so take the whole thing
			_name = ref new String(data + i);
		}

		// So much for name, now for path
		if (subpath && subpath->Length())
		{
			WCHAR data2[MAX_PATH];
			wcscpy_s(data2, subpath->Data());
			// First trim the end, starting with the last character...
			int j = (int)subpath->Length() - 1;
			while (('\\' == data2[j] || iswspace(data2[j])) && j > -1)
			{
				j--;
			}
			data2[j+1] = L'\0';
			// Now trim the front, assuming there's anything left
			for (j = 0; data2[j]; j++)
			{
				if (!iswspace(data2[j]) && L'\\' != data2[j])
				{
					// It's a real character
					break;
				}
			}
			_path = data2[j] ? 
				// Append the name, which may contain some path
				ref new String(data2 + j)+ SEPARATOR + _name : _name;
		}
		else
		{
			// No subpath; just use the name (might have some path in it)
			_path = _name;
		}
	}
}

RegistryKey::RegistryKey (RegistryHive hive, String ^path)
	: _root((HKEY)hive)
{
	if (path && path->Length())
	{
		WCHAR data[MAX_PATH];
		wcscpy_s(data, path->Data());
		// First trim the end, starting with the last character...
		int i = (int)path->Length() - 1;
		while (('\\' == data[i] || iswspace(data[i])) && i > -1)
		{
			i--;
		}
		data[i+1] = L'\0';
		// Now trim the front, assuming there's anything left
		for (i = 0; data[i]; i++)
		{
			if (!iswspace(data[i]) && L'\\' != data[i])
			{
				// It's a real character
				break;
			}
		}
		_path = data[i] ? ref new String(data + i) : nullptr;
		if (_path)
		{
			// Get the name from the path
			PCWSTR name = wcsrchr(_path->Data(), L'\\');
			if (name)
			{
				// There was a separator, so take the last part
				_name = ref new String(name + 1);
			}
			else
			{
				// No separator, so take the whole thing
				_name = _path;
			}
		}
		else
		{
			// No path at all, so use the hive key name
			_name = this->HiveName;
		}
	}
	else
	{
		// Wasn't any path to begin with
		_path = nullptr;
		_name = this->HiveName;
	}

}

bool RegistryKey::GetSubKeys (Array<RegistryKey^> ^*sukeys)
{
	// TODO: stub
	return false;
}

RegistryHive RegistryKey::Hive::get ()
{
	return (RegistryHive)(int)_root;
}

String^ RegistryKey::HiveName::get ()
{
	if (!_hivename)
	{
		_hivename = ref new String(RootName(_root));
	}
	return _hivename;
}

String^ RegistryKey::Name::get ()
{
	return _name;
}

String^ RegistryKey::Path::get ()
{
	return _path;
}

String^ RegistryKey::FullName::get ()
{
	if (!_fullname)
	{
		_fullname = _path ? this->HiveName + SEPARATOR + _path : this->HiveName;
	}
	return _fullname;
}

bool RegistryKey::ensureAccess (REGSAM desiredaccess, RegCreateOrOpenKey disposition)
{
	if ((_access == desiredaccess) || (_access & desiredaccess))
	{
		// Desired access is available. Make sure the hkey is open...
		if (_hkey)
		{
			// Old key exists and has enough access; we're good.
			return true;
		}
		else
		{
			// Oddly, the key is not open. Fix that (shouldn't fail but check anyhow).
			HKEY key = Registry::GetHKey(_root, (_path ? _path->Data() : nullptr), _access, disposition);
			if (key && INVALID_HANDLE_VALUE != key)
			{
				// Key opened successfully! We're good to go
				return true;
			}
			else
			{
				// Failed to open or create the key handle. Clear the access mask and key.
				_access = 0x0;
				_hkey = nullptr;
				return false;
			}
		}
	}
	else
	{
		// Desired access not included in current hkey, if any. Get a new one
		REGSAM sam = _access | desiredaccess;
		HKEY key = Registry::GetHKey(_root, (_path ? _path->Data() : nullptr), sam, disposition);
		if (key && INVALID_HANDLE_VALUE != key)
		{
			// Key opened successfully! Check for an old key...
			if (_hkey)
			{
				// Old key exists; swap it out and close it
				HKEY old = _hkey;
				_hkey = key;
				_access = sam;
				::RegCloseKey(old);
				return true;
			}
			else
			{
				// Key not open yet
				_hkey = key;
				_access = sam;
				return true;
			}
		}
		else
		{
			// Creating (or opening) the key failed.
			return false;
		}
	}
}