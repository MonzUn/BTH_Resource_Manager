#pragma once

#include "ResourceManagerLibraryDefine.h"

#include <GLEW/glew.h>
#include <mutex>

struct LoadSoundTask
{
	// We obviously don't want to load the texture directly from file later on,
	// but this will do for now
	RESOURCEMANAGER_API void* operator()(unsigned char* buf, unsigned int bufSize, std::mutex* mutex, void* _fSystem);
};