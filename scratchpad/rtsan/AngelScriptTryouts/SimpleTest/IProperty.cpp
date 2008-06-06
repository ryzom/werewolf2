#include "IProperty.h"
#include "ConstantFloatProperty.h"
#include "ConstantIntProperty.h"
#include "CgGLEnumMatrixProperty.h"
#include "FloatProperty.h"
#include "FloatArrayProperty.h"
#include "stdio.h"
#include "ScriptVariable.h"

void ConstantFloatProperty::setCGGLParameter(CGparameter& parameter) {
	switch(m_size) {
	case 1:
		cgGLSetParameter1fv(parameter, m_value);
		break;
	case 2:
		cgGLSetParameter2fv(parameter, m_value);
		break;
	case 3:
		cgGLSetParameter3fv(parameter, m_value);
		break;
	case 4:
		cgGLSetParameter4fv(parameter, m_value);
		break;
	default:
		printf("Ooops - wrong size for constant float property.");
	}
}

void ConstantFloatProperty::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME DECENTLY 
	var->setValue(*m_value);
}

void ConstantIntProperty::setCGGLParameter(CGparameter& parameter) {
	float *val = new float[m_size];
	for(unsigned int i = 0; i < m_size; i++) {
		val[i] = (float)m_value[i];
	}
	switch(m_size) {
	case 1:
		cgGLSetParameter1fv(parameter, val);
		break;
	case 2:
		cgGLSetParameter2fv(parameter, val);
		break;
	case 3:
		cgGLSetParameter3fv(parameter, val);
		break;
	case 4:
		cgGLSetParameter4fv(parameter, val);
		break;
	default:
		printf("Ooops - wrong size for constant float property.");
	}
	delete[] val;
}

void ConstantIntProperty::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME DECENTLY 
	var->setValue(*m_value);
}

void FloatProperty::setCGGLParameter(CGparameter& parameter) {
	switch(m_size) {
	case 1:
		cgGLSetParameter1fv(parameter, (*m_func)());
		break;
	case 2:
		cgGLSetParameter2fv(parameter, (*m_func)());
		break;
	case 3:
		cgGLSetParameter3fv(parameter, (*m_func)());
		break;
	case 4:
		cgGLSetParameter4fv(parameter, (*m_func)());
		break;
	default:
		printf("Ooops - wrong size for float property.");
	}
}

void FloatProperty::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME DECENTLY
	var->setValue(*((*m_func)()));
}

void FloatArrayProperty::setCGGLParameter(CGparameter& parameter) {
	// TODO: maybe if it is a single float - set it to the first array element
	if(cgGetParameterType(parameter) != CG_ARRAY)
		return;
	int arraySize = cgGetArraySize(parameter, 0);
	for(int i = 0; i < arraySize; i++) {
		setCGGLParameterSingle(cgGetArrayParameter(parameter, i), i);
	}
}

void FloatArrayProperty::setCGGLParameterSingle(CGparameter parameter, int i) {
	switch(m_size) {
	case 1:
		cgGLSetParameter1fv(parameter, (*m_func)(i));
		break;
	case 2:
		cgGLSetParameter2fv(parameter, (*m_func)(i));
		break;
	case 3:
		cgGLSetParameter3fv(parameter, (*m_func)(i));
		break;
	case 4:
		cgGLSetParameter4fv(parameter, (*m_func)(i));
		break;
	default:
		printf("Ooops - wrong size for float array property.");
	}	
}

void FloatArrayProperty::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME
}

void CgGLEnumMatrixProperty::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME
}