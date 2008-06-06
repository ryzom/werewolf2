#ifndef __SCRIPTLOADERDEFS_H__
#define __SCRIPTLOADERDEFS_H__

#if defined( __WIN32__ ) || defined( _WIN32 )
#   ifdef SCRIPT_LOADER_EXPORTS
#       define SCRIPT_LOADER_API __declspec(dllexport)
#   else
#       define SCRIPT_LOADER_API __declspec(dllimport)
#   endif
#else
#   define SCRIPT_LOADER_API
#endif

#endif // __GPUMANAGERDEFS_H__