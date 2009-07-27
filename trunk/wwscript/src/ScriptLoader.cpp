/**
 * \file ScriptLoader.cpp
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

//
// Standard Includes
//
#include "stdpch.h"

//
// System Includes
//

//
// NeL Includes
//

//
// Werewolf Includes
//
#include "wwscript/ScriptEngine/ScriptLoader.h"

//
// Namespaces
//

namespace WWSCRIPT {

void TScriptLoader::readGeorges(const NLMISC::CSmartPtr<NLGEORGES::UForm> &form, const std::string &sheetId) {
	NLGEORGES::UFormElm &root = form->getRootNode();
	root.getValueByName(Name,".Name");
	root.getValueByName(Source,".Source");
	root.getValueByName(Section,".Section");
		
	NLGEORGES::UFormElm *functions;
	bool res=root.getNodeByName(&functions, ".Functions");
	if(res==false || functions==NULL) {
		nlwarning("Failed to load any functions for script %s!",Name.c_str());
		return;
	}

	uint numFunctions;
	functions->getArraySize(numFunctions);

	for(uint idx=0 ; idx<numFunctions ; idx++) {
		TScriptFunction aFunc;
		std::string funcname;
		std::string returntype;

		// get the struct in the array.		
		NLGEORGES::UFormElm *function;
		functions->getArrayNode(&function, idx);

		// save the datas
		function->getValueByName(funcname, ".FunctionName");
		function->getValueByName(returntype, ".ReturnType");
		aFunc.FunctionName=funcname;
		aFunc.ReturnType=returntype;

		NLGEORGES::UFormElm *arguments;
		res=function->getNodeByName(&arguments, ".Arguments");
		if(res==false || arguments==NULL) {
			nlinfo("No arguments found for function %s", aFunc.FunctionName.c_str());
		} else {
			uint numArguments;
			arguments->getArraySize(numArguments);
			for(uint aIdx=0 ; aIdx<numArguments ; aIdx++) {
				TScriptArgument aArg;
				std::string argName;
				std::string argType;
				uint32 argId;

				NLGEORGES::UFormElm *argument;
				arguments->getArrayNode(&argument, aIdx);

				argument->getValueByName(argName, "ArgumentName");
				argument->getValueByName(argType, "ArgumentType");
				argument->getValueByName(argId, "ArgumentId");

				aArg.ArgumentName=argName;
				aArg.ArgumentType=argType;
				aArg.ArgumentId=argId;

				aFunc.Arguments.push_back(aArg);
			}
		}

		ScriptFunctions.push_back( aFunc );
	}
}

void TScriptLoader::serial(NLMISC::IStream &f) {
	if(f.isReading()) {
		f.serial(Source);
		f.serial(Name);
		f.serial(Section);
		f.serialCont(ScriptFunctions);
	} else {
		f.serial(Source);
		f.serial(Name);
		f.serial(Section);
		f.serialCont(ScriptFunctions);
	}
}

void TScriptArgument::serial(NLMISC::IStream &f) {
	if(f.isReading()) {
		f.serial(ArgumentName);
		f.serial(ArgumentType);
		f.serial(ArgumentId);
	} else {
		f.serial(ArgumentName);
		f.serial(ArgumentType);
		f.serial(ArgumentId);
	}
}

void TScriptFunction::serial(NLMISC::IStream &f) {
	if(f.isReading()) {
		f.serial(FunctionName);
		f.serial(ReturnType);
		f.serialCont(Arguments);
	} else {
		f.serial(FunctionName);
		f.serial(ReturnType);
		f.serialCont(Arguments);
	}
}

}; // END NAMESPACE WWSCRIPT

