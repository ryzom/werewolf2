#include "component_factory.h"
#include "component.h"
#include "entity_exceptions.h"

NLMISC_SAFE_SINGLETON_IMPL(ComponentFactory)

void ComponentFactory::RegisterComponent(const std::string &type, ComponentCreator functor) {
	m_components.insert(TComponentMap::value_type(type, functor(type)));
}

Component *ComponentFactory::GetComponent(const std::string &componentType) {
	//// Find the controller by id, make sure we can find it.
	TComponentMap::const_iterator itr = m_components.find(componentType);
	if( itr == m_components.end())
		throw EComponentNotRegistered("Unable to create component '" + componentType + "'");

	// return a pointer to the component.
	return (itr->second);
}
