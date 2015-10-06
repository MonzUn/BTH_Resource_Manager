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
		ifile.open(fileDir, std::ios::in | std::ios::binary | std::ios::ate);
		return ifile.is_open();
	}

	void CloseFile()
	{
		file.close();
		ifile.close();
	}

	char * ReadBytesFromFile()
	{
		std::ifstream::pos_type size = ifile.tellg();
		return ReadBytesFromFile((int)size);
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