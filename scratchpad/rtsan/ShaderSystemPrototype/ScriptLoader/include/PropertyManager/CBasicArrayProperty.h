#ifndef __CBASIC_ARRAY_PROPERTY_H__
#define __CBASIC_ARRAY_PROPERTY_H__

// Local includes
#include "IProperty.h"
#include "ScriptLoader/ScriptVariable.h"
#include "ScriptLoaderGeneral/ScriptLoaderDefs.h"
#include "IArraySource.h"

template<class T>
class CBasicArrayProperty : public IProperty {
public:
	CBasicArrayProperty(IArraySource<T>* value, const char* name) : m_value(value) { setName(name); };
	virtual ~CBasicArrayProperty() {
		delete m_value;
	};

	void setCGGLParameter(CGparameter& parameter);
	void setCGD3DParameter(CGparameter& parameter) { /* TODO: IMPLEMENT ME! */ };

	void setScriptParameter(ScriptVariable* var);
	
	void getHash(std::stringstream &hash);

private:
	IArraySource<T>* m_value;
	void setCGGLParameterSingle(CGparameter parameter, int i);
};

template<class T>
void CBasicArrayProperty<T>::setCGGLParameter(CGparameter& parameter) {
	// TODO: maybe if it is a single float - set it to the first array element
	if(cgGetParameterType(parameter) != CG_ARRAY)
		return;
	int arraySize = cgGetArraySize(parameter, 0);
	for(int i = 0; i < arraySize; i++) {
		setCGGLParameterSingle(cgGetArrayParameter(parameter, i), i);
	}
}

template<class T>
void CBasicArrayProperty<T>::setCGGLParameterSingle(CGparameter parameter, int i) {
	float *val = new float[m_value->getSize()];
	for(unsigned int j = 0; j < m_value->getSize(); j++) {
		val[j] = (float)m_value->getValue(i)[j];
	}
	switch(m_value->getSize()) {
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
		printf("Ooops - wrong size for array property.");
	}
	delete[] val;
}

template<class T>
void CBasicArrayProperty<T>::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME
}

template<class T>
void CBasicArrayProperty<T>::getHash(std::stringstream &hash) {
	// TODO: IMPLEMENT ME
}

#endif // __C_BASIC_ARRAY_PROPERTY_H__
