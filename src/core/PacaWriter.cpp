#include "PacaWriter.h"
#include <vector>
#include <cassert>
#include "PlatformDefinitions.h"
#include "FileUtility.h"

void PacaWriter::WritePaca( const std::string& startingDirectory )
{
	std::vector<std::string> filePaths;
	std::vector<std::string> subfolderPaths;

	subfolderPaths.push_back( startingDirectory );
	do
	{
		std::string directoryToSearch = subfolderPaths.back() + "/";
		subfolderPaths.pop_back();

		std::vector<std::string> contentPaths;
		FileUtility::GetListOfContentInDirectory( directoryToSearch, contentPaths );
		for ( int i = 0; i < contentPaths.size(); ++i )
			contentPaths[i].find( '.' ) == std::string::npos ? subfolderPaths.push_back( directoryToSearch + contentPaths[i] ) : filePaths.push_back( directoryToSearch + contentPaths[i] ); // Sort the content into subfolders and files

		
	} while ( !subfolderPaths.empty() );

	unsigned int dataTotalByteSize = 0;

	std::vector<unsigned int> fileLengths;
	for ( int i = 0; i < filePaths.size(); ++i )
	{
		unsigned int fileByteSize = FileUtility::GetFileContentSize( filePaths[i], std::ios::binary );
		fileLengths.push_back( fileByteSize );
		dataTotalByteSize += dataTotalByteSize;
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

	std::ofstream outStream; 
	outStream.open( startingDirectory + "/assets.paca", std::ios::binary );
	if ( outStream.is_open() )
	{
		outStream.write( buffer, totalByteSize );
	}

	assert( walker - totalByteSize == buffer );

	free( buffer );
}