#ifndef __FLOATPROPERTY_H__
#define __FLOATPROPERTY_H__

#include "IProperty.h"

class FloatProperty : public IProperty {
private:
	unsigned int m_size;
	float* (*m_func)(void);

public:
	FloatProperty(float* (*func)(void), unsigned int size, const char* name) : m_size(min(4,size)), m_func(func) { setName(name); };
	~FloatProperty() {};

	void setCGGLParameter(CGparameter& parameter);
	void setCGD3DParameter(CGparameter& parameter) { /* TODO: IMPLEMENT ME! */ };

	void setScriptParameter(ScriptVariable* var);
};

#endif // __FLOATPROPERTY_H__