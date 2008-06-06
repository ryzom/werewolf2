#include "GpuManager/GpuManager.h"
#include "GpuManager/GpuProgram.h"
#include "GpuManager/GpuProgramFactory.h"

CGcontext shaderContext;

void cgErrorCallback(void) {
    CGerror LastError = cgGetError();

    if(LastError)
    {
        const char *Listing = cgGetLastListing(shaderContext);
        printf("\n---------------------------------------------------\n");
        printf("%s\n\n", cgGetErrorString(LastError));
        printf("%s\n", Listing);
        printf("---------------------------------------------------\n");
        printf("Cg error, exiting...\n");
		system("PAUSE");
        exit(0);
    }
}

GpuManager::GpuManager() {
	cgSetErrorCallback(cgErrorCallback);
	shaderContext = cgCreateContext();

	m_vProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
	m_fProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
}

GpuManager::~GpuManager() {
	for(GpuManager::programMap::iterator iter = m_programs.begin(); iter != m_programs.end(); iter++) {
		delete iter->second;
	}
	m_programs.clear();
	if(shaderContext) cgDestroyContext(shaderContext);
}

CGcontext GpuManager::getContext() {
	return shaderContext;
}

CGprofile GpuManager::getVertexProfile() {
	return m_vProfile;
}

CGprofile GpuManager::getFragmentProfile() {
	return m_fProfile;
}

GpuProgram* GpuManager::getProgram(const char* file) {
	GpuManager::programMap::iterator iter = m_programs.find(file);
	if(iter == m_programs.end())
		return NULL;
	return iter->second;
}

GpuProgram* GpuManager::loadProgram(std::string file, std::string &type) {
	GpuProgram* prog = getProgram(file.c_str());
	if(prog)
		return prog;
	prog = GpuProgramFactory::instance().create(type.c_str(), file);
	m_programs[prog->getFile().c_str()] = prog;
	return prog;
}