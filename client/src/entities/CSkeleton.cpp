/**
 * \file CSkeleton.cpp
 * \date February 2006
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
#include "entities/CSkeleton.h"
#include "tasks/C3DTask.h"

//
// Namespaces
//

namespace WWCLIENT {

CSkeleton::CSkeleton(NLGEORGES::UFormElm* root) {
	root->getValueByName(FriendlyName,".FriendlyName");
	root->getValueByName(Filename,".Filename");
	nlinfo("Got skeleton %s -> %s", FriendlyName.c_str(), Filename.c_str());
}

CSkeleton::~CSkeleton() {
	if(hasSkeleton())
		C3DTask::instance().scene().deleteSkeleton(skeleton);
}

void CSkeleton::serial(NLMISC::IStream &f) {
	f.serial(FriendlyName);
	f.serial(Filename);
}

bool CSkeleton::hasSkeleton() {
	return !skeleton.empty();
}

void CSkeleton::update() {
	if(isDirty()) {
		updateControllers();
		setClean();
	}
}

}; // END NAMESPACE WWCLIENT
