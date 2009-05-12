// Local includes
#include "CNeLLeafClusterWriter.h"
#include "general.h"
#include "CArgs.h"

// NeL includes
#include <nel/misc/file.h>

// Maya includes
#include <maya/MPlug.h>

CNeLLeafClusterWriter::CNeLLeafClusterWriter(const MDagPath path, MStatus &status) : CNeLBBClusterWriter(path, status) {
}

CNeLLeafClusterWriter::~CNeLLeafClusterWriter() {
}

void CNeLLeafClusterWriter::init() {
	extractLOD();
	makeClusterBuild();
}

void CNeLLeafClusterWriter::addExtraSlot(int index, NL3D::CBBClusterShape::CBBClusterBuild::CBBBuildSlot *slot) {
	NL3D::CLeafClusterShape::CLeafClusterBuild::CLeafBuildSlot *lslot = dynamic_cast<NL3D::CLeafClusterShape::CLeafClusterBuild::CLeafBuildSlot *>(slot);
	MFnDagNode mslot(mLODGroups[index].path);

	bool useBBCastShadows = false;
	float useFaceNormal = 0;
	float Translucency = 0;

	mslot.findPlug("useBBCastShadows").getValue(useBBCastShadows);
	if(useBBCastShadows)
		lslot->Flags |= NL3D::CLeafClusterShape::UseBBCastShadows;

	mslot.findPlug("useFaceNormal").getValue(useFaceNormal);
	lslot->useFaceNormal = useFaceNormal;

	mslot.findPlug("Translucency").getValue(Translucency);
	lslot->Translucency = Translucency;
}