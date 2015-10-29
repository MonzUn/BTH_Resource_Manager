#pragma once
#include <fstream>
#include <string>
#include <map>

struct ResoureMetaData
{
	ResoureMetaData( uint32_t startIndex, uint32_t byteSize ) : StartIndex( startIndex ), ByteSize( byteSize ) {}

	uint32_t StartIndex;
	uint32_t ByteSize;
};

class PacaReader
{
public:
	bool			Open( const std::string& filePath );
	void			Close();
	bool			IsOpen() const;
	std::string		GetOpenPath() const;

	uint32_t	GetResourceSize( const std::string& resourcePath ) const;
	bool			GetResource( const std::string& resourcePath, void* outBuffer, uint32_t outBufferSize );

private:
	uint32_t HashString( const std::string& toHash ) const;

	std::string									m_OpenFilePath;
	std::ifstream								m_InStream;
	std::map<uint32_t, ResoureMetaData>			m_HeaderInfo;
};