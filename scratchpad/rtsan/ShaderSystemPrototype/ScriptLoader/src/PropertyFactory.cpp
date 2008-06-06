#include "PropertyManager/PropertyFactory.h"
#include "PropertyManager/TextureDdsProperty.h"
#include "PropertyManager/TextureDdsCubeProperty.h"
#include "PropertyManager/CBasicProperty.h"

PropertyFactory::PropertyFactory() {
	registerProperty("float", CBasicProperty<float>::create);
	registerProperty("textureDDS", TextureDdsProperty::create);
	registerProperty("textureDDSCube", TextureDdsCubeProperty::create);
}

void PropertyFactory::registerProperty(const char* typeId, PropertyFactory::CreatePropertyCallback createFn) {
	m_callbacks[typeId] = createFn;
}

void PropertyFactory::unregisterProperty(const char* typeId) {
	m_callbacks.erase(typeId);
}

IProperty* PropertyFactory::create(const char *typeId, xercesc::DOMNode* node) {
	PropertyFactory::callbackMap::iterator iter = m_callbacks.find(typeId);
	if(iter == m_callbacks.end()) {
		return NULL;
	}
	return (iter->second)(node);
}
