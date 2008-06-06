#ifndef __SCRIPTFUNCTIONINSTANCE_H__
#define __SCRIPTFUNCTIONINSTANCE_H__

#include "ScriptFunction.h"
#include "ScriptVariable.h"
#include "ScriptArg.h"
#include <angelscript.h>
#include <hash_map>

class ScriptFunctionInstance {
public:
	ScriptFunctionInstance(const ScriptFunction* const function);
	~ScriptFunctionInstance();

	void execute();
	void reset();

	typedef std::hash_map<const char*, ScriptVariable*, std::hash<const char*>, streqpred> argInstMap;

	ScriptVariable* getArg(const char* name);
	argInstMap::iterator begin();
	argInstMap::iterator end();
	ScriptVariable* getRetVal();
	void setBoundArgs();

	asIScriptContext* getContext();

private:
	void prepare();
	void clear();
	void addArgument(ScriptArg* arg);
	bool m_executed;
	asIScriptContext* m_context;
	const ScriptFunction* const m_function;
	argInstMap m_args;
	ScriptVariable m_return;
};

#endif // __SCRIPTFUNCTIONINSTANCE_H__