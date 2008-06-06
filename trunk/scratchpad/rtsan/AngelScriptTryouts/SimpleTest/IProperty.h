#ifndef __IPROPERTY_H__
#define __IPROPERTY_H__

#include <Cg/cgGL.h>
#include <vector>
#include <angelscript.h>

class ScriptVariable;

class IProperty {
protected:
	const char* m_name;

	IProperty() {};

public:
	virtual void setCGGLParameter(CGparameter& parameter) = 0;
	virtual void setCGD3DParameter(CGparameter& parameter) = 0;
	virtual void setScriptParameter(ScriptVariable* var) = 0;

	~IProperty() {};
	void setName(const char *name) { m_name = name; };
	const char* getName() { return m_name; };
};

#endif // __IPROPERTY_H__
