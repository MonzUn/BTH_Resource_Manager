#include "PacaWriter.h"
#include <vector>
#include <cassert>
#include "PlatformDefinitions.h"
#include "FileUtility.h"

void PacaWriter::WritePaca( const std::string& startingDirectory )
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
				if ( contentPaths[i].find( ".paca" ) == std::string::npos ) // Don't store .paca in .paca
					filePaths.push_back( directoryToSearch + contentPaths[i] );
			}
		}
		
	} while ( !subfolderPaths.empty() );

	unsigned int dataTotalByteSize = 0;

	// Find all fileLengths
	std::vector<unsigned int> fileLengths;
	for ( int i = 0; i < filePaths.size(); ++i )
	{
		unsigned int fileByteSize = FileUtility::GetFileContentSize( filePaths[i], std::ios::binary );
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

	unsigned int headerByteSize = filePaths.size() * sizeof( unsigned int ) * 2; // One hash and one length variable for each entry
	unsigned int totalByteSize	= sizeof( unsigned int ) + headerByteSize + dataTotalByteSize;

	// Create the output buffer
	char* buffer = static_cast<char*>( malloc( totalByteSize ) );
	char* walker = buffer;

	// Write the number of objects in the file
	unsigned int objectCount = filePaths.size();
	memcpy( walker, &objectCount, sizeof( sizeof( unsigned int ) ) );
	walker += sizeof( unsigned int );

	// Write all hashes
	std::hash<std::string> hashFunction;
	for ( int i = 0; i < filePaths.size(); ++i )
	{
		unsigned int hash = hashFunction( filePaths[i] );
		memcpy( walker, &hash, sizeof( unsigned int ) );
		walker += sizeof( unsigned int );
	}

	// Write all sizes
	for ( int i = 0; i < filePaths.size(); ++i )
	{
		memcpy( walker, &fileLengths[i], sizeof( unsigned int ) );
		walker += sizeof( unsigned int );
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
}