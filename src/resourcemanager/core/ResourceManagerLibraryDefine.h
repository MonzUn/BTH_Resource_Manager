#pragma once

#define RESOURCEMANAGER_DLL_EXPORT 1
#ifdef _WIN32
     #ifdef RESOURCEMANAGER_DLL_EXPORT
        #define RESOURCEMANAGER_API __declspec(dllexport)
    #else
        #define RESOURCEMANAGER_API __declspec(dllimport)
    #endif
#else
    #define RESOURCEMANAGER_API
#endif