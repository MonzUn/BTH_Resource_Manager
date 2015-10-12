#pragma once

#ifdef _WIN32
#ifdef RESOURCEMANAGER_DLL_EXPORT
#define RESOURCEMANAGER_API __declspec(dllexport)
#else
#define RESOURCEMANAGER_API __declspec(dllimport)
#endif
#else
#define RESOURCEMANAGER_API
#endif

class RESOURCEMANAGER_API ResourceManager
{
};