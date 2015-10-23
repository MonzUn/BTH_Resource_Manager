/**************************************************
Zlib Copyright 2015 Daniel "MonzUn" Bengtsson
***************************************************/

#include "PlatformDefinitions.h"
#if PLATFORM == PLATFORM_WINDOWS
#define WINDOWS_MEAN_AND_LEAN
#define WC_EXTREALEAN
#include <Windows.h>
#endif
#include "FileUtility.h"
#include <iostream>
#if PLATFORM == PLATFORM_WINDOWS
#include <direct.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

char* FileUtility::GetFileContent( const std::string& filePath, std::ios::open_mode openMode )
{
	char* toReturn = nullptr;

	std::ifstream inStream;
	inStream.open( filePath.c_str(), openMode );
	if ( inStream.is_open() )
	{
		// Check the file to see how big a buffer we are going to need
		size_t bufferSize = GetFileContentSize( inStream );

		// Allocate the buffer and fill it with the data from the file
		if ( bufferSize > 0 )
		{
			toReturn = static_cast< char* >( malloc( bufferSize ) ); // TODODB: Take this as out parameter instead
			inStream.read( toReturn, bufferSize );
		} else
			std::cout << "Attempted to get content of empty file(Path = " << filePath.c_str() << " )";
		inStream.close(); // We now have all the data in memory so the file can be closed
	} else
		std::cout << "GetFileContent was unable to open file (Path = " << filePath.c_str() << " )";

	return toReturn;
}

void FileUtility::GetFileContent( const std::string& filePath, char*& out, std::ios::open_mode openMode )
{
	out = GetFileContent( filePath, openMode );
}

size_t FileUtility::GetFileContentSize( const std::string& filePath, std::ios::openmode openMode )
{
	size_t size = 0;

	std::ifstream inStream;
	inStream.open( filePath.c_str(), openMode );

	if ( inStream.is_open() )
	{
		inStream.seekg( 0, std::ios::end );
		size = inStream.tellg();
		inStream.close();
	} else
		std::cout << "GetFileSize was unable to open file (Path = " << filePath.c_str() << " )";

	return size;
}

size_t FileUtility::GetFileContentSize( std::ifstream& inStream )
{
	size_t size = 0;

	if ( inStream.is_open() )
	{
		size_t startPos = inStream.tellg();
		inStream.seekg( 0, std::ios::end );
		size = inStream.tellg();
		inStream.clear();
		inStream.seekg( startPos );
	} else
		std::cout << "GetFileContentSize received an unopened ifstream";

	return size;
}

void FileUtility::GetListOfContentInDirectory( const std::string& directoryPath, std::vector<std::string>& outListWithEntries )
{
#if PLATFORM == PLATFORM_LINUX
	DIR *directory = opendir( directoryPath.c_str() );
	if ( directory )
	{
		struct dirent *directoryEntry;
		while ( ( directoryEntry = readdir( directory ) ) != nullptr )
		{
			std::string inFolder( directoryEntry->d_name );
			if ( inFolder != "." && inFolder != ".." ) // Ignore irrelevant folders
			{
				outListWithEntries.push_back( inFolder.c_str() );
			}
		}
	} else
	{
		std::cout << "Failed to open directory: " << std::string( directoryPath.c_str() );
	}
#endif
#if PLATFORM == PLATFORM_WINDOWS
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	hFind = FindFirstFile( ( directoryPath + "*" ).c_str(), &findFileData );
	if ( hFind == INVALID_HANDLE_VALUE )
	{
		std::cout << "Failed to open directory: " << directoryPath.c_str();
		return;
	}
	else
	{
		do
		{
			std::string inFolder( findFileData.cFileName );
			if ( inFolder != "." && inFolder != ".." ) // Ignore irrelevant folders
				outListWithEntries.push_back( inFolder.c_str() );
		} while ( FindNextFile( hFind, &findFileData ) != 0 );

		FindClose( hFind );
	}
#endif
}