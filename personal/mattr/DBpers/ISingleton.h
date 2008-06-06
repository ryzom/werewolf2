/**
 * \file ISingleton.h
 * \date May 2004
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

#ifndef __ISINGLETON_H__
#define __ISINGLETON_H__

//
// System Includes
//

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>

//
// Werewolf Includes
//

//
// Class
//

/**
 * \interface ISingleton ISingleton.h "ISingleton.h"
 * \brief Singleton Interface
 *
 * This provides a generic Singleton interface with static
 * references. Usage is simple, example:
 * class Foobar : public ISingleton<Foobar>
 */
template<class T>
class ISingleton
{
public:
	
	static T &instance()
	{
		if(!m_Instance)
		{
			m_Instance = new T;
			nlassert(m_Instance);
		}
		return *m_Instance;
	}
	
	static void uninstance()
	{
		if(m_Instance)
		{
			delete m_Instance;
			m_Instance = 0;
		}
	}
	
protected:
	
	ISingleton()
	{
	}
	
	static T *m_Instance;
};

template <class T>
T* ISingleton<T>::m_Instance = 0;

#endif
