/**
 * \file CPerformer.h
 * \date April 2005
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

#ifndef __CPERFORMER_H__
#define __CPERFORMER_H__

//
// Standard Includes
//
#include <list>

//
// System Includes
//

//
// NeL Includes
//
#include <nel/pacs/u_move_primitive.h>
#include <nel/misc/quat.h>

//
// Werewolf Includes
//	
#include "ISimulationObj.h"
//#include "CMovementState.h"
#include "CGeneralInterpolator.h"
#include "CYawMotionInterpKey.h"
#include "CDRMotionInterpKey.h"
#include "CSobFactory.h"
#include "CFactoryRegistrar.h"

//
// Namespaces
//

namespace WWCOMMON {

class CMovementState;
class CStrafeState;
class COrientState;

class CPerformer : public ISimulationObj {
	OF_SETUP_REGISTRAR(CSobFactory, ISimulationObj, CPerformer, std::string);
public:
	typedef CYawMotionInterpKey CPerformerInterpKey;
	typedef CGeneralInterpolator<CPerformerInterpKey> CPerformerInterp;
	typedef CDRMotionInterpKey CorrectInterpKey;
	typedef CGeneralInterpolator<CorrectInterpKey> CorrectInterp;
	
	static ISimulationObj *Create();

	void update();

	// motion / position information
	float getMaxSpeed();
	float getAcceleration();
	float getBrakingFactor();
	float getMaxRotSpeed();
	float getRotAcceleration();
	void setRotAcceleration(float a);
	float getRotBrakingFactor();
	double getMotionBlendDt();
	void setMotionBlendDt(double dt);
	CPerformerInterpKey* getCurrentKey() { return &m_CurrentKey; }

	NLPACS::UMovePrimitive* getMovePrimitive() { return m_MovePrimitive; }
	
	/// Returns the current position in the interpolator.
	NLMISC::CVector getPosition();
	NLMISC::CMatrix getMatrix();
	/// Adds a new position key to the interpolator.
	void setPosition(NLMISC::CVector pos);
	/// Sets the move primitive's global position. Expert use only!
	void setGlobalPosition(NLMISC::CVector pos);

	CPerformerInterp *getInterpolator() { return m_Interpolator; }
	// crappy solution - need data containers
	virtual bool useCorrection() { return false; }
	CorrectInterp *getCorrectionInterp() { return m_CorrectionInterp; }

protected:
	CPerformerInterpKey m_CurrentKey;

	CPerformer();
	virtual ~CPerformer();

	// motion / position information
	float m_MaxSpeed;
	float m_Acceleration;
	float m_BrakingFactor;
	float m_YawVelocity;
	float m_MaxRotSpeed;
	float m_RotAcceleration;
	float m_RotBrakingFactor;
	double m_MotionBlendDt;

	CPerformerInterp *m_Interpolator;
	CorrectInterp *m_CorrectionInterp;

	// PACS stuff.
	NLPACS::UMovePrimitive *m_MovePrimitive;
};

}; // END NAMESPACE WWCOMMON

#endif // __CPERFORMER_H__

