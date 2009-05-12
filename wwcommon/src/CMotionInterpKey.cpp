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
#include "wwcommon/CMotionInterpKey.h"
#include "wwcommon/IBaseSimulation.h"

//
// Namespaces
//

namespace WWCOMMON {

CMotionInterpKey::CMotionInterpKey()
	: m_Time(::getSimulation()->adjustedTime()), m_Rotation(NLMISC::CQuat::Identity), m_Position(NLMISC::CVector::Null),
	m_bMatrixComputed(false), m_bRotationComputed(false) {
	;
}

CMotionInterpKey::CMotionInterpKey(NLMISC::CVector pos, NLMISC::CQuat rot, double time) 
	: m_Time(time), m_Rotation(rot), m_Position(pos), m_bMatrixComputed(false), m_bRotationComputed(false) {
	;
}

CMotionInterpKey::~CMotionInterpKey() {
	// does nothing
}

NLMISC::CMatrix CMotionInterpKey::getMatrix() {
	if(!m_bMatrixComputed)
		computeMatrix();
	return m_Matrix;
}

NLMISC::CVector CMotionInterpKey::getPosition() const {
	return m_Position;
}

NLMISC::CQuat CMotionInterpKey::getRotation() const {
	return m_Rotation;
}

void CMotionInterpKey::setRotation(NLMISC::CQuat rot) {
	m_Rotation = rot;
	m_bRotationComputed=false;
	m_bMatrixComputed=false;
}

float CMotionInterpKey::getYaw() {
	if(!m_bRotationComputed)
		computeRotation();
	return m_Yaw;
}

float CMotionInterpKey::getPitch() {
	if(!m_bRotationComputed)
		computeRotation();
	return m_Pitch;
}

float CMotionInterpKey::getRoll() {
	if(!m_bRotationComputed)
		computeRotation();
	return m_Roll;
}

void CMotionInterpKey::computeMatrix() {
	m_Matrix.setPos(m_Position);
	m_Matrix.setRot(m_Rotation);
}

// TODO henri:everyone this should probably be in some general mathsy file
void quatToEuler(NLMISC::CQuat q, float &yaw, float &pitch, float &roll) {
	double sqw = q.w*q.w;
	double sqx = q.x*q.x;
	double sqy = q.y*q.y;
	double sqz = q.z*q.z;
	if (q.x*q.z + q.y*q.w > 0.449) { // singularity at north pole
		yaw = 2 * atan2(q.x,q.w);
		pitch = (float)NLMISC::Pi/2;
		roll = 0;
		return;
	}
    
	if (q.x*q.z + q.y*q.w < -0.449) { // singularity at south pole
		yaw = -2 * atan2(q.x,q.w);
		pitch = (float)NLMISC::Pi/2;
		roll = 0;
		return;
	}
	
	yaw = atan2((float)(2*q.z*q.w-2*q.x*q.y) , float(1 - 2*sqz - 2*sqy));
	pitch = asin((float)(2*q.x*q.z + 2*q.y*q.w));
	roll = atan2((float)(2*q.x*q.w-2*q.z*q.y) , (float)(1 - 2*sqx - 2*sqy));
}

void CMotionInterpKey::computeRotation() {
	quatToEuler(m_Rotation, m_Yaw, m_Pitch, m_Roll);
}

CMotionInterpKey CMotionInterpKey::interpolate(double time, CMotionInterpKey &next) {
	double pos = (time - m_Time)/(next.getTime() - m_Time);
	if(pos <= 0) {
		return CMotionInterpKey(*this);
	} else if( pos >= 1) {
		return CMotionInterpKey(next);
	}

	// linear interpolation
	NLMISC::CQuat rot = m_Rotation;
	if(NLMISC::CQuat::dotProduct(rot, next.getRotation()) < 0)
		rot.invert();
	NLMISC::CQuat interpRot = NLMISC::CQuat::slerp(m_Rotation, next.getRotation(), pos);
	NLMISC::CVector interpPos = (float)(1.0f - pos)*m_Position + (float)pos * next.getPosition();
//	nlinfo("Source key: %f / %f / %f", m_Position.x, m_Position.y, m_Position.z);
//	nlinfo("Next key: %f / %f / %f", next.getPosition().x, next.getPosition().y, next.getPosition().z);
//	nlinfo("Interpolated key: %f / %f / %f", interpPos.x, interpPos.y, interpPos.z);
	return CMotionInterpKey(interpPos, interpRot, time);
}

}; // END OF NAMESPACE WWCOMMON

