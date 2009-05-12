// Copyright (C) Full Moon Studios
// File: NeLPatchPainterNode.cpp
// Dependency Graph Node: NeLPatchPainter
// Author: Henri Kuuste

#include "NeLPatchPainterNode.h"

#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MFnCompoundAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnDagNode.h>
#include <math.h>

#include <maya/MGlobal.h>

#include <iostream>
using std::cerr;
using std::endl;

#define MAKE_INPUT(attr)	\
    CHECK_MSTATUS(attr.setKeyable(true) );		\
    CHECK_MSTATUS(attr.setStorable(true) );		\
    CHECK_MSTATUS(attr.setReadable(true) );		\
    CHECK_MSTATUS(attr.setWritable(true) );

#define MAKE_OUTPUT(attr)	\
    CHECK_MSTATUS(attr.setKeyable(false) );		\
    CHECK_MSTATUS(attr.setStorable(false) );	\
    CHECK_MSTATUS(attr.setReadable(true) ); 	\
    CHECK_MSTATUS(attr.setWritable(false) );

MTypeId     NeLPatchPainter::id( 0x00040 );

MObject     NeLPatchPainter::aTiles;
MObject		NeLPatchPainter::aTileLayer0;
MObject		NeLPatchPainter::aTileLayer1;
MObject		NeLPatchPainter::aTileLayer2;
MObject		NeLPatchPainter::aTileColors;
MObject		NeLPatchPainter::aPatches;
MObject		NeLPatchPainter::aNumPatchesU;
MObject		NeLPatchPainter::aNumPatchesV;
MObject		NeLPatchPainter::aUVCoord;
MObject		NeLPatchPainter::aURange;
MObject		NeLPatchPainter::aVRange;
MObject		NeLPatchPainter::aTileBank;
MObject		NeLPatchPainter::aVerbose;
MObject		NeLPatchPainter::aConnectionToZone;

MObject     NeLPatchPainter::aOutColor;
MObject		NeLPatchPainter::aDummy;
MObject		NeLPatchPainter::aNumTilesU;
MObject		NeLPatchPainter::aNumTilesV;

NeLPatchPainter::NeLPatchPainter() { m_Dummy = 0; }
NeLPatchPainter::~NeLPatchPainter() {}

void* NeLPatchPainter::creator() {
	return new NeLPatchPainter();
}

MStatus NeLPatchPainter::initialize() {
	MFnNumericAttribute nAttr;
	MFnCompoundAttribute cAttr;
	MFnTypedAttribute tAttr;
	// Make inputs

	MObject UVChild1 = nAttr.create( "uCoord", "u", MFnNumericData::kFloat);
    MObject UVChild2 = nAttr.create( "vCoord", "v", MFnNumericData::kFloat);
    aUVCoord = nAttr.create( "uvCoord","uv", UVChild1, UVChild2);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(nAttr.setHidden(true));
	CHECK_MSTATUS(addAttribute(aUVCoord));

	MObject URChild1 = nAttr.create( "uMin", "umn", MFnNumericData::kFloat);
    MObject URChild2 = nAttr.create( "uMax", "umx", MFnNumericData::kFloat, 1.0f);
    aURange = nAttr.create( "URange","urg", URChild1, URChild2);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute(aURange));

	aVerbose = nAttr.create("Verbose", "vb", MFnNumericData::kBoolean, 0);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute(aVerbose));

	MObject VRChild1 = nAttr.create( "vMin", "vmn", MFnNumericData::kFloat);
    MObject VRChild2 = nAttr.create( "vMax", "vmx", MFnNumericData::kFloat, 1.0f);
    aVRange = nAttr.create( "VRange","vrg", VRChild1, VRChild2);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(addAttribute(aVRange));

	aTileLayer0 = nAttr.create("TileLayer0", "tl0", MFnNumericData::kInt);
	MAKE_INPUT(nAttr);
//	CHECK_MSTATUS(nAttr.setArray(true));
	aTileLayer1 = nAttr.create("TileLayer1", "tl1", MFnNumericData::kInt);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(nAttr.setArray(true));
	aTileLayer2 = nAttr.create("TileLayer2", "tl2", MFnNumericData::kInt);
	MAKE_INPUT(nAttr);
//	CHECK_MSTATUS(nAttr.setArray(true));

	aTiles = cAttr.create("Tiles", "tl");
	MAKE_INPUT(cAttr);
	CHECK_MSTATUS(cAttr.setArray(true));
	cAttr.addChild(aTileLayer0);
//	cAttr.addChild(aTileLayer1);
	cAttr.addChild(aTileLayer2);

/*	aTiles = nAttr.create("Tiles", "tl", MFnNumericData::kInt);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(nAttr.setArray(true));
*/
	aTileColors = nAttr.createColor("TileColors", "tlc");
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(nAttr.setArray(true));

	aPatches = cAttr.create("Patches", "pch");
	MAKE_INPUT(cAttr);
	CHECK_MSTATUS(cAttr.setArray(true));
//	cAttr.addChild(aTiles);
	cAttr.addChild(aTileLayer1);
//	cAttr.addChild(aTileLayer2);
//	cAttr.addChild(aTileColors);
	CHECK_MSTATUS(addAttribute(aPatches));

	aNumTilesU = nAttr.create("NumTilesU", "ntu", MFnNumericData::kInt, 4);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(nAttr.setMax(16));
	CHECK_MSTATUS(nAttr.setMin(1));
	CHECK_MSTATUS(addAttribute(aNumTilesU));

	aNumTilesV = nAttr.create("NumTilesV", "ntv", MFnNumericData::kInt, 4);
	MAKE_INPUT(nAttr);
	CHECK_MSTATUS(nAttr.setMax(16));
	CHECK_MSTATUS(nAttr.setMin(1));
	CHECK_MSTATUS(addAttribute(aNumTilesV));

	aConnectionToZone = tAttr.create("ConnectionToZone", "ctz", MFnData::kNurbsSurface);
	MAKE_INPUT(tAttr);
//	CHECK_MSTATUS(tAttr.setHidden(true));
	CHECK_MSTATUS(addAttribute(aConnectionToZone));

	aTileBank = tAttr.create("TileBank", "tb", MFnData::kString);
	MAKE_INPUT(tAttr);
	CHECK_MSTATUS(addAttribute(aTileBank));

	// Make outputs
	aNumPatchesU = nAttr.create("NumPatchesU", "npu", MFnNumericData::kInt);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute(aNumPatchesU));

	aNumPatchesV = nAttr.create("NumPatchesV", "npv", MFnNumericData::kInt);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute(aNumPatchesV));

	aOutColor = nAttr.createColor("outColor", "oc");
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute(aOutColor));

	aDummy = nAttr.create("Dummy", "dmy", MFnNumericData::kShort);
	MAKE_OUTPUT(nAttr);
	CHECK_MSTATUS(addAttribute(aDummy));

	// Affect output color
	CHECK_MSTATUS(attributeAffects(aUVCoord, aOutColor));
	CHECK_MSTATUS(attributeAffects(aPatches, aOutColor));
	CHECK_MSTATUS(attributeAffects(aNumTilesU, aOutColor));
	CHECK_MSTATUS(attributeAffects(aNumTilesV, aOutColor));
	CHECK_MSTATUS(attributeAffects(aNumPatchesU, aOutColor));
	CHECK_MSTATUS(attributeAffects(aNumPatchesV, aOutColor));
	CHECK_MSTATUS(attributeAffects(aConnectionToZone, aOutColor));
	CHECK_MSTATUS(attributeAffects(aURange, aOutColor));
	CHECK_MSTATUS(attributeAffects(aVRange, aOutColor));
	CHECK_MSTATUS(attributeAffects(aTileBank, aOutColor));

	CHECK_MSTATUS(attributeAffects(aNumTilesU, aDummy));

	CHECK_MSTATUS(attributeAffects(aConnectionToZone, aNumPatchesU));
	CHECK_MSTATUS(attributeAffects(aConnectionToZone, aNumPatchesV));

	return MS::kSuccess;

}

MStatus NeLPatchPainter::compute( const MPlug& plug, MDataBlock& data ) {
	MStatus stat;
	m_Verbose = data.inputValue(aVerbose).asBool();
	MArrayDataHandle patches = data.inputArrayValue(aPatches, &stat);
//	MGlobal::displayInfo(MString("Compute called for plug: ") + plug.name());

	if( plug == aOutColor ) {
		// NOT USED AT THE MOMENT
		MString tileBank = data.inputValue(aTileBank).asString();

		m_NumPatchesU = data.inputValue(aNumPatchesU).asInt();
		m_NumPatchesV = data.inputValue(aNumPatchesV).asInt();
		m_NumTilesU = data.inputValue(aNumTilesU).asInt();
		m_NumTilesV = data.inputValue(aNumTilesV).asInt();
		float2 &URange = data.inputValue(aURange).asFloat2();
		m_URange[0] = URange[0]; m_URange[1] = URange[1];
		float2 & VRange = data.inputValue(aVRange).asFloat2();
		m_VRange[0] = VRange[0]; m_VRange[1] = VRange[1];
		m_PatchRange[0] = (URange[1]-URange[0])/(float)m_NumPatchesU;
		m_PatchRange[1] = (VRange[1]-VRange[0])/(float)m_NumPatchesV;

		float2 &uv = data.inputValue(aUVCoord, &stat).asFloat2();
		float u = uv[0];
		float v = uv[1];

		int patchIndex = getPatchIndex(u, v);
		int tileIndex = getTileIndex(u, v);

		if(m_Verbose && (patchIndex > ((m_NumPatchesU*m_NumPatchesV) - 2)))
			printf("Looking for patch %i\n", patchIndex);

		MFloatVector result(1, 1, 1);
		// THIS OUTPUTS BAD DATA IF THE NUMBER OF PATCHES IS LARGE
		// ENABLE VERBOSE TO SEE IT
		// SHOULD OUTPUT THE COMPLETE LIST OF PATCH INDICES,
		// BUT INSTEAD SOME INDICES ARE MISSING AND SOME APPEAR MORE THAN ONCE
		do {
			if(m_Verbose && (patchIndex > ((m_NumPatchesU*m_NumPatchesV) - 2)))
				printf("Got patch: %i\n", patches.elementIndex(&stat));
				if(stat != MStatus::kSuccess) {
					printf("^ False index\n");
				}
			}
		} while (patches.next());
		if(m_Verbose && (patchIndex > ((m_NumPatchesU*m_NumPatchesV) - 2)))
			printf("Done Looking\n");
		// SHOULD ACTUALLY DO THIS
/*		if((stat = patches.jumpToElement(patchIndex)) == MStatus::kSuccess) {
			MArrayDataHandle tileColors = patches.inputValue().child(aTileColors);
			if(tileColors.jumpToElement(tileIndex) == MStatus::kSuccess) {
				result = tileColors.inputValue(&stat).asFloatVector();
			}
		}
*/		MDataHandle outColorHandle = data.outputValue( aOutColor );
	    MFloatVector& outColor = outColorHandle.asFloatVector();
		outColor = result;


		data.setClean(plug);
	}
	else if ( plug == aNumPatchesU || plug == aNumPatchesV ) {
		// IS THERE A BETTER WAY TO GET THESE?!
		MDataHandle numPatchesUHandle = data.outputValue( aNumPatchesU );
		int& numPatchesU = numPatchesUHandle.asInt();
		MDataHandle numPatchesVHandle = data.outputValue( aNumPatchesV );
		int& numPatchesV = numPatchesVHandle.asInt();
		numPatchesU = 0;
		numPatchesV = 0;

		MFnDependencyNode fThis(thisMObject());
		MPlug connection = fThis.findPlug(aConnectionToZone);
		MPlugArray connections;
		connection.connectedTo(connections, true, false);
		for(unsigned int i = 0; i < connections.length(); i++) {
			if(connections[i].node().hasFn(MFn::kNurbsSurface)) {
				MFnNurbsSurface surface(connections[i].node());
				numPatchesU = surface.numPatchesInU();
				numPatchesV = surface.numPatchesInV();
			}
		}
		data.setClean( aNumPatchesU );
		data.setClean( aNumPatchesV );
	}
	else if ( plug == aDummy) {
		// THIS WORKS FINE
		// DO A getAttr ON THIS WITH VERBOSE ENABLE TO SEE IT
		printf("Scanning patches %i\n", patches.elementCount());
		do {
			if(m_Verbose) {
				printf("Got patch: %i\n", patches.elementIndex(&stat));
				if(stat != MStatus::kSuccess) {
					printf("^ False index\n");
				}
			}
		} while (patches.next());
	} else {
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}

int NeLPatchPainter::getTileIndex(float u, float v) {
	int iU, iV;
	u -= floor(u/m_PatchRange[0])*m_PatchRange[0];
	v -= floor(v/m_PatchRange[1])*m_PatchRange[1];
	float deltaU = m_PatchRange[0]/(float)m_NumTilesU;
	float deltaV = m_PatchRange[1]/(float)m_NumTilesV;

	iU = (int)floor(u/deltaU);
	iV = (int)floor(v/deltaV);
	return iV*m_NumTilesU + iU;
}

int NeLPatchPainter::getPatchIndex(float u, float v) {
	int iU, iV;
	iU = (int)floor((u-m_URange[0])/m_PatchRange[0]);
	iV = (int)floor((v-m_VRange[0])/m_PatchRange[1]); 
	int result = iV*m_NumPatchesU + iU;
	return result;
}