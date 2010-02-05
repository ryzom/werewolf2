#ifndef WW_COMPONENT_CONTAINER_H
#define WW_COMPONENT_CONTAINER_H

#include <string>
#include <vector>

#include "component.h"

class ComponentContainer {
public:
	ComponentContainer() { };
	virtual ~ComponentContainer();

	Component *AddComponent(const std::string &componentType, const std::string &componentName);

	bool HasComponent(const std::string &componentName);

	Component *GetComponent(const std::string &componentName);

	void UpdateComponents(int deltaTime);

	typedef std::vector<Component*> ComponentVector;
	ComponentVector &GetComponents() { return m_components; }

	//void RequestCommandsFromComponents(std::vector<std::string> &requestCommands, ServerPlayer *player);
	//void ExecuteCommandOnComponents(const std::string &command, ServerPlayer *player);
protected:
	ComponentVector m_components;
};

#endif // WW_COMPONENT_CONTAINER_H