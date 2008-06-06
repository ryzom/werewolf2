#include "stdafx.h"
#include "EMD.h"
#include "SOB.h"

EMD::~EMD() {
	EMD::meshGroupMap::iterator iter = m_MeshGroups.begin();
	for( ; iter != m_MeshGroups.end(); ++iter ) {
		delete iter->second;
	}
	m_MeshGroups.clear();

	EMD::mediaControllerMap::iterator iter2 = m_Controllers.begin();
	for( ; iter2 != m_Controllers.end(); ++iter2) {
		delete iter2->second;
	}
	m_Controllers.clear();
}

void EMD::update() {
	if(isDirty()) {
		EMD::meshGroupMap::iterator iter = m_MeshGroups.begin();
		for( ; iter != m_MeshGroups.end(); ++iter ) {
			iter->second->update();
		}

		EMD::mediaControllerMap::iterator iter2 = m_Controllers.begin();
		for( ; iter2 != m_Controllers.end(); ++iter2) {
			iter2->second->update();
		}
		setClean();
	}
}

void EMD::showMesh(const char* group, const char* mesh) {
	EMD::meshGroupMap::iterator iter = m_MeshGroups.find(group);
	if(iter == m_MeshGroups.end())
		return;
	iter->second->show();
	iter->second->showMesh(mesh);
	printf("Showing mesh %s.%s\n", group, mesh);
	setDirty();
}

void EMD::hideMesh(const char* group, const char* mesh) {
	EMD::meshGroupMap::iterator iter = m_MeshGroups.find(group);
	if(iter == m_MeshGroups.end())
		return;
	iter->second->hide();
	iter->second->hideMesh(mesh);
	printf("Hiding mesh %s.%s\n", group, mesh);
}

void EMD::addMeshGroup(MeshGroup* meshGrp) {
	m_MeshGroups[meshGrp->getMeshGroupName()] = meshGrp;
	makeInputConnection(meshGrp);
	setDirty();
}

void EMD::removeMeshGroup(const char* name) {
	EMD::meshGroupMap::iterator iter = m_MeshGroups.find(name);
	if(iter == m_MeshGroups.end())
		return;
	delete iter->second;
	m_MeshGroups.erase(iter);
	setDirty();
}

void EMD::addController(IMediaController* controller) {
	m_Controllers[controller->getControllerName()] = controller;
	makeInputConnection(controller);
	controller->init();
	setDirty();
}

void EMD::removeController(const char* name) {
	EMD::mediaControllerMap::iterator iter = m_Controllers.find(name);
	if(iter == m_Controllers.end())
		return;
	delete iter->second;
	m_Controllers.erase(iter);
	setDirty();
}

void EMD::init_script_class() {
	using namespace boost::python::api;
	using namespace boost::python;
	class_<EMD, bases<WWPROPERTY::IDirtyPropagator> > w("EMD", no_init);
	w.def("update", &EMD::update);
	w.def("showMesh", &EMD::showMesh);
	w.def("hideMesh", &EMD::hideMesh);
	w.def("getOwner", &EMD::getOwner, return_value_policy<reference_existing_object>());
	w.add_property("owner", w.attr("getOwner"));
}
