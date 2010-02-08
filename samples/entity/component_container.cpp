#include "component_container.h"
#include "component_factory.h"
#include "entity.h"
#include "component.h"

ComponentContainer::~ComponentContainer() {
	ComponentVector::iterator itr=m_components.begin();
	while(itr != m_components.end()) {
		delete (*itr);
		itr++;
	}

	m_components.clear();
};

Component *ComponentContainer::AddComponent(const std::string &componentType, const std::string &componentName) {
	Component *component = GetComponent(componentName);
	if(component == NULL)
		component = ComponentFactory::getInstance().CreateComponent(dynamic_cast<Entity*>(this), componentType, componentName);
	return component;
};

bool ComponentContainer::HasComponent(const std::string &componentName) {
	Component *component = GetComponent(componentName);
	if(component)
		return true;
	return false;
};

Component *ComponentContainer::GetComponent(const std::string &componentName) {
	// Prevent adding the same component multiple times.
	ComponentVector::iterator itr=m_components.begin();
	while(itr != m_components.end()) {
		Component *component = (*itr);
		if(component->GetName() == componentName)
			return component;
		itr++;
	}
	return NULL;
};

void ComponentContainer::UpdateComponents(int deltaTime) {
ComponentVector::iterator itr=m_components.begin();
	while(itr != m_components.end()) {
		(*itr)->Update(deltaTime);
		itr++;
	}
}