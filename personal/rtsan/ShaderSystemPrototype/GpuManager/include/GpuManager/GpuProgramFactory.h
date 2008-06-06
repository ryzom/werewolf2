#ifndef __GPUPROGRAMFACTORY_H__
#define __GPUPROGRAMFACTORY_H__

#include <hash_map>

#include "GpuManagerDefs.h"
#include "GpuProgram.h"

class GPU_MANAGER_API GpuProgramFactory : public ISingleton<GpuProgramFactory> {
public:
	GpuProgramFactory();
	typedef GpuProgram * (*CreateProgramCallback) (std::string);
	void registerProgram(const char* typeId, CreateProgramCallback createFn);
	void unregisterProgram(const char* typeId);
	GpuProgram *create(const char* typeId, std::string file);
private:
	typedef std::hash_map<const char*, CreateProgramCallback, std::hash<const char*>, streqpred> callbackMap;
	callbackMap m_callbacks;

};

#endif // __GPUPROGRAMFACTORY_H__