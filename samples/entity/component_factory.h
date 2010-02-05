#ifndef WW_COMPONENT_FACTORY_H
#define WW_COMPONENT_FACTORY_H

#include <string>

#include <nel/misc/types_nl.h>
#include <nel/misc/app_context.h>
#include <nel/misc/debug.h>

class Component;
class Entity;

class ComponentFactory {
	ComponentFactory() { };
	virtual ~ComponentFactory() { };

	NLMISC_SAFE_SINGLETON_DECL(ComponentFactory);
public:
	typedef Component*(* ComponentCreator)(Entity* entity, const std::string &name);

	void RegisterComponent(const std::string &type, ComponentCreator functor);
	Component *CreateComponent(Entity *entity, const std::string &componentType, const std::string &componentName);
private:
	typedef std::map<std::string, ComponentCreator> ComponentCreatorMap;
	ComponentCreatorMap m_creators;
};

#endif // WW_COMPONENT_FACTORY_H