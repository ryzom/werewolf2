#include "GpuManager/GpuProgram.h"
#include "GpuManager/GpuManager.h"
#include <sstream>
#include <PropertyManager/PropertyManager.h>

GpuProgram::GpuProgram(std::string file) : m_file(file) {
	m_script = ScriptManager::instance().loadScript(file, "gpu");
	m_inst = m_script->getFunction("getShader")->getInstance();

	ScriptFunctionInstance *type = m_script->getFunction("getType")->getInstance();
	type->execute();
	int val;
	type->getRetVal()->getValue(val);
	m_type = (GpuProgram::eType)val;
	delete type;

	if(m_type == VERTEX)
		m_profile = GpuManager::instance().getVertexProfile();
	else
		m_profile = GpuManager::instance().getFragmentProfile();

	ScriptFunctionInstance::argInstMap::const_iterator iter = m_inst->begin();
	for(;iter != m_inst->end(); iter++) {
		m_dbindings.push_back(iter->second->getBinding());
	}
}

GpuProgram::~GpuProgram() {
	for(GpuProgram::instanceMap::iterator iter = m_instances.begin(); iter != m_instances.end(); iter++) {
		delete iter->second;
	}
	m_instances.clear();
	if(m_inst) delete m_inst;
}

GpuProgram::eType GpuProgram::getType() {
	return m_type;
}

std::string GpuProgram::getHash() {
	std::stringstream stream;
	stream << 'a'; // so it's not empty when there are no bindings
	std::vector<const ScriptArg::Binding*>::iterator iter = m_dbindings.begin();
	for(;iter != m_dbindings.end(); iter++) {
		IProperty* prop = PropertyManager::instance().getProperty((*iter)->map.c_str(), (*iter)->property.c_str());
		prop->getHash(stream);
	}
	return stream.str();
}

void GpuProgram::activate() {
	GpuProgramInstance* prog = getInstance();
	prog->activate();
	cgGLEnableProfile(m_profile);
}

void GpuProgram::deactivate() {
	GpuProgramInstance* prog = getInstance();
	prog->deactivate();
	cgGLDisableProfile(m_profile);
}

GpuProgramInstance* GpuProgram::getInstance() {
	std::string hash = getHash();
	GpuProgramInstance* prog_inst;
	GpuProgram::instanceMap::iterator iter = m_instances.find(hash);
	if(iter == m_instances.end()) {
		prog_inst = new GpuProgramInstance(this);
		m_instances[hash] = prog_inst;
	} else {
		prog_inst = iter->second;
	}
	return prog_inst;
}

const std::string& GpuProgram::getFile() const {
	return m_file;
}

const std::string& GpuProgram::getName() const {
	return m_script->getName();
}

std::string* GpuProgram::getProgramBody() {
	std::string *program;
	m_inst->setBoundArgs();
	m_inst->execute();
	m_inst->getRetVal()->getValue((void**)&program);
	return program;
}
CGprofile GpuProgram::getProfile() {
	return m_profile;
}

GpuProgram* GpuProgram::create(std::string file) {
	return new GpuProgram(file);
}

const std::vector<std::string> &GpuProgram::getSamplerBindings() {
	GpuProgramInstance* inst = getInstance();
	return inst->m_samplerBindings;
}