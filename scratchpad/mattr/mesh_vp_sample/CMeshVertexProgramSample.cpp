/**
 * \file CMeshVertexProgramSample.cpp
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

//
// System Includes
//
#include "stdpch.h"
#include <string>

//
// NeL Includes
//
#include <nel/misc/types_nl.h>
#include <nel/misc/debug.h>
#include <nel/misc/log.h>
#include <nel/misc/path.h>
#include <nel/3d/u_scene.h>
#include <nel/3d/u_instance.h>
#include <nel/3d/u_skeleton.h>
#include <nel/3d/u_text_context.h>
#include <nel/3d/u_texture.h>
#include <nel/misc/rgba.h>

//
// Werewolf Includes
//
#include "CMeshVertexProgramSample.h"

//
// Namespaces
//

//
// Global Variables
//

//
// Class
//

CMeshVertexProgramSample::CMeshVertexProgramSample() {
	;
}

CMeshVertexProgramSample::~CMeshVertexProgramSample() {
	;
}

void CMeshVertexProgramSample::initInstance(NL3D::CMeshBaseInstance *mbi) {
	;
}

bool CMeshVertexProgramSample::begin(NL3D::IDriver *drv, NL3D::CScene *scene, NL3D::CMeshBaseInstance *mbi, const NLMISC::CMatrix &invertedModelMat, const NLMISC::CVector &viewerPos) {
	nlinfo("Begin CMeshVertexProgramSample");
	return true;
}

void CMeshVertexProgramSample::end(NL3D::IDriver *drv) {
	nlinfo("Begin CMeshVertexProgramSample");
}

void CMeshVertexProgramSample::setupForMaterial(const NL3D::CMaterial &mat, NL3D::IDriver *drv, NL3D::CScene *scene, NL3D::CVertexBuffer *vb) {
	;
}

void CMeshVertexProgramSample::serial(NLMISC::IStream &f) {
	;
}

std::string CMeshVertexProgramSample::getClassName() {
	return "CMeshVertexProgramSample";
}
