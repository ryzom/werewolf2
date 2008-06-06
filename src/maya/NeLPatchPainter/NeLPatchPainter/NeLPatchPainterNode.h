// Copyright (C) Full Moon Studios
// File: NeLPatchPainterNode.h
// Dependency Graph Node: NeLPatchPainter
// Author: Henri Kuuste

#ifndef _NeLPatchPainterNode
#define _NeLPatchPainterNode

#include <maya/MPxNode.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MTypeId.h> 
#include <maya/MFloatVector.h>

 
class NeLPatchPainter : public MPxNode
{
public:
						NeLPatchPainter();
	virtual				~NeLPatchPainter(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );

	static  void*		creator();
	static  MStatus		initialize();

			int			getTileIndex(float u, float v);
			int			getPatchIndex(float u, float v);
public:

	static  MObject		aTiles;
	static	MObject		aTileLayer0, aTileLayer1, aTileLayer2;
	static	MObject		aTileColors;
	static	MObject		aPatches;
	static	MObject		aNumTilesU, aNumTilesV;
	static	MObject		aNumPatchesU, aNumPatchesV;
	static	MObject		aURange, aVRange;
	static	MObject		aUVCoord;
	static	MObject		aTileBank;
	static	MObject		aVerbose;

	static  MObject		aOutColor;
	static	MObject		aDummy;
	static	MObject		aConnectionToZone;

	static	MTypeId		id;

protected:

			int				m_NumTilesU;
			int				m_NumTilesV;
			int				m_NumPatchesU;
			int				m_NumPatchesV;
			float2			m_URange;
			float2			m_VRange;
			float2			m_PatchRange;
			int				m_Dummy;
			bool			m_Verbose;
};

#endif
