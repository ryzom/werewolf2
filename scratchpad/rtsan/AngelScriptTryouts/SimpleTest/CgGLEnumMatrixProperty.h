#ifndef __CGGLENUMMATRIXPROPERTY_H__
#define __CGGLENUMMATRIXPROPERTY_H__

#include "IProperty.h"

class CgGLEnumMatrixProperty : public IProperty {
private:
	CGGLenum m_type;
	CGGLenum m_transform;

public:
	CgGLEnumMatrixProperty(CGGLenum type, CGGLenum transform, const char* name) : m_type(type), m_transform(transform) { setName(name); };
	~CgGLEnumMatrixProperty() {};

	void setCGGLParameter(CGparameter& parameter) { cgGLSetStateMatrixParameter(parameter, m_type, m_transform); };
	void setCGD3DParameter(CGparameter& parameter) { /* TODO: IMPLEMENT ME! */ };

	void setScriptParameter(ScriptVariable* var);

};

#endif // __CGGLENUMMATRIXPROPERTY_H__