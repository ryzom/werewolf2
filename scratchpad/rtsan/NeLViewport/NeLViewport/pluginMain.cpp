/**
 * \file pluginMain.cpp
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

#include <maya/MFnPlugin.h>
#include <CNeLViewport.h>

static CNeLViewport *g_NeLViewport = 0;

MStatus initializePlugin( MObject obj ) { 
	MStatus   status = MStatus::kFailure;
	MFnPlugin plugin( obj, "Ironic Entertainment", "2008", "Any");

	g_NeLViewport = new CNeLViewport();
	if (g_NeLViewport)
	{
		status = g_NeLViewport->registerRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to register NeL viewport properly.");
		}
	}

	return status;
}

MStatus uninitializePlugin( MObject obj ) {
	MFnPlugin plugin( obj );
	MStatus   status = MStatus::kSuccess;

	if (g_NeLViewport)
	{
		status = g_NeLViewport->deregisterRenderer();
		if (status != MStatus::kSuccess)
		{
			status.perror("Failed to deregister NeL viewport properly.");
		}
	}
	g_NeLViewport = 0;
	return status;
}
