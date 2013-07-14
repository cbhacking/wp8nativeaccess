/*
 * FileSystem\FileSystem.cpp
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.2.0
 *
 * This file implements the WinRT-visible wrappers around Win32 file APIs.
 * All functions are thread-safe except against mid-API changs the file system itself.
 */

#include "pch.h"
#include "FileSystem.h"

using namespace FileSystem;
using namespace Platform;

NativeFileSystem::NativeFileSystem ()
{
}

String^ NativeFileSystem::GetFileNames (String ^pattern)
{
	WIN32_FIND_DATA data;
	HANDLE finder = ::FindFirstFileEx(pattern->Data(), FindExInfoBasic, &data, FindExSearchNameMatch, NULL, 0);
	if (INVALID_HANDLE_VALUE == finder)
	{
		return nullptr;
	}
	String ^ret = "";
	BOOL res;
	do
	{
		ret += ref new String(data.cFileName) + "|";
		res = ::FindNextFile(finder, &data);
	} while (res);
	if (GetLastError() != ERROR_NO_MORE_FILES)
	{
		ret = nullptr;
	}
	else
	{
		WCHAR *buf = new WCHAR[ret->Length()];
		memcpy(buf, ret->Data(), (ret->Length() * sizeof(WCHAR)));
		buf[ret->Length() - 1] = L'\0';
		ret = ref new String(buf);
		delete[] buf;
	}
	::CloseHandle(finder);
	return ret;
}

Array<BYTE, 1>^ NativeFileSystem::ReadFile (String ^path)
{
	HANDLE file = ::CreateFile2(path->Data(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, NULL);
	if (INVALID_HANDLE_VALUE == file) return nullptr;
	FILE_STANDARD_INFO info;
	if (!::GetFileInformationByHandleEx(file, FileStandardInfo, &info, sizeof(FILE_STANDARD_INFO)))
	{
		::CloseHandle(file);
		return nullptr;
	}
	Array<BYTE, 1> ^ret = ref new Array<BYTE, 1>(info.EndOfFile.LowPart);
	DWORD bytes = 0;
	if (!::ReadFile(file, ret->Data, info.EndOfFile.LowPart, &bytes, NULL))
	{
		::CloseHandle(file);
		return nullptr;
	}
	::CloseHandle(file);
	return ret;
}

bool NativeFileSystem::WriteFile (String ^path, const Array<BYTE> ^data)
{
	HANDLE file = ::CreateFile2(path->Data(), GENERIC_WRITE, 0, CREATE_ALWAYS, NULL);
	if (INVALID_HANDLE_VALUE == file) return false;
	if (data && data->Length)
	{
		DWORD bytes = 0;
		if (!::WriteFile(file, data->Data, data->Length, &bytes, NULL))
		{
			::CloseHandle(file);
			return false;
		}
	}
	::CloseHandle(file);
	return true;;
}

bool NativeFileSystem::DeleteFile (String ^path)
{
	return (0 != ::DeleteFileW(path->Data()));
}

int NativeFileSystem::GetError ()
{
	return ::GetLastError();
}
