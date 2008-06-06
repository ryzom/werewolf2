#ifndef __TEXTUREDDSCUBEPROPERTY_H__
#define __TEXTUREDDSCUBEPROPERTY_H__

// Local includes
#include "IProperty.h"
#include "ScriptLoaderGeneral/ScriptLoaderDefs.h"

class SCRIPT_LOADER_API TextureDdsCubeProperty : public IProperty {
private:
	GLuint m_value;
	std::string m_file;

public:
	TextureDdsCubeProperty(const char* file, const char* name);
	virtual ~TextureDdsCubeProperty();

	void setCGGLParameter(CGparameter& parameter);
	void setCGD3DParameter(CGparameter& parameter) { /* TODO: IMPLEMENT ME! */ };

	void setScriptParameter(ScriptVariable* var);

	void getHash(std::stringstream& hash);
	static IProperty* create(xercesc::DOMNode* node);
};

#endif // __TEXTUREDDSCUBEPROPERTY_H__