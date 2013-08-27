/*
 * Processes\Processes.h
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.1.0
 *
 * This file defines the WinRT-visible NativeProcess class, which wraps Win32 process APIs.
 * This class and all of its functions are thread-safe except against mid-API changs to the relevant processes.
 */

#pragma once

using namespace Platform;

namespace Processes
{
    public ref class NativeProcess sealed
    {
        NativeProcess ();
    public:
		static uint32 CreateProc (String ^commandLine);
		static bool KillProc (uint32 processID);

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
}