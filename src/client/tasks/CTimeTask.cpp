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

//
// Werewolf Includes
//
#include "CTimeTask.h"

//
// Namespaces
//
using namespace std;
using namespace NLMISC;

namespace WWCLIENT {

void CTimeTask::init() {
	m_FirstTime = m_OldTime = m_Time = NLMISC::CTime::ticksToSecond(NLMISC::CTime::getPerformanceTime());
	m_DeltaTimeSmooth.init(100);
}

void CTimeTask::update() {
	m_OldTime = m_Time;
	double newTime = NLMISC::CTime::ticksToSecond(NLMISC::CTime::getPerformanceTime());
	m_Time = newTime - m_FirstTime;
	m_DeltaTime = m_Time - m_OldTime;
	m_DeltaTimeSmooth.addValue(m_DeltaTime);
}

void CTimeTask::render() {
	;
}

void CTimeTask::release() {
	;
}

std::string CTimeTask::name() {
	return "CTimeTask";
}

double CTimeTask::time() const {
	return m_Time;
}

double CTimeTask::deltaTime () const {
	return m_DeltaTime;
}

double CTimeTask::fps() const {
	return m_DeltaTimeSmooth.getSmoothValue() ? 1.0 / m_DeltaTimeSmooth.getSmoothValue() : 0.0;
}

double CTimeTask::spf() const {
	return m_DeltaTimeSmooth.getSmoothValue();
}

}; // END NAMESPACE WWCLIENT
