#ifndef __GPUPROGRAM_H__
#define __GPUPROGRAM_H__

// Global includes
#include <hash_map>
#include <string>
#include <vector>
#include <ScriptLoader/ScriptManager.h>
#include <ScriptLoader/ScriptFunctionInstance.h>
#include <ScriptLoaderGeneral/general.h>

// Local includes
#include "GpuProgramInstance.h"
#include "GpuManagerDefs.h"

class GPU_MANAGER_API GpuProgram {
public:
	GpuProgram(std::string file);
	~GpuProgram();

	virtual void activate();
	virtual void deactivate();

	const std::string& getFile() const;
	const std::string& getName() const;

	enum eType {
		VERTEX,
		FRAGMENT
	};

	eType getType();
	CGprofile getProfile();
	const std::vector<std::string> &getSamplerBindings();

	static GpuProgram *create(std::string file);

	typedef std::hash_map<std::string, GpuProgramInstance*, std::hash<std::string>, stdstreqpred> instanceMap;
protected:
	GpuProgramInstance* getInstance();
	std::string getHash();
	std::string* getProgramBody();

	friend GpuProgramInstance;

	instanceMap m_instances;
	std::vector<const ScriptArg::Binding*> m_dbindings;
	ScriptFunctionInstance* m_inst;
	Script* m_script;
	std::string m_file;
	eType m_type;
	CGprofile m_profile;
};

#endif // __GPUPROGRAM_H__