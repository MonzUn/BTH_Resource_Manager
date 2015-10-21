#pragma once
#include <fstream>
#include <string>
#include <map>

struct ResoureMetaData
{
	ResoureMetaData( unsigned int startIndex, unsigned int byteSize ) : StartIndex( startIndex ), ByteSize( byteSize ) {}

	unsigned int StartIndex;
	unsigned int ByteSize;
};

class PacaReader
{
public:
	bool		Open( const std::string& filePath );
	void		Close();
	bool		IsOpen() const;
	std::string GetOpenPath() const;

private:
	std::string									m_OpenFilePath;
	std::ifstream								m_InStream;
	std::map<unsigned int, ResoureMetaData>		m_HeaderInfo;
};