/**
* \file CEntityMedia.h
* \date May 2004
* \author Matt Raykowski
* \author Some code adapted from MTP-Target.
*/

/* Copyright, 2004 Werewolf
*
* This file is part of Werewolf.
* Werewolf is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2, or (at your option)
* any later version.

* Werewolf is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with Werewolf; see the file COPYING. If not, write to the
* Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
* MA 02111-1307, USA.
*/

//
// Standard Includes
//
#include "stdpch.h"

//
// System Includes
//
#include <set>

//
// NeL Includes
//
#include <nel/3d/u_play_list_manager.h>

//
// Werewolf Includes
//
#include "entities/CEntityMedia.h"
#include "tasks/C3DTask.h"
#include "tasks/CResourceTask.h"
#include "tasks/CAnimateTask.h"

//
// Namespaces
//

//
// Class References
//

//
// Class
//

namespace WWCLIENT {

CEntityMedia::CEntityMedia() : PlayList(NULL), m_SlotManager(NULL), m_Owner(NULL) {
}

CEntityMedia::~CEntityMedia() {
	if(PlayList != NULL) {
		CAnimateTask::instance().getPlayListManager()->deletePlayList(PlayList);
	}
	if(!m_Transform.empty()) {
		C3DTask::instance().scene().deleteTransform(m_Transform);
	}
	if(m_SlotManager) {
		delete m_SlotManager;
	}

	CEntityMedia::mediaControllerMap::iterator ctrlIter = m_Controllers.begin();
	for( ; ctrlIter != m_Controllers.end(); ++ctrlIter) {
		delete ctrlIter->second;
	}
	m_Controllers.clear();
}

void CEntityMedia::readGeorges(const NLMISC::CSmartPtr<NLGEORGES::UForm> &form, const std::string &sheetId) {
	NLGEORGES::UFormElm &root = form->getRootNode();

	// get the descriptive values.s
	root.getValueByName(Name,".Name");
	root.getValueByName(Description,".Description");
	root.getValueByName(EMT,".EMT");

	nlinfo("Loading EMD %s", Name.c_str());

	NLGEORGES::UFormElm *animLayers;
	bool res=root.getNodeByName(&animLayers, ".AnimationLayers");
	if(res==false || animLayers==NULL) {
		nlwarning("Failed to load any animation layers for EMD %s!",Name.c_str());
		return;
	}

	uint numGroups;
	animLayers->getArraySize(numGroups);
	for(uint idx=0 ; idx<numGroups ; idx++) {
		// get the struct in the array.		
		NLGEORGES::UFormElm *layer;
		animLayers->getArrayNode(&layer, idx);

		CAnimLayer alayer(layer);
		m_AnimationLayers[alayer.AnimLayerName] = alayer;
		nlinfo("Got animation layer %s", alayer.AnimLayerName.c_str());
	}

	NLGEORGES::UFormElm *meshGroups;
	res=root.getNodeByName(&meshGroups, ".MeshGroups");
	if(res==false || meshGroups==NULL) {
		nlwarning("Failed to load any mesh groups for EMD %s!",Name.c_str());
		return;
	}

	meshGroups->getArraySize(numGroups);
	for(uint idx=0 ; idx<numGroups ; idx++) {
		// get the struct in the array.		
		NLGEORGES::UFormElm *group;
		meshGroups->getArrayNode(&group, idx);

		CMeshGroup mgroup(group);
		m_MeshGroups[mgroup.MeshGroupName] = mgroup;
		nlinfo("Got mesh group %s", mgroup.MeshGroupName.c_str());
	}

	NLGEORGES::UFormElm *skeletons;
	res=root.getNodeByName(&skeletons, ".Skeletons");
	if(res==false || skeletons==NULL) {
		nlwarning("Failed to load any skeletons for EMD %s!",Name.c_str());
		return;
	}

	skeletons->getArraySize(numGroups);
	for(uint idx=0 ; idx<numGroups ; idx++) {
		// get the struct in the array.		
		NLGEORGES::UFormElm *skeleton;
		skeletons->getArrayNode(&skeleton, idx);

		CSkeleton skel(skeleton);
		m_Skeletons[skel.FriendlyName] = skel;
	}
}

void CEntityMedia::initInstance(WWCOMMON::ISimulationObj* sob) {
	PlayList = 	CAnimateTask::instance().getPlayListManager()->createPlayList(CAnimateTask::instance().getAnimationSet());
	m_Owner = sob;
}

void CEntityMedia::serial(NLMISC::IStream &f) {
	f.serial(Name);
	f.serial(Description);
	f.serial(EMT);
	f.serialCont(m_AnimationLayers);
	f.serialCont(m_MeshGroups);
	f.serialCont(m_Skeletons);
}

void CEntityMedia::showMesh(std::string group, std::string mesh) {
	CEntityMedia::meshGroupMap::iterator iter = m_MeshGroups.find(group);
	if(iter == m_MeshGroups.end())
		return;
	if(iter->second.getEMD() == NULL)
		iter->second.setEMD(this);
	iter->second.showMesh(mesh);
}

void CEntityMedia::hideMesh(std::string group) {
	CEntityMedia::meshGroupMap::iterator iter = m_MeshGroups.find(group);
	if(iter == m_MeshGroups.end())
		return;
	iter->second.hideMesh();
}

void CEntityMedia::transform(NLMISC::CMatrix& matrix) {
	if(!m_Transform.empty()) {
		m_Transform.setMatrix(matrix);
	}
}

NL3D::UTransform* CEntityMedia::getTransform() {
	if(m_Transform.empty()) {
		m_Transform = C3DTask::instance().scene().createTransform();
		m_Transform.setTransformMode(NL3D::UTransformable::DirectMatrix);
	}
	return &m_Transform;
}

void CEntityMedia::playAnimation(std::string layer, std::string group) {
	CEntityMedia::animLayerMap::iterator iter = m_AnimationLayers.find(layer);
	if(iter != m_AnimationLayers.end()) {
		if(!iter->second.getEMD())
			iter->second.setEMD(this);
		iter->second.play(group);
	}
}

CSequenceManager<uint8>* CEntityMedia::getSlotManager() {
	if(!m_SlotManager) {
		m_SlotManager = new CSequenceManager<uint8>(0, 255, 2);
	}
	return m_SlotManager;
}

void CEntityMedia::update() {
	if(isDirty()) {
		updateControllers();
		updateAnimations();
		updateMeshes();
		updateSkeletons();
		setClean();
	}
}

void CEntityMedia::updateAnimations() {
	CEntityMedia::animLayerMap::iterator iter = m_AnimationLayers.begin();
	for( ; iter != m_AnimationLayers.end(); ++iter) {
		iter->second.update();
	}
}

void CEntityMedia::updateMeshes() {
	CEntityMedia::meshGroupMap::iterator iter = m_MeshGroups.begin();
	for( ; iter != m_MeshGroups.end(); ++iter) {
		iter->second.update();
	}
}

void CEntityMedia::updateSkeletons() {
	CEntityMedia::skeletonMap::iterator iter = m_Skeletons.begin();
	for( ; iter != m_Skeletons.end(); ++iter) {
		iter->second.update();
	}
}

CAnimLayer* CEntityMedia::getAnimLayer(std::string& name) {
	if(m_AnimationLayers.find(name) != m_AnimationLayers.end()) return &m_AnimationLayers[name];
	return NULL;
}

CMeshGroup* CEntityMedia::getMeshGroup(std::string& name) {
	if(m_MeshGroups.find(name) != m_MeshGroups.end()) return &m_MeshGroups[name];
	return NULL;
}

CSkeleton* CEntityMedia::getSkeleton(std::string& name) {
	if(m_Skeletons.find(name) != m_Skeletons.end()) return &m_Skeletons[name];
	return NULL;
}

}; // END NAMESPACE WWCLIENT
