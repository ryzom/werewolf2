// Local includes
#include "ScriptLoader/ScriptFunctionInstance.h"
#include "ScriptLoader/ScriptManager.h"

// Global includes
#include <iostream>

ScriptFunctionInstance::ScriptFunctionInstance(const ScriptFunction* const function) :
m_function(function), m_executed(false), m_context(NULL), m_return(function->getRetValType(), "return") {
		for(ScriptFunction::argMap::const_iterator iter = function->begin(); iter != function->end(); iter++) {
			addArgument(iter->second);
		}
		reset();
}

ScriptFunctionInstance::~ScriptFunctionInstance() {
	clear();
}

void ScriptFunctionInstance::reset() {

	if(m_context)
		m_context->Release();

	m_executed = false;
	
	asIScriptEngine* engine = ScriptManager::instance().getEngine();
	m_context = engine->CreateContext();

	m_context->Prepare(m_function->getId());
	
	for(ScriptFunctionInstance::argInstMap::iterator iter = m_args.begin(); iter != m_args.end(); iter++) {
		iter->second->reset();
	}
}

void ScriptFunctionInstance::execute() {
	if(!m_context)
		return;
	prepare();
	for(ScriptFunctionInstance::argInstMap::iterator iter = begin(); iter != end(); iter++) {
		if(!iter->second->isSet()) {
			std::cout << "Execution failed : argument " << iter->second->getName() << " not set!" << std::endl;
			return;
		}
		iter->second->setScriptArg(m_context);
	}
	m_context->Execute();
	m_executed = true;
}

ScriptFunctionInstance::argInstMap::iterator ScriptFunctionInstance::begin() {
	prepare();
	return m_args.begin();
}

ScriptFunctionInstance::argInstMap::iterator ScriptFunctionInstance::end() {
	prepare();
	return m_args.end();
}

ScriptVariable* ScriptFunctionInstance::getArg(const char* name) {
	prepare();
	ScriptFunctionInstance::argInstMap::iterator iter = m_args.find(name);
	if(iter == m_args.end())
		return NULL;
	return iter->second;
}

void ScriptFunctionInstance::prepare() {
	if(m_executed) {
		m_executed = false;
		m_context->Prepare(m_function->getId());
	}
}

void ScriptFunctionInstance::clear() {
	if(m_context)
		m_context->Release();
	for(ScriptFunctionInstance::argInstMap::iterator iter = m_args.begin(); iter != m_args.end(); iter++) {
		delete iter->second;
	}
	m_args.clear();
}

ScriptVariable* ScriptFunctionInstance::getRetVal() {
	if(m_return.getType() == ScriptArg::VOID_TYPE)
		return NULL;
	if(m_executed && m_context) {
		m_return.setValueFromReturn(m_context);
		return &m_return;
	}
	else
		return NULL;
}

void ScriptFunctionInstance::addArgument(ScriptArg* arg) {
	m_args[arg->getName().c_str()] = new ScriptVariable(arg);
}

void ScriptFunctionInstance::setBoundArgs() {
	prepare();
	for(ScriptFunctionInstance::argInstMap::const_iterator iter = begin(); iter != end(); iter++) {
		iter->second->setValueFromBinding();
	}
}