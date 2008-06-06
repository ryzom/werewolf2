/**
 * \file CYawMotionInterpKey.h
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

#ifndef __CYAWMOTIONINTERPKEY_H__
#define __CYAWMOTIONINTERPKEY_H__

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
#include <nel/misc/matrix.h>
#include <nel/misc/vector.h>

//
// Werewolf Includes
//	

//
// Namespaces
//

namespace WWCOMMON {

class CYawMotionInterpKey {
public:
	CYawMotionInterpKey();
	CYawMotionInterpKey(NLMISC::CVector pos, float yaw, NLMISC::CVector speed, float yawSpeed, double time);
	// this is a very strange thing to do but using the nel value smoother would require it
	CYawMotionInterpKey(uint val);
	~CYawMotionInterpKey();

	CYawMotionInterpKey interpolate(double time, CYawMotionInterpKey &next);
	NLMISC::CMatrix getMatrix();
	NLMISC::CVector getPosition() const;
	float getYaw();
	void setTime(double time) { m_Time = time; };
	double getTime() { return m_Time; };
	void setPosition(NLMISC::CVector pos) { m_Position=pos; m_bMatrixComputed=false; };
	void setYaw(float yaw);
	NLMISC::CVector getSpeed();
	void setSpeed(NLMISC::CVector speed);
	float getYawSpeed();
	void setYawSpeed(float speed);
	NLMISC::CVector getVelocity();

	CYawMotionInterpKey& operator+=(const CYawMotionInterpKey& rhs);
	CYawMotionInterpKey& operator-=(const CYawMotionInterpKey& rhs);
	const CYawMotionInterpKey operator/(const CYawMotionInterpKey& rhs);

protected:
	bool m_bMatrixComputed;
	bool m_bVelocityComputed;

	void computeMatrix();

	NLMISC::CVector m_Velocity;
	NLMISC::CVector m_Position;
	NLMISC::CMatrix m_Matrix;
	NLMISC::CVector m_Speed;
	float m_Yaw;
	float m_YawSpeed;
	double m_Time;
	float m_Scalar;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CYAWMOTIONINTERPKEY_H__
