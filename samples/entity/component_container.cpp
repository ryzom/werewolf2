#include "component_container.h"
#include "component_factory.h"
#include "entity.h"
#include "component.h"
#include "entity_exceptions.h"

ComponentContainer::~ComponentContainer() {
	ComponentVector::iterator itr=m_components.begin();
	while(itr != m_components.end()) {
		delete (*itr);
		itr++;
	}

	m_components.clear();
};

Component *ComponentContainer::AddComponent(const std::string &componentName) {
	Component *component = NULL;
	
	// Try and retrieve the component. If it isn't there (exception) then create it and add it.
	try {
		component = GetComponent(componentName);
	} catch(EComponentNotFound) {
		// Retrieve the component object.
		component = ComponentFactory::getInstance().GetComponent(componentName);
		
		// Apply this to the entity requesting.
		component->ApplyEntity(dynamic_cast<Entity*>(this));

		m_components.push_back(component);
	}
	return component;
};

bool ComponentContainer::HasComponent(const std::string &componentName) {
	try {
		Component *component = GetComponent(componentName);
		return true;
	} catch(EComponentNotFound) { }

	return false;
};

Component *ComponentContainer::GetComponent(const std::string &componentName) {
	// Prevent adding the same component multiple times.
	Component *component = NULL;
	bool componentFound = false;
	ComponentVector::iterator itr=m_components.begin();
	while(itr != m_components.end()) {
		component = (*itr);
		if(component->GetName() == componentName) {
			componentFound=true;
			break;
		}
		itr++;
	}

	if(!componentFound)
		throw EComponentNotFound("Unable to find component '%s' in component contaner.");

	return component;
};

void ComponentContainer::UpdateComponents(int deltaTime) {
ComponentVector::iterator itr=m_components.begin();
	while(itr != m_components.end()) {
		(*itr)->Update(deltaTime);
		itr++;
	}
}