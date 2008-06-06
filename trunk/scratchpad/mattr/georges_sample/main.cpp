/**
 * \file main.cpp
 * \date October 2004
 * \author Matt Raykowski
 *
 * This sample shows how to load a Georges form and read parts from it.
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
// System Includes
//
#include <stdio.h>

//
// NeL Includes
//
#include "nel/misc/debug.h"

#include "nel/georges/u_form_loader.h"
#include "nel/georges/u_form.h"
#include "nel/georges/u_form_elm.h"
#include "nel/georges/load_form.h"


int main(void) {
	// get a pointer ready for the form loader.
	NLGEORGES::UFormLoader *formLoader = NULL;

	try {
		// set the name of the form you're going to load.
		std::string sampleConfigFile = NLMISC::CPath::lookup("default.sample_config", false);

		// check to see if CPath found the config.
		if (!sampleConfigFile.empty()) {
			// we'll use this to test retrieving vars.
			bool res;

			// get a form loader.
			formLoader = NLGEORGES::UFormLoader::createLoader();

			// this will hold the form we load - it must be contained in a smart pointer.
			NLMISC::CSmartPtr<NLGEORGES::UForm> form;

			// load the form.
			form = formLoader->loadForm(sampleConfigFile.c_str());

			// get the root of the form.
			NLGEORGES::UFormElm &root = form->getRootNode();

			// retrieve the two test values
			uint32 testVal1, testVal2, badVar1;
			root.getValueByName(testVal1, ".TestVal1");
			root.getValueByName(testVal2, ".TestVal2");

			// try and get a var that doesn't exist.
			res=root.getValueByName(badVar1, ".Foobar");
			if(res) {
				nlinfo("This should never have matched.");
				exit(1);
			}

			// and output them
			nlinfo("Test Value 1: %d",testVal1);
			nlinfo("Test Value 2: %d",testVal2);
			nlinfo("Foo Retrieval was: %s", res ? "true" : "false");

			// Retrieve data from a root-level named struct
			uint xTest, yTest, zTest;
			root.getValueByName(xTest,"PositionData.x");
			root.getValueByName(yTest,"PositionData.y");
			root.getValueByName(zTest,"PositionData.z");
			nlinfo("Retrieved Position Data (x,y,z): %d, %d, %d",xTest,yTest,zTest);

			// get a reference to the TestArray array...
			NLGEORGES::UFormElm *testArray;
			res=root.getNodeByName(&testArray, ".TestArray");

			nlinfo("TestArray retrieval returned: %s", res ? "true" : "false");
			// make sure it was there.
			if(testArray != NULL) {
				// get the size of the array.
				uint size;
				testArray->getArraySize(size);

				// cycle through the atoms in the array
				for(uint idx=0 ; idx<size ; idx++) {
					// string to store our array values in.
					std::string arrayValue;

					// and get the value of the array, we know it's a string.
					testArray->getArrayValue(arrayValue, idx);

					nlinfo("Found TestArray atom: %s",arrayValue.c_str());
				}
			} else {
				if(res==true)
					nlinfo("TestArray wasn't configured properly but was found, double check the form.");
				else
					nlinfo("Something's wrong, the TestArray is missing from the form.");
			}

			// Next grab a set of structures from an array.
			NLGEORGES::UFormElm *coolFiles;
			res=root.getNodeByName(&coolFiles, ".CoolFilesInfo");

			nlinfo("Retrieving CoolFilesInfo returned: %s", res ? "true" : "false");
			if(coolFiles != NULL) {
				uint size;
				coolFiles->getArraySize(size);

				// cycle through the structs in the array.
				for(uint idx=0 ; idx<size ; idx++) {
					// storage for our vars in the structs
					std::string name, shortname;
					uint rank, toptenrank;

					// since we know the structure of the file, we know the array contains structs
					// retrieve the struct for use.
					NLGEORGES::UFormElm *files;
					coolFiles->getArrayNode(&files, idx);

					// we can now access this struct in the array as a root.
					files->getValueByName(rank, ".Ranking");
					files->getValueByName(toptenrank, ".TopTenRanking");
					files->getValueByName(name, ".Name");
					files->getValueByName(shortname, ".ShortName");
					nlinfo("Retrieved struct %d: |%s| - %s at %d (%d)", idx, name.c_str(), shortname.c_str(), rank, toptenrank);
				}

			} else {
				if(res==true)
					nlinfo("CoolFilesInfo was found, but might have been entered incorrectly, double check the form.");
				else
					nlinfo("CoolFilesInfo wasn't found or there was a syntax error loading your form.");
			}

			// we can also access elements of an array by their backet identifier.
			std::string bbool1, bbool2;
			root.getValueByName(bbool1, ".TestByBracket[0]");
			root.getValueByName(bbool2, ".TestByBracket[1]");
			nlinfo("Bool Backet 1: %s, Bool Bracket 2: %s",bbool1.c_str(), bbool2.c_str());

			// likewise you can access structures by index and dot notation. take the CoolFilesInfo.
			// the following should return 1, the ranking for the Nevrax Presents element.
			// and the 2nd one should return 'Crazy Car Jump' from that element.
			uint rankTest;
			std::string nameTest;
			root.getValueByName(rankTest, ".CoolFilesInfo[2].Ranking");
			root.getValueByName(nameTest, ".CoolFilesInfo[1].ShortName");
			nlinfo("Retrieved .CoolFilesInfo[2].Ranking using bracket and dot notation: %d", rankTest);
			nlinfo("Retrieved .CoolFilesInfo[1].ShortName using bracket and dot notation: %s", nameTest.c_str());

			// if you set a value to a node name that doesn't exist, it's created. after running this you will see it listed in the form:
			uint writeValue;

			// first lets see if we can find it:
			res=root.getValueByName(writeValue,".WriteVal");
			if(res) // if it existed, save the value and increment it.
				writeValue++;
			else
				writeValue=3;

			// now set the value. created tells us if the value was creaed or just updated.
			bool created;
			root.setValueByName(writeValue,".WriteVal",&created);
			if(created) {
				nlinfo("WriteVal wasn't found and was created.");
			} else {
				nlinfo("Updated WriteVal, it already existed in the form.");
			}

			// and finally save the form out in case we made changes.
			// if you're accessing a form read-only (not using set*) you can skip this.
			NLMISC::COFile saveSample(sampleConfigFile);
			form->write(saveSample, false);
			nlinfo("Saved sample config file.");
		} else {
			// CPath didn't find the file, just print an error and exit.
			nlinfo("Couldn't find the config file.");
			exit(1);
		}

		// finished, unload the form loader.
		NLGEORGES::UFormLoader::releaseLoader(formLoader);

	} catch(...) {
		// something went wrong, unload the form loader.
		nlinfo("Caught an exception, quitting.");
		NLGEORGES::UFormLoader::releaseLoader(formLoader);
	}
	
}
