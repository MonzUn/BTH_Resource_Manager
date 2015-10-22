#include "../interface/ResourceManager.h"
#include "../core/TextureTask.h"
#include <zzip/zzip.h>

void ResourceManager::StartUp( SDL_Window* window )
{
    mThreadPool.Create( 4, window );
}

void ResourceManager::ShutDown()
{
    mThreadPool.Destroy();
}

std::future<GLuint> ResourceManager::LoadTexture( const char* filepath )
{
    return mThreadPool.AddTask<LoadTextureTask>( filepath, &mGlLock );
}

std::future<void> ResourceManager::DeleteTexture( GLuint texture )
{
    return mThreadPool.AddTask<DeleteTextureTask>( texture, &mGlLock );
}

ModelFileParser* ResourceManager::LoadModel(const char* file)
{
	ZZIP_DIR* dir = zzip_dir_open("../assets/dip.zip", 0);
	//if (dir) {
	zzip_ssize_t len;

	int width, height, elementCount;

	ZZIP_FILE* fp = zzip_file_open(dir, file, 0);
	char *buf;
	zzip_seek(fp, 0, SEEK_END);
	len = zzip_tell(fp);
	zzip_rewind(fp);

	buf = new char[len];
	len = zzip_file_read(fp, buf, len);
	
	zzip_file_close(fp);
	zzip_dir_close(dir);

	ModelFileParser* mParser = new ObjParser();
	mParser->Load(buf, len);
	delete[] buf;

	return mParser;
}

void ResourceManager::FreeModelData(ModelFileParser* parser)
{
	delete parser;
}