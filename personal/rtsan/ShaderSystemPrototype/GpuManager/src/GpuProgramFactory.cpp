#include "GpuManager/GpuProgramFactory.h"

GpuProgramFactory::GpuProgramFactory() {
	registerProgram("default", GpuProgram::create);
}

void GpuProgramFactory::registerProgram(const char* typeId, GpuProgramFactory::CreateProgramCallback createFn) {
	m_callbacks[typeId] = createFn;
}

void GpuProgramFactory::unregisterProgram(const char* typeId) {
	m_callbacks.erase(typeId);
}

GpuProgram* GpuProgramFactory::create(const char *typeId, std::string file) {
	GpuProgramFactory::callbackMap::iterator iter = m_callbacks.find(typeId);
	if(iter == m_callbacks.end()) {
		return NULL;
	}
	return (iter->second)(file);
}
