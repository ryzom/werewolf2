/**
 * \file CAnimLayer.cpp
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
#include "entities/CAnimLayer.h"
#include "entities/CEntityMedia.h"
#include "CClientSimulation.h"

//
// Namespaces
//

namespace WWCLIENT {

CAnimLayer::CAnimLayer() 
	: m_EMD(NULL), m_Playing(NULL), m_CurrentSlotIndex(0), m_HasSlots(false) {
		;
}

CAnimLayer::CAnimLayer(NLGEORGES::UFormElm* root)
	: m_EMD(NULL), m_Playing(NULL), m_CurrentSlotIndex(0), m_HasSlots(false) {

	root->getValueByName(AnimLayerName,".AnimLayerName");

	NLGEORGES::UFormElm *animGroups;
	bool res=root->getNodeByName(&animGroups, ".AnimationGroups");
	if(res==false || animGroups==NULL) {
		nlwarning("Failed to load any animation groups for layer %s!", AnimLayerName.c_str());
		return;
	}

	uint numGroups;
	animGroups->getArraySize(numGroups);
	for(uint idx=0 ; idx<numGroups ; idx++) {
		// get the struct in the array.		
		NLGEORGES::UFormElm *group;
		animGroups->getArrayNode(&group, idx);

		CAnimGroup agroup(group);
		m_AnimationGroups[agroup.AnimGroupName] = agroup;
		nlinfo("Got animation group %s in layer %s", agroup.AnimGroupName.c_str(), AnimLayerName.c_str());
	}

}

void CAnimLayer::serial(NLMISC::IStream &f) {
	f.serial(AnimLayerName);
	f.serialCont(m_AnimationGroups);
}

void CAnimLayer::play(std::string group) {
	if(!m_EMD)
		return;

	if(!m_HasSlots) {
		if(!m_EMD->getSlotManager())
			return;
		if(m_EMD->getSlotManager()->hasVacancy()) {
			m_Slots[0] = m_EMD->getSlotManager()->get();
			m_Slots[1] = m_Slots[0] + 1;
			m_HasSlots = true;
		} else
			return;
	}

	if(m_Playing) {
		if(group == m_Playing->AnimGroupName)
			return;
		m_Playing->Playing = false;
	}

	CAnimLayer::animGroupMap::iterator iter = m_AnimationGroups.find(group);
	if(iter != m_AnimationGroups.end()) {
		if(!iter->second.getEMD())
			iter->second.setEMD(m_EMD);

		float transition = 0.25f;

		uint8 oldSlot = m_Slots[m_CurrentSlotIndex];
		m_CurrentSlotIndex = 1 - m_CurrentSlotIndex;
		uint8 newSlot = m_Slots[m_CurrentSlotIndex];

		iter->second.play(newSlot);

		NL3D::TAnimationTime currentTime = (NL3D::TAnimationTime)getSimulation()->adjustedTime();
		NL3D::TGlobalAnimationTime OldStartTime, OldEndTime;
		NL3D::TAnimationTime NewStartTime, NewEndTime;
		m_EMD->PlayList->getStartWeight	(oldSlot, OldStartTime);
		NL3D::TAnimationTime dt = (NL3D::TAnimationTime)currentTime - OldStartTime;
		if (dt > transition)
			dt = transition;
		OldStartTime = currentTime - (transition - dt);
		OldEndTime = currentTime + dt;
		NewStartTime = currentTime;
		NewEndTime = currentTime + dt;
		m_EMD->PlayList->setStartWeight	(oldSlot, 1.0f, OldStartTime);
		m_EMD->PlayList->setEndWeight	(oldSlot, 0.0f, OldEndTime);
		m_EMD->PlayList->setStartWeight	(newSlot, 0.0f, NewStartTime);
		m_EMD->PlayList->setEndWeight	(newSlot, 1.0f, OldEndTime);

		m_Playing = &(iter->second);
		m_Playing->Playing = true;
	}
}

CEntityMedia* CAnimLayer::getEMD() {
	return m_EMD;
}

void CAnimLayer::setEMD(CEntityMedia* emd) {
	m_EMD = emd;
}

CAnimGroup* CAnimLayer::getAnimGroup(std::string& name) {
	if(m_AnimationGroups.find(name) != m_AnimationGroups.end()) return &m_AnimationGroups[name];
	return NULL;
}

void CAnimLayer::update() {
	if(isDirty()) {
		updateControllers();
		if(m_Playing)
			m_Playing->update();
		setClean();
	}
}

void CAnimLayer::updateAnimations() {
	CAnimLayer::animGroupMap::iterator iter = m_AnimationGroups.begin();
	for( ; iter != m_AnimationGroups.end(); ++iter ) {
		iter->second.update();
	}
}

}; // END NAMESPACE WWCLIENT
