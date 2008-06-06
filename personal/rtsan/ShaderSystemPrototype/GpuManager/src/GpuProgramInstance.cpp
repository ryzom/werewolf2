#include "GpuManager/GpuProgramInstance.h"
#include "GpuManager/GpuManager.h"
#include "GpuManager/GpuProgram.h"
#include <PropertyManager/PropertyMap.h>
#include <PropertyManager/PropertyManager.h>
#include <Cg/cgGL.h>
#include <iostream.h>

GpuProgramInstance::GpuProgramInstance(GpuProgram* program) : m_samplers(NULL), m_varying(NULL), m_numSamplers(0), m_numVarying(0) {
	m_prog = cgCreateProgram(GpuManager::instance().getContext(), CG_SOURCE, program->getProgramBody()->c_str(), program->getProfile(), "main", NULL);
	cgGLLoadProgram(m_prog);
	m_engineParams = cgGetNamedParameter(m_prog, "ENGINE");
    m_userParams = cgGetNamedParameter(m_prog, "USER");
	m_modelParams = cgGetNamedParameter(m_prog, "MODEL");
	getSamplers();
	getVarying();
}

GpuProgramInstance::~GpuProgramInstance() {
	if(m_prog) cgDestroyProgram(m_prog);
	if(m_samplers) delete[] m_samplers;
	if(m_varying) delete[] m_varying;
}

CGprogram GpuProgramInstance::getProgram() {
	return m_prog;
}

void GpuProgramInstance::activate() {
	PropertyMap* map;
	activateVarying();
	if(m_engineParams) {
		map = PropertyManager::instance().getPropertyMap("ENGINE");
		map->traverseGLParameters(cgGetFirstStructParameter(m_engineParams));
	}
	if(m_userParams) {
		map = PropertyManager::instance().getPropertyMap("USER");
		map->traverseGLParameters(cgGetFirstStructParameter(m_userParams));
	}
	if(m_modelParams) {
		map = PropertyManager::instance().getPropertyMap("MODEL");
		map->traverseGLParameters(cgGetFirstStructParameter(m_modelParams));
	}
	activateSamplers();

	cgGLBindProgram(m_prog);
}

void GpuProgramInstance::deactivate() {
	deactivateSamplers();
	deactivateVarying();
}

void GpuProgramInstance::getSamplers() {
	std::vector<CGparameter> samplers;
	if(m_userParams)
		getSamplersSingle(cgGetFirstStructParameter(m_userParams), &samplers);
	if(m_engineParams)
		getSamplersSingle(cgGetFirstStructParameter(m_engineParams), &samplers);
	m_numSamplers = samplers.size();
	if(m_numSamplers > 0) {
		m_samplers = new CGparameter[m_numSamplers];
		for(unsigned int i = 0; i < m_numSamplers; i++) {
			m_samplers[i] = samplers[i];
			m_samplerBindings.push_back(cgGetParameterName(samplers[i]));
		}
	}
}

void GpuProgramInstance::getSamplersSingle(CGparameter parameter, std::vector<CGparameter>* vect) {
	if(parameter == 0)
		return;
	static int i = 0;
	do {
		CGtype type = cgGetParameterType(parameter);
		const char* name = cgGetParameterName(parameter);
		if(type >= CG_SAMPLER1D && type <= CG_SAMPLERCUBE) {
			vect->push_back(parameter);
		}
	} while((parameter = cgGetNextParameter(parameter))!=0);
}

void GpuProgramInstance::activateSamplers() {
	if(m_numSamplers == 0)
		return;
	for(unsigned int i = 0; i < m_numSamplers; i++) {
		cgGLEnableTextureParameter(m_samplers[i]);
	}
}

void GpuProgramInstance::deactivateSamplers() {
	if(m_numSamplers == 0)
		return;
	for(unsigned int i = 0; i < m_numSamplers; i++) {
		cgGLDisableTextureParameter(m_samplers[i]);
	}
}

void GpuProgramInstance::getVarying() {
	std::vector<CGparameter> varying;
	if(m_modelParams)
		getVaryingSingle(cgGetFirstStructParameter(m_modelParams), &varying);
	m_numVarying = varying.size();
	if(m_numVarying > 0) {
		m_varying = new CGparameter[m_numVarying];
		for(unsigned int i = 0; i < m_numVarying; i++) {
			m_varying[i] = varying[i];
		}
	}
}

void GpuProgramInstance::getVaryingSingle(CGparameter parameter, std::vector<CGparameter>* vect) {
	if(parameter == 0)
		return;
	static int i = 0;
	do {
		if(cgGetParameterVariability(parameter) == CG_VARYING) {
			vect->push_back(parameter);
		}
	} while((parameter = cgGetNextParameter(parameter))!=0);
}

void GpuProgramInstance::activateVarying() {
	if(m_numVarying == 0)
		return;
	for(unsigned int i = 0; i < m_numVarying; i++) {
		cgGLEnableClientState(m_varying[i]);
	}
}

void GpuProgramInstance::deactivateVarying() {
	if(m_numVarying == 0)
		return;
	for(unsigned int i = 0; i < m_numVarying; i++) {
		cgGLDisableClientState(m_varying[i]);
	}
}