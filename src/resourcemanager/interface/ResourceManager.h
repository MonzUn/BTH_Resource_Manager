#pragma once

#include "../core/ResourceManagerLibraryDefine.h"
#include "../core/GLThreadPool.h"

#include <GLEW/glew.h>
#include <vector>

#include <zzip/zzip.h>

class ResourceManager
{
private:
    GLThreadPool mThreadPool;

    std::mutex mGlLock;

	ZZIP_DIR* mDir;
public:
    RESOURCEMANAGER_API void StartUp( SDL_Window* window );
    RESOURCEMANAGER_API void ShutDown();

    RESOURCEMANAGER_API std::future<GLuint> LoadTexture( const char* filepath );
    RESOURCEMANAGER_API std::future<void> DeleteTexture( GLuint texture );
};