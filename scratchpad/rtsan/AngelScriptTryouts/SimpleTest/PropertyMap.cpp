#include "PropertyMap.h"
#include <stdio.h>

void PropertyMap::registerProperty(IProperty* property) {
	IProperty *old = getProperty(property->getName());
	if(old)
		delete old;
	m_propertyMap[property->getName()] = property;
}

void PropertyMap::removeProperty(const char* name) {
	propMap::const_iterator iter;
	iter = m_propertyMap.find(name);
	if(iter != m_propertyMap.end()) {
		delete iter->second;
	}
	m_propertyMap.erase(name);
}

IProperty* PropertyMap::getProperty(const char* name) {
	propMap::const_iterator iter;
	iter = m_propertyMap.find(name);
	if(iter == m_propertyMap.end())
		return NULL;
	return iter->second;
}

PropertyMap::~PropertyMap() {
	propMap::iterator iter;
	for(iter = m_propertyMap.begin(); iter != m_propertyMap.end(); iter++) {
		delete iter->second;
	}
	m_propertyMap.clear();
}

void PropertyMap::traverseGLParameters(CGparameter parameter) {
	if(parameter == 0)
		return;
	static int i = 0;
	int arraySize = 0;
	do {
		IProperty* property = getProperty(cgGetParameterName(parameter));
		if(property == NULL) {
			printf("Parameter \"%s\" not found in property map!\n", cgGetParameterName(parameter));
		} else {
			property->setCGGLParameter(parameter);
		}
	} while((parameter = cgGetNextParameter(parameter))!=0);
}

void PropertyMap::traverseD3DParameters(CGparameter parameter) {
	if(parameter == 0)
		return;
	static int i = 0;
	int arraySize = 0;
	do {
		IProperty* property = getProperty(cgGetParameterName(parameter));
		if(property == NULL) {
			printf("Parameter \"%s\" not found in property map!\n", cgGetParameterName(parameter));
		} else {
			property->setCGD3DParameter(parameter);
		}
	} while((parameter = cgGetNextParameter(parameter))!=0);
}
