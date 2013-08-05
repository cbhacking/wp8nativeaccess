/*
 * Registry\Registry.h
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.1.0
 *
 * This file defines the WinRT-visible NativeRegistry class, which enables registry access.
 */

#pragma once

#include "pch.h"

using namespace Platform;

namespace Registry
{
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

		bool ReadDWORD (STDREGARGS, uint32 *data);
		bool ReadString (STDREGARGS, String ^*data);
		bool WriteDWORD (STDREGARGS, uint32 data);
		bool WriteString (STDREGARGS, String ^data);
		bool DeleteValue (STDREGARGS);
		bool DeleteKey (RegistryHive hive, String ^path, bool recursive);
		uint32 GetError ();
    };

	HKEY GetHKey (HKEY base, PCWSTR path, REGSAM permission);
}

