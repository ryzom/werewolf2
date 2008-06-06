#ifndef __FLOATARRAYPROPERTY_H__
#define __FLOATARRAYPROPERTY_H__

#include "IProperty.h"

class FloatArrayProperty : public IProperty {
private:
	int m_size;
	float* (*m_func)(int);
	void setCGGLParameterSingle(CGparameter parameter, int i);
public:
	FloatArrayProperty(float* (*func)(int), unsigned int size, const char* name) : m_size(size), m_func(func) { setName(name); };
	~FloatArrayProperty() {};

	void setCGGLParameter(CGparameter& parameter);
	void setCGD3DParameter(CGparameter& parameter) { /* TODO: IMPLEMENT ME! */ };

	void setScriptParameter(ScriptVariable* var);

};

#endif // __FLOATARRAYPROPERTY_H__
