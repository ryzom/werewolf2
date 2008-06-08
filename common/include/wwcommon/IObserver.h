/**
 * \file IObserver.h
 * \date May 2005
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

#ifndef __IOBSERVER_H__
#define __IOBSERVER_H__

//
// System Includes
//
#include <vector>

//
// NeL Includes
//

//
// Werewolf Includes
//

//
// Class
//

namespace WWCOMMON {

template<class T>
class IObserver {
public:
	IObserver() {}
	virtual ~IObserver() {}
	virtual void update(T *subject)=0;
};

template<class T>
class ISubject {
public:
	ISubject() {}
	virtual ~ISubject() {}

	void attach(IObserver<T> &observer) {
		m_Observers.push_back(&observer);
	}

	void notify () {
		typename std::vector<IObserver<T> *>::iterator itr;
		for (itr=m_Observers.begin();itr!=m_Observers.end();itr++) 
			(*itr)->update(static_cast<T *>(this));
	}

private:
	std::vector<IObserver<T> *> m_Observers;
};

}; // END OF NAMESPACE WWCOMMON

#endif // __IOBSERVER_H__
