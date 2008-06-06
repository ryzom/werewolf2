#ifndef __CONSTANTINTPROPERTY_H__
#define __CONSTANTINTPROPERTY_H__

#include "IProperty.h"

class ConstantIntProperty : public IProperty {
private:
	unsigned int m_size;
	int* m_value;

public:
	ConstantIntProperty(int* value, unsigned int size, const char* name) : m_size(min(4,size)), m_value(value) { setName(name); };
	~ConstantIntProperty() {};

	void setCGGLParameter(CGparameter& parameter);
	void setCGD3DParameter(CGparameter& parameter) { /* TODO: IMPLEMENT ME! */ };

	void setScriptParameter(ScriptVariable* var);

};

#endif // __CONSTANTFLOATPROPERTY_H__