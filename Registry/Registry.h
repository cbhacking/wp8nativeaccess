#pragma once

#include "pch.h"

using namespace Platform;

namespace Registry
{
	public enum class RegistryHive
	{
		HKCR = (DWORD_PTR) HKEY_CLASSES_ROOT,
		HKCU = (DWORD_PTR) HKEY_CURRENT_USER,
		HKLM = (DWORD_PTR) HKEY_LOCAL_MACHINE,
		HKU = (DWORD_PTR) HKEY_USERS
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

	HKEY GetHKey (HKEY base, PCWSTR path);
}

