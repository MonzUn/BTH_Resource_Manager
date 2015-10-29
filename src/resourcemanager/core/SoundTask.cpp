#include "SoundTask.h"
#include <stdio.h>
#include <stb_image.h>
#include "fmod\fmod.hpp"



void* LoadSoundTask::operator()(unsigned char* buf, unsigned int bufSize, std::mutex* mutex, void* _fSystem)
{
	FMOD::System* system = static_cast<FMOD::System*>(_fSystem);
	FMOD::Sound* sound = 0;
	FMOD_CREATESOUNDEXINFO exinfo;
	memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	exinfo.length = bufSize;

	FMOD_RESULT	result = system->createSound((const char*)buf, FMOD_OPENMEMORY, &exinfo, &sound);

	delete buf;

	std::lock_guard<std::mutex> lock(*mutex);
	GLsync fenceId = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	while (glClientWaitSync(fenceId, GL_SYNC_FLUSH_COMMANDS_BIT, GLuint64(5000000000)) == GL_TIMEOUT_EXPIRED) {}
	glDeleteSync(fenceId);

	return (void*)sound;
}