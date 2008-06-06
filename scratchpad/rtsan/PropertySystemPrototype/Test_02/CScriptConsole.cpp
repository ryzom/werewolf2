/**
 * \file IGameEvent.cpp
 * \date June 2006
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
#include "stdafx.h"

//
// System Includes
//
#include <string>

//
// NeL Includes
//

//
// Werewolf Includes
//	
#include "CScriptConsole.h"

//
// Namespaces
//

bool CScriptConsole::update() {
	std::string input;
	input.resize(256);

	std::cout << ">>>> ";
	std::cin.getline(&input[0], 256);

	// Trim unused characters
	input.resize(strlen(input.c_str()));

	if( input.empty() )
		return true;

	// Interpret the command
	if( input == "quit" )
		return false;
	else if( input == "q" )
		return false;
	else if( input == "exit" )
		return false;
	else
		CScriptEngine::instance().execCommand(input.c_str());

	return true;
}
