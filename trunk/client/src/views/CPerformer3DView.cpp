/**
 * \file CPerformer3DView.cpp
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
#include "views/CPerformer3DView.h"
#include <wwcommon/CMovementState.h>
#include <wwcommon/CStateManager.h>
#include <wwcommon/CGameEventServer.h>
#include <wwcommon/CSobMoveEvent.h>
#include "CClientSimulation.h"
#include "entities/CAnimationController.h"

//
// Namespaces
//

namespace WWCLIENT {

const char* CPerformer3DView::m_Name = "3D_EMD";

CPerformer3DView::CPerformer3DView(WWCOMMON::CPerformer* performer, CEntityMedia& emd) : 
	m_performer(performer), m_emd(emd) {
	m_emd.initInstance(performer);
	showMesh("body", "normal");
	showMesh("legs", "normal");
	showMesh("head", "normal");

	CAnimationController::AnimDesc idled("motion", "idle");
	CAnimationController::AnimDesc walkd("motion", "walk");
	CAnimationController::AnimationList mvList;
	mvList.push_back(CAnimationController::Animation(WWCOMMON::CStateFactory::ST_MV_IDLE, idled));
	mvList.push_back(CAnimationController::Animation(WWCOMMON::CStateFactory::ST_MV_BACKWARD, walkd));
	mvList.push_back(CAnimationController::Animation(WWCOMMON::CStateFactory::ST_MV_FORWARD, walkd));
	CAnimationController::AnimationLayer mvLayer(WWCOMMON::CStateFactory::ST_MOVEMENT, mvList);
	CAnimationController::AnimationLayerList animList;
	animList.push_back(mvLayer);
	CAnimationController *animCtrl = new CAnimationController("movementController", &m_emd, animList);
	m_emd.addController(animCtrl);

	WWCOMMON::CGameEventServer::instance().addListener(
		this, WWCOMMON::CSobMoveEvent::CSobMoveEventID, performer->getSobId(), WWCOMMON::CGameEventServer::POST_EVENT);
}

CPerformer3DView::~CPerformer3DView() {
}

void CPerformer3DView::render() {
}

void CPerformer3DView::update() {
	NLMISC::CMatrix matrix = m_performer->getInterpolator()->getSmoothKey().getMatrix();
	m_emd.transform(matrix);
	NLMISC::CVector pos = m_performer->getInterpolator()->getSmoothKey().getMatrix().getPos();
	m_emd.update();
//	nlinfo("Rendering sob %d at %f %f %f", m_performer->getSobId(), pos.x, pos.y, pos.z);
}

const char* CPerformer3DView::getViewName() {
	return m_Name;
}

bool CPerformer3DView::observePreGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	return true;
}

bool CPerformer3DView::observeGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	return true;
}

bool CPerformer3DView::observePostGameEvent(WWCOMMON::CGameEventServer::EventPtr event) {
	if(event->getId() == WWCOMMON::CSobMoveEvent::CSobMoveEventID) {
		if(m_performer->getState(WWCOMMON::CStateFactory::ST_MOVEMENT)->getStateId() == WWCOMMON::CStateFactory::ST_MV_BACKWARD) {
			showMesh("body", "armored");
		} else {
			showMesh("body", "normal");
		}
	}
	return true;
}

void CPerformer3DView::showMesh(std::string group, std::string mesh) {
	m_emd.showMesh(group, mesh);
}

void CPerformer3DView::hideMesh(std::string group) {
	m_emd.hideMesh(group);
}

void CPerformer3DView::playAnimation(std::string layer, std::string group) {
	m_emd.playAnimation(layer, group);
}

}; // END NAMESPACE WWCLIENT
