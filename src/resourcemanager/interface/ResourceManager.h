#pragma once

#include "../core/ResourceManagerLibraryDefine.h"
#include "../core/GLThreadPool.h"
#include "../core/ObjParser.h"
#include "../core/MeshParser.h"

#include <GLEW/glew.h>
#include <vector>

#include <zzip/zzip.h>
#include "../../resourcemanager/Core/PacaReader.h"

#define _CACH_PARSED_DATA_

enum AssetPacketExtension
{
	PACA,
	ZIP
};

struct SoundData
{
	unsigned char* buffer = 0;
	unsigned int bufferSize = 0;
};

struct TextureResource
{
private:
	bool isLoaded = false;
	GLuint texture = 0;
public:
	std::future<GLuint> future;
	bool isReady()
	{
		if (isLoaded == true) return true;
		if (future._Is_ready())
		{
			isLoaded = true;
			texture = future.get();
			assert(texture != 0);
			return true;
		}
		return false;
	}
	GLuint get() { return texture; }
};

class ResourceManager
{
private:
	int memory;
	void addToMemCount(int bytes);

    GLThreadPool mThreadPool;

    std::mutex mGlLock;

	ZZIP_DIR* mDir;

	PacaReader mPacaReader;

	AssetPacketExtension mAssetPacketExtension;

	// maps
	std::map<const char*, ModelFileParser*> mModelFileParsers;
	std::map<const char*, TextureResource*> mTextureResource;
	std::map<const char*, SoundData*> mSoundResource;
	//std::map<const char*, Buffer*> m_meshs;

public:
    RESOURCEMANAGER_API bool StartUp( SDL_Window* window, size_t threadCount, const std::string& assetFilePath );
    RESOURCEMANAGER_API void ShutDown();

    RESOURCEMANAGER_API TextureResource* LoadTexture( const char* filepath );
    RESOURCEMANAGER_API std::future<void> DeleteTexture( GLuint texture );

	RESOURCEMANAGER_API ModelFileParser* LoadModel(const char* file);
	RESOURCEMANAGER_API void FreeModelData(ModelFileParser* parser);

	RESOURCEMANAGER_API std::future<void*> LoadSound(const char* filepath, void* _fSystem);
};