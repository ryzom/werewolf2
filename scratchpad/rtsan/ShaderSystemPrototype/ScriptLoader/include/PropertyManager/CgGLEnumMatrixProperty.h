#ifndef __CGGLENUMMATRIXPROPERTY_H__
#define __CGGLENUMMATRIXPROPERTY_H__

// Local includes
#include "IProperty.h"
#include "ScriptLoaderGeneral/ScriptLoaderDefs.h"

class SCRIPT_LOADER_API CgGLEnumMatrixProperty : public IProperty {
private:
	CGGLenum m_type;
	CGGLenum m_transform;

public:
	CgGLEnumMatrixProperty(CGGLenum type, CGGLenum transform, const char* name) : m_type(type), m_transform(transform) { setName(name); };
	virtual ~CgGLEnumMatrixProperty() {};

	void setCGGLParameter(CGparameter& parameter) { cgGLSetStateMatrixParameter(parameter, m_type, m_transform); };
	void setCGD3DParameter(CGparameter& parameter) { /* TODO: IMPLEMENT ME! */ };

	void setScriptParameter(ScriptVariable* var);

	void getHash(std::stringstream& hash);
};

#endif // __CGGLENUMMATRIXPROPERTY_H__