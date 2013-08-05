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
		HKU = (int) HKEY_USERS
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
		uint32 GetError ();
    };

	HKEY GetHKey (HKEY base, PCWSTR path, REGSAM permission);
}

