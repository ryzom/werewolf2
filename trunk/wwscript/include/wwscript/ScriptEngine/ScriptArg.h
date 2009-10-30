/**
 * \file ScriptArg.h
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
#ifndef __SCRIPTARG_H__
#define __SCRIPTARG_H__

//
// Standard Includes
//
#include <string>

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "wwcommon/ISingleton.h"
#include "wwcommon/general.h"
#include "ScriptLoader.h"

//
// Namespaces
//

namespace WWSCRIPT {

class ScriptArg {
public:
	ScriptArg(TScriptArgument arg);
	~ScriptArg();

	enum eType {
		VOID_TYPE,
		INT,
		UINT,
		LONG,
		FLOAT,
		DOUBLE,
		BOOL,
		OBJECT,
		STRING
	};

	class Binding {
	public:
		Binding(std::string _map, std::string _property) : map(_map), property(_property) {};
		std::string map;
		std::string property;
	};

	const std::string& getName() const;
	int getId() const;
	eType getType() const;

	bool isBound() const;
	const Binding* getBinding() const;

	class Helper : public WWCOMMON::ISingleton<Helper> {
	public:
		Helper();
		~Helper();

		ScriptArg::eType find(std::string name) const;

		typedef CHashMap<const char *, ScriptArg::eType, strltpred_hash_compare> TypeMap;
//		typedef std::pair<const char*, ScriptArg::eType> typePair;

	private:
		TypeMap m_types;
	};

private:
	void parseBinding(TScriptArgument arg);

	std::string m_name;
	int	m_id;
	eType m_type;
	Binding* m_binding;
};

}; // END NAMESPACE WWSCRIPT

#endif // __SCRIPTARG_H__
