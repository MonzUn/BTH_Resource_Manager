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

			uint32_t fileSize = FileUtility::GetFileContentSize( m_InStream);

			// Read object count
			uint32_t objectCount;
			m_InStream.read( reinterpret_cast<char*>( &objectCount ), sizeof( uint32_t ) );

			// Read header data
			uint32_t headerSize = objectCount * sizeof( uint32_t ) * 2; // * 2 since it is both hash and size
			char* headerData = static_cast<char*>( malloc( headerSize ) );
			m_InStream.read( headerData, headerSize );

			uint32_t nextStartIndex = 0;
			for ( int i = 0; i < objectCount; ++i )
			{
				uint32_t pathHash, resourceByteSize;
				pathHash			= *reinterpret_cast<uint32_t*>( headerData + ( sizeof( uint32_t ) * i ) );
				resourceByteSize	= *reinterpret_cast<uint32_t*>( headerData + ( objectCount * sizeof( uint32_t ) ) + sizeof( uint32_t ) * i );
				m_HeaderInfo.emplace( pathHash, ResoureMetaData( sizeof( uint32_t ) +  headerSize + nextStartIndex, resourceByteSize ) );
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

uint32_t PacaReader::GetResourceSize( const std::string& resourcePath ) const
{
	uint32_t resourceSize = 0;
	uint32_t pathHash = HashString( resourcePath );
	if ( m_HeaderInfo.find( pathHash ) != m_HeaderInfo.end() )
	{
		resourceSize = m_HeaderInfo.at( pathHash ).ByteSize;
	}

	return resourceSize;
}

bool PacaReader::GetResource( const std::string& resourcePath, void* outBuffer, uint32_t outBufferSize )
{
	bool result = false;
	uint32_t pathHash = HashString( resourcePath );
	if ( m_HeaderInfo.find( pathHash ) != m_HeaderInfo.end() )
	{
		const ResoureMetaData& resourceMetaData = m_HeaderInfo.at( pathHash );
		if ( resourceMetaData.ByteSize <= outBufferSize )
		{
			m_InStream.seekg( resourceMetaData.StartIndex );
			m_InStream.read( static_cast<char*>( outBuffer ), resourceMetaData.ByteSize );
			result = true;
		}
	}

	return result;
}

uint32_t PacaReader::HashString( const std::string& toHash ) const
{
	std::hash<std::string> hashFunction;
	return hashFunction( toHash );
}