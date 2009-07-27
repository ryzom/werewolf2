/**
 * \file ScriptNelBindery.h
 * \date February 2006
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
#ifndef __SCRIPTNELBINDERY_H__
#define __SCRIPTNELBINDERY_H__

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

//
// Namespaces
//

namespace WWSCRIPT {

class ScriptManager;

class ScriptNelBindery {
public:
	static void NelInfo(std::string &log) {
		nlinfo(log.c_str());
	}

private:
	friend class ScriptManager;
	ScriptNelBindery() { }

	bool bindNel() {
		ScriptManager::instance().getEngine()->RegisterGlobalFunction(
			"void NelInfo(string &in)", asFUNCTION(ScriptNelBindery::NelInfo), asCALL_CDECL);

		return true;
	}
};

}; // END NAMESPACE WWSCRIPT

#endif // __SCRIPTNELBINDERY_H__
