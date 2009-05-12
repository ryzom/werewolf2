/**
 * \file CYawMotionInterpKey.cpp
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
#include "wwcommon/CYawMotionInterpKey.h"
#include "wwcommon/IBaseSimulation.h"

//
// Namespaces
//

namespace WWCOMMON {

CYawMotionInterpKey::CYawMotionInterpKey() 
	: m_Scalar(1), m_Time(::getSimulation()->adjustedTime()), m_Yaw(0.0f), m_Position(NLMISC::CVector::Null), m_bMatrixComputed(false),
	m_Speed(NLMISC::CVector::Null), m_YawSpeed(0.0f), m_bVelocityComputed(false) {
	;
}

CYawMotionInterpKey::CYawMotionInterpKey(NLMISC::CVector pos, float yaw, NLMISC::CVector speed, float yawSpeed, double time) 
	: m_Scalar(1), m_Time(time), m_Position(pos), m_Yaw(yaw), m_bMatrixComputed(false), m_Speed(speed), m_YawSpeed(yawSpeed),
	m_bVelocityComputed(false) {
	;
}

CYawMotionInterpKey::CYawMotionInterpKey(uint val)
	: m_Scalar((float)val), m_Time(0.0f), m_Yaw(0.0f), m_Position(NLMISC::CVector::Null), m_Speed(NLMISC::CVector::Null), m_YawSpeed(0.0f),
	m_bMatrixComputed(false), m_bVelocityComputed(false) {
	;
}

CYawMotionInterpKey::~CYawMotionInterpKey() {
	// does nothing
}

NLMISC::CMatrix CYawMotionInterpKey::getMatrix() {
	if(!m_bMatrixComputed)
		computeMatrix();
	return m_Matrix;
}

NLMISC::CVector CYawMotionInterpKey::getPosition() const {
	return m_Position;
}

void CYawMotionInterpKey::setYaw(float yaw) {
	m_Yaw = yaw;
	m_bMatrixComputed=false;
	m_bVelocityComputed=false;
}

float CYawMotionInterpKey::getYaw() {
	return m_Yaw;
}

NLMISC::CVector CYawMotionInterpKey::getSpeed() {
	return m_Speed;
}

void CYawMotionInterpKey::setSpeed(NLMISC::CVector speed) {
	m_Speed = speed;
	m_bVelocityComputed = false;
}

float CYawMotionInterpKey::getYawSpeed() {
	return m_YawSpeed;
}

void CYawMotionInterpKey::setYawSpeed(float speed) {
	m_YawSpeed = speed;
}

NLMISC::CVector CYawMotionInterpKey::getVelocity() {
	if(!m_bVelocityComputed) {
        m_Velocity = getMatrix().mulVector(m_Speed);
		m_bVelocityComputed = true;
	}
	return m_Velocity;
}

void CYawMotionInterpKey::computeMatrix() {
	m_Matrix = NLMISC::CMatrix::Identity;
	m_Matrix.setPos(m_Position);
	m_Matrix.rotateZ(m_Yaw);
	m_bMatrixComputed = true;
}

CYawMotionInterpKey CYawMotionInterpKey::interpolate(double time, CYawMotionInterpKey &next) {
	double pos = (time - m_Time)/(next.getTime() - m_Time);
	if(pos <= 0) {
		return CYawMotionInterpKey(*this);
	} else if( pos >= 1) {
		return CYawMotionInterpKey(next);
	}

	// linear interpolation
	float interpYaw = (float)(1.0f - pos)*m_Yaw + (float)pos*next.getYaw();
	NLMISC::CVector interpPos = (float)(1.0f - pos)*m_Position + (float)pos * next.getPosition();
	NLMISC::CVector interpSpeed = (float)(1.0f - pos)*m_Speed + (float)pos * next.getSpeed();
	float interpYawSpeed = (float)(1.0f - pos)*m_YawSpeed + (float)pos * next.getYawSpeed();
	return CYawMotionInterpKey(interpPos, interpYaw, interpSpeed, interpYawSpeed, time);
}

CYawMotionInterpKey& CYawMotionInterpKey::operator +=(const CYawMotionInterpKey &rhs) {
	m_Yaw += rhs.m_Yaw;
	m_Position += rhs.m_Position;
	m_Time += rhs.m_Time;
	m_Speed += rhs.m_Speed;
	m_YawSpeed += rhs.m_YawSpeed;
	m_bMatrixComputed = false;
	m_bVelocityComputed = false;
	return *this;
}

CYawMotionInterpKey& CYawMotionInterpKey::operator -=(const CYawMotionInterpKey &rhs) {
	m_Yaw -= rhs.m_Yaw;
	m_Position -= rhs.m_Position;
	m_Time -= rhs.m_Time;
	m_Speed -= rhs.m_Speed;
	m_YawSpeed -= rhs.m_YawSpeed;
	m_bMatrixComputed = false;
	m_bVelocityComputed = false;
	return *this;
}

const CYawMotionInterpKey CYawMotionInterpKey::operator /(const CYawMotionInterpKey &rhs) {
	CYawMotionInterpKey result = *this;
	result.m_Position /= rhs.m_Scalar;
	result.m_Yaw /= rhs.m_Scalar;
	result.m_Time /= rhs.m_Scalar;
	result.m_Speed /= rhs.m_Scalar;
	result.m_YawSpeed /= rhs.m_Scalar;
	result.m_bMatrixComputed = false;
	result.m_bVelocityComputed = false;
	return result;
}

}; // END OF NAMESPACE WWCOMMON
