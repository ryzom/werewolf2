// Local includes
#include "PropertyManager/IProperty.h"
#include "PropertyManager/CgGLEnumMatrixProperty.h"
#include "ScriptLoader/ScriptVariable.h"

// Global includes
#include <stdio.h>

void CgGLEnumMatrixProperty::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME
}

void CgGLEnumMatrixProperty::getHash(std::stringstream& hash) {
	unsigned int length = sizeof(CGGLenum);
	char* t = new char[length];
	memcpy(&t, &m_transform, length);
	for(unsigned int i = 0; i < length; i++)
		hash << t[i];
	memcpy(&t, &m_type, length);
	for(unsigned int i = 0; i < length; i++)
		hash << t[i];
	delete[] t;
}