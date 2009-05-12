/**
 * \file CMotionController.cpp
 * \date March 2006
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
#include <nel/misc/vectord.h>
#include <nel/misc/common.h> // clamp() in here
#include <nel/misc/hierarchical_timer.h>

//
// Werewolf Includes
//	
#include "wwcommon/CMotionController.h"
#include "wwcommon/IBaseSimulation.h"
#include "wwcommon/CClientMotionHandler.h"
#include "wwcommon/general.h"
#include "wwcommon/CGameEventServer.h"
#include "wwcommon/CStateManager.h"

//
// Namespaces
//

namespace WWCOMMON {

const char* CMotionController::m_Name = "motion";

#define ODE_STEP_DT 0.0001 /// 100ns

CMotionController::CMotionController(CPerformer* subject)
	: m_CorrectionDt(0), m_Subject(subject), m_VCE(NULL), m_VCM(NULL), Duration(0) {
}

CMotionController::~CMotionController() {
	if(m_VCE && m_VCM) {
		m_VCM->deleteEntity(m_VCE);
	}
}

void CMotionController::setVisualCollisionEntity(NL3D::UVisualCollisionManager* vcm) {
	if(!vcm)
		return;
	m_Subject->getMovePrimitive()->setDontSnapToGround(true);
	m_VCM = vcm;
	m_VCE = vcm->createEntity();
}

void CMotionController::update() {
	move(::getSimulation()->deltaTime() - m_CorrectionDt, false);
	m_CorrectionDt = 0;
	setKey();
}

void CMotionController::update(double dt) {
	move(dt, true);
	setKey();
	m_CorrectionDt = dt;
}

const char* CMotionController::getControllerName() {
	return m_Name;
}

void CMotionController::ODE(float dt, float& yaw, float& vYaw, NLMISC::CVector& pos, NLMISC::CVector& sPos, NLMISC::CVector& vPos) {
	H_AUTO(CMotionController_ODE);
	// TODO henri:henri get rid of hardcoded acceleration/decceleration values
	ISimulationState* orientState = m_Subject->getState(CStateFactory::ST_ORIENTATION);
	ISimulationState* moveState = m_Subject->getState(CStateFactory::ST_MOVEMENT);
	NLMISC::CMatrix rotMat;
	rotMat.rotateZ(yaw);
	float t = 0;
	for(; t < dt; t += (float)ODE_STEP_DT) {
		ODEStep((float)ODE_STEP_DT, yaw, vYaw, pos, sPos, vPos, moveState, orientState, rotMat);
	}

	t = dt - t;
	ODEStep(t, yaw, vYaw, pos, sPos, vPos, moveState, orientState, rotMat);
}

void CMotionController::ODEStep(float dt, float& yaw, float& vYaw, NLMISC::CVector& pos, NLMISC::CVector& sPos, NLMISC::CVector& vPos, ISimulationState* moveState, ISimulationState* orientState, NLMISC::CMatrix &rotMat) {
	H_AUTO(CMotionController_ODEStep);
	float dYaw = (float)dt*vYaw;
	yaw += dYaw;
	if(orientState == NULL || orientState->getStateId() == CStateFactory::ST_OR_IDLE) {
		if(vYaw != 0) {
			vYaw += (float)dt*-vYaw*40;
		}
	} else {
		vYaw += (float)dt*m_Subject->getRotAcceleration();
	}
	NLMISC::clamp(vYaw, -(m_Subject->getMaxRotSpeed()), m_Subject->getMaxRotSpeed());
	if(moveState != NULL && moveState->getStateId() != CStateFactory::ST_MV_IDLE) {
		if(dYaw != 0) {
			rotMat.rotateZ(dYaw);
			vPos = rotMat.mulVector(sPos);
		}
		pos += vPos*(float)dt;
	}
}

#define PACS_ERROR_TOLERANCE_PER_S 10

void CMotionController::move(double dt, bool correction) {
	H_AUTO(CMotionController_Move);
	if(dt < 0)
		nlinfo("ATTENTION!!!!! Moving with negative dt %.6f", dt);
	Duration += dt;

	CPerformer::CPerformerInterpKey* key = m_Subject->getCurrentKey();
	float elapsed = (float)dt;

	float vYaw = key->getYawSpeed();
	float yaw = key->getYaw();
	NLMISC::CVector topos(key->getPosition());
	NLMISC::CVector pacsPos(m_Subject->getMovePrimitive()->getFinalPosition(0));
	// TODO henri:henri figure out some logic that thinks if it should use topos or pacsPos
/*
	if(!correction) {
		NLMISC::CVector diff(pacsPos - topos);
		diff.z = 0;
		if(diff.norm() < (PACS_ERROR_TOLERANCE_PER_S*getSimulation()->deltaTime())) {
			m_Subject->setGlobalPosition(topos);
			pacsPos = topos;
		} else {
			nlinfo("USING PACS!!");
			topos = pacsPos;
		}
	}
*/

	NLMISC::CVector veloc(key->getVelocity());
	NLMISC::CVector speed(key->getSpeed());

	if(m_Subject->useCorrection()) {
		if(m_Subject->getCorrectionInterp()->getNumKeys() > 1) {
			CPerformer::CorrectInterpKey& ckey = m_Subject->getCorrectionInterp()->getLastKey();
	//		if((ckey.getTime() < (key->getTime()+elapsed)) && (ckey.getTime() > key->getTime())) {
			if(!ckey.isHandled() && (ckey.getTime() < (key->getTime()+elapsed))) {
	//			nlinfo("ATTENTION: Doing DR correction at %.6f %.6f", ckey.getTime(), key->getTime()+elapsed);
				vYaw = ckey.getYawVelocity();
				yaw = ckey.getYaw();
				topos = ckey.getPosition();
				elapsed = key->getTime() + elapsed - ckey.getTime();
				pacsPos = ckey.getPosition();
				m_Subject->setGlobalPosition(pacsPos);
				ckey.setHandled(true);
			}
		}
	}
	ODE(elapsed, yaw, vYaw, topos, speed, veloc);

	topos = snapToGround(topos);
/*
	if(!correction) {
		NLMISC::CVector vPos = (topos - pacsPos)/getSimulation()->deltaTime();
		m_Subject->getMovePrimitive()->move(vPos, 0);
	}
*/
	key->setTime(getSimulation()->adjustedTime());
	key->setYawSpeed(vYaw);
	key->setYaw(yaw);
	key->setSpeed(speed);
	key->setPosition(topos);
}

NLMISC::CVector CMotionController::snapToGround(NLMISC::CVector pos) {
	if(m_VCE != NULL) {
		NLPACS::UGlobalPosition	gPos = ::getSimulation()->getGlobalRetriever().retrievePosition(pos);
		gPos.LocalPosition.Estimation.z = 0.0f;
		pos.z = ::getSimulation()->getGlobalRetriever().getMeanHeight(gPos);
		if (!::getSimulation()->getGlobalRetriever().isInterior(gPos)) {
			m_VCE->snapToGround(pos);
		}
	}
	return pos;
}

void CMotionController::setKey() {
	CPerformer::CPerformerInterpKey* key = m_Subject->getCurrentKey();
	m_Subject->getInterpolator()->addKey(*key);
}

}; // END OF NAMESPACE WWCOMMON
