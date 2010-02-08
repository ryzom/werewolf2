#ifndef WW_PROPERTY_CONTAINER_H
#define WW_PROPERTY_CONTAINER_H

#include <string>
#include <map>

#include "iproperty.h"
#include "property.h"

class Component;

class PropertyContainer {
public:
	PropertyContainer() { }
	virtual ~PropertyContainer() { RemoveAllProperties(); }

	bool HasProperty(const std::string &propName);

	template<class T> Property<T> AddProperty(const std::string &propName, const T &defaultValue);
	
	void AddProperty(IProperty *property);

	template<class T>Property<T> GetProperty(const std::string &propName);
	IProperty *GetIProperty(const std::string &propName);
	
	void RemoveProperty(const std::string &propName, bool postponeDelete = false);
	void RemoveAllProperties();

	typedef std::map<std::string, IProperty*> PropertyMap;

	PropertyMap &GetProperties() { return properties; }

	typedef std::vector<IProperty*> PropertyVector;
	PropertyVector &GetNewProperties() { return addedProperties; }
	PropertyVector &GetDeletedProperties() { return deletedProperties; }

	void UpdateProperties(int deltaTime);
	//CL_Signal_v1<int> SigUpdateProperties;

protected:
	void ClearDirtyProperties();
	void ClearAddedProperties();
	void ClearDeletedProperties();

	PropertyMap properties;

	PropertyVector addedProperties;
	PropertyVector deletedProperties;
};

inline bool PropertyContainer::HasProperty(const std::string &propName) {
	PropertyMap::iterator it = properties.find(propName);
	if(it != properties.end())
		return true;
	return false;
}

inline void PropertyContainer::AddProperty(IProperty *property) {
	PropertyMap::iterator it = properties.find(property->GetName());
	if(it == properties.end()) {
		addedProperties.push_back(property);
		properties[property->GetName()] = property;
	}
}

template<class T>
inline Property<T> PropertyContainer::AddProperty(const std::string &propName, const T &defaultValue) {
	PropertyMap::iterator it = properties.find(propName);
	if(it != properties.end()) {
		Property<T>* property = dynamic_cast< Property<T>* >(it->second);
		if(!property)
			throw NLMISC::Exception("Property " + propName + " already exists, but with another type!");
		return *property;
	}

	Property<T> *property = new Property<T>(propName);
	property->Set(defaultValue);
	addedProperties.push_back(property);
	properties[property->GetName()] = property;

	return *property;
}

template<class T>
inline Property<T> PropertyContainer::GetProperty(const std::string &propName) {
	PropertyMap::iterator it = properties.find(propName);
	if(it == properties.end())
		throw NLMISC::Exception("Unable to get property " + propName);

	Property<T>* property = dynamic_cast< Property<T>* >(it->second);
	if(!property)
			throw NLMISC::Exception("Tried to get property " + propName + ", but the type was wrong!");
	
	return *property;
}

inline IProperty *PropertyContainer::GetIProperty(const std::string &propName) {
	PropertyMap::iterator it = properties.find(propName);
	if(it == properties.end())
		throw NLMISC::Exception("Unable to get property " + propName);

	return it->second;
}

inline void PropertyContainer::RemoveProperty(const std::string &propName, bool postponeDelete) {
	PropertyMap::iterator it = properties.find(propName);
	if(it != properties.end()) {
		IProperty* property = (*it).second;
		if(postponeDelete)
			deletedProperties.push_back(property);
		else
			delete property;
		properties.erase(it);
	}
}

inline void PropertyContainer::RemoveAllProperties() {
	PropertyMap::iterator itr = properties.begin();
	while(itr != properties.end()) {
		IProperty* property = (*itr).second;
		delete property;
		itr++;
	}
	properties.clear();
	ClearAddedProperties();
	ClearDeletedProperties();
}

inline void PropertyContainer::UpdateProperties(int deltaTime) {
	//SigUpdateProperties.invoke(deltaTime);

	ClearDirtyProperties();
	ClearAddedProperties();
	ClearDeletedProperties();
}

inline void PropertyContainer::ClearDirtyProperties() {
	PropertyMap::iterator itr = properties.begin();
	while(itr != properties.end()) {
		IProperty* property = (*itr).second;
		if(property->IsDirty())
			property->ClearDirty();
		itr++;
	}
}

inline void PropertyContainer::ClearAddedProperties() {
	addedProperties.clear();
}

inline void PropertyContainer::ClearDeletedProperties() {
	PropertyVector::iterator itr = deletedProperties.begin();
	while(itr != deletedProperties.end()) {
		IProperty* property = (*itr);
		delete property;
		itr++;
	}
	deletedProperties.clear();
}


#endif // WW_PROPERTY_CONTAINER_H