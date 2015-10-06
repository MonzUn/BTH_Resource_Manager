#include "Serializer.h"
#include <iostream>

int main()
{
	Serializer importer;
	if (importer.OpenFileForRead("totem.mesh")) 
	{
		int* n = new int(0);
		n = (int*)(importer.ReadBytesFromFile(sizeof(int)));
		float* pos = (float*)(importer.ReadBytesFromFile(*n));
		n = (int*)(importer.ReadBytesFromFile(sizeof(int)));
		float* norm = (float*)(importer.ReadBytesFromFile(*n));
		n = (int*)(importer.ReadBytesFromFile(sizeof(int)));
		float* tan = (float*)(importer.ReadBytesFromFile(*n));
		n = (int*)(importer.ReadBytesFromFile(sizeof(int)));
		float* bitan = (float*)(importer.ReadBytesFromFile(*n));
		n = (int*)(importer.ReadBytesFromFile(sizeof(int)));
		float* tex = (float*)(importer.ReadBytesFromFile(*n));
		n = (int*)(importer.ReadBytesFromFile(sizeof(int)));
		float* kex = (float*)(importer.ReadBytesFromFile(*n));

		delete n;
		delete pos;
		delete norm;
		delete tan;
		delete bitan;
		delete tex;
		delete kex;

		importer.CloseFile();
	}
	if (importer.OpenFileForRead("totem.mesh"))
	{
		char* data = importer.ReadBytesFromFile();
		delete data;
		importer.CloseFile();
	}
	return 0;
}