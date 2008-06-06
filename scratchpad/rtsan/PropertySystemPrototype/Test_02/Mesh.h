/**
 * \file Mesh.h
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

#ifndef __MESH_H__
#define __MESH_H__

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
#include "general.h"
#include "IMediaController.h"

//
// Namespaces
//

class EMD;

class Mesh : public WWPROPERTY::IDirtyPropagator {
public:
	Mesh(const char* name, EMD* emd) : m_Name(name), m_EMD(emd), m_Visible(false) { ; }
	virtual ~Mesh();

	void update();

	const char* getMeshName() { return m_Name; }

	void addController(IMediaController* controller);
	void removeController(const char* name);

	bool isVisible() { return m_Visible; }
	void show() { m_Visible = true; }
	void hide() { m_Visible = false; }

	EMD* getEMD() { return m_EMD; }

	void morph(const char* morphName, float morph) {
		printf("Morphing %s.%s to %f\n", m_Name, morphName, morph);
	}

	static void init_script_class();

	typedef std::hash_map<const char*, IMediaController*, std::hash<const char*>, streqpred> mediaControllerMap;
private:
	mediaControllerMap m_Controllers;
	const char* m_Name;
	EMD* m_EMD;
	bool m_Visible;
};

#endif // __MESH_H__
