#pragma once

#include "ResourceManagerLibraryDefine.h"

#include <GLEW/glew.h>
#include <mutex>

struct LoadTextureTask
{
    // We obviously don't want to load the texture directly from file later on,
    // but this will do for now
    RESOURCEMANAGER_API GLuint operator()( const char* filepath, std::mutex* mutex );
};

struct DeleteTextureTask
{
    RESOURCEMANAGER_API void operator()( GLuint texture, std::mutex* mutex );
};