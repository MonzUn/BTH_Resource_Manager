#include "GLThreadPool.h"

#include <cassert>

GLThreadWorker::GLThreadWorker( GLThreadPool* pool, SDL_Window* window, SDL_GLContext context )
    : mPool( pool ), mWindow( window ), mContext( context )
{
}

void GLThreadWorker::operator()()
{
    SDL_GL_MakeCurrent( mWindow, mContext );

    while ( true )
    {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock( mPool->mMutex );

            while ( mPool->mTasks.empty() && !mPool->mStop )
            {
                mPool->mCondition.wait( lock );
            }

            if ( mPool->mStop )
                return;

            task = std::move( mPool->mTasks.front() );
            mPool->mTasks.pop();
        }

        task();
    }

    SDL_GL_MakeCurrent( mWindow, NULL );
    SDL_GL_DeleteContext( mContext );
}

void GLThreadPool::Create( size_t threadCount, SDL_Window* window )
{
    std::vector<SDL_GLContext> contexts( threadCount + 1 );
    for ( size_t i = 0; i < contexts.size(); ++i )
    {
        contexts[ i ] = SDL_GL_CreateContext( window );
        assert( contexts[ i ] != NULL );
    }

    mThreads.resize( threadCount );
    for ( size_t i = 0; i < mThreads.size(); ++i )
    {
        mThreads[ i ] = std::thread( GLThreadWorker( this, window, contexts[ i ] ) );
    }

    mStop = false;
}

void GLThreadPool::Destroy()
{
    {
        std::unique_lock<std::mutex> lock( mMutex );
        mStop = true;
    }
    mCondition.notify_all();

    for ( size_t i = 0; i < mThreads.size(); ++i )
    {
        mThreads[ i ].join();
    }
}