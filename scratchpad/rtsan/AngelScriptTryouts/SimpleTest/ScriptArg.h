#ifndef __SCRIPTARG_H__
#define __SCRIPTARG_H__

#include <xercesc/dom/DOM.hpp>
#include <string>
#include <hash_map>

#include "general.h"
#include "ISingleton.h"

class ScriptArg {
public:
	ScriptArg(xercesc::DOMNode* node);
	~ScriptArg();

	enum eType {
		VOID_TYPE,
		INT,
		UINT,
		LONG,
		FLOAT,
		DOUBLE,
		BOOL,
		OBJECT,
		STRING
	};

	class Binding {
	public:
		Binding(const char* _map, const char* _property) : map(_map), property(_property) {};
		std::string map;
		std::string property;
	};

	const std::string& getName() const;
	int getId() const;
	eType getType() const;

	bool isBound() const;
	const Binding* getBinding() const;

	class Helper : public ISingleton<Helper> {
	public:
		Helper();
		~Helper();

		ScriptArg::eType find(const char* name) const;

		typedef std::hash_map<const char*, ScriptArg::eType, std::hash<const char*>, streqpred> typeMap;
//		typedef std::pair<const char*, ScriptArg::eType> typePair;

	private:
		typeMap m_types;
	};

private:
	void parseArg(xercesc::DOMNode* node);
	void parseBinding(xercesc::DOMNode* node);

	std::string m_name;
	int	m_id;
	eType m_type;
	Binding* m_binding;
};

#endif // __SCRIPTARG_H__