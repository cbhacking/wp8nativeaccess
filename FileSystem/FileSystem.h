/*
 * FileSystem\FileSystem.h
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.2.0
 *
 * This file defines the WinRT-visible NativeFileSystem class, which wraps Win32 file APIs.
 * This class and all of its functions are thread-safe except against mid-API changs the file system itself.
 */

#pragma once

using namespace Platform;

namespace FileSystem
{
	public ref class NativeFileSystem sealed
	{
	public:
		NativeFileSystem ();
		String^ GetFileNames (String ^pattern);
		Array<BYTE>^ ReadFile (String ^path);
		bool WriteFile (String ^path, const Array<BYTE> ^data);
#undef DeleteFile
		bool DeleteFile (String ^path);
		int GetError ();
	};
}