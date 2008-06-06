/**
 * \file CDRMotionInterpKey.cpp
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

//
// Werewolf Includes
//
#include "CDRMotionInterpKey.h"
#include "IBaseSimulation.h"

//
// Namespaces
//

namespace WWCOMMON {

CDRMotionInterpKey::CDRMotionInterpKey() 
	: m_Scalar(1), m_Time(::getSimulation()->adjustedTime()), m_Yaw(0), m_Position(NLMISC::CVector::Null),
	m_Handled(false), m_YawVelocity(0) {
	;
}

CDRMotionInterpKey::CDRMotionInterpKey(NLMISC::CVector pos, float yaw, float yawVelocity, double time) 
	: m_Scalar(1), m_Time(time), m_Position(pos), m_Yaw(yaw), m_Handled(false), m_YawVelocity(yawVelocity) {
	;
}

CDRMotionInterpKey::CDRMotionInterpKey(uint val)
: m_Scalar(val), m_Time(0), m_Yaw(0), m_Position(NLMISC::CVector::Null), m_Handled(false), m_YawVelocity(0) {
	;
}

CDRMotionInterpKey::~CDRMotionInterpKey() {
	// does nothing
}

CDRMotionInterpKey CDRMotionInterpKey::interpolate(double time, CDRMotionInterpKey &next) {
	double pos = (time - m_Time)/(next.getTime() - m_Time);
	if(pos <= 0) {
		return CDRMotionInterpKey(*this);
	} else if( pos >= 1) {
		return CDRMotionInterpKey(next);
	}

	// linear interpolation
	float interpYaw = (float)(1.0f - pos)*m_Yaw + (float)pos*next.getYaw();
	NLMISC::CVector interpPos = (float)(1.0f - pos)*m_Position + (float)pos * next.getPosition();
	float interpYawVel = (float)(1.0f - pos)*m_YawVelocity + (float)pos*next.getYawVelocity();
	return CDRMotionInterpKey(interpPos, interpYaw, interpYawVel, time);
}

CDRMotionInterpKey& CDRMotionInterpKey::operator +=(const CDRMotionInterpKey &rhs) {
	m_Yaw += rhs.m_Yaw;
	m_Position += rhs.m_Position;
	m_Time += rhs.m_Time;
	m_YawVelocity += rhs.m_YawVelocity;
	return *this;
}

CDRMotionInterpKey& CDRMotionInterpKey::operator -=(const CDRMotionInterpKey &rhs) {
	m_Yaw -= rhs.m_Yaw;
	m_Position -= rhs.m_Position;
	m_Time -= rhs.m_Time;
	m_YawVelocity -= rhs.m_YawVelocity;
	return *this;
}

const CDRMotionInterpKey CDRMotionInterpKey::operator /(const CDRMotionInterpKey &rhs) {
	CDRMotionInterpKey result = *this;
	result.m_Position /= rhs.m_Scalar;
	result.m_Yaw /= rhs.m_Scalar;
	result.m_Time /= rhs.m_Scalar;
	result.m_YawVelocity /= rhs.m_Scalar;
	return result;
}

}; // END OF NAMESPACE WWCOMMON
