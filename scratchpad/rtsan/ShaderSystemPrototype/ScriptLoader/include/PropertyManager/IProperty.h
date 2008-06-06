#ifndef __IPROPERTY_H__
#define __IPROPERTY_H__

// Global includes
#include <Cg/cgGL.h>
#include <vector>
#include <angelscript.h>
#include <sstream>
#include <xercesc/dom/DOM.hpp>
#include "ScriptLoaderGeneral/ScriptLoaderDefs.h"

class ScriptVariable;

class SCRIPT_LOADER_API IProperty {
protected:
	const char* m_name;

	IProperty() {};

public:
	virtual void setCGGLParameter(CGparameter& parameter) = 0;
	virtual void setCGD3DParameter(CGparameter& parameter) = 0;
	virtual void setScriptParameter(ScriptVariable* var) = 0;

	/**
	 * Not a typical hash function but rather a unique hash should be added to the end of the stream.
	 */
	virtual void getHash(std::stringstream& hash) = 0;

	static IProperty *create(xercesc::DOMNode* node) { return NULL; };

	virtual ~IProperty() {};
	void setName(const char *name) { m_name = name; };
	const char* getName() { return m_name; };
};

#endif // __IPROPERTY_H__
