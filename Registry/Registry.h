﻿/*
 * Registry\Registry.h
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.2.2
 *
 * This file defines the WinRT-visible NativeRegistry class, which enables registry access.
 */

#pragma once

#include "pch.h"

using namespace Platform;

namespace Registry
{
	extern PCWSTR REG_ROOTS[];
#define RootName(KEY) (REG_ROOTS[((uint32)KEY) & 0xF])

	enum RegCreateOrOpenKey
	{
		RCOOK_DONT_CARE,
		RCOOK_CREATE_NEW,
		RCOOK_OPEN_EXISTING
	};

	public enum class RegistryType
	{
		None = REG_NONE,
		String = REG_SZ,
		VariableString = REG_EXPAND_SZ,
		Binary = REG_BINARY,
		Integer = REG_DWORD,
		IntegerBigEndian = REG_DWORD_BIG_ENDIAN,
		SymbolicLink = REG_LINK,
		MultiString = REG_MULTI_SZ,
		ResourceList = REG_RESOURCE_LIST,
		HardwareResourceLIst = REG_FULL_RESOURCE_DESCRIPTOR,
		ResourceRequirement = REG_RESOURCE_REQUIREMENTS_LIST,
		Long = REG_QWORD
	};

	public enum class RegistryHive
	{
		HKCR = (int) HKEY_CLASSES_ROOT,
		HKCU = (int) HKEY_CURRENT_USER,
		HKLM = (int) HKEY_LOCAL_MACHINE,
		HKU = (int) HKEY_USERS,
		HKPD = (int) HKEY_PERFORMANCE_DATA,
		HKCC = (int) HKEY_CURRENT_CONFIG
	};

	public ref class RegistryKey sealed
	{
	private:
		static RegistryKey ^HKCR;
		static RegistryKey ^HKCU;
		static RegistryKey ^HKLM;
		static RegistryKey ^HKU;
		static RegistryKey ^HKPD;
		static RegistryKey ^HKCC;

		HKEY _root;
		String ^_path;
		String ^_name;
		String ^_fullname;

		RegistryKey (HKEY hkey, String ^path, String ^name);

	public:
		RegistryKey (RegistryHive hive, String ^path);
		//CreateKey
		//OpenKey
		//GetSubKeys
		//GetValues

		//  Public properties
		property String^ Name { String^ get (); }
		property String^ Path { String^ get (); }
		property String^ FullName { String^ get (); }

		// Static readonly root keys
		static property RegistryKey ^HKeyClassesRoot { RegistryKey ^get(); }
		static property RegistryKey ^HKeyCurrentUser { RegistryKey ^get(); }
		static property RegistryKey ^HKeyLocalMachine { RegistryKey ^get(); }
		static property RegistryKey ^HKeyUsers { RegistryKey ^get(); }
		static property RegistryKey ^HKeyPerformanceData { RegistryKey ^get(); }
		static property RegistryKey ^HKeyCurrentConfig { RegistryKey ^get(); }
		static RegistryKey^ GetRootKey (RegistryHive hive);
	};

	public ref class RegistryValue sealed
	{
		String ^_name;
		String ^_fullpath;
		RegistryKey ^_key;
		RegistryType _type;
		Object ^_data;

	public:
		//property String^ Name { String^ get (); };
		//property RegistryKey^ Key { RegistryKey^ get (); };
		//property RegistryType Type { RegistryType get (); };
		//property Object^ Data { Object^ get (); };
	};

#define STDREGARGS RegistryHive hive, String ^path, String ^value

    public ref class NativeRegistry sealed
    {
    public:
        NativeRegistry();

		static bool ReadDWORD (STDREGARGS, uint32 *data);
		static bool ReadString (STDREGARGS, String ^*data);
		static bool WriteDWORD (STDREGARGS, uint32 data);
		static bool WriteString (STDREGARGS, String ^data);
		static bool DeleteValue (STDREGARGS);
		static bool DeleteKey (RegistryHive hive, String ^path, bool recursive);
		static bool CreateKey (RegistryHive hive, String ^path);
		static bool GetSubKeyNames (RegistryHive hive, String ^path, Array<String^> ^*names);
		static uint32 GetError ();
    };

	HKEY GetHKey (HKEY base, PCWSTR path, REGSAM permission, RegCreateOrOpenKey disposition);
	
	// Gets the names of the subkeys. Maxlen includes null terminator.
	bool EnumSubKeys (HKEY key, PWSTR *names, DWORD count, DWORD maxlen);
}

