#ifndef WW_COMPONENT_H
#define WW_COMPONENT_H

#include <string>

class Entity;

class Component {
public:
	Component(Entity *entity, const std::string &name) : m_entity(entity), m_name(name) { };

	std::string GetName() { return m_name; };
	void Update(int deltaTime) { };

//	virtual void RequestCommands(std::vector<T_String> &requestCommands, ServerPlayer *player) {};
//	virtual void ExecuteCommand(const T_String &command, ServerPlayer *player) {};

//	template<class T>Property<T> AddProperty(const T_String &propName, const T &defaultValue);
//	template<class T>Property<T> GetProperty(const T_String &propName);
//	void RemoveProperty(const T_String &propName);

protected:
	Entity *m_entity;
	std::string m_name;
};

//template<class T>
//inline Property<T> Component::AddProperty(const T_String &propName, const T &defaultValue)
//{
//	T_String compPropName = name + ":" + propName;
//	return entity->AddProperty<T>(compPropName, defaultValue);
//}
//
//template<class T>
//inline Property<T> Component::GetProperty(const T_String &propName)
//{
//	T_String compPropName = name + ":" + propName;
//	return entity->GetProperty<T>(compPropName);
//}
//
//inline void Component::RemoveProperty(const T_String &propName)
//{
//	T_String compPropName = name + ":" + propName;
//	entity->RemoveProperty(compPropName);
//}
//}


#endif // WW_COMPONENT_H