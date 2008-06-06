// Local includes
#include "PropertyManager/PropertyManager.h"

PropertyManager::PropertyManager() {
	// do nothing
}

PropertyManager::~PropertyManager() {
/*	propertyMapMap::iterator iter;
	for(iter = m_maps.begin(); iter != m_maps.end(); iter++) {
		delete iter->second;
	}
	m_maps.clear();
*/
}

void PropertyManager::registerProperty(const char* map, IProperty* property) {
	PropertyMap* m = getPropertyMap(map);
	if(m == NULL)
		return;
	m->registerProperty(property);
}

IProperty* PropertyManager::getProperty(const char* map, const char* property) {
	PropertyMap* m = getPropertyMap(map);
	if(m == NULL)
		return NULL;
	return m->getProperty(property);
}

PropertyMap* PropertyManager::getPropertyMap(const char* name) {
	propertyMapMap::iterator iter = m_maps.find(name);
	if(iter == m_maps.end())
		return NULL;
	return iter->second;
}

void PropertyManager::setPropertyMap(const char* name, PropertyMap* map) {
	m_maps[name] = map;
}