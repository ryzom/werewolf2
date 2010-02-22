#ifndef WW_COMPONENT_H
#define WW_COMPONENT_H

#include <string>

#include "property.h"
#include "entity.h"
#include "entity_event.h"

class Component {
public:
	Component(Entity *entity, const std::string &name) : m_entity(entity), m_name(name) { };
	virtual ~Component() {}

	std::string GetName() { return m_name; };
	void Update(int deltaTime) { };
	void Render() { };

	typedef std::vector<uint16> eventList;

	/// Returns the events this handler handles.
	virtual eventList* getEventList() { return &m_eventList; };

	/// Handle the event for the given sob.
	virtual bool handleEvent(EntityEvent event, Entity *subject) = 0;
//	virtual void RequestCommands(std::vector<T_String> &requestCommands, ServerPlayer *player) {};
//	virtual void ExecuteCommand(const T_String &command, ServerPlayer *player) {};

	template<class T>Property<T> AddProperty(const std::string &propName, const T &defaultValue);
	template<class T>Property<T> GetProperty(const std::string &propName);
	void RemoveProperty(const std::string &propName);

protected:
	Entity *m_entity;
	std::string m_name;
	eventList m_eventList;
};

template<class T>
inline Property<T> Component::AddProperty(const std::string &propName, const T &defaultValue) {
	std::string compPropName = name + ":" + propName;
	return m_entity->AddProperty<T>(compPropName, defaultValue);
}

template<class T>
inline Property<T> Component::GetProperty(const std::string &propName) {
	std::string compPropName = name + ":" + propName;
	return m_entity->GetProperty<T>(compPropName);
}

inline void Component::RemoveProperty(const std::string &propName) {
	std::string compPropName = m_name + ":" + propName;
	m_entity->RemoveProperty(compPropName);
}

#endif // WW_COMPONENT_H