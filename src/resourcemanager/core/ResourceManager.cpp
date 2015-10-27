#include "../interface/ResourceManager.h"
#include "../core/TextureTask.h"
#include <zzip/zzip.h>

bool ResourceManager::StartUp( SDL_Window* window, const std::string& assetFilePath )
{
	std::string assetFileExtension = assetFilePath.substr( assetFilePath.find_last_of( '.' ) );
	if ( assetFileExtension == ".paca" )
		mAssetPacketExtension = AssetPacketExtension::PACA;
	else if ( assetFileExtension == ".zip" )
		mAssetPacketExtension = AssetPacketExtension::ZIP;
	else
		return false;

    mThreadPool.Create( 8, window );

	switch ( mAssetPacketExtension )
	{
		case PACA:
			mPacaReader.Open( assetFilePath );
		break;

		case ZIP:
			mDir = zzip_dir_open( assetFilePath.c_str(), 0 );
		break;
	}
	
	return true;
}

void ResourceManager::ShutDown()
{
#ifdef _CACH_PARSED_DATA_
	for (std::map<const char*, ModelFileParser*>::iterator it = mModelFileParsers.begin(); it != mModelFileParsers.end(); it++)
	{
		delete it->second;
	}
#endif

	switch ( mAssetPacketExtension )
	{
		case PACA:
			mPacaReader.Close();
		break;

		case ZIP:
			zzip_dir_close( mDir );
		break;
	}

    mThreadPool.Destroy();
}

std::future<GLuint> ResourceManager::LoadTexture( const char* filepath )
{	
	unsigned int bufferSize;
	unsigned char* buffer;
	switch ( mAssetPacketExtension )
	{
		case PACA:
			bufferSize	= mPacaReader.GetResourceSize( filepath );
			buffer		= new unsigned char[bufferSize];
			if ( mPacaReader.GetResource( filepath, buffer, bufferSize ) )
			{
				return mThreadPool.AddTask<LoadTextureTask>( buffer, bufferSize, &mGlLock );
			}
			break;

		case ZIP:
			ZZIP_FILE* fp = zzip_file_open( mDir, filepath + 10, 0 );
			zzip_seek( fp, 0, SEEK_END );
			bufferSize = zzip_tell( fp );
			zzip_rewind( fp );

			buffer = new unsigned char[bufferSize];
			bufferSize = zzip_file_read( fp, buffer, static_cast<int>( bufferSize ) );

			zzip_file_close( fp );

			return mThreadPool.AddTask<LoadTextureTask>( buffer, bufferSize, &mGlLock );
	}
}

std::future<void> ResourceManager::DeleteTexture( GLuint texture )
{
    return mThreadPool.AddTask<DeleteTextureTask>( texture, &mGlLock );
}

ModelFileParser* ResourceManager::LoadModel(const char* file)
{
#ifdef _CACH_PARSED_DATA_
	for (std::map<const char*, ModelFileParser*>::iterator it = mModelFileParsers.begin(); it != mModelFileParsers.end(); it++)
	{
		if (it->first == file)
			return it->second;
	}
#endif

	unsigned int bufferSize;
	char* buffer;

	switch ( mAssetPacketExtension )
	{
		case PACA:
		{
			bufferSize	= mPacaReader.GetResourceSize( file );
			buffer		= new char[bufferSize];
			if ( !mPacaReader.GetResource( file, buffer, bufferSize ) )
				return nullptr;
			break;
		}
		break;

		case ZIP:
		{
			ZZIP_FILE* fp = zzip_file_open( mDir, file + 10, 0 );
			zzip_seek( fp, 0, SEEK_END );
			bufferSize = zzip_tell( fp );
			zzip_rewind( fp );

			buffer = new char[bufferSize];
			bufferSize = zzip_file_read( fp, buffer, bufferSize );

			zzip_file_close( fp );
		}
		break;
	}

	ModelFileParser* mParser;
	std::string fileString;
	fileString = std::string(file);
	if (fileString.substr(fileString.find_last_of(".") + 1) == "mesh")
	{
		mParser = new MeshParser();
	}
	else if (fileString.substr(fileString.find_last_of(".") + 1) == "obj")
	{
		mParser = new ObjParser();
	}
	else
	{
		delete[] buffer;
		return NULL;
	}

	mParser->Load(buffer, bufferSize);
	delete[] buffer;

#ifdef _CACH_PARSED_DATA_
	mModelFileParsers.insert(std::pair<const char*, ModelFileParser*>(file, mParser));
#endif
	return mParser;
}

void ResourceManager::FreeModelData(ModelFileParser* parser)
{
#ifdef _CACH_PARSED_DATA_
	for (std::map<const char*, ModelFileParser*>::iterator it = mModelFileParsers.begin(); it != mModelFileParsers.end(); it++)
	{
		if (it->second == parser)
		{
			delete parser;
			mModelFileParsers.erase(it);
			return;
		}
	}
#else
	delete parser;
#endif
}