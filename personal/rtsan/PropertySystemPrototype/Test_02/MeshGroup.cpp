#include "stdafx.h"
#include "MeshGroup.h"
#include "EMD.h"

MeshGroup::~MeshGroup() {
	MeshGroup::meshMap::iterator iter = m_Meshes.begin();
	for( ; iter != m_Meshes.end(); ++iter ) {
		delete iter->second;
	}
	m_Meshes.clear();

	MeshGroup::mediaControllerMap::iterator iter2 = m_Controllers.begin();
	for( ; iter2 != m_Controllers.end(); ++iter2 ) {
		delete iter2->second;
	}
	m_Controllers.clear();

	delete m_Name;
}

void MeshGroup::update() {
	if(isVisible()) {
		if(isDirty()) {
			printf("Updating %s\n", getMeshGroupName());
			MeshGroup::meshMap::iterator iter = m_Meshes.begin();
			for( ; iter != m_Meshes.end(); ++iter ) {
				iter->second->update();
			}

			MeshGroup::mediaControllerMap::iterator iter2 = m_Controllers.begin();
			for( ; iter2 != m_Controllers.end(); ++iter2 ) {
				iter2->second->update();
			}
			setClean();
		}
	}
}

void MeshGroup::showMesh(const char* mesh) {
	MeshGroup::meshMap::iterator iter = m_Meshes.find(mesh);
	if(iter == m_Meshes.end())
		return;
	iter->second->show();
	setDirty();
}

void MeshGroup::hideMesh(const char* mesh) {
	MeshGroup::meshMap::iterator iter = m_Meshes.find(mesh);
	if(iter == m_Meshes.end())
		return;
	iter->second->hide();
}

void MeshGroup::addMesh(Mesh* mesh) {
	m_Meshes[mesh->getMeshName()] = mesh;
	makeInputConnection(mesh);
	setDirty();
}

void MeshGroup::removeMesh(const char* name) {
	MeshGroup::meshMap::iterator iter = m_Meshes.find(name);
	if(iter == m_Meshes.end())
		return;
	delete iter->second;
	m_Meshes.erase(iter);
	setDirty();
}

void MeshGroup::addController(IMediaController* controller) {
	m_Controllers[controller->getControllerName()] = controller;
	makeInputConnection(controller);
	controller->init();
	setDirty();
}

void MeshGroup::removeController(const char* name) {
	MeshGroup::mediaControllerMap::iterator iter = m_Controllers.find(name);
	if(iter == m_Controllers.end())
		return;
	delete iter->second;
	m_Controllers.erase(iter);
	setDirty();
}

void MeshGroup::init_script_class() {
	using namespace boost::python::api;
	using namespace boost::python;
	class_<MeshGroup, bases<WWPROPERTY::IDirtyPropagator> > w("MeshGroup", no_init);
	w.def("update", &MeshGroup::update);
	w.add_property("visible", &MeshGroup::isVisible);
	w.add_property("name", &MeshGroup::getMeshGroupName);
	w.def("show", &MeshGroup::show);
	w.def("hide", &MeshGroup::hide);
	w.def("showMesh", &MeshGroup::showMesh);
	w.def("hideMesh", &MeshGroup::hideMesh);
	w.def("getEMD", &MeshGroup::getEMD, return_value_policy<reference_existing_object>());
	w.add_property("emd", w.attr("getEMD"));
}