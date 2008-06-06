#ifndef __GPUMANAGERDEFS_H__
#define __GPUMANAGERDEFS_H__

#if defined( __WIN32__ ) || defined( _WIN32 )
#   ifdef GPU_MANAGER_EXPORTS
#       define GPU_MANAGER_API __declspec(dllexport)
#   else
#       define GPU_MANAGER_API __declspec(dllimport)
#   endif
#else
#   define GPU_MANAGER_API
#endif

#endif // __GPUMANAGERDEFS_H__