#include "PropertyManager/VaryingProperty.h"

#include "ScriptLoader/ScriptVariable.h"

VaryingProperty::VaryingProperty(GLint size, GLenum type, GLsizei stride, GLvoid* value, const char* name) 
: m_size(size), m_type(type), m_stride(stride), m_value(value) {
	setName(name);
}

VaryingProperty::~VaryingProperty() {
	// do nothing
}

void VaryingProperty::setCGGLParameter(CGparameter& parameter) {
	cgGLSetParameterPointer(parameter, m_size, m_type, m_stride, m_value);
}

void VaryingProperty::setScriptParameter(ScriptVariable* var) {
	// TODO: IMPLEMENT ME?
}

void VaryingProperty::getHash(std::stringstream& hash) {
	// TODO: IMPLEMENT ME?
}