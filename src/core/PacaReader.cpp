#include "PacaReader.h"
#include "FileUtility.h"

bool PacaReader::Open( const std::string & filePath )
{
	bool result = false;
	if ( !m_InStream.is_open() )
	{
		m_InStream.open( filePath , std::ios::binary );
		result = m_InStream.is_open();

		if ( result )
		{
			m_OpenFilePath = filePath;

			unsigned int fileSize = FileUtility::GetFileContentSize( m_InStream);

			// Read object count
			unsigned int objectCount;
			m_InStream.read( reinterpret_cast<char*>( &objectCount ), sizeof( unsigned int ) );

			// Read header data
			unsigned int headerSize = objectCount * sizeof( unsigned int ) * 2; // * 2 since it is both hash and size
			char* headerData = static_cast<char*>( malloc( headerSize ) );
			m_InStream.read( headerData, headerSize );

			unsigned int nextStartIndex = 0;
			for ( int i = 0; i < objectCount; ++i )
			{
				unsigned int pathHash, resourceByteSize;
				pathHash			= *reinterpret_cast<unsigned int*>( headerData + ( sizeof( unsigned int ) * i ) );
				resourceByteSize	= *reinterpret_cast<unsigned int*>( headerData + ( objectCount * sizeof( unsigned int ) ) + sizeof( unsigned int ) * i );
				m_HeaderInfo.emplace( pathHash, ResoureMetaData( nextStartIndex, resourceByteSize ) );
				nextStartIndex += resourceByteSize;
			}

			free( headerData );
		}
	}

	return result;
}

void PacaReader::Close()
{
	if ( m_InStream.is_open() )
	{
		m_InStream.close();
		m_OpenFilePath.clear();
		m_HeaderInfo.clear();
	}
}

bool PacaReader::IsOpen() const
{
	return m_InStream.is_open();
}

std::string PacaReader::GetOpenPath() const
{
	return m_OpenFilePath;
}