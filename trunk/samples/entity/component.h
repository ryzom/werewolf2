#ifndef WW_COMPONENT_H
#define WW_COMPONENT_H

#include <string>

#include "property.h"
#include "entity.h"

class Component {
public:
	Component(Entity *entity, const std::string &name) : m_entity(entity), m_name(name) { };
	virtual ~Component() {}

	std::string GetName() { return m_name; };
	void Update(int deltaTime) { };

//	virtual void RequestCommands(std::vector<T_String> &requestCommands, ServerPlayer *player) {};
//	virtual void ExecuteCommand(const T_String &command, ServerPlayer *player) {};

	template<class T>Property<T> AddProperty(const std::string &propName, const T &defaultValue);
	template<class T>Property<T> GetProperty(const std::string &propName);
	void RemoveProperty(const std::string &propName);

protected:
	Entity *m_entity;
	std::string m_name;
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