/*
 * Registry\RegKey.cpp
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.2.1
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

RegistryKey^ RegistryKey::HKeyClassesRoot::get ()
{
	if (!HKCR)
	{
		HKCR = ref new RegistryKey(HKEY_CLASSES_ROOT, nullptr, ref new String(RootName(HKEY_CLASSES_ROOT)));
	}
	return HKCR;
}

RegistryKey^ RegistryKey::HKeyCurrentUser::get ()
{
	if (!HKCU)
	{
		HKCU = ref new RegistryKey(HKEY_CURRENT_USER, nullptr, ref new String(RootName(HKEY_CURRENT_USER)));
	}
	return HKCU;
}

RegistryKey^ RegistryKey::HKeyLocalMachine::get ()
{
	if (!HKLM)
	{
		HKLM = ref new RegistryKey(HKEY_LOCAL_MACHINE, nullptr, ref new String(RootName(HKEY_LOCAL_MACHINE)));
	}
	return HKLM;
}

RegistryKey^ RegistryKey::HKeyUsers::get ()
{
	if (!HKU)
	{
		HKU = ref new RegistryKey(HKEY_USERS, nullptr, ref new String(RootName(HKEY_USERS)));
	}
	return HKU;
}

RegistryKey^ RegistryKey::HKeyPerformanceData::get ()
{
	if (!HKPD)
	{
		HKPD = ref new RegistryKey(HKEY_PERFORMANCE_DATA, nullptr, ref new String(RootName(HKEY_PERFORMANCE_DATA)));
	}
	return HKPD;
}

RegistryKey^ RegistryKey::HKeyCurrentConfig::get ()
{
	if (!HKCC)
	{
		HKCC = ref new RegistryKey(HKEY_CURRENT_CONFIG, nullptr, ref new String(RootName(HKEY_CURRENT_CONFIG)));
	}
	return HKCC;
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
	default:
		throw ref new InvalidArgumentException();
	}
}

RegistryKey::RegistryKey (HKEY hkey, String ^path, String ^name)
	: _root((HKEY)hkey), _path(path), _name(name), _fullname(nullptr)
{
}

RegistryKey::RegistryKey (RegistryHive hive, String ^path)
	: _root((HKEY)hive), _fullname(nullptr)
{
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
		_fullname = (_path ? _path + _name : _name);
	}
	return _fullname;
}