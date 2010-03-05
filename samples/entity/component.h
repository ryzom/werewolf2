#ifndef WW_COMPONENT_H
#define WW_COMPONENT_H

#include <string>

#include "property.h"
#include "entity.h"
#include "entity_event.h"

class Component {
public:
	Component(const std::string &name) : m_name(name) { };
	virtual ~Component() {}

	std::string GetName() { return m_name; };
	void Update(int deltaTime) { };
	void Render() { };
	virtual void ApplyEntity(Entity *entity) { };

	typedef std::vector<uint16> eventList;

	/// Returns the events this handler handles.
	virtual eventList* getEventList() { return &m_eventList; };

	/// Handle the event for the given sob.
	virtual bool handleEvent(EntityEvent event, Entity *subject) = 0;


protected:
	std::string m_name;
	eventList m_eventList;
};

#endif // WW_COMPONENT_H