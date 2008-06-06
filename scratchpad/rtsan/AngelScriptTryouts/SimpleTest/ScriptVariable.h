#ifndef __SCRIPTVARIABLE_H__
#define __SCRIPTVARIABLE_H__

#include <string>
#include "ScriptArg.h"
#include <angelscript.h>

class ScriptVariable {
public:
	ScriptVariable(ScriptArg* arg);
	ScriptVariable(ScriptArg::eType type, const char* name, ScriptArg::Binding* binding = NULL);

	~ScriptVariable();

	bool setScriptArg(asIScriptContext* context) const;

	bool setValue(int value);
	bool setValue(unsigned int value);
	bool setValue(long value);
	bool setValue(bool value);
	bool setValue(float value);
	bool setValue(double value);
	bool setValue(void* value);

	bool setValueFromReturn(asIScriptContext* context);
	bool setValueFromBinding();
	bool isBound() const;
	void Bind(const ScriptArg::Binding* binding);

	bool getValue(int& value) const;
	bool getValue(unsigned int& value) const;
	bool getValue(long& value) const;
	bool getValue(bool& value) const;
	bool getValue(float& value) const;
	bool getValue(double& value) const;
	bool getValue(void** value) const;

	ScriptArg::eType getType() const;
	const std::string& getName() const;

	void reset();
	bool isSet() const;
	bool isArg() const;

	int getId() const;

private:
	typedef union {
		int int_val;
		unsigned int uint_val;
		long long_val;
		bool bool_val;
		float float_val;
		double double_val;
		void* object_val;
	} valueType;

	valueType m_value;
	const ScriptArg::Binding* m_binding;
	ScriptArg::eType m_type;
	std::string m_name;
	bool m_set;
	int m_id;
};

#endif // __SCRIPTVARIABLE_H__