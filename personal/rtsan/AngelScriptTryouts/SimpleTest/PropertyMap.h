#ifndef __PROPERTYMAP_H__
#define __PROPERTYMAP_H__

#include "IProperty.h"
#include "general.h"
#include <hash_map>

typedef std::hash_map<const char*, IProperty*, std::hash<const char*>, streqpred> propMap;

class PropertyMap {
private:
	propMap m_propertyMap;

public:
	PropertyMap() {};
	~PropertyMap();

	void registerProperty(IProperty* property);
	void removeProperty(const char* name);
	IProperty* getProperty(const char* name);
	void traverseGLParameters(CGparameter parameter);
	void traverseD3DParameters(CGparameter parameter);
};



#endif // __PROPERTYMAP_H__
