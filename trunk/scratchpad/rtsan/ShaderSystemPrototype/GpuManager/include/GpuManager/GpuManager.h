#ifndef __GPUMANAGER_H__
#define __GPUMANAGER_H__

// Global includes
#include <ScriptLoaderGeneral/ISingleton.h>
#include <ScriptLoaderGeneral/general.h>
#include <string>
#include <hash_map>
#include <Cg/cgGL.h>

#include "GpuManagerDefs.h"

// Local includes
class GpuProgram;

class GPU_MANAGER_API GpuManager : public ISingleton<GpuManager> {
public:
	GpuManager();
	~GpuManager();

	GpuProgram* loadProgram(std::string file, std::string &type);
	GpuProgram* getProgram(const char* file);

	CGprofile getVertexProfile();
	CGprofile getFragmentProfile();

	CGcontext getContext();

	typedef std::hash_map<const char*, GpuProgram*, std::hash<const char*>, streqpred> programMap;

private:
	programMap m_programs;
	
	CGprofile m_vProfile;
	CGprofile m_fProfile;
};

#endif // __GPUMANAGER_H__