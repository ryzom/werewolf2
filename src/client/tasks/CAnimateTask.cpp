/**
 * \file CAnimateTask.cpp
 * \date May 2004
 * \author Matt Raykowski
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
// System Includes
//
#include "stdpch.h"

//
// NeL Includes
//
#include <nel/misc/config_file.h>

#include <nel/3d/u_animation_set.h>

//
// Werewolf Includes
//
#include "tasks/CAnimateTask.h"
#include "tasks/CConfigTask.h"
#include "tasks/C3DTask.h"
#include "CClientSimulation.h"

#include "CAnimationClip.h"

//
// Namespaces
//

namespace WWCLIENT {

void CAnimateTask::init() {
	// create the animation set.
	m_AnimationSet=C3DTask::instance().driver().createAnimationSet();

	// clip container.
	std::map<std::string, CAnimationClip> container;

	// load the clips in from the packed sheet loader.
	::loadForm("animation", "data/animationclips.packed_sheets", container, true, false);
	std::map<std::string, CAnimationClip>::iterator iter(container.begin());
	for(; iter != container.end(); ++iter) {
		iter->second.ID=m_AnimationSet->addAnimation(iter->second.Filename.c_str(),iter->second.FriendlyName.c_str());
		iter->second.Animation=m_AnimationSet->getAnimation(iter->second.ID);
		m_AnimationClips[iter->second.FriendlyName] = iter->second;
	}

	// build the set, note to make sure not to do this if optimizing the set.
	m_AnimationSet->build();

	// create the play list manager
	m_PlayListManager=C3DTask::instance().scene().createPlayListManager();
}

void CAnimateTask::update() {
	m_PlayListManager->animate((NL3D::TAnimationTime)getSimulation()->adjustedTime());;
}

void CAnimateTask::render() {
	;
}

void CAnimateTask::release() {
	// release the aniamtion set
	C3DTask::instance().driver().deleteAnimationSet(m_AnimationSet);
	// release the play list manager
	C3DTask::instance().scene().deletePlayListManager(m_PlayListManager);
}

std::string CAnimateTask::name() {
	return "CAnimateTask";
}

NL3D::UPlayListManager* CAnimateTask::getPlayListManager() {
	return m_PlayListManager;
}

NL3D::UAnimationSet* CAnimateTask::getAnimationSet() {
	return m_AnimationSet;
}

CAnimationClip* CAnimateTask::getClip(std::string name) {
	CAnimateTask::clipMap::iterator iter = m_AnimationClips.find(name);
	if(iter != m_AnimationClips.end())
		return &(iter->second);
	return NULL;
}

}; // END NAMESPACE WWCLIENT
