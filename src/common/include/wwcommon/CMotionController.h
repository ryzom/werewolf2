/**
 * \file CMotionController.h
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

#ifndef __CMOTIONCONTROLLER_H__
#define __CMOTIONCONTROLLER_H__

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/matrix.h>
#include <nel/3d/u_visual_collision_entity.h>
#include <nel/3d/u_visual_collision_manager.h>

//
// Werewolf Includes
//
#include "ISobController.h"
#include "CPerformer.h"
#include "ISimulationState.h"

//
// Namespaces
//

namespace WWCOMMON {

class CMotionController : public ISobController {
public:
	CMotionController(CPerformer* subject);
	virtual ~CMotionController();

	void setVisualCollisionEntity(NL3D::UVisualCollisionManager* vcm);

	void update();
	void update(double dt);

	const char* getControllerName();
	NLMISC::CVector snapToGround(NLMISC::CVector pos);
	double Duration;

private:
	void move(double dt, bool correction);
	void ODE(float dt, float& yaw, float& vYaw, NLMISC::CVector& pos, NLMISC::CVector& sPos, NLMISC::CVector& vPos);
	void ODEStep(float dt, float& yaw, float& vYaw, NLMISC::CVector& pos, NLMISC::CVector& sPos, NLMISC::CVector& vPos, ISimulationState* moveState, ISimulationState* orientState, NLMISC::CMatrix &rotMat);
	void setKey();

	double m_CorrectionDt;
	static const char* m_Name;
	NL3D::UVisualCollisionEntity *m_VCE;
	NL3D::UVisualCollisionManager *m_VCM;
	CPerformer* m_Subject;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CMOTIONCONTROLLER_H__
