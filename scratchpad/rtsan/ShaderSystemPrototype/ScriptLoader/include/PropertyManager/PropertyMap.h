#ifndef __PROPERTYMAP_H__
#define __PROPERTYMAP_H__

// Local includes
#include "IProperty.h"
#include "ScriptLoaderGeneral/general.h"
#include "ScriptLoaderGeneral/ScriptLoaderDefs.h"

// Global includes
#include <hash_map>
#include <nel/misc/smart_ptr.h>


class SCRIPT_LOADER_API PropertyMap : public NLMISC::CRefCount {
public:
	typedef std::hash_map<const char*, IProperty*, std::hash<const char*>, streqpred> propMap;

	PropertyMap() {};
	PropertyMap(std::string file);
	~PropertyMap();

	void loadXML(std::string file);
	void registerProperty(IProperty* property);
	void removeProperty(const char* name);
	IProperty* getProperty(const char* name);
	void traverseGLParameters(CGparameter parameter);
	void traverseD3DParameters(CGparameter parameter);

private:
	propMap m_propertyMap;

};



#endif // __PROPERTYMAP_H__
