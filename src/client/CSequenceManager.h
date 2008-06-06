/**
 * \file CSequenceManager.h
 * \date February 2005
 * \author Henri Kuuste
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

#ifndef __CSEQUENCEMANAGER_H__
#define __CSEQUENCEMANAGER_H__

//
// Standard Includes
//
#include <deque>

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//	

//
// Namespaces
//

namespace WWCLIENT {

template<class T>
class CSequenceManager {
public:
	CSequenceManager(T min, T max, T step) 
		: m_Min(min), m_Max(max), m_Step(step), m_Current(min), m_Out(false) {
		;
	}

	bool hasVacancy() {
		return !m_Out || (m_Free.size() > 0);
	}

	T get() {
		T ret;

		if(m_Free.size() == 0) {
			ret=m_Current;
			if(m_Out)
				return ret;
			if(m_Current > (m_Max - m_Step))
				m_Out = true;
			m_Current += m_Step;
		} else {
			ret=m_Free.front();
			m_Free.pop_front();
		}

		return ret;
	}

	void release(T value) {
		m_Free.push_back(value);
	}

private:
	T m_Min;
	T m_Max;
	T m_Step;
	T m_Current;

	bool m_Out;

	std::deque<T> m_Free;
};

}; // END NAMESPACE WWCLIENT

#endif // __CSEQUENCEMANAGER_H__