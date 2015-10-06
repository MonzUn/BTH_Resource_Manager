#pragma once

#include <vector>
#include <fstream>
#include <iostream> // TEMP

class Serializer {
public:
	std::fstream file;
	std::ifstream ifile;
	bool OpenFileForRead(const char* fileDir)
	{
		if (ifile.is_open())
			ifile.close();
		ifile.open(fileDir, std::ios::in | std::ios::binary);
		return ifile.is_open();
	}

	void CloseFile()
	{
		file.close();
		ifile.close();
	}
	char * ReadBytesFromFile()
	{
		// get size of file
		ifile.seekg(0, ifile.end);
		long size = (long)ifile.tellg();
		ifile.seekg(0);
		return ReadBytesFromFile(size);
	}
	char * ReadBytesFromFile(int bytes)
	{
		if (bytes < 0) return NULL;
		char *chars = new char[bytes];
		ifile.read((char*)&chars[0], bytes);
		std::cout << "Importing " << bytes << " bytes.\n"; // TEMP
		return chars;
	}
};