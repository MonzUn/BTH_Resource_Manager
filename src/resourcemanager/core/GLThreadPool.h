#pragma once

#include "ResourceManagerLibraryDefine.h"

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <SDL/SDL.h>
#include <future>

class GLThreadPool;

class GLThreadWorker
{
private:
    GLThreadPool* mPool;
    SDL_Window* mWindow;
    SDL_GLContext mContext;

public:
    RESOURCEMANAGER_API GLThreadWorker( GLThreadPool* pool, SDL_Window* window, SDL_GLContext context );
    RESOURCEMANAGER_API void operator()();
};

class GLThreadPool
{
private:
    std::vector<std::thread> mThreads;
    std::mutex mMutex;
    std::condition_variable mCondition;
    bool mStop;
    std::queue<std::function<void()>> mTasks;

public:
    friend class GLThreadWorker;

    RESOURCEMANAGER_API void Create( size_t threadCount, SDL_Window* window );
    RESOURCEMANAGER_API void Destroy();

    template<typename T, typename... Params>
    std::future<typename std::result_of<T( Params... )>::type> AddTask( Params&&... params );
};

template<typename T, typename... Params>
std::future<typename std::result_of<T( Params... )>::type> GLThreadPool::AddTask( Params&&... params )
{
    auto task = std::make_shared<std::packaged_task<typename std::result_of<T( Params... )>::type()>>( std::bind( T(), std::forward<Params>( params )... ) );
    std::future<typename std::result_of<T( Params... )>::type> result = task->get_future();

    {
        std::unique_lock<std::mutex> lock( mMutex );
        mTasks.push( [ task ]() { ( *task )( ); } );
    }
    mCondition.notify_one();

    return result;
}