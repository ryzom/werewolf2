/**
 * \file CNeLViewport.h
 * \date October 2007
 * \author Henri Kuuste
 */

/* Copyright, 2007 Ironic Entertainment
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

#ifndef __CNELVIEWPORT_H__
#define __CNELVIEWPORT_H__

#include <maya/MViewportRenderer.h>

class MDagPath;
class MBoundingBox;
class MQuaternion;
class MVector;
class MSpace;

namespace NL3D
{
	class UDriver;
	class UScene;
}

class CNeLViewport : public MViewportRenderer
{
public:
	CNeLViewport();
	virtual ~CNeLViewport();

	// Required virtual overrides from MViewportRenderer
	//
	virtual	MStatus	initialize();
	virtual	MStatus	uninitialize();
	virtual MStatus	render( const MRenderingInfo &renderInfo );
	virtual bool	nativelySupports( MViewportRenderer::RenderingAPI api, 
										  float version );
	virtual bool	override( MViewportRenderer::RenderingOverride override );

protected:
	bool			renderToTarget( const MRenderingInfo &renderInfo );
	void			drawBounds( const MDagPath &dagPath, const MBoundingBox &box);
	MStatus			getTransforms(MDagPath path, double scale[3], MQuaternion &rot, MVector &trans, MSpace::Space spc, bool rhs = false); 

	RenderingAPI	m_API;		// Rendering API
	float			m_Version;	// OpenGL version number as float.
	NL3D::UDriver	*m_Driver;
	NL3D::UScene	*m_Scene;
	unsigned int	m_width;
	unsigned int	m_height;
};

#endif // __CNELVIEWPORT_H__
