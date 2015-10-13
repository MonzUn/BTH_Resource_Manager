#include "../interface/ResourceManager.h"
#include "../core/TextureTask.h"

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