/**
 * \file CDRMotionInterpKey.h
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

#ifndef __CDRMOTIONINTERPKEY_H__
#define __CDRMOTIONINTERPKEY_H__

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
#include <nel/misc/vector.h>

//
// Werewolf Includes
//	

//
// Namespaces
//

namespace WWCOMMON {

class CDRMotionInterpKey {
public:
	CDRMotionInterpKey();
	CDRMotionInterpKey(NLMISC::CVector pos, float yaw, float yawVelocity, double time);
	// this is a very strange thing to do but using the nel value smoother would require it
	CDRMotionInterpKey(uint val);
	~CDRMotionInterpKey();

	CDRMotionInterpKey interpolate(double time, CDRMotionInterpKey &next);
	NLMISC::CVector getPosition() const { return m_Position; };
	float getYaw() { return m_Yaw; }
	double getTime() { return m_Time; };
	float getYawVelocity() { return m_YawVelocity; }
	bool isHandled() { return m_Handled; }
	void setHandled(bool b) { m_Handled = b; }

	CDRMotionInterpKey& operator+=(const CDRMotionInterpKey& rhs);
	CDRMotionInterpKey& operator-=(const CDRMotionInterpKey& rhs);
	const CDRMotionInterpKey operator/(const CDRMotionInterpKey& rhs);

protected:
	NLMISC::CVector m_Position;
	float m_Yaw;
	float m_YawVelocity;
	double m_Time;
	bool m_Handled;
	float m_Scalar;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __CDRMOTIONINTERPKEY_H__
