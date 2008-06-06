#ifndef __GPUPROGRAMINSTANCE_H__
#define __GPUPROGRAMINSTANCE_H__

// Global include
#include <Cg/cgGL.h>
#include <string>
#include <vector>

#include "GpuManagerDefs.h"
class GpuProgram;

class GPU_MANAGER_API GpuProgramInstance {
public:
	GpuProgramInstance(GpuProgram* program);
	~GpuProgramInstance();

	void deactivate();
	void activate();

	CGprogram getProgram();

private:
	friend GpuProgram;

	void activateSamplers();
	void deactivateSamplers();
	void getSamplers();
	void getSamplersSingle(CGparameter parameter, std::vector<CGparameter>* vect);

	void activateVarying();
	void deactivateVarying();
	void getVarying();
	void getVaryingSingle(CGparameter parameter, std::vector<CGparameter>* vect);
	CGparameter *m_samplers;
	std::vector<std::string> m_samplerBindings;
	CGparameter *m_varying;
	unsigned int m_numSamplers;
	unsigned int m_numVarying;
	CGparameter m_userParams;
	CGparameter m_engineParams;
	CGparameter m_modelParams;
	CGprogram m_prog;
};

#endif // __GPUPROGRAMINSTANCE_H__