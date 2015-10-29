#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <limits.h>
#include <SDL/SDL.h>
#include <GLEW/glew.h>

#include "ResourceManager.h"

void TestTextureLoading( ResourceManager* resourceManager, const char* textureFilepath, int textureCount );

#undef main
int main( int argc, char* argv[] )
{
    SDL_Init( SDL_INIT_VIDEO );
    SDL_GL_SetAttribute( SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1 );
    SDL_Window* window = SDL_CreateWindow( "Test: Texture Loading", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1, 1, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN );
    SDL_GLContext context = SDL_GL_CreateContext( window );

    glewInit();

    std::cout << "Enter number of worker threads: ";
    std::string input;
    std::cin >> input;
    std::cout << std::endl;
    size_t threadCount = ( size_t )atoi( input.c_str() );

    std::cout << "Enter number of textures to load: ";
    std::cin >> input;
    std::cout << std::endl;
    int textureCount = atoi( input.c_str() );

    ResourceManager resourceManager;
    resourceManager.StartUp( window, threadCount, "../assets/assets.paca" );
    SDL_GL_MakeCurrent( window, context );

    bool exit = false;
    while ( exit == false )
    {
        std::cout << "> ";
        std::cin >> input;
        std::cout << std::endl;
        std::transform( input.begin(), input.end(), input.begin(), ::tolower );

        if ( input == "exit" || input == "quit" )
            exit = true;

        else if ( input == "lowres" )
            TestTextureLoading( &resourceManager, "lowres.png", textureCount );
        else if ( input == "mediumres" )
            TestTextureLoading( &resourceManager, "mediumres.png", textureCount );
        else if ( input == "highres" )
            TestTextureLoading( &resourceManager, "highres.png", textureCount );
    }
	
    resourceManager.ShutDown();

    SDL_GL_DeleteContext( context );
    SDL_DestroyWindow( window );
    SDL_Quit();
	
	return 0;
}

void TestTextureLoading( ResourceManager* resourceManager, const char* filepath, int count )
{
    // Start loading textures
    struct TextureLoadingData
    {
        std::chrono::steady_clock::time_point start;
        std::chrono::steady_clock::time_point finish;
		TextureResource* textureResource;
        std::future<void> deleteFuture;
        GLuint texture;
        bool isActive;
    };
    std::vector<TextureLoadingData> textureFutures( count );
    for ( int i = 0; i < count; ++i )
    {
        textureFutures[ i ].start = std::chrono::high_resolution_clock::now();
        textureFutures[ i ].textureResource = resourceManager->LoadTexture(filepath);
        textureFutures[ i ].isActive = true;
    }

    // Finish loading textures
    int texturesLeft = count;
    while ( texturesLeft > 0 )
    {
        for ( int i = 0; i < count; ++i )
        {
            if ( textureFutures[ i ].isActive && textureFutures[ i ].textureResource->future._Is_ready() )
            {
                textureFutures[ i ].finish = std::chrono::high_resolution_clock::now();
                textureFutures[ i ].isActive = false;
                --texturesLeft;
            }
        }
    }

    // Calculate average, best and average values
    long long sum = 0, best = LLONG_MAX, worst = 0;
    for ( int i = 0; i < count; ++i )
    {
        textureFutures[ i ].texture = textureFutures[ i ].textureResource->future.get();
        long long duration = std::chrono::duration_cast<std::chrono::milliseconds>( textureFutures[ i ].finish - textureFutures[ i ].start ).count();
        
        sum += duration;
        if ( duration < best )
            best = duration;
        if ( duration > worst )
            worst = duration;
    }
    long long average = sum / count;

    // Print values
    std::cout << "---------------- Loading ----------------" << std::endl;
    std::cout << "Average: " << average << " ms" << std::endl;
    std::cout << "Best: " << best << " ms" << std::endl;
    std::cout << "Worst: " << worst << " ms" << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << std::endl;

    // Start deleting textures
    for ( int i = 0; i < count; ++i )
    {
        textureFutures[ i ].start = std::chrono::high_resolution_clock::now();
        textureFutures[ i ].deleteFuture = resourceManager->DeleteTexture( textureFutures[ i ].texture );
        textureFutures[ i ].isActive = true;
    }

    // Finish deleting textures
    texturesLeft = count;
    while ( texturesLeft > 0 )
    {
        for ( int i = 0; i < count; ++i )
        {
            if ( textureFutures[ i ].isActive && textureFutures[ i ].deleteFuture._Is_ready() )
            {
                textureFutures[ i ].finish = std::chrono::high_resolution_clock::now();
                textureFutures[ i ].isActive = false;
                --texturesLeft;
            }
        }
    }

    // Calculate average, best and average values
    sum = 0, best = LLONG_MAX, worst = 0;
    for ( int i = 0; i < count; ++i )
    {
        textureFutures[ i ].deleteFuture.get();
        long long duration = std::chrono::duration_cast<std::chrono::milliseconds>( textureFutures[ i ].finish - textureFutures[ i ].start ).count();

        sum += duration;
        if ( duration < best )
            best = duration;
        if ( duration > worst )
            worst = duration;
    }
    average = sum / count;

    // Print values
    std::cout << "---------------- Deleting ---------------" << std::endl;
    std::cout << "Average: " << average << " ms" << std::endl;
    std::cout << "Best: " << best << " ms" << std::endl;
    std::cout << "Worst: " << worst << " ms" << std::endl;
    std::cout << "-----------------------------------------" << std::endl;
    std::cout << std::endl;
}