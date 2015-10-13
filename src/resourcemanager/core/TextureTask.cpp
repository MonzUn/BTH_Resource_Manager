#include "TextureTask.h"

#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

GLuint LoadTextureTask::operator()( const char* filepath, std::mutex* mutex )
{
    std::lock_guard<std::mutex> lock( *mutex );

    FILE* file;
    if ( fopen_s( &file, filepath, "rb" ) != 0 )
        return 0;

    int width, height, elementCount;
    unsigned char* imageData = stbi_load_from_file( file, &width, &height, &elementCount, 0 );

    fclose( file );

    GLuint texture;
    glGenTextures( 1, &texture );
    glBindTexture( GL_TEXTURE_2D, texture );

    GLint format;
    switch ( elementCount )
    {
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
    }

    glTexImage2D( GL_TEXTURE_2D, 0, format, width, height,
                  0, format, GL_UNSIGNED_BYTE, ( GLvoid* )imageData );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glGenerateMipmap( GL_TEXTURE_2D );

    stbi_image_free( imageData );

    GLsync fenceId = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );
    while ( glClientWaitSync( fenceId, GL_SYNC_FLUSH_COMMANDS_BIT, GLuint64( 5000000000 ) ) == GL_TIMEOUT_EXPIRED ) {}
    glDeleteSync( fenceId );

    return texture;
}

void DeleteTextureTask::operator()( GLuint texture, std::mutex* mutex )
{
    glDeleteTextures( 1, &texture );

    std::lock_guard<std::mutex> lock( *mutex );

    GLsync fenceId = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );
    while ( glClientWaitSync( fenceId, GL_SYNC_FLUSH_COMMANDS_BIT, GLuint64( 5000000000 ) ) == GL_TIMEOUT_EXPIRED ) {}
    glDeleteSync( fenceId );
}