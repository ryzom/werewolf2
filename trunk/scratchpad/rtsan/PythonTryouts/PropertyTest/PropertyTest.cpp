// PropertyTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <boost/python.hpp>
#include <pythonrun.h>
#include <iostream>
#include <string>
#include <conio.h>
#include "ISingleton.h"
#include <wwproperty/IDirtyPropagator.h>
#include <wwproperty/IProperty.h>
#include <wwproperty/CBaseProperty.h>
#include <hash_map>
#include <vector>

using namespace boost::python::api;
using namespace boost::python;

class Console : public ISingleton<Console> {
public:
	Console() {
		Py_Initialize();
	}
	~Console() {
		delete main_module;
		Py_Finalize();
	}

	void configure() {
		main_module = new object((
	    handle<>(borrowed(PyImport_AddModule("__main__")))));
		main_namespace = main_module->attr("__dict__");
		namespace_dict = extract<dict>(main_namespace);
	}

	bool update() {
		std::string input;
		input.resize(256);

		std::cout << "> ";
		std::cin.getline(&input[0], 256);

		// Trim unused characters
		input.resize(strlen(input.c_str()));

		// Interpret the command
		if( input == "quit" )
			return false;
		else if( input == "exit" )
			return false;
		else
			execCommand(input);

		return true;
	}

	void execCommand(std::string &s) {
		execCommand(s.c_str());
	}

	void execCommand(const char* s) {
		try {
			object result((handle<>(PyRun_String(s,
				Py_single_input,
				main_namespace.ptr(),
				main_namespace.ptr()))));

		} catch(error_already_set) {
			PyErr_Print();
		}
	}

	object evalCommand(const char* s) {
		try {
			object result((handle<>(PyRun_String(s,
				Py_eval_input,
				main_namespace.ptr(),
				main_namespace.ptr()))));
			return result;

		} catch(error_already_set) {
			PyErr_Print();
		}
		return object();
	}

	object* main_module;
	object main_namespace;
	dict namespace_dict;
};

struct streqpred
{
   bool operator()(const char* a, const char* b) const
   {
      return strcmp(a, b) == 0;
   }
};

class IDataContainer {
public:
	virtual ~IDataContainer() { ; }
	virtual const char* getContainerName() = 0;
};

class IDataContainerWrap : public IDataContainer, public wrapper<IDataContainer> {
	const char* getContainerName() {
		return call<const char*>(this->get_override("getContainerName").ptr());
	}
};

class IController {
public:
	virtual ~IController() { ; }
	virtual const char* getControllerName() = 0;
	virtual void update() = 0;
	virtual void init() = 0;
};

class IControllerWrap : public IController, public wrapper<IController> {
public:
	const char* getControllerName() { return call<const char*>(this->get_override("getControllerName").ptr()); }
	void update() { call<void>(this->get_override("update").ptr()); }
	void init() { call<void>(this->get_override("init").ptr()); }
};

class SOB {
public:
	~SOB() {
		dataMap::iterator iter = m_Data.begin();
		for( ; iter != m_Data.end(); ++iter) {
			delete iter->second;
		}
		m_Data.clear();

		controllerMap::iterator iter2 = m_Controllers.begin();
		for( ; iter2 != m_Controllers.end(); ++iter2) {
			delete iter2->second;
		}
		m_Controllers.clear();
	}

	void update() {
		controllerMap::iterator iter = m_Controllers.begin();
		for( ; iter != m_Controllers.end(); ++iter) {
			iter->second->update();
		}
	}

	void addDataContainer(IDataContainer* data) {
		m_Data[data->getContainerName()] = data;
	}

	void removeDataContainer(const char* name) {
		dataMap::iterator iter = m_Data.find(name);
		if(iter == m_Data.end())
			return;
		delete iter->second;
		m_Data.erase(iter);
	}

	IDataContainer* getDataContainer(const char* name) {
		dataMap::iterator iter = m_Data.find(name);
		if(iter == m_Data.end())
			return NULL;
		return iter->second;
	}

	void addController(IController* controller) {
		m_Controllers[controller->getControllerName()] = controller;
		controller->init();
	}

	void removeController(const char* name) {
		controllerMap::iterator iter = m_Controllers.find(name);
		if(iter == m_Controllers.end())
			return;
		delete iter->second;
		m_Controllers.erase(iter);
	}

	IController* getController(const char* name) {
		controllerMap::iterator iter = m_Controllers.find(name);
		if(iter == m_Controllers.end())
			return NULL;
		return iter->second;
	}

	typedef std::hash_map<const char*, IDataContainer*, std::hash<const char*>, streqpred> dataMap;
	typedef std::hash_map<const char*, IController*, std::hash<const char*>, streqpred> controllerMap;
private:
	dataMap	m_Data;
	controllerMap m_Controllers;
};

class CharacterData : public IDataContainer {
public:
	CharacterData(SOB* sob) : m_Owner(sob) { setFat(10); setAge(25); }
	virtual ~CharacterData() { ; }

	const char* getContainerName() { return m_Name; }

	float getAge() { return m_Age.getValue(); }
	void setAge(float age) { m_Age.setValue(age); }

	float getFat() { return m_Fat.getValue(); }
	void setFat(float fat) { m_Fat.setValue(fat); printf("Fat value changed to %f\n", m_Fat.getValue()); }

	WWPROPERTY::IProperty *getFatProperty() { return &m_Fat; }
	WWPROPERTY::IProperty *getAgeProperty() { return &m_Age; }

	static void initScriptClass() {
		class_<CharacterData, bases<IDataContainer> > cdw("CharacterData", no_init);
		cdw.add_property("age", &CharacterData::getAge, &CharacterData::setAge);
		cdw.add_property("fat", &CharacterData::getFat, &CharacterData::setFat);
		cdw.def("getFatProperty", &CharacterData::getFatProperty, return_value_policy<reference_existing_object>());
		cdw.def("getAgeProperty", &CharacterData::getAgeProperty, return_value_policy<reference_existing_object>());
		cdw.add_property("fatProperty", cdw.attr("getFatProperty"));
		cdw.add_property("ageProperty", cdw.attr("getAgeProperty"));
	}

private:
	static const char* m_Name;
	
	SOB* m_Owner;
	WWPROPERTY::CBaseProperty<float> m_Fat;
	WWPROPERTY::CBaseProperty<float> m_Age;
};

const char* CharacterData::m_Name = "character";

class MorphController : public IController, public WWPROPERTY::IDirtyPropagator {
public:
	MorphController(const char* name, SOB* sob, const char* initScript, const char* script) : m_Name(name), m_Owner(sob), m_Script(script), m_InitScript(initScript) { 
	}

	virtual ~MorphController() { 
		printf("Destruction of morph controller %s\n", m_Name);
	}
	
	void init() {
		Console::instance().execCommand(m_InitScript);
		object func = Console::instance().evalCommand("doStuff");
		try {
			call<void>(func.ptr(), boost::ref(this));
		} catch(error_already_set)  {
			PyErr_Print();
		}
	}
	
	void update() {
		if(isDirty()) {
			Console::instance().execCommand(m_Script);
			object func = Console::instance().evalCommand("doStuff");
			call<void>(func.ptr(), boost::ref(this));
//			Console::instance().namespace_dict["ctrl"] = this;
			setClean();
//			Console::instance().namespace_dict.
		}
	}

	void morph(float f) {
		printf("Morphing %s to %f\n", m_Name, f);
	}

	SOB* getOwner() { return m_Owner; }
	const char* getControllerName() { return m_Name; }

	static void initScriptClass() {
		class_<MorphController, bases<IController, WWPROPERTY::IDirtyPropagator> > mcw("MorphController", no_init);
		mcw.def("getOwner", &MorphController::getOwner, return_value_policy<reference_existing_object>());
		mcw.add_property("owner", mcw.attr("getOwner"));
		mcw.def("morph", &MorphController::morph);
	}

private:
	const char* m_Name;
	SOB* m_Owner;
	const char* m_Script;
	const char* m_InitScript;	
};


class Manager : public ISingleton<Manager> {
public:
	Manager() { ; }

	SOB* getSob(unsigned int i) {
		if(i < m_Sobs.size())
			return m_Sobs[i];
		return NULL;
	}

	void addSob(SOB* sob) {
		printf("Added sob %d\n", m_Sobs.size());
		m_Sobs.push_back(sob);
	}

private:
	std::vector<SOB*> m_Sobs;
};

void init_module_test() {
	class_<Manager> mw("Manager", no_init);
	mw.def("instance", &Manager::instance, return_value_policy<reference_existing_object>());
	mw.staticmethod("instance");
	mw.def("getSob", &Manager::getSob, return_value_policy<reference_existing_object>());

	class_<IDataContainerWrap, boost::noncopyable> dcw("IDataContainer", no_init);
	dcw.def("getContainerName", pure_virtual(&IDataContainer::getContainerName));
	dcw.add_property("name", dcw.attr("getContainerName"));
	
	class_<IControllerWrap, boost::noncopyable> cw("IController", no_init);
	cw.def("getControllerName", pure_virtual(&IController::getControllerName));
	cw.def("update", pure_virtual(&IController::update));
	cw.def("init", pure_virtual(&IController::init));
	cw.add_property("name", cw.attr("getControllerName"));

	class_<SOB> sw("SOB", no_init);
	sw.def("getDataContainer", &SOB::getDataContainer, return_value_policy<reference_existing_object>());
	sw.def("getController", &SOB::getController, return_value_policy<reference_existing_object>());
	sw.def("update", &SOB::update);

	WWPROPERTY::IDirtyPropagatorWrap::init_script_class();
	WWPROPERTY::IProperty::init_script_class();

	CharacterData::initScriptClass();
	MorphController::initScriptClass();
}

void inittest() { 
	boost::python::detail::init_module("test", &init_module_test);
	Console::instance().execCommand("import test");
}

int _tmain(int argc, _TCHAR* argv[]) {
	Console::instance().configure();
	inittest();

	SOB sob;
	CharacterData* data = new CharacterData(&sob);
	sob.addDataContainer(data);
	sob.addController(new MorphController("fatMorph", &sob,
		"def doStuff(ctrl):                              \n"
		"    c = ctrl.owner.getDataContainer('character')\n"
		"    if(c):                                      \n"
		"        ctrl.makeInputConnection(c.fatProperty) \n",
		"def doStuff(ctrl):                              \n"
		"    c = ctrl.owner.getDataContainer('character')\n"
		"    if(c):                                      \n"
		"        ctrl.morph((c.fat-10)*2)                \n"));

	Manager::instance().addSob(&sob);

	Console::instance().namespace_dict["sob"] = boost::ref(sob);
	Console::instance().namespace_dict["c"] = boost::ref(*data);

	while(Console::instance().update()) {
		sob.update();
	}
	return 0;
}