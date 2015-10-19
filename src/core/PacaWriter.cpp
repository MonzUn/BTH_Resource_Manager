#include "PacaWriter.h"
#include <vector>
#include <iostream> // debug
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
}