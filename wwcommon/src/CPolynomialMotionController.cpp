/**
 * \file CPolynomialMotionController.cpp
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
#include "wwcommon/CPolynomialMotionController.h"
#include "wwcommon/IBaseSimulation.h"
#include "wwcommon/general.h"
#include "wwcommon/CGameEventServer.h"
#include "wwcommon/CStateManager.h"

//
// Namespaces
//

namespace WWCOMMON {

const char* CPolynomialMotionController::m_Name = "polynomial motion";

CPolynomialMotionController::CPolynomialMotionController(CPerformer* subject)
	: m_Subject(subject), m_VCE(NULL), m_VCM(NULL), m_HasPolynomial(false) {
}

CPolynomialMotionController::~CPolynomialMotionController() {
	if(m_VCE && m_VCM) {
		m_VCM->deleteEntity(m_VCE);
	}
}

void CPolynomialMotionController::setVisualCollisionEntity(NL3D::UVisualCollisionManager* vcm) {
	if(!vcm)
		return;
	m_Subject->getMovePrimitive()->setDontSnapToGround(true);
	m_VCM = vcm;
	m_VCE = vcm->createEntity();
}

void CPolynomialMotionController::update() {
	float time = (float)getSimulation()->adjustedTime();
	uint numKeys = m_Subject->getCorrectionInterp()->getNumKeys();
	if(numKeys >= 3) {
		if(!m_Subject->getCorrectionInterp()->getLastKey().isHandled()) {
			CPerformer::CorrectInterpKey key0(m_Subject->getCorrectionInterp()->getIndexKey(2));
			CPerformer::CorrectInterpKey key1(m_Subject->getCorrectionInterp()->getIndexKey(1));
			CPerformer::CorrectInterpKey key2(m_Subject->getCorrectionInterp()->getIndexKey(0));
			float t[3] = { (float)key0.getTime(), (float)key1.getTime(), (float)key2.getTime() };
			float v[3];
			v[0] = key0.getPosition().x;
			v[1] = key1.getPosition().x;
			v[2] = key2.getPosition().x;
			solveQuadraticInterp3f(t, v, m_A[0], m_B[0], m_C[0]);
			v[0] = key0.getPosition().y;
			v[1] = key1.getPosition().y;
			v[2] = key2.getPosition().y;
			solveQuadraticInterp3f(t, v, m_A[1], m_B[1], m_C[1]);
			v[0] = key0.getPosition().z;
			v[1] = key1.getPosition().z;
			v[2] = key2.getPosition().z;
			solveQuadraticInterp3f(t, v, m_A[2], m_B[2], m_C[2]);
			v[0] = key0.getYaw();
			v[1] = key1.getYaw();
			v[2] = key2.getYaw();
			solveQuadraticInterp3f(t, v, m_A[3], m_B[3], m_C[3]);

			key0.setHandled(true);
			key1.setHandled(true);
			key2.setHandled(true);
			m_HasPolynomial = true;

		}
	}
	if(m_HasPolynomial) {
		NLMISC::CVector pos;
		float yaw;

		pos.x = quadraticInterp(time, m_A[0], m_B[0], m_C[0]);
		pos.y = quadraticInterp(time, m_A[1], m_B[1], m_C[1]);
		pos.z = quadraticInterp(time, m_A[2], m_B[2], m_C[2]);
		yaw = quadraticInterp(time, m_A[3], m_B[3], m_C[3]);

		pos = snapToGround(pos);

		CPerformer::CPerformerInterpKey* key = m_Subject->getCurrentKey();
		key->setPosition(pos);
		key->setYaw(yaw);
		key->setTime(getSimulation()->adjustedTime());

		key->setPosition(snapToGround(m_Subject->getCorrectionInterp()->getKey(time).getPosition()));
		key->setYaw(m_Subject->getCorrectionInterp()->getKey(time).getYaw());

		setKey();
	}
}

const char* CPolynomialMotionController::getControllerName() {
	return m_Name;
}

NLMISC::CVector CPolynomialMotionController::snapToGround(NLMISC::CVector pos) {
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

void CPolynomialMotionController::setKey() {
	CPerformer::CPerformerInterpKey* key = m_Subject->getCurrentKey();
	m_Subject->getInterpolator()->addKey(*key);
}

}; // END OF NAMESPACE WWCOMMON
