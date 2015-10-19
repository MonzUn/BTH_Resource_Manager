/**************************************************
Zlib Copyright 2015 Daniel "MonzUn" Bengtsson
***************************************************/
#pragma once
#include <vector>
#include <fstream>

namespace FileUtility
{	
	char*	GetFileContent( const std::string& filePath, std::ios::open_mode openMode = std::ios::in );
	void	GetFileContent( const std::string& filePath, char*& out, std::ios::open_mode openMode = std::ios::in );
	size_t	GetFileContentSize( const std::string& filePath, std::ios::openmode openMode = std::ios::in );
	size_t	GetFileContentSize( std::ifstream& inStream );
	void 	GetListOfContentInDirectory( const std::string& directoryPath, std::vector<std::string>& outListWithEntries );
}