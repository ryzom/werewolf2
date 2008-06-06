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
#include <nel/misc/file.h>
#include <nel/misc/path.h>
#include <nel/misc/bitmap.h>

//
// Werewolf Includes
//
#include "wwcommon/CTaskManager.h"
#include "tasks/CInputTask.h"
#include "CInputController.h"
#include "CWWTask.h"

//
// Namespaces
//
using namespace std;
using namespace NLMISC;
using namespace NL3D;

namespace WWCLIENT {

static bool			 CaptureState = false;

NLMISC::CVector ControlerFreeLookPos(0,0,0);
NLMISC::CMatrix ControlerCamMatrix;

CInputController::CInputController() {
	reset();
}

CInputController::~CInputController() {
	;
}

void CInputController::reset() {
	m_FirstFrame = true;
	m_Accel = CVector(0,0,0);
	m_VirtualFrameTime = 0.0f;

	m_MissedTime = 0.0f;
	RotZ = 0.0f;
}

void CInputController::update() {
	if(m_FirstFrame) {
		m_FirstFrame=false;
		m_Accel=CVector::Null;
		m_VirtualFrameTime=0.0f;
	}
	
	// if the escape key was hit at any time, just bail.
	// update this later.
	if(CInputTask::instance().kbPressed(KeyESCAPE)) {
		nlinfo("Escape pressed, signalling task manager to exit.");
		WWCOMMON::CTaskManager::instance().exit();
	}
}

}; // END NAMESPACE WWCLIENT
