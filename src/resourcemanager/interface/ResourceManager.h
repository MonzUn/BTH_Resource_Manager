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

class ResourceManager
{
private:
    GLThreadPool mThreadPool;

    std::mutex mGlLock;

	ZZIP_DIR* mDir;

	PacaReader mPacaReader;

	AssetPacketExtension mAssetPacketExtension;

	// maps
	std::map<const char*, ModelFileParser*> mModelFileParsers;
	//std::map<const char*, Buffer*> m_meshs;

public:
    RESOURCEMANAGER_API bool StartUp( SDL_Window* window, const std::string& assetFilePath );
    RESOURCEMANAGER_API void ShutDown();

    RESOURCEMANAGER_API std::future<GLuint> LoadTexture( const char* filepath );
    RESOURCEMANAGER_API std::future<void> DeleteTexture( GLuint texture );

	RESOURCEMANAGER_API ModelFileParser* LoadModel(const char* file);
	RESOURCEMANAGER_API void FreeModelData(ModelFileParser* parser);
};