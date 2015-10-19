#include <algorithm>
#include <iostream>
#include <string>
#include "Serializer.h"
#include "PacaWriter.h"

void PrintHelp();
void ReadMesh();
void WritePaca();

int main()
{
	PrintHelp();

	bool quit = false;
	while ( !quit )
	{
		std::cout << "> ";
		std::string input;
		std::cin >> input;
		std::transform( input.begin(), input.end(), input.begin(), ::tolower );

		if ( input == "quit" || input == "exit" || input == "q" )
			quit = true;

		if ( input == "mesh" || input == "m" )
			ReadMesh();

		if ( input == "pacaWrite" || input == "pw" )
			WritePaca();
	}
	return 0;
}

void ReadMesh()
{
	Serializer importer;
	if ( importer.OpenFileForRead( "totem.mesh" ) )
	{
		int* n = new int( 0 );
		n = ( int* )( importer.ReadBytesFromFile( sizeof( int ) ) );
		float* pos = ( float* )( importer.ReadBytesFromFile( *n ) );
		n = ( int* )( importer.ReadBytesFromFile( sizeof( int ) ) );
		float* norm = ( float* )( importer.ReadBytesFromFile( *n ) );
		n = ( int* )( importer.ReadBytesFromFile( sizeof( int ) ) );
		float* tan = ( float* )( importer.ReadBytesFromFile( *n ) );
		n = ( int* )( importer.ReadBytesFromFile( sizeof( int ) ) );
		float* bitan = ( float* )( importer.ReadBytesFromFile( *n ) );
		n = ( int* )( importer.ReadBytesFromFile( sizeof( int ) ) );
		float* tex = ( float* )( importer.ReadBytesFromFile( *n ) );

		delete n;
		delete pos;
		delete norm;
		delete tan;
		delete bitan;
		delete tex;

		importer.CloseFile();
	}
	if ( importer.OpenFileForRead( "totem.mesh" ) )
	{
		float* data = ( float* )( importer.ReadBytesFromFile() );
		delete data;

		importer.CloseFile();
	}
}

void WritePaca()
{
	std::cout << "Input starting directory;";
	std::string startingDirectory;
	std::cin >> startingDirectory;

	PacaWriter writer;
	writer.WritePaca( startingDirectory.c_str() );
}

void PrintHelp()
{
	std::cout << "The following commands are supported\n\n";

	std::cout << "- Read .mesh (mesh)\n";
	std::cout << "- Write .paca file (pw)\n";

	std::cout << "\n";
}