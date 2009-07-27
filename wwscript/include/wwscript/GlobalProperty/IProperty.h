/**
 * \file IProperty.h
 * \date February 2006
 * \author Henri Kuuste
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
#ifndef __IPROPERTY_H__
#define __IPROPERTY_H__

//
// Standard Includes
//
#include <vector>
#include <sstream>

//
// System Includes
//
//#include <Cg/cgGL.h>
#include <angelscript.h>
//#include <xercesc/dom/DOM.hpp>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "wwscript/ScriptEngine/ScriptEngineDefs.h"

//
// Namespaces
//

namespace WWSCRIPT {

class ScriptVariable;

class WWSCRIPT_API IProperty {
protected:
	const char* m_name;

	IProperty() {};

public:
	//virtual void setCGGLParameter(CGparameter& parameter) = 0;
	//virtual void setCGD3DParameter(CGparameter& parameter) = 0;
	virtual void setScriptParameter(ScriptVariable* var) = 0;

	/**
	 * Not a typical hash function but rather a unique hash should be added to the end of the stream.
	 */
	// TODO determine waht thsi was for and if it is needed.
	//virtual void getHash(std::stringstream& hash) = 0;

	//static IProperty *create(xercesc::DOMNode* node) { return NULL; };

	~IProperty() {};
	void setName(const char *name) { m_name = name; };
	const char* getName() { return m_name; };
};

}; // END OF NAMESPACE WWSCRIPT

#endif // __IPROPERTY_H__
