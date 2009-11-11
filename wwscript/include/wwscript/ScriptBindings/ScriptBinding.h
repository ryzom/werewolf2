/**
 * \file ScriptBinding.h
 * \date September 2009
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
#ifndef __SCRIPTBINDING_H__
#define __SCRIPTBINDING_H__

//
// Standard Includes
//

//
// System Includes
//
#include <angelscript.h>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>

//
// Werewolf Includes
//
#include "wwscript/ScriptEngine/ScriptManager.h"

//
// Namespaces
//

namespace WWSCRIPT {

template<class T>
T &createSingleton() {
	return T::instance();
}

template<class T1, class T2>
T2* refCast(T1 *t1) {
	if(!t1) return 0;

	T2 *t2 = dynamic_cast<T2*>(t1);
	if(t2 != 0) {
		//t2->addref();
	}
	return t2;
}

class asRefDummy {
public:
	void addRef() { };
	void release() { };
};

template<class T>
T *asCreateFactory() { return new T(); }

class ScriptBinding {
public:
	virtual bool bindObjects()=0;
};

}; // END NAMESPACE WWSCRIPT

#endif // __SCRIPTBINDING_H__
