#include "TextureTask.h"

#include <zzip/zzip.h>

#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLuint LoadTextureTask::operator()(const char* filepath, std::mutex* mutex)
{
	std::lock_guard<std::mutex> lock(*mutex);

	//FILE* file;
	//if ( fopen_s( &file, filepath, "rb" ) != 0 )
	//    return 0;
	ZZIP_DIR* dir = zzip_dir_open("../assets/dip.zip", 0);
	//if (dir) {
	zzip_ssize_t len;

	int width, height, elementCount;

	ZZIP_FILE* fp = zzip_file_open(dir, filepath + 10, 0);
	unsigned char *buf;
	zzip_seek(fp, 0, SEEK_END);
	len = zzip_tell(fp);
	zzip_rewind(fp);

	buf = new unsigned char[len];
	len = zzip_file_read(fp, buf, len);

	zzip_file_close(fp);
	zzip_dir_close(dir);
	//unsigned char* imageData = stbi_load_from_file( file, &width, &height, &elementCount, 0 );
	unsigned char* imageData = stbi_load_from_memory(buf, len, &width, &height, &elementCount, 0);
	delete[] buf;
	//fclose( file );
	//}
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	GLint format;
	switch (elementCount)
	{
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, imageData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(imageData);

	GLsync fenceId = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	while (glClientWaitSync(fenceId, GL_SYNC_FLUSH_COMMANDS_BIT, GLuint64(5000000000)) == GL_TIMEOUT_EXPIRED) {}
	glDeleteSync(fenceId);

	return texture;
}

void DeleteTextureTask::operator()(GLuint texture, std::mutex* mutex)
{
	glDeleteTextures(1, &texture);

	std::lock_guard<std::mutex> lock(*mutex);

	GLsync fenceId = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	while (glClientWaitSync(fenceId, GL_SYNC_FLUSH_COMMANDS_BIT, GLuint64(5000000000)) == GL_TIMEOUT_EXPIRED) {}
	glDeleteSync(fenceId);
}