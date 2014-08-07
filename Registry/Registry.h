/*
 * Registry\Registry.h
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.4.2
 *
 * This file defines the WinRT-visible NativeRegistry class, which enables registry access.
 */

#pragma once

#include "pch.h"

using namespace Platform;

namespace Registry
{
	extern PCWSTR REG_ROOTS[];
	extern String ^SEPARATOR;

#define HKEY_MIN HKEY_CLASSES_ROOT
#define HKEY_MAX HKEY_CURRENT_USER_LOCAL_SETTINGS

#define RootName(KEY) (((UINT)KEY < (UINT)HKEY_MIN || (UINT)KEY > (UINT)HKEY_MAX) ? \
						L"UNKNOWN_HKEY_NAME" : REG_ROOTS[((UINT)KEY) & 0xF])

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
		HKCC = (int) HKEY_CURRENT_CONFIG,
		HKDD = (int) HKEY_DYN_DATA,
		HKCULS = (int) HKEY_CURRENT_USER_LOCAL_SETTINGS
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
		static RegistryKey ^HKDD;
		static RegistryKey ^HKCULS;

		HKEY _root;
		String ^_hivename;
		String ^_path;
		String ^_name;
		String ^_fullname;

		/** This constructor is intended to allow creating subkeys of a key */
		RegistryKey (HKEY hkey, String ^subpath, String ^name);

	public:
		RegistryKey (RegistryHive hive, String ^path);
		//CreateKey
		//OpenKey
		//GetSubKeys
		//GetValues

		//  Public properties
		/** The root key, one of the well-known HKEY values. */
		property RegistryHive Hive { RegistryHive get (); }
		/** The name of the root key, in "HKEY_NAME_FORMAT". Set on demand. */
		property String^ HiveName {String^ get (); }
		/** The name of the current key (final part of the full path). Always set. */
		property String^ Name { String^ get (); }
		/** The path, not including hive name but including final key name. May be NULL. */
		property String^ Path { String^ get (); }
		/** The full path, including hive name and key name. Set on demand. */
		property String^ FullName { String^ get (); }

		// Static readonly root keys
		static property RegistryKey ^HKeyClassesRoot { RegistryKey ^get(); }
		static property RegistryKey ^HKeyCurrentUser { RegistryKey ^get(); }
		static property RegistryKey ^HKeyLocalMachine { RegistryKey ^get(); }
		static property RegistryKey ^HKeyUsers { RegistryKey ^get(); }
		static property RegistryKey ^HKeyPerformanceData { RegistryKey ^get(); }
		static property RegistryKey ^HKeyCurrentConfig { RegistryKey ^get(); }
		static property RegistryKey ^HKeyDynData { RegistryKey ^get(); }
		static property RegistryKey ^HKeyCurrentUserLocalSettings { RegistryKey ^get(); }
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

	public value struct ValueInfo
	{
		String ^Name;
		RegistryType Type;
		uint32 Length;
	};

#define STDREGARGS RegistryHive hive, String ^path
#define STDREGVALARGS STDREGARGS, String ^value

    public ref class NativeRegistry sealed
    {
        NativeRegistry();
    public:
		// Value read/write functions
		static bool ReadDWORD (STDREGVALARGS, uint32 *data);
		static bool WriteDWORD (STDREGVALARGS, uint32 data);
		static bool ReadString (STDREGVALARGS, String ^*data);
		static bool WriteString (STDREGVALARGS, String ^data);
		static bool ReadMultiString (STDREGVALARGS, Array<String^> ^*data);
		static bool WriteMultiString (STDREGVALARGS, const Array<String^> ^data);
		static bool ReadBinary (STDREGVALARGS, Array<uint8> ^*data);
		static bool WriteBinary (STDREGVALARGS, const Array<uint8> ^data);
		static bool ReadQWORD (STDREGVALARGS, uint64 *data);
		static bool WriteQWORD (STDREGVALARGS, uint64 data);
		static bool QueryValue (STDREGVALARGS, RegistryType *type, Array<uint8> ^*data);
		static bool SetValue (STDREGVALARGS, RegistryType type, const Array<uint8> ^data);
		// Key and value manipulation
		static bool DeleteValue (STDREGVALARGS);
		static bool DeleteKey (STDREGARGS, bool recursive);
		static bool CreateKey (STDREGARGS);
		static bool GetSubKeyNames (STDREGARGS, Array<String^> ^*names);
		static bool GetValues (STDREGARGS, Array<ValueInfo> ^*values);
		// File I/O operations
		static bool SaveKeyToFile (STDREGARGS, String ^filename);
		// Permission functions
		static bool CanWrite (STDREGARGS);
		// Miscellaneous
		static uint32 GetError ();
    };

	HKEY GetHKey (HKEY base, PCWSTR path, REGSAM permission, RegCreateOrOpenKey disposition);
	
	// Gets the names of the subkeys. Maxlen includes null terminator.
	bool EnumSubKeys (HKEY key, PWSTR *names, DWORD count, DWORD maxlen);
}

