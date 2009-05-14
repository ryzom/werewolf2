/**
 * \file CAnimGroup.cpp
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
#include "entities/CAnimGroup.h"
#include "tasks/C3DTask.h"
#include "tasks/CResourceTask.h"
#include "entities/CEntityMedia.h"
#include "CSimulationImpl.h"
#include "tasks/CAnimateTask.h"
#include "selectors/CSelectorFactory.h"

//
// Namespaces
//

namespace WWCLIENT {

CAnimGroup::CAnimGroup() : m_Selector(NULL), m_EMD(NULL), Playing(false) {
	;
}

CAnimGroup::CAnimGroup(NLGEORGES::UFormElm *root) : m_Selector(NULL), m_EMD(NULL), Playing(false) {
	// get the descriptive values.
	root->getValueByName(AnimGroupName,".AnimGroupName");
	root->getValueByName(AnimCycles,".AnimCycles");
	uint8 wrap = 0;
	root->getValueByName(wrap,".WrapMode");
	WrapMode = (NL3D::UPlayList::TWrapMode)wrap;
	root->getValueByName(Selector,".Selector");

	// load the clips used by this group.
	NLGEORGES::UFormElm *clips;
	bool res=root->getNodeByName(&clips, ".AnimGroupClips");
	if(res==false || clips==NULL) {
		nlwarning("Failed to load any clips for group %s!",AnimGroupName.c_str());
		return;
	}

	// get how many clips are in the list.
	uint numClip;
	clips->getArraySize(numClip);
	for(uint idx=0 ; idx<numClip ; idx++) {
		// get the struct in the array.		
		NLGEORGES::UFormElm *clip;
		clips->getArrayNode(&clip, idx);

		CAnimGroup::CAnimClip aclip(clip);
		AnimClipList.push_back( aclip );
	}
}

CAnimGroup::~CAnimGroup() {
	if(m_Selector)
		delete m_Selector;
}

void CAnimGroup::serial(NLMISC::IStream &f) {
	f.serial(AnimGroupName);
	f.serialEnum(WrapMode);
	f.serial(AnimCycles);
	f.serial(Selector);
	f.serialCont(AnimClipList);
}

CAnimationClip* CAnimGroup::getClip() {
	if(m_Selector) {
		CAnimGroup::CAnimClip* clip = m_Selector->get(&AnimClipList);
		if(clip)
			return clip->getClip();
	}
	return NULL;
}

void CAnimGroup::play(uint8 slot) {
	if(!m_EMD)
		return;

	CAnimationClip* clip = getClip();
	if(!clip)
		return;

	m_EMD->PlayList->setAnimation(slot, clip->ID);
	m_EMD->PlayList->setTimeOrigin(slot, (NL3D::TAnimationTime)getSimulation()->adjustedTime());
	m_EMD->PlayList->setWeightSmoothness(slot, 1);
	m_EMD->PlayList->setWrapMode(slot, WrapMode);
}

CEntityMedia* CAnimGroup::getEMD() {
	return m_EMD;
}

void CAnimGroup::setEMD(CEntityMedia* emd) {
	m_EMD = emd;
	if(m_Selector == NULL && !Selector.empty()) {
		m_Selector = CSelectorFactory<CAnimGroup::CAnimClip>::instance().createSelector(Selector);
	}
}

void CAnimGroup::update() {
	if(isDirty()) {
		updateControllers();
		setClean();
	}
}

/**************************************
* CAnimGroup::CAnimClip               *
**************************************/

CAnimGroup::CAnimClip::CAnimClip() : m_Clip(NULL) {
	;
}

CAnimGroup::CAnimClip::CAnimClip(NLGEORGES::UFormElm *root) : m_Clip(NULL) {
	root->getValueByName(FriendlyClipName,".FriendlyAnimName");
	root->getValueByName(Priority,".Priority");
	nlinfo("Got an animation clip %s", FriendlyClipName.c_str());
}

void CAnimGroup::CAnimClip::serial(NLMISC::IStream &f) {
	f.serial(FriendlyClipName);
	f.serial(Priority);
}

CAnimationClip* CAnimGroup::CAnimClip::getClip() {
	if(m_Clip == NULL) {
		m_Clip = CAnimateTask::instance().getClip(FriendlyClipName);
	}

	return m_Clip;
}

unsigned int CAnimGroup::CAnimClip::getPriority() {
	return Priority;
}

}; // END NAMESPACE WWCLIENT
