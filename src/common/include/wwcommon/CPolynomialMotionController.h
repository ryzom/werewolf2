/**
 * \file CPolynomialMotionController.h
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

#ifndef __CPOLYNOMIALMOTIONCONTROLLER_H__
#define __CPOLYNOMIALMOTIONCONTROLLER_H__

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

class CPolynomialMotionController : public ISobController {
public:
	CPolynomialMotionController(CPerformer* subject);
	virtual ~CPolynomialMotionController();

	void setVisualCollisionEntity(NL3D::UVisualCollisionManager* vcm);

	void update();

	const char* getControllerName();
	NLMISC::CVector snapToGround(NLMISC::CVector pos);

private:
	void setKey();

	static const char* m_Name;
	NL3D::UVisualCollisionEntity *m_VCE;
	NL3D::UVisualCollisionManager *m_VCM;
	CPerformer* m_Subject;

	float m_A[4];
	float m_B[4];
	float m_C[4];
	bool m_HasPolynomial;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CPOLYNOMIALMOTIONCONTROLLER_H__
