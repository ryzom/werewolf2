#ifndef __PROPERTY_MANAGER_H__
#define __PROPERTY_MANAGER_H__

// Local includes
#include "PropertyMap.h"
#include "IProperty.h"
#include "ScriptLoaderGeneral/ISingleton.h"
#include "ScriptLoaderGeneral/general.h"
#include "ScriptLoaderGeneral/ScriptLoaderDefs.h"

// Global includes
#include <hash_map>


class SCRIPT_LOADER_API PropertyManager : public ISingleton<PropertyManager> {
public:
	typedef std::hash_map<const char*, PropertyMap*, std::hash<const char*>, streqpred> propertyMapMap;
	
	PropertyManager();
	~PropertyManager();

	IProperty* getProperty(const char* map, const char* property);
	/**
	 * Adds a new property to the specified map if the map exists.
	 */
	void registerProperty(const char* map, IProperty* property);

	PropertyMap* getPropertyMap(const char* name);
	void setPropertyMap(const char* name, PropertyMap* map);

private:
	propertyMapMap m_maps;
};

#endif // __PROPERTYMANAGER_H__