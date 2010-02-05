#include "component_factory.h"

NLMISC_SAFE_SINGLETON_IMPL(ComponentFactory)

Component *ComponentFactory::CreateComponent(Entity *entity, 
											 const std::string &componentType, 
											 const std::string &componentName) {
	ComponentCreatorMap::iterator itr = m_creators.find(componentType);
	if(itr == m_creators.end())
		throw NLMISC::Exception("Unable to create component '" + componentType + "'");

	ComponentCreator creator = itr->second;

	if(componentName.empty())
		return creator(entity, componentType);
	else
		return creator(entity, componentName);
}

void ComponentFactory::RegisterComponent(const std::string &type, ComponentCreator functor) {
	if(m_creators.find(type) == m_creators.end()) {
		std::pair<std::string, ComponentCreator> value(type, functor);
		m_creators.insert(value);
	}

}