/*
 * Processes\Processes.h
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.1.3
 *
 * This file defines the WinRT-visible NativeProcess class, which wraps Win32 process APIs.
 * This class and all of its functions are thread-safe except against mid-API changs to the relevant processes.
 */

#pragma once

using namespace Platform;

namespace Processes
{
	public value struct ProcessInfo
	{
		uint32 ProcessID;
		String ^ImageName;
	};

	public value struct TokenPrivilege
	{
		int64 Luid;
		String ^Name;
		String ^Purpose;
		Boolean Enabled;
		Boolean Default;
		Boolean Removed;
		Boolean WasUsed;
	};

    public ref class NativeProcess sealed
    {
        NativeProcess ();
    public:
		static uint32 CreateProc (String ^commandLine);
		static bool KillProc (uint32 processID);
		static Array<uint32>^ GetProcessIds ();
		static Array<ProcessInfo>^ GetProcesses ();
		static Array<uint32>^ SearchProcessIds ();
		static Array<ProcessInfo>^ SearchProcesses ();
		static uint32 CreateProcInChamber (String ^sid, String ^commandLine);

		static uint32 GetError ();
    };

	public ref class CurrentProcess sealed
	{
	public:
		static property uint32 ProcessId { uint32 get(); }
		static property String^ CommandLine { String^ get(); }
		static String^ GetEnvironmentVar (String ^name);
		static bool SetEnvironmentVar (String ^name, String ^value);
	};

	public ref class Security sealed
	{
	public:
		static uint32 TryGetCurrentToken ();
		static uint32 LogonUser (String ^username, String ^password, uint32 type);
		static Array<TokenPrivilege>^ GetPrivileges (uint32 processId);
		static String^ GetChamberSid (String ^identifier);
	};
}