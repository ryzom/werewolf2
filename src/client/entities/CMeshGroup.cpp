/**
 * \file CMeshGroup.cpp
 * \date February 2006
 * \author Henri Kuuste
 */

/* Copyright, 2006 Werewolf
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

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "entities/CMeshGroup.h"
#include "entities/CEntityMedia.h"
#include "tasks/C3DTask.h"
#include "tasks/CResourceTask.h"

//
// Namespaces
//

namespace WWCLIENT {

/**************************************
* CMeshGroup::CMesh                  *
**************************************/

CMeshGroup::CMesh::CMesh() : skeleton(NULL), m_EMD(NULL) {
	;
}

CMeshGroup::CMesh::CMesh(NLGEORGES::UFormElm *root) : skeleton(NULL), m_EMD(NULL) {
	root->getValueByName(FriendlyName,".FriendlyName");
	root->getValueByName(MeshName,".MeshName");
	root->getValueByName(SkeletonName,".SkeletonName");
	nlinfo("Got mesh %s -> %s", FriendlyName.c_str(), MeshName.c_str());
}

CMeshGroup::CMesh::~CMesh() {
	if(hasInstance()) {
		C3DTask::instance().scene().deleteInstance(m_instance);
	}
}

void CMeshGroup::CMesh::serial(NLMISC::IStream &f) {
	f.serial(FriendlyName);
	f.serial(MeshName);
	f.serial(SkeletonName);
}

bool CMeshGroup::CMesh::hasInstance() {
	return !m_instance.empty();
}

NL3D::UInstance* CMeshGroup::CMesh::getInstance() {
	if(m_EMD == NULL)
		return NULL;
	if(!hasInstance()) {
		m_instance = C3DTask::instance().scene().createInstance(
			CResourceTask::instance().getMesh(MeshName));
		if(hasInstance()) {
			if(hasSkeleton()) {
				skeleton->skeleton.bindSkin(m_instance);
			} else {
				m_instance.parent(*(m_EMD->getTransform()));
				m_instance.enableCastShadowMap(true);
			}
			if(m_EMD->PlayList) {
				m_EMD->PlayList->registerTransform(m_instance);
			}
		}
	}
	if(hasInstance()) {
		return &m_instance;
	}
	return NULL;
}

bool CMeshGroup::CMesh::hasSkeleton() {
	return skeleton != NULL && skeleton->hasSkeleton();
}

NL3D::USkeleton* CMeshGroup::CMesh::getSkeleton() {
	if(m_EMD == NULL) {
		return NULL;
	}
	// init skeleton if it does not exist
	if(skeleton == NULL && !SkeletonName.empty()) {
		skeleton = m_EMD->getSkeleton(SkeletonName);
	}
	if(skeleton != NULL) {
		if(!skeleton->hasSkeleton()) {
			skeleton->skeleton.cast(C3DTask::instance().scene().createSkeleton(CResourceTask::instance().get(skeleton->Filename)));
			if(skeleton->hasSkeleton()) {
				skeleton->skeleton.enableCastShadowMap(true);
				skeleton->skeleton.parent(*(m_EMD->getTransform()));
				if(m_EMD->PlayList) {
					m_EMD->PlayList->registerTransform(skeleton->skeleton);
				}
			}
		}
		if(skeleton->hasSkeleton()) {
			return &(skeleton->skeleton);
		}
	}
	return NULL;
}

void CMeshGroup::CMesh::hide() {
	if(hasInstance()) {
		getInstance()->hide();
		if(skeleton != NULL && skeleton->hasSkeleton()) {
			skeleton->skeleton.hide();
		}
	}
}

bool CMeshGroup::CMesh::show() {
	getInstance();
	if(hasInstance()) {
		m_instance.show();
		if(skeleton != NULL && skeleton->hasSkeleton()) {
			skeleton->skeleton.show();
		}
		setDirty();
		return true;
	}
	return false;
}

CEntityMedia* CMeshGroup::CMesh::getEMD() {
	return m_EMD;
}

void CMeshGroup::CMesh::setEMD(CEntityMedia* emd) {
	m_EMD = emd;
}

void CMeshGroup::CMesh::update() {
	if(hasInstance()) {
		if(isDirty()) {
			updateControllers();
			setClean();
		}
	}
}

/**************************************
* CMeshGroup                         *
**************************************/

CMeshGroup::CMeshGroup() : m_displayedMesh(NULL), m_EMD(NULL) {
	;
}

CMeshGroup::CMeshGroup(NLGEORGES::UFormElm *root) : m_displayedMesh(NULL), m_EMD(NULL) {
	// get the descriptive values.
	root->getValueByName(MeshGroupName,".MeshGroupName");

	NLGEORGES::UFormElm *meshes;
	bool res=root->getNodeByName(&meshes, ".Meshes");
	if(res==false || meshes==NULL) {
		nlwarning("Failed to load any meshes for group %s!",MeshGroupName.c_str());
		return;
	}

	uint numMeshes;
	meshes->getArraySize(numMeshes);
	for(uint idx=0 ; idx<numMeshes ; idx++) {
		// get the struct in the array.		
		NLGEORGES::UFormElm *mesh;
		meshes->getArrayNode(&mesh, idx);

		CMeshGroup::CMesh tmesh(mesh);
		addMesh(tmesh);
	}
}

void CMeshGroup::addMesh(CMeshGroup::CMesh& mesh) {
	m_Meshes[mesh.FriendlyName] = mesh;
	makeInputConnection(&mesh);
}

void CMeshGroup::serial(NLMISC::IStream &f) {
	f.serial(MeshGroupName);
	f.serialCont(m_Meshes);
}

bool CMeshGroup::isVisible() {
	return (m_displayedMesh != NULL);
}

CMeshGroup::CMesh* CMeshGroup::getMesh() {
	return m_displayedMesh;
}

void CMeshGroup::setMesh(CMesh* mesh) {
	m_displayedMesh = mesh;
}

NL3D::UTransform* CMeshGroup::getTransform() {
	if(getMesh() == NULL) {
		return NULL;
	}
	if(getMesh()->hasSkeleton()) {
		return getMesh()->getSkeleton();
	}
	if(getMesh()->hasInstance()) {
		return getMesh()->getInstance();
	}
	return NULL;
}

void CMeshGroup::showMesh(std::string mesh) {

	// hide currently displayed mesh
	if(getMesh() != NULL) {
		getMesh()->hide();
	}

	// if no mesh name given
	if(mesh.empty()) {
		setMesh(NULL);
		return;
	}

	// find the new mesh
	CMeshGroup::meshMap::iterator mIter = m_Meshes.find(mesh);
	if(mIter == m_Meshes.end()) {
		setMesh(NULL);
		return;
	}

	setMesh(&(mIter->second));

	if(getMesh()->getEMD() == NULL)
		getMesh()->setEMD(getEMD());

	getMesh()->getSkeleton();

	// show the mesh
	if(!getMesh()->show())
		setMesh(NULL);

	setDirty();
}

void CMeshGroup::hideMesh() {
	setMesh(NULL);
}

void CMeshGroup::hide() {
	if(getMesh() != NULL && getMesh()->hasInstance()) {
		getMesh()->getInstance()->hide();
	}
}

void CMeshGroup::show() {
	if(getMesh() != NULL && getMesh()->hasInstance()) {
		getMesh()->getInstance()->show();
	}
}

CEntityMedia* CMeshGroup::getEMD() {
	return m_EMD;
}

void CMeshGroup::setEMD(CEntityMedia* emd) {
	m_EMD = emd;
}

CMeshGroup::CMesh* CMeshGroup::getMesh(std::string& name) {
	if(m_Meshes.find(name) != m_Meshes.end()) return &m_Meshes[name];
	return NULL;
}

void CMeshGroup::update() {
	if(isVisible()) {
		if(isDirty()) {
			updateControllers();
			getMesh()->update();
			setClean();
		}
	}
}

void CMeshGroup::updateMeshes() {
	CMeshGroup::meshMap::iterator iter = m_Meshes.begin();
	for( ; iter != m_Meshes.end(); ++iter) {
		iter->second.update();
	}
}

}; // END NAMESPACE WWCLIENT
