// Copyright (C) Full Moon Studios
// File: pluginMain.cpp
// Author: Henri Kuuste

#include "NeLPatchPainterNode.h"

#include <maya/MFnPlugin.h>

MStatus initializePlugin( MObject obj ) { 
	MStatus   status;
	const MString UserClassify( "texture/2d" );
	MFnPlugin plugin( obj, "Full Moon Studios", "6.0", "Any");

	status = plugin.registerNode( "NelPatchPainter", NeLPatchPainter::id, NeLPatchPainter::creator,
								  NeLPatchPainter::initialize, MPxNode::kDependNode, &UserClassify );
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj) {
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterNode( NeLPatchPainter::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}

	return status;
}
