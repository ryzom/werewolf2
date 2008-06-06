#ifndef __MATRIX_PROPERTY_H__
#define __MATRIX_PROPERTY_H__

// Local includes
#include "IProperty.h"
#include "ScriptLoader/ScriptVariable.h"
#include "ScriptLoaderGeneral/ScriptLoaderDefs.h"
#include "ISource.h"

#include <nel/misc/matrix.h>

class MatrixProperty : public IProperty {
public:
	enum eType {
		IDENTITY,
		INVERSE,
		INVERSE_TRANSPOSE,
		TRANSPOSE
	};

	MatrixProperty(ISource<NLMISC::CMatrix>* matrix, ISource<eType>* type, const char* name) : m_matrix(matrix), m_type(type) { setName(name); };
	virtual ~MatrixProperty() {
		delete m_matrix;
		delete m_type;
	};

	void setCGGLParameter(CGparameter& parameter);
	void setCGD3DParameter(CGparameter& parameter) { /* TODO: IMPLEMENT ME! */ };

	void setScriptParameter(ScriptVariable* var);

	void getHash(std::stringstream& hash);
private:
	ISource<NLMISC::CMatrix>* m_matrix;
	ISource<eType>* m_type;
};

void MatrixProperty::setCGGLParameter(CGparameter& parameter) {
	NLMISC::CMatrix temp;
	switch(*(m_type->getValue())) {
		case IDENTITY:
			cgGLSetMatrixParameterfc(parameter, m_matrix->getValue()->get());
			break;
		case INVERSE:
			cgGLSetMatrixParameterfc(parameter, m_matrix->getValue()->inverted().get());
			break;
		case TRANSPOSE:
			temp = *(m_matrix->getValue());
			temp.transpose();
			cgGLSetMatrixParameterfc(parameter, temp.get());
			break;
		case INVERSE_TRANSPOSE:
			temp = m_matrix->getValue()->inverted();
			temp.transpose();
			cgGLSetMatrixParameterfc(parameter, temp.get());
			break;
		default:
			// should never happen
			break;
	}
}

void MatrixProperty::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME DECENTLY
	var->setValue((void*)(m_matrix->getValue()));
}

void MatrixProperty::getHash(std::stringstream& hash) {
	unsigned int length = sizeof(NLMISC::CMatrix);
	char* t = new char[length];
	memcpy(t, m_matrix->getValue(), length);
	for(unsigned int i = 0; i < length; i++)
		hash << t[i];
	delete[] t;
}

#endif // __MATRIX_PROPERTY_H__