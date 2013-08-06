/*
 * Registry\Registry.h
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.2.0
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

	public ref class RegistryKey sealed
	{
	private:
		static RegistryKey ^HKCR;
		static RegistryKey ^HKCU;
		static RegistryKey ^HKLM;
		static RegistryKey ^HKU;
		static RegistryKey ^HKCC;

		HKEY _hkey;
		String ^_path;
		String ^_name;
		String ^_fullname;
		bool _opened;

		RegistryKey (HKEY hkey, String ^path, String ^name, bool needsClosing);
		~RegistryKey ();

	public:
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
		static property RegistryKey ^HKeyCurrentConfig { RegistryKey ^get(); }
	};

	public value struct RegistryValue
	{
		String ^name;

	};

	public enum class RegistryHive
	{
		HKCR = (int) HKEY_CLASSES_ROOT,
		HKCU = (int) HKEY_CURRENT_USER,
		HKLM = (int) HKEY_LOCAL_MACHINE,
		HKU = (int) HKEY_USERS,
		HKCC = (int) HKEY_CURRENT_CONFIG
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
		static uint32 GetError ();
    };

	HKEY GetHKey (HKEY base, PCWSTR path, REGSAM permission);
}

