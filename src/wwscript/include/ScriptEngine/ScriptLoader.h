/**
 * \file ScriptLoader.h
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
#ifndef __SCRIPTLOADER_H__
#define __SCRIPTLOADER_H__

//
// Standard Includes
//
#include <string>

//
// System Includes
//
#include <angelscript.h>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/georges/u_form_loader.h>
#include <nel/georges/u_form.h>
#include <nel/georges/u_form_elm.h>
#include <nel/georges/load_form.h>

//
// Werewolf Includes
//

//
// Namespaces
//


namespace WWSCRIPT {

struct TScriptArgument {
public:
	std::string ArgumentName;
	std::string ArgumentType;
	uint32 ArgumentId;

	void serial(NLMISC::IStream &f);
};

struct TScriptFunction {
public:
	std::string FunctionName;
	std::string ReturnType;

	typedef std::vector<TScriptArgument> ArgumentVector;
	std::vector<TScriptArgument> Arguments;

	void serial(NLMISC::IStream &f);
};

/**
 * \class TScriptLoader Script.h "wwcommon/Script.h"
 * \brief This loads a script from packed sheets.
 */
class TScriptLoader {
public:
	/**
	 * \brief Here you read in the form if necessary, storing it in memebers of the TMyLoader class
	 */
	void readGeorges(const NLMISC::CSmartPtr<NLGEORGES::UForm> &form, const std::string &sheetId);

	/**
	 * \brief Here you write a standard NeL serial for all the member of TMyLoader that need to be packed.
	 */
	void serial(NLMISC::IStream &f);

	/**
	 * \brief This methed returns the version of the packed sheet.
	 * Version 0 - non-existant.
	 * Version 1 - Fileame, FriendlyName, Description.
	 *
	 * \return uint This must always return an integer. Unsigned is probably best.
	 */
	static uint getVersion() {
		return 1;
	}

	/**
	 * \brief Here you can write custom code for when the packed sheet loader needs to remove an old sheet.
	 * \note This is rarely used.
	 */
	void removed() {
		;
	}

	std::string Source;
	std::string Name;
	std::string Section;
	typedef std::vector<TScriptFunction> FunctionVector;
	FunctionVector ScriptFunctions;
};

}; // END NAMESPACE WWSCRIPT

#endif // __SCRIPTLOADER_H__

