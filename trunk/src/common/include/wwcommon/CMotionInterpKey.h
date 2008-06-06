/**
 * \file CMotionInterpKey.h
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

#ifndef __CMOTIONINTERPKEY_H__
#define __CMOTIONINTERPKEY_H__

//
// Standard Includes
//

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/quat.h>
#include <nel/misc/matrix.h>
#include <nel/misc/vector.h>
//
// Werewolf Includes
//	

//
// Namespaces
//

namespace WWCOMMON {

// TODO henri:everyone this should probably be in some general mathsy file
void quatToEuler(NLMISC::CQuat q, float &yaw, float &pitch, float &roll);

class CMotionInterpKey {
public:
	CMotionInterpKey();
	CMotionInterpKey(NLMISC::CVector pos, NLMISC::CQuat rot, double time);
	~CMotionInterpKey();

	CMotionInterpKey interpolate(double time, CMotionInterpKey &next);
	NLMISC::CMatrix getMatrix();
	NLMISC::CVector getPosition() const;
	NLMISC::CQuat getRotation() const;
	float getYaw();
	float getPitch();
	float getRoll();
	void setTime(double time) { m_Time = time; };
	double getTime() { return m_Time; };
	void setPosition(NLMISC::CVector pos) { m_Position=pos; m_bMatrixComputed=false; };
	void setRotation(NLMISC::CQuat rot);

protected:
	bool m_bMatrixComputed, m_bRotationComputed;

	void computeMatrix();
	void computeRotation();

	NLMISC::CVector m_Position;
	NLMISC::CQuat m_Rotation;
	NLMISC::CMatrix m_Matrix;
	float m_Yaw;
	float m_Pitch;
	float m_Roll;
	double m_Time;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CMOTIONINTERPKEY_H__
