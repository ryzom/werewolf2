#ifndef WW_ENTITY_H
#define WW_ENTITY_H

#include "component_container.h"
#include "property_container.h"

class Entity : public ComponentContainer, public PropertyContainer {
public:
	virtual ~Entity() { };
};

#endif // WW_ENTITY_H