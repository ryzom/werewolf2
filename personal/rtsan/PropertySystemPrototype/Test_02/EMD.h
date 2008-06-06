/**
 * \file EMD.h
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

#ifndef __EMD_H__
#define __EMD_H__

//
// Standard Includes
//

//
// System Includes
//
#include <hash_map>
#include <boost/python.hpp>

//
// NeL Includes
//

//
// Werewolf Includes
//
#include <wwproperty/IDirtyPropagator.h>
#include "MeshGroup.h"
#include "general.h"

//
// Namespaces
//

class SOB;

class EMD : public WWPROPERTY::IDirtyPropagator {
public:
	EMD(SOB* sob) : m_Owner(sob) { ; }
	virtual ~EMD();

	void update();

	void showMesh(const char* group, const char* mesh);
	void hideMesh(const char* group, const char* mesh);

	void addMeshGroup(MeshGroup* meshGrp);
	void removeMeshGroup(const char* name);

	void addController(IMediaController* controller);
	void removeController(const char* name);

	SOB* getOwner() { return m_Owner; }

	typedef std::hash_map<const char*, IMediaController*, std::hash<const char*>, streqpred> mediaControllerMap;
	typedef std::hash_map<const char*, MeshGroup*, std::hash<const char*>, streqpred> meshGroupMap;

	static void init_script_class();

private:
	meshGroupMap m_MeshGroups;
	mediaControllerMap m_Controllers;
	SOB* m_Owner;
};

#endif // __EMD_H__
