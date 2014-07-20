/*
 * FileSystem\FileSystem.h
 * Author: GoodDayToDie on XDA-Developers forum
 * License: Microsoft Public License (MS-PL)
 * Version: 0.4.0
 *
 * This file defines the WinRT-visible NativeFileSystem class, which wraps Win32 file APIs.
 * This class and all of its functions are thread-safe except against mid-API changs the file system itself.
 */

#pragma once

#define USE_NON_PUBLIC_APIS	// Comment this out if you want to be able to compile without needing Kernelbase.lib

using namespace Platform;
namespace wfm = Windows::Foundation::Metadata;

namespace FileSystem
{
	[Metadata::FlagsAttribute()]
	public enum class FileAttributes : uint32
	{
		None = 0,
		ReadOnly = FILE_ATTRIBUTE_READONLY,
		Hidden = FILE_ATTRIBUTE_HIDDEN,
		System = FILE_ATTRIBUTE_SYSTEM,
		Directory = FILE_ATTRIBUTE_DIRECTORY,
		Archive = FILE_ATTRIBUTE_ARCHIVE,
		Device = FILE_ATTRIBUTE_DEVICE,
		Normal = FILE_ATTRIBUTE_NORMAL,
		Temporary = FILE_ATTRIBUTE_TEMPORARY,
		Sparse = FILE_ATTRIBUTE_SPARSE_FILE,
		ReparsePoint = FILE_ATTRIBUTE_REPARSE_POINT,
		Compressed = FILE_ATTRIBUTE_COMPRESSED,
		Offline = FILE_ATTRIBUTE_OFFLINE,
		NotIndexed = FILE_ATTRIBUTE_NOT_CONTENT_INDEXED,
		Encrypted = FILE_ATTRIBUTE_ENCRYPTED,
		IntegrityStream = FILE_ATTRIBUTE_INTEGRITY_STREAM,
		Virtual = FILE_ATTRIBUTE_VIRTUAL,
		NoScrub = FILE_ATTRIBUTE_NO_SCRUB_DATA
	};

	[Metadata::FlagsAttribute()]
	public enum class MoveFlags : uint32
	{
		None = 0,
		ReplaceExisting = MOVEFILE_REPLACE_EXISTING,
		AcrossVolumes = MOVEFILE_COPY_ALLOWED,
		DelayUntilReboot = MOVEFILE_DELAY_UNTIL_REBOOT,
		WriteThroughBeforeReturn = MOVEFILE_WRITE_THROUGH,
		FailIfDestinationLosesLinks = MOVEFILE_FAIL_IF_NOT_TRACKABLE
	};

	public value struct FileInfo
	{
		String ^Name;
		int64 Size;
		FileAttributes Attributes;
	};

//	[wfm::StaticAttribute(nullptr, 0x40)][wfm::VersionAttribute(0x40)]
	public ref class NativeFileSystem sealed
	{
//		[wfm::Deprecated("All functions are now static; there is no use constructing this class",
//			wfm::DeprecationType::Deprecate, 0x0)]
		NativeFileSystem ();
	public:
		static String^ GetFileNames (String ^pattern);
		static String^ GetFileNames (String ^pattern, bool includeFiles, bool includeDirs);
		static Array<FileInfo>^ GetFiles (String ^pattern);
		static Array<FileInfo>^ GetFiles (String ^pattern, bool includeDirs);
		static Array<uint8>^ ReadFile (String ^path);
		static Array<uint8>^ ReadFile (String ^path, int64 offset, uint32 length);
		static bool WriteFile (String ^path, const Array<uint8> ^data);
		static bool WriteFile (String ^path, int64 offset, const Array<uint8> ^data);
		static bool CopyFile (String ^sourceName, String ^destName);
		static bool MoveFile (String ^sourceName, String ^destName);
		static bool MoveFile (String ^sourceName, String ^destName, MoveFlags flags);
#undef DeleteFile
		static bool DeleteFile (String ^path);
#undef CreateDirectory
		static bool CreateDirectory (String ^fullpath);
		static bool DeleteDirectory (String ^fullpath);
#ifdef USE_NON_PUBLIC_APIS
		static bool CreateSymbolicLink (String ^target, String ^linkname, bool directory);
		static Array<String^>^ GetDriveLetters ();
#endif
		static uint32 GetError ();
	};
}