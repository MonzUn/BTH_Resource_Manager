#include "PacaWriter.h"
#include <vector>
#include <iostream> // debug
#include "PlatformDefinitions.h"
#include "FileUtility.h"

void PacaWriter::WritePaca( const char* startingDirectory )
{
	std::vector<std::string> filePaths;
	FileUtility::GetListOfContentInDirectory( std::string( startingDirectory ), filePaths );
}