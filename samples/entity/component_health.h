#ifndef WW_COMPONENT_HEALTH_H
#define WW_COMPONENT_HEALTH_H

#include "component.h"
#include "property.h"

class ComponentHealth : public Component {
public:
	ComponentHealth(const std::string &name) : Component(name) { };

	void Update(int deltaTime) { };
	void Render() { };

	void ApplyEntity(Entity *entity) {
		entity->AddProperty<int>("MaxHealth", 0);
	}

	bool handleEvent(EntityEvent event, Entity *subject);

	static Component *Create(const std::string &name) { return dynamic_cast<Component *>(new ComponentHealth(name)); }
};

#endif // WW_COMPONENT_HEALTH_H