#ifndef WW_COMPONENT_HEALTH_H
#define WW_COMPONENT_HEALTH_H

#include "component.h"
#include "property.h"

class ComponentHealth : public Component {
public:
	ComponentHealth(Entity* entity, const std::string &name) : Component(entity, name) {
		m_maxHealth = entity->AddProperty<int>("MaxHealth", 0);
	}

	void Update(int deltaTime) { };
	void Render() { };

	static Component *Create(Entity* entity, const std::string &name) { return dynamic_cast<Component *>(new ComponentHealth(entity, name)); }

protected:
	Property<int> m_maxHealth;
};

#endif // WW_COMPONENT_HEALTH_H