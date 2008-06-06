#include "stdafx.h"
#include "Mesh.h"
#include "EMD.h"

Mesh::~Mesh() {
	Mesh::mediaControllerMap::iterator iter = m_Controllers.begin();
	for( ; iter != m_Controllers.end(); ++iter ) {
		delete iter->second;
	}
	m_Controllers.clear();
	delete m_Name;
}

void Mesh::update() {
	if(m_Visible) {
		if(isDirty()) {
			printf("Updating %s\n", getMeshName());
			Mesh::mediaControllerMap::iterator iter = m_Controllers.begin();
			for( ; iter != m_Controllers.end(); ++iter ) {
				iter->second->update();
			}
			setClean();
		}
	}
}

void Mesh::addController(IMediaController* controller) {
	m_Controllers[controller->getControllerName()] = controller;
	makeInputConnection(controller);
	controller->init();
	setDirty();
}

void Mesh::removeController(const char* name) {
	Mesh::mediaControllerMap::iterator iter = m_Controllers.find(name);
	if(iter == m_Controllers.end())
		return;
	delete iter->second;
	m_Controllers.erase(iter);
	setDirty();
}

void Mesh::init_script_class() {
	using namespace boost::python::api;
	using namespace boost::python;
	class_<Mesh, bases<WWPROPERTY::IDirtyPropagator> > w("Mesh", no_init);
	w.def("update", &Mesh::update);
	w.add_property("name", &Mesh::getMeshName);
	w.add_property("visible", &Mesh::isVisible);
	w.def("show", &Mesh::show);
	w.def("hide", &Mesh::hide);
	w.def("getEMD", &Mesh::getEMD, return_value_policy<reference_existing_object>());
	w.add_property("emd", w.attr("getEMD"));
	w.def("morph", &Mesh::morph);
}
