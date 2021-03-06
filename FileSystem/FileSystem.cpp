﻿/*
 * FileSystem\FileSystem.cpp
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.4.0
 *
 * This file implements the WinRT-visible wrappers around Win32 file APIs.
 * All functions are thread-safe except against mid-API changs the file system itself.
 */

#include "pch.h"
#include "FileSystem.h"

using namespace FileSystem;

NativeFileSystem::NativeFileSystem ()
{
}

#ifdef USE_ROOTRPC
uint32 NativeFileSystem::InitializeRoot()
{
	return ::InitRootRpc();
}
#endif

String^ NativeFileSystem::GetFileNames (String ^pattern)
{
	return GetFileNames(pattern, true, true);
}

String^ NativeFileSystem::GetFileNames (String ^pattern, bool includeFiles, bool includeDirs)
{
	if (!includeFiles && !includeDirs)
	{
		::SetLastError(ERROR_BAD_ARGUMENTS);
		return nullptr;
	}
	WIN32_FIND_DATA data;
	FINDEX_SEARCH_OPS searchop = includeFiles ? FindExSearchNameMatch : FindExSearchLimitToDirectories;
	HANDLE finder = ::FindFirstFileEx(pattern->Data(), FindExInfoBasic, &data, searchop, NULL, 0);
	if (!finder || (INVALID_HANDLE_VALUE == finder))
	{
		if (ERROR_NO_MORE_FILES == ::GetLastError())
		{
			// There simply aren't any matching files/folders...
			::SetLastError(ERROR_SUCCESS);
		}
		return nullptr;
	}
	String ^ret = "";
	do
	{
		if ((includeFiles && !(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) ||
			(includeDirs && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)))
		{
			ret += ref new String(data.cFileName) + "|";
		}
	} while (::FindNextFile(finder, &data));
	if (::GetLastError() != ERROR_NO_MORE_FILES)
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
		::SetLastError(ERROR_SUCCESS);
	}
	::FindClose(finder);
	return ret;
}

Array<FileInfo>^ NativeFileSystem::GetFiles (String ^pattern)
{
	return GetFiles(pattern, false);
}

Array<FileInfo>^ NativeFileSystem::GetFiles (String ^pattern, bool includeDirs)
{
	WIN32_FIND_DATA data;
	Array<FileInfo> ^ret;
	HANDLE finder = ::FindFirstFileEx(pattern->Data(), FindExInfoBasic, &data, FindExSearchNameMatch, NULL, 0);
	if (INVALID_HANDLE_VALUE != finder)
	{
		vector<FileInfo> infos;
		infos.reserve(16);
		do
		{
			if (!includeDirs && (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			{
				continue;
			}
			LARGE_INTEGER li;
			li.HighPart = data.nFileSizeHigh;
			li.LowPart = data.nFileSizeLow;
			FileInfo info = {ref new String(data.cFileName), li.QuadPart, (FileAttributes)(data.dwFileAttributes)};
			infos.push_back(info);
		} while (::FindNextFile(finder, &data));
		if (::GetLastError() == ERROR_NO_MORE_FILES)
		{
			// In case there are no non-directories and we only want files, length will be zero which results in null.
			// Work around this by explicitly clearing the error state
			::SetLastError(ERROR_SUCCESS);
			// Create an array that exactly fits
			ret = ref new Array<FileInfo>(infos.data(), infos.size());
		}
		else
		{
			// There was some other error
			ret = nullptr;
		}
	}
	else if (::GetLastError() == ERROR_NO_MORE_FILES)
	{
		// There *are* no files, but returning a 0-length array doesn't work, so this is a workaround:
		::SetLastError(ERROR_SUCCESS);
		ret = nullptr;
	}
	else
	{
		// Something went wrong opening the find handle
		return nullptr;
	}
	::FindClose(finder);
	return ret;
}

Array<uint8>^ NativeFileSystem::ReadFile (String ^path)
{
	return ReadFile(path, 0, 0xFFFFFFFF);
}

Array<uint8>^ NativeFileSystem::ReadFile (String ^path, int64 offset, uint32 length)
{
	HANDLE file = ::CreateFile2(path->Data(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, NULL);
	if (!file || INVALID_HANDLE_VALUE == file)
	{
		return nullptr;
	}
	FILE_STANDARD_INFO info;
	if (!::GetFileInformationByHandleEx(file, FileStandardInfo, &info, sizeof(FILE_STANDARD_INFO)))
	{
		::CloseHandle(file);
		return nullptr;
	}
	if (info.EndOfFile.QuadPart < offset)
	{
		::SetLastError(ERROR_BAD_ARGUMENTS);
		return nullptr;
	}
	uint32 len = ((info.EndOfFile.QuadPart - offset) > length) ? 
		length	// There's at least length bytes remaining
		: (uint32)(info.EndOfFile.QuadPart - offset);
	Array<uint8> ^ret = ref new Array<uint8>(len);
	DWORD bytes = 0;
	LARGE_INTEGER li; li.QuadPart = offset;
	if (!(::SetFilePointerEx(file, li, NULL, FILE_BEGIN) && 
		::ReadFile(file, ret->Data, len, &bytes, NULL)))
	{
		::CloseHandle(file);
		return nullptr;
	}
	::CloseHandle(file);
	return ret;
}

bool NativeFileSystem::WriteFile (String ^path, const Array<BYTE> ^data)
{
	return WriteFile(path, 0LL, data);
}

bool NativeFileSystem::WriteFile (String ^path, int64 offset, const Array<uint8> ^data)
{
	HANDLE file = ::CreateFile2(path->Data(), GENERIC_WRITE, 0, CREATE_ALWAYS, NULL);
	if (!file || INVALID_HANDLE_VALUE == file)
	{
		return false;
	}
	if (data && data->Length)
	{
		DWORD bytes = 0;
		LARGE_INTEGER li; li.QuadPart = offset;
		if (!(::SetFilePointerEx(file, li, NULL, FILE_BEGIN) && 
			::WriteFile(file, data->Data, data->Length, &bytes, NULL)))
		{
			::CloseHandle(file);
			return false;
		}
	}
	::CloseHandle(file);
	return true;
}

bool NativeFileSystem::CopyFile (String ^sourceName, String ^destName)
{
	HRESULT h = ::CopyFile2(sourceName->Data(), destName->Data(), NULL);
	return SUCCEEDED(h);
}

bool NativeFileSystem::MoveFile (String ^sourceName, String ^destName)
{
	return NativeFileSystem::MoveFile(sourceName, destName, MoveFlags::AcrossVolumes);
}

bool NativeFileSystem::MoveFile (String ^sourceName, String ^destName, MoveFlags flags)
{
	return !!::MoveFileEx(sourceName->Data(), destName->Data(), (DWORD)flags);
}

bool NativeFileSystem::DeleteFile (String ^path)
{
#ifdef USE_ROOTRPC
	return !!::RootDeleteFile(path->Data());
#else
	return !!::DeleteFileW(path->Data());
#endif
}

bool NativeFileSystem::CreateDirectory (String ^fullpath)
{
#ifdef USE_ROOTRPC
	return !!::RootCreateDirectory(fullpath->Data(), NULL);
#else
	return !!::CreateDirectoryW(fullpath->Data(), nullptr);
#endif
}

bool NativeFileSystem::DeleteDirectory (String ^fullpath)
{
	return !!::RemoveDirectory(fullpath->Data());
}

#ifdef USE_NON_PUBLIC_APIS
#undef CreateSymbolicLink
bool NativeFileSystem::CreateSymbolicLink (String ^target, String ^linkname, bool directory)
#ifdef USE_ROOTRPC
#define CreateSymbolicLink RootCreateSymbolicLink
#else
#define CreateSymbolicLink CreateSymbolicLinkW
#endif
{
	PWCHAR t = new WCHAR[target->Length() + 1];
	PWCHAR n = new WCHAR[linkname->Length() + 1];
	wcscpy_s(t, target->Length() + 1, target->Data());
	wcscpy_s(n, linkname->Length() + 1, linkname->Data());
	bool ret = !!::CreateSymbolicLink(n, t, directory ? 1 : 0);
	delete[] t;
	delete[] n;
	return ret;
}

Array<String^>^ NativeFileSystem::GetDriveLetters ()
{
	PWSTR buf = NULL;
	// Get the required buffer length, then get the strings
	DWORD len = ::GetLogicalDriveStrings(0, NULL);
	if (!len) return nullptr;
	buf = new WCHAR[len + 1];
	buf[0] = L'\0';
	if (!::GetLogicalDriveStrings(len, buf))
	{
		delete[] buf;
		return nullptr;
	}
	// Ok, we now have a multi-string array; parse it
	vector<String^> vec;
	PWSTR cur = buf;
	DWORD curlen = 0x0;
	while ( (curlen = ::wcslen(cur)) )
	{
		vec.push_back(ref new String(cur));
		cur += (curlen + 1);
	}
	// Clean up and return
	delete[] buf;
	return ref new Array<String^>(vec.data(), vec.size());
}
#endif

uint32 NativeFileSystem::GetError ()
{
	return ::GetLastError();
}
