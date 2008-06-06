#ifndef __TEXTUREDDSPROPERTY_H__
#define __TEXTUREDDSPROPERTY_H__

// Local includes
#include "IProperty.h"
#include "ScriptLoaderGeneral/ScriptLoaderDefs.h"

class SCRIPT_LOADER_API TextureDdsProperty : public IProperty {
private:
	GLuint m_value;
	std::string m_file;

public:
	TextureDdsProperty(const char* file, const char* name);
	virtual ~TextureDdsProperty();

	void setCGGLParameter(CGparameter& parameter);
	void setCGD3DParameter(CGparameter& parameter) { /* TODO: IMPLEMENT ME! */ };

	void setScriptParameter(ScriptVariable* var);

	void getHash(std::stringstream& hash);
	static IProperty* create(xercesc::DOMNode* node);
};

#endif // __TEXTUREDDSPROPERTY_H__