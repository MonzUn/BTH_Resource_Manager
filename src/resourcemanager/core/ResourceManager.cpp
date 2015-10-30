#include "../interface/ResourceManager.h"
#include "../core/TextureTask.h"
#include "../core/SoundTask.h"
#include <zzip/zzip.h>

bool ResourceManager::StartUp( SDL_Window* window, size_t threadCount, const std::string& assetFilePath )
{
	memory = 0;
	std::string assetFileExtension = assetFilePath.substr( assetFilePath.find_last_of( '.' ) );
	if ( assetFileExtension == ".paca" )
		mAssetPacketExtension = AssetPacketExtension::PACA;
	else if ( assetFileExtension == ".zip" )
		mAssetPacketExtension = AssetPacketExtension::ZIP;
	else
		return false;

    mThreadPool.Create( threadCount, window );

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
	for (std::map<const char*, SoundData*>::iterator it = mSoundResource.begin(); it != mSoundResource.end(); it++)
	{
		delete it->second;
	}
	mModelFileParsers.clear();
	mSoundResource.clear();
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

TextureResource* ResourceManager::LoadTexture( const char* file)
{	
#ifdef _CACH_PARSED_DATA_
	for (std::map<const char*, TextureResource*>::iterator it = mTextureResource.begin(); it != mTextureResource.end(); it++)
	{
		if (it->first == file)
			return it->second;
	}
#endif

	unsigned int bufferSize;
	unsigned char* buffer;

	switch ( mAssetPacketExtension )
	{
		case PACA:
			bufferSize	= mPacaReader.GetResourceSize(file);
			buffer		= new unsigned char[bufferSize];
			if ( mPacaReader.GetResource(file, buffer, bufferSize ) )
			{
				TextureResource* textureResource = new TextureResource();
				textureResource->future = mThreadPool.AddTask<LoadTextureTask>(buffer, bufferSize, &mGlLock);
#ifdef _CACH_PARSED_DATA_
				mTextureResource.insert(std::pair<const char*, TextureResource*>(file, textureResource));
#endif
				return textureResource;
			}
			break;

		case ZIP:
			ZZIP_FILE* fp = zzip_file_open( mDir, file, 0 );
			zzip_seek( fp, 0, SEEK_END );
			bufferSize = zzip_tell( fp );
			zzip_rewind( fp );

			buffer = new unsigned char[bufferSize];
			bufferSize = zzip_file_read( fp, buffer, static_cast<int>( bufferSize ) );

			zzip_file_close( fp );

			TextureResource* textureResource = new TextureResource();
			textureResource->future = mThreadPool.AddTask<LoadTextureTask>(buffer, bufferSize, &mGlLock);
#ifdef _CACH_PARSED_DATA_
			mTextureResource.insert(std::pair<const char*, TextureResource*>(file, textureResource));
#endif
			return textureResource;
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
			ZZIP_FILE* fp = zzip_file_open( mDir, file, 0 );
			zzip_seek( fp, 0, SEEK_END );
			bufferSize = zzip_tell( fp );
			zzip_rewind( fp );

			buffer = new char[bufferSize];
			bufferSize = zzip_file_read( fp, buffer, bufferSize );

			zzip_file_close( fp );
		}
		break;
	}
	addToMemCount(bufferSize);

	ModelFileParser* mParser;
	std::string fileString;
	fileString = std::string(file);
	if (fileString.substr(fileString.find_last_of(".") + 1) == "mesh")
	{
		mParser = new MeshParser();
		addToMemCount(sizeof(mParser));
	}
	else if (fileString.substr(fileString.find_last_of(".") + 1) == "obj")
	{
		mParser = new ObjParser();
		addToMemCount(sizeof(mParser));
	}
	else
	{
		delete[] buffer;
		addToMemCount(-bufferSize);
		return NULL;
	}

	mParser->Load(buffer, bufferSize);
	addToMemCount(mParser->memory);
	delete[] buffer;
	addToMemCount(-bufferSize);

#ifdef _CACH_PARSED_DATA_
	mModelFileParsers.insert(std::pair<const char*, ModelFileParser*>(file, mParser));
#endif

	return mParser;
}

void ResourceManager::FreeModelData(ModelFileParser* parser)
{
	addToMemCount(-parser->memory);
	addToMemCount(-sizeof(parser));
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

std::future<void*> ResourceManager::LoadSound(const char* filepath, void* _fSystem)
{
#ifdef _CACH_PARSED_DATA_
	for (std::map<const char*, SoundData*>::iterator it = mSoundResource.begin(); it != mSoundResource.end(); it++)
	{
		if (it->first == filepath)
			return mThreadPool.AddTask<LoadSoundTask>(it->second->buffer, it->second->bufferSize, &mGlLock, _fSystem);
	}
#endif

	unsigned int bufferSize;
	unsigned char* buffer;

	switch (mAssetPacketExtension)
	{
	case PACA:
		bufferSize = mPacaReader.GetResourceSize(filepath);
		buffer = new unsigned char[bufferSize];
		if (mPacaReader.GetResource(filepath, buffer, bufferSize))
		{
#ifdef _CACH_PARSED_DATA_
			SoundData* sd = new SoundData();
			sd->buffer = buffer;
			sd->bufferSize = bufferSize;
			mSoundResource.insert(std::pair<const char*, SoundData*>(filepath, sd));
#endif
			return mThreadPool.AddTask<LoadSoundTask>(buffer, bufferSize, &mGlLock, _fSystem);
		}
		break;

	case ZIP:
		ZZIP_FILE* fp = zzip_file_open(mDir, filepath, 0);
		zzip_seek(fp, 0, SEEK_END);
		bufferSize = zzip_tell(fp);
		zzip_rewind(fp);

		buffer = new unsigned char[bufferSize];
		bufferSize = zzip_file_read(fp, buffer, static_cast<int>(bufferSize));

		zzip_file_close(fp);

#ifdef _CACH_PARSED_DATA_
		SoundData* sd = new SoundData();
		sd->buffer = buffer;
		sd->bufferSize = bufferSize;
		mSoundResource.insert(std::pair<const char*, SoundData*>(filepath, sd));
#endif
		return mThreadPool.AddTask<LoadSoundTask>(buffer, bufferSize, &mGlLock, _fSystem);
	}
}

void ResourceManager::addToMemCount(int bytes)
{
	memory += bytes;
	if (memory > 250000000)
		printf("WARNING! memory usage: %d bytes\n", memory);
}