// EventSystemTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CScriptConsole.h"
#include "CScriptEngine.h"
#include <wwcommon/CGameEventServer.h>
#include "ISource.h"
#include "CConstantSource.h"
#include "CPyFunctionSource.h"
#include "CEventTest.h"
#include <boost/python.hpp>

void reload() {
	CScriptEngine::instance().execScript("test.py");
}

BOOST_PYTHON_MODULE(ww) {
	ISourceWrap<float>::init_script_class("ISourceFloat");
	CConstantSource<float>::init_script_class("CConstantSourceFloat");
	CPyFunctionSource<float>::init_script_class("CPyFunctionSourceFloat");
	ISourceWrap<unsigned int>::init_script_class("ISourceUint");
	CConstantSource<unsigned int>::init_script_class("CConstantSourceUint");
	CPyFunctionSource<unsigned int>::init_script_class("CPyFunctionSourceUint");
	CEventTest::init_script_class("CEventTest");
	boost::python::def("reload", reload);
}

using namespace WWCOMMON;

int _tmain(int argc, _TCHAR* argv[]) {
	CScriptEngine::instance().init();
	if (PyImport_AppendInittab("ww", initww) == -1)
		throw std::runtime_error("Failed to add ww to the interpreter's builtin modules");
	CScriptEngine::instance().execCommand("import ww");
	reload();

	while(CScriptConsole::instance().update()) {
	}
	return 0;
}

