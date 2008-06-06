/**
 * \file CMeshVertexProgramSample.h
 * \date January 2005
 * \author Matt Raykowski
 *
 * This sample demonstrates creating custom vertex programs for any models based
 * CMeshInstance.
 */

/* Copyright, 2005 Werewolf
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

#ifndef __CMESHVERTEXPROGRAMSAMPLE_H__
#define __CMESHVERTEXPROGRAMSAMPLE_H__

//
// System Includes
//
#include <string>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>
#include <nel/misc/log.h>
#include <nel/misc/displayer.h>
#include <nel/misc/window_displayer.h>
#include <nel/misc/path.h>
#include <nel/misc/rgba.h>

//
// NeL Includes
//
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_skeleton.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_texture.h>
#include <3d/mesh_vertex_program.h>

//
// Werewolf Includes
//	

//
// Namespaces
//

//
// Global Variables
//

//
// Class
//

class CMeshVertexProgramSample : public NL3D::IMeshVertexProgram {
public:
	CMeshVertexProgramSample();
	~CMeshVertexProgramSample();

	void initInstance(NL3D::CMeshBaseInstance *mbi);
	bool begin(NL3D::IDriver *drv, NL3D::CScene *scene, NL3D::CMeshBaseInstance *mbi, const NLMISC::CMatrix &invertedModelMat, const NLMISC::CVector &viewerPos);
	void end(NL3D::IDriver *drv);
	void setupForMaterial(const NL3D::CMaterial &mat, NL3D::IDriver *drv, NL3D::CScene *scene, NL3D::CVertexBuffer *vb);

	void serial(NLMISC::IStream &f);
	std::string getClassName();
private:

};

#endif // __CMESHVERTEXPROGRAMSAMPLE_H__