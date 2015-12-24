﻿/*
 * Registry\Registry.cpp
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.4.5
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
		L"HKEY_CURRENT_CONFIG",
		L"HKEY_DYN_DATA",
		L"HKEY_CURRENT_USER_LOCAL_SETTINGS"};

String^ Registry::SEPARATOR = ref new String(L"\\");

NativeRegistry::NativeRegistry()
{
}

uint32 NativeRegistry::InitializeRoot()
{
#if USE_ROOTRPC
	return ::InitRootRpc();
#else
	return ERROR_CALL_NOT_IMPLEMENTED;
#endif
}

bool NativeRegistry::ReadDWORD (STDREGVALARGS, uint32 *data)
{
	DWORD bytes = sizeof(*data);
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : NULL;
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = ::RegGetValue(
		(HKEY)hive, key, val, RRF_RT_DWORD, NULL, data, &bytes);
	if (ERROR_SUCCESS == err)
	{
		return true;
	}
	::SetLastError(err);
	*data = 0x0;
	return false;
}

bool NativeRegistry::ReadString (STDREGVALARGS, String ^*data)
{
	DWORD bytes = 0;
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : NULL;
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = ::RegGetValue((HKEY)hive, key, val, RRF_RT_REG_SZ, NULL, NULL, &bytes);
	if (ERROR_SUCCESS == err)
	{
		// Got the length...
		size_t chars = bytes / sizeof(WCHAR);
		PWSTR str = new WCHAR[chars + 1];
		// Read the value and null the extra byte, just in case
		err = ::RegGetValue(
			(HKEY)hive, key, val, RRF_RT_REG_SZ, NULL, str, &bytes);
		str[chars] = L'\0';
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

bool NativeRegistry::ReadMultiString (STDREGVALARGS, Array<String^> ^*data)
{
	std::vector<String^> strings;
	DWORD bytes = 0;
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : NULL;
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = ::RegGetValue((HKEY)hive, key, val, RRF_RT_REG_MULTI_SZ, NULL, NULL, &bytes);
	if (ERROR_SUCCESS == err)
	{
		// Got the length...
		size_t chars = bytes / sizeof(WCHAR);
		PWSTR strbuf = new WCHAR[chars + 1];
		PWSTR strptr = strbuf;
		// Read value then null the last character
		err = ::RegGetValue((HKEY)hive, key, val, RRF_RT_REG_MULTI_SZ, NULL, strbuf, &bytes);
		strbuf[chars] = L'\0';
		if (ERROR_SUCCESS == err)
		{
			//Apparently, the second call tends to return a lesser size?
			chars = bytes / sizeof(WCHAR);
			PWSTR endpoint = strbuf + chars;
			PWSTR lastchar = endpoint - 1;
			while (strptr < endpoint)
			{
				// Check for empty string at end of buffer
				if (!(*strptr) && (strptr == lastchar))
				{
					// Don't return the empty string "between" the two terminating null chars
					break;
				}
				// Get a string and add it to the vector
				String ^curstr = ref new String(strptr);
				strings.push_back(curstr);
				// Advance the pointer by the string's length plus its null
				strptr += (curstr->Length() + 1);
			}
			*data = ref new Array<String^>(strings.data(), strings.size());
			delete[] strbuf;
			return true;
		}
		else
		{
			delete[] strbuf;
		}
	}
	// Can't get here without something going wrong...
	::SetLastError(err);
	*data = nullptr;
	return false;
}

bool NativeRegistry::ReadBinary (STDREGVALARGS, Array<uint8> ^*data)
{
	DWORD bytes = 0;
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : NULL;
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = ::RegGetValue((HKEY)hive, key, val, RRF_RT_REG_BINARY, NULL, NULL, &bytes);
	if (ERROR_SUCCESS == err)
	{
		// Got the length...
		PBYTE buf = new BYTE[bytes];
//		err = ::RegGetValue((HKEY)hive, path->Data(), value->Data(), RRF_RT_REG_BINARY, NULL, buf, &bytes);
		err = ::RegGetValue((HKEY)hive, key, val, RRF_RT_ANY, NULL, buf, &bytes);
		if (ERROR_SUCCESS == err)
		{
			*data = ref new Array<uint8>(buf, bytes);
			delete[] buf;
			return true;
		}
		delete[] buf;
	}
	::SetLastError(err);
	*data = nullptr;
	return false;
}

bool NativeRegistry::ReadQWORD (STDREGVALARGS, uint64 *data)
{
	DWORD bytes = sizeof(*data);
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : NULL;
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = ::RegGetValue(
		(HKEY)hive, key, val, RRF_RT_QWORD, NULL, data, &bytes);
	if (ERROR_SUCCESS == err)
	{
		return true;
	}
	::SetLastError(err);
	*data = 0x0LL;
	return false;
}

bool NativeRegistry::WriteDWORD (STDREGVALARGS, uint32 data)
{
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : L"";
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = RegSetKeyValue(
		(HKEY)hive, key, val, REG_DWORD, &data, sizeof(uint32));
/*	HKEY hkey = NULL;
	LSTATUS err = ::RegOpenKeyEx((HKEY)hive, key, 0x0, KEY_SET_VALUE, &hkey);
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		return false;
	}
	err = ::RegSetValueEx(hkey, val, 0x0, REG_DWORD, (PBYTE)(&data), sizeof(data));
	::RegCloseKey(hkey);*/
	if (ERROR_SUCCESS == err)
	{
		return true;
	}
	::SetLastError(err);
	return false;
}

bool NativeRegistry::WriteString (STDREGVALARGS, String ^data)
{
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : L"";
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = RegSetKeyValue(
		(HKEY)hive, key, val, REG_SZ, data->Data(), ((data->Length() + 1) * (sizeof(WCHAR))));
/*	HKEY hkey = NULL;
	LSTATUS err = ::RegOpenKeyEx((HKEY)hive, key, 0x0, KEY_SET_VALUE, &hkey);
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		return false;
	}
	err = ::RegSetValueEx(hkey, val, 0x0, REG_SZ, (PBYTE)(data->Data()), ((data->Length() + 1) * (sizeof(WCHAR))));
	::RegCloseKey(hkey);*/
	if (ERROR_SUCCESS == err)
	{
		return true;
	}
	::SetLastError(err);
	return false;
}

bool NativeRegistry::WriteMultiString (STDREGVALARGS, const Array<String^> ^data)
{
	// Build the buffer
	size_t chars = 0x0;
	for (String ^*str = data->begin(); str != data->end(); str++)
	{
		chars += (*str)->Length();
		chars++;	// For the null character
	}
	PWCHAR buffer = new WCHAR[++chars];	// pre-increment for extra null
	int index = 0;
	for (String ^*str = data->begin(); str != data->end(); str++)
	{
		// Copy this string, including its terminating null
		::memcpy(buffer + index, (*str)->Data(), (((*str)->Length() + 1) * sizeof(WCHAR)));
		index += ((*str)->Length() + 1);
	}
	buffer[chars - 1] = L'\0';

	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : L"";
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = ::RegSetKeyValue(
		(HKEY)hive, key, val, REG_MULTI_SZ, buffer, chars * sizeof(WCHAR));
/*	HKEY hkey = NULL;
	LSTATUS err = ::RegOpenKeyEx((HKEY)hive, key, 0x0, KEY_SET_VALUE, &hkey);
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		return false;
	}
	err = ::RegSetValueEx(hkey, val, 0x0, REG_MULTI_SZ, (PBYTE)(buffer), (chars * (sizeof(WCHAR))));
	::RegCloseKey(hkey);*/
	delete[] buffer;
	if (ERROR_SUCCESS == err)
	{
		return true;
	}
	::SetLastError(err);
	return false;
}

bool NativeRegistry::WriteBinary (STDREGVALARGS, const Array<uint8> ^data)
{
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : L"";
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = ::RegSetKeyValue(
		(HKEY)hive, key, val, REG_BINARY, data->Data, data->Length);
/*	HKEY hkey = NULL;
	LSTATUS err = ::RegOpenKeyEx((HKEY)hive, key, 0x0, KEY_SET_VALUE, &hkey);
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		return false;
	}
	err = ::RegSetValueEx(hkey, val, 0x0, REG_BINARY, data->Data, data->Length);
	::RegCloseKey(hkey);*/
	if (ERROR_SUCCESS == err)
	{
		return true;
	}
	::SetLastError(err);
	return false;
}

bool NativeRegistry::WriteQWORD (STDREGVALARGS, uint64 data)
{
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : L"";
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = ::RegSetKeyValue(
		(HKEY)hive, key, val, REG_QWORD, &data, sizeof(data));
/*	HKEY hkey = NULL;
	LSTATUS err = ::RegOpenKeyExW((HKEY)hive, key, 0x0, KEY_SET_VALUE, &hkey);
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		return false;
	}
	err = ::RegSetValueEx(hkey, val, 0x0, REG_QWORD, (PBYTE)(&data), sizeof(data));
	::RegCloseKey(hkey);*/
	if (ERROR_SUCCESS == err)
	{
		return true;
	}
	::SetLastError(err);
	return false;
}

bool NativeRegistry::QueryValue (STDREGVALARGS, RegistryType *type, Array<uint8> ^*data)
{
	HKEY hkey = NULL;
	*data = nullptr;
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : L"";
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = ::RegOpenKeyEx((HKEY)hive, key, 0x0, KEY_QUERY_VALUE, &hkey);
	if (err != ERROR_SUCCESS)
	{
		// Unable to open key
		::SetLastError(err);
		return false;
	}
	// Get the data length
	DWORD len = 0x0;
	err = ::RegQueryValueEx(hkey, val, NULL, NULL, NULL, &len);
	if (ERROR_SUCCESS == err)
	{
		// Got the length, now for data and type
		uint8* buf = new uint8[len];
		err = ::RegQueryValueEx(hkey, val, NULL, (LPDWORD)type, buf, &len);
		if (ERROR_SUCCESS == err)
		{
			*data = ref new Array<uint8>(buf, len);
		}
		delete[] buf;
	}
	::RegCloseKey(hkey);
	::SetLastError(err);
	return !err;
}

bool NativeRegistry::SetValue (STDREGVALARGS, RegistryType type, const Array<uint8> ^data)
{
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : L"";
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err = ::RegSetKeyValue(
		(HKEY)hive, key, val, (DWORD)type, data->Data, data->Length);
/*	HKEY hkey = NULL;
	LSTATUS err = ::RegOpenKeyEx((HKEY)hive, key, 0x0, KEY_SET_VALUE, &hkey);
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		return false;
	}
	err = ::RegSetValueEx(hkey, val, 0x0, (DWORD)type, data->Data, data->Length);
	::RegCloseKey(hkey);*/
	if (ERROR_SUCCESS == err)
	{
		return true;
	}
	::SetLastError(err);
	return false;
}

bool NativeRegistry::DeleteValue (STDREGVALARGS)
{
	// Key or value name can be null; in that case, use the default value and/or the specified key
	HKEY hkey = (HKEY)hive;
	PCWSTR val = value ? value->Data() : NULL;
	LSTATUS err;
	if ((nullptr != path) && (path->Length() > 0))
	{
		// Need to open a sub-key
		err = ::RegOpenKeyEx((HKEY)hive, path->Data(), 0x0, KEY_SET_VALUE, &hkey);
		if (err != ERROR_SUCCESS)
		{
			::SetLastError(err);
			return false;
		}
	}
	err = ::RegDeleteValue(hkey, val);
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
		err = ::RegDeleteTree((HKEY)hive, path->Data());
		if (err != ERROR_SUCCESS)
		{
			::SetLastError(err);
			return false;
		}
	}
	err = ::RegDeleteKey((HKEY)hive, path->Data());
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

bool NativeRegistry::RenameKey (RegistryHive hive, String ^path, String ^newname)
{
	LSTATUS stat = ::RegRenameKey((HKEY)hive, (path ? path->Data() : L""), newname->Data());
	if (ERROR_SUCCESS != stat)
	{
		::SetLastError((DWORD)stat);
		return false;
	}
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
	if (hk)
	{
		// Get the info needed for the enumeration
		err = ::RegQueryInfoKey(hk, NULL, NULL, NULL, &count, &maxlen, NULL, NULL, NULL, NULL, NULL, NULL);
		maxlen++;
	}
	else
	{
		goto Cleanup;
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
	for (i = 0; i < (int)count; i++)
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
	for (unsigned j = 0; j < count; j++)
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
	if (hk)
	{
		// Get the info needed for the enumeration
		err = ::RegQueryInfoKey(hk, NULL, NULL, NULL, NULL, NULL, NULL, &count, &maxlen, NULL, NULL, NULL);
		maxlen++;	// For the NULL character
	}
	else
	{
		goto Cleanup;
	}
	if (ERROR_SUCCESS != err)
	{
		::SetLastError(err);
		goto Cleanup;
	}
	
	//vals = new ValueInfo[count];
	// C++/CX doesn't like doing new on a value type. Fine then...
	// Create a C array of values. Clear it before use though, so the smart pointers don't get upset
	vals = (ValueInfo*) ::calloc(count, sizeof(ValueInfo));
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
	for (i = 0; i < (int)count; i++)
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
	if (vals)
	{
		// Clean up the string references
		for (i = 0; (unsigned)i < count; i++) vals[i].Name = nullptr;
		::free(vals);
	}
	if (hk && (hk != (HKEY)hive))
	{
		::RegCloseKey(hk);
	}

	return (values != nullptr);
}

bool NativeRegistry::SaveKeyToFile (STDREGARGS, String ^filename)
{
	bool ret = false;
	HKEY hk = (HKEY)hive;
	PWSTR p = path ? path->Data() : L"";
	if (!filename || !(filename->Length()))
	{
		::SetLastError(ERROR_BAD_ARGUMENTS);
		goto Cleanup;
	}
	// Make sure not to pass a null path, since we want a new handle
	hk = GetHKey((HKEY)hive, p,
		KEY_READ | KEY_QUERY_VALUE | KEY_ENUMERATE_SUB_KEYS,
		RegCreateOrOpenKey::RCOOK_OPEN_EXISTING);
	if (!hk) goto Cleanup;	// Should already have set last error
	LONG err = ::RegSaveKey(hk, filename->Data(), nullptr);
	if (ERROR_SUCCESS != err)
	{
		::SetLastError(err);
		goto Cleanup;
	}
	ret = true;
Cleanup:
	if (hk && (hk != (HKEY)hive))
	{
		::RegCloseKey(hk);
	}
	return ret;
}

bool NativeRegistry::CanWrite (STDREGARGS)
{
	HKEY hkey = NULL;
	// Key or value name can be null; in that case, use the default value and/or the specified key
	PCWSTR key = path ? path->Data() : L"";
	LSTATUS err = ::RegOpenKeyEx((HKEY)hive, key, 0x0, KEY_SET_VALUE | KEY_CREATE_SUB_KEY, &hkey);
	if (err != ERROR_SUCCESS)
	{
		::SetLastError(err);
		return false;
	}
	// Open succeeded, so clean up
	::RegCloseKey(hkey);
	return true;
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
		err = ::RegOpenKeyEx(base, path, 0x0, permission, &ret);
	}
	else
	{
		err = ::RegCreateKeyEx(base, path, 0x0, NULL, 0x0, permission, NULL, &ret, &disp);
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
