#ifndef __CONSTANTFLOATPROPERTY_H__
#define __CONSTANTFLOATPROPERTY_H__

#include "IProperty.h"

class ConstantFloatProperty : public IProperty {
private:
	unsigned int m_size;
	float* m_value;

public:
	ConstantFloatProperty(float* value, unsigned int size, const char* name) : m_size(min(4,size)), m_value(value) { setName(name); };
	~ConstantFloatProperty() {};

	void setCGGLParameter(CGparameter& parameter);
	void setCGD3DParameter(CGparameter& parameter) { /* TODO: IMPLEMENT ME! */ };

	void setScriptParameter(ScriptVariable* var);

};

#endif // __CONSTANTFLOATPROPERTY_H__