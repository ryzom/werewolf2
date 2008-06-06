#ifndef __PROPERTYFACTORY_H__
#define __PROPERTYFACTORY_H__

#include "ScriptLoaderGeneral/general.h"
#include "ScriptLoaderGeneral/ISingleton.h"
#include "ScriptLoaderGeneral/ScriptLoaderDefs.h"
#include "ScriptLoaderGeneral/ScriptLoaderDefs.h"
#include "IProperty.h"
#include <hash_map>
#include <xercesc/dom/DOM.hpp>

class SCRIPT_LOADER_API PropertyFactory : public ISingleton<PropertyFactory> {
public:
	PropertyFactory();
	typedef IProperty * (*CreatePropertyCallback) (xercesc::DOMNode*);
	void registerProperty(const char* typeId, CreatePropertyCallback createFn);
	void unregisterProperty(const char* typeId);
	IProperty *create(const char* typeId, xercesc::DOMNode*);
private:
	typedef std::hash_map<const char*, CreatePropertyCallback, std::hash<const char*>, streqpred> callbackMap;
	callbackMap m_callbacks;

};

#endif // __PROPERTYFACTORY_H__