#ifndef __VARYINGPROPERTY_H__
#define __VARYINGPROPERTY_H__

// Local includes
#include "IProperty.h"
#include "ScriptLoaderGeneral/ScriptLoaderDefs.h"

// TODO: Very OGL specific atm - generalize
class SCRIPT_LOADER_API VaryingProperty : public IProperty {
private:
	GLvoid* m_value;
	GLint m_size;
	GLsizei m_stride;
	GLenum m_type;

public:
	VaryingProperty(GLint size, GLenum type, GLsizei stride, GLvoid* value, const char* name);
	virtual ~VaryingProperty();

	void setCGGLParameter(CGparameter& parameter);
	void setCGD3DParameter(CGparameter& parameter) { /* TODO: IMPLEMENT ME! */ };

	void setScriptParameter(ScriptVariable* var);

	void getHash(std::stringstream& hash);
};

#endif // __VARYINGPROPERTY_H__