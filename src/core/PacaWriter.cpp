#include "PacaWriter.h"
#include <vector>
#include <cassert>
#include <iostream>
#include <stdint.h>
#include "../resourcemanager/core/PlatformDefinitions.h"
#include "../resourcemanager/core/FileUtility.h"

uint32_t PacaWriter::WritePaca( const std::string& startingDirectory )
{
	std::vector<std::string> filePaths;
	std::vector<std::string> subfolderPaths;

	// Recursively look through all subfolders for resources
	subfolderPaths.push_back( startingDirectory );
	do
	{
		std::string directoryToSearch = subfolderPaths.back() + "/";
		subfolderPaths.pop_back();

		std::vector<std::string> contentPaths;
		FileUtility::GetListOfContentInDirectory( directoryToSearch, contentPaths );
		for ( int i = 0; i < contentPaths.size(); ++i )
		{
			if ( contentPaths[i].find( '.' ) == std::string::npos ) // It's a subfolder
			{
				subfolderPaths.push_back( directoryToSearch + contentPaths[i] );
			}
			else // It's a resource
			{
				if ( contentPaths[i].find( ".paca" ) == std::string::npos && contentPaths[i].find( ".zip" ) == std::string::npos ) // Don't store .paca and .zip in .paca
					filePaths.push_back( directoryToSearch + contentPaths[i] );
			}
		}
		
	} while ( !subfolderPaths.empty() );

	uint32_t dataTotalByteSize = 0;

	// Find all fileLengths
	std::vector<uint32_t> fileLengths;
	for ( int i = 0; i < filePaths.size(); ++i )
	{
		uint32_t fileByteSize = FileUtility::GetFileContentSize( filePaths[i], std::ios::binary );
		fileLengths.push_back( fileByteSize );
		dataTotalByteSize += fileByteSize;
	}

	// Load all file content
	std::vector<char*> fileContents;
	for ( int i = 0; i < filePaths.size(); ++i )
	{
		char* fileContent = static_cast<char*>( malloc( fileLengths[i] ) );
		FileUtility::GetFileContent( filePaths[i], fileContent, std::ios::binary );
		fileContents.push_back( fileContent );
	}

	uint32_t headerByteSize = filePaths.size() * sizeof( uint32_t ) * 2; // One hash and one length variable for each entry
	uint32_t totalByteSize	= sizeof( uint32_t ) + headerByteSize + dataTotalByteSize;

	// Create the output buffer
	char* buffer = static_cast<char*>( malloc( totalByteSize ) );
	char* walker = buffer;

	// Write the number of objects in the file
	uint32_t objectCount = filePaths.size();
	memcpy( walker, &objectCount, sizeof( sizeof( uint32_t ) ) );
	walker += sizeof( uint32_t );

	// Write all hashes
	std::hash<std::string> hashFunction;
	for ( int i = 0; i < filePaths.size(); ++i )
	{
		uint32_t hash = hashFunction( filePaths[i] );
		memcpy( walker, &hash, sizeof( uint32_t ) );
		walker += sizeof( uint32_t );

		std::cout << "Writing " << filePaths[i] << " (Hash = " << hash << ")\n";
	}

	// Write all sizes
	for ( int i = 0; i < filePaths.size(); ++i )
	{
		memcpy( walker, &fileLengths[i], sizeof( uint32_t ) );
		walker += sizeof( uint32_t );
	}

	// Write all the data
	for ( int i = 0; i < fileContents.size(); ++i )
	{
		memcpy( walker, fileContents[i], fileLengths[i] );
		walker += fileLengths[i];
	}

	std::ofstream outStream; 
	outStream.open( startingDirectory + "/assets.paca", std::ios::binary );
	if ( outStream.is_open() )
	{
		outStream.write( buffer, totalByteSize );
	}

	assert( walker - totalByteSize == buffer );

	free( buffer );
	for ( int i = 0; i < fileContents.size(); ++i )
		free( fileContents[i] );

	return objectCount;
}