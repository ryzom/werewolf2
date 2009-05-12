// Local includes
#include "CNeLBBClusterWriter.h"
#include "general.h"
#include "CArgs.h"

// NeL includes
#include <nel/misc/file.h>

// Maya includes
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MObject.h>
#include <maya/MItDag.h>
#include <maya/MFnMesh.h>
#include <maya/MColorArray.h>
#include <maya/MObjectArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnTransform.h>

CNeLBBClusterWriter::CNeLBBClusterWriter(const MDagPath path, MStatus &status) {
	mPath = path;
	mbHasMaterial = false;
	mClusterBuild = NULL;
	mLODNode.setObject(path.node());
	if(CArgs::instance().verboseMode)
		printf("Billboard Cluster: %s\n", mLODNode.partialPathName().asChar());
	status = MStatus::kSuccess;
}

void CNeLBBClusterWriter::init() {
	extractLOD();
	makeClusterBuild();
}

CNeLBBClusterWriter::~CNeLBBClusterWriter() {
	mLODGroups.clear();
	if(mClusterBuild) {
		delete mClusterBuild;
		mClusterBuild = NULL;
	}
}

MStatus CNeLBBClusterWriter::writeToFile(MString f) {
	NLMISC::COFile file;
	NL3D::CBBClusterShape *bbCluster = createShape();
	bbCluster->build(mClusterBuild);
	f += ".shape";
	if(file.open(f.asChar())) {
		try {
			NL3D::CShapeStream streamableMesh (bbCluster);
			streamableMesh.serial (file);
		}
		catch(...) {
			return MStatus::kFailure;
		}
		file.close ();
	}
	return MStatus::kSuccess;
}

MStatus CNeLBBClusterWriter::extractLOD() {
	float distMax = 1000;
	mLODNode.findPlug("DistMax").getValue(distMax);
	MPlug outPlug = mLODNode.findPlug("output");
	for(unsigned int i = 0; i < outPlug.numElements(); i++) {
		MPlugArray conPlugs;
		outPlug[i].connectedTo(conPlugs, false, true);
		if(conPlugs.length() > 0) {
			MFnDagNode node(conPlugs[0].node());
			MDagPath transformPath;
			node.getPath(transformPath);
			MItDag it(MItDag::kDepthFirst, MFn::kTransform);
			it.reset(transformPath, MItDag::kDepthFirst, MFn::kTransform);
			for(;!it.isDone();it.next()) {
				ClusterLODGroup slot;
				it.getPath(slot.path);
				MFnDagNode slotNode(slot.path);
				if(slotNode.isIntermediateObject() || !isSlot(slot.path)) {
					continue;
				}
				if(i != (outPlug.numElements()-1)) {
					MPlug distPlug = mLODNode.findPlug("threshold");
					distPlug[i].getValue(slot.dist);
				} else {
					slot.dist = distMax;
				}
				if(CArgs::instance().verboseMode)
					printf("\tGot slot: %s %f\n", it.partialPathName().asChar(), slot.dist);
				mLODGroups.push_back(slot);
			}
		}
	}
	return MStatus::kSuccess;
}

MStatus CNeLBBClusterWriter::makeClusterBuild() {
	if(mClusterBuild) {
		delete mClusterBuild;
		mClusterBuild = NULL;
	}
	createBuild();
	float radius = 1;
	mLODNode.findPlug("Radius").getValue(radius);
	mClusterBuild->Radius = radius;

	bool isTransparent = false;
	for(unsigned int i = 0; i < mLODGroups.size(); i++) {
		MStatus status;
		status = addSlot(i);
		if(status != MStatus::kSuccess)
			return status;
	}
	return MStatus::kSuccess;
}

MStatus CNeLBBClusterWriter::addSlot(int index) {
	MStatus status;
	NL3D::CBBClusterShape::CBBClusterBuild::CBBBuildSlot* slot = createSlot();
	MFnDagNode mslot(mLODGroups[index].path);
	slot->DistMax = mLODGroups[index].dist;
	bool useBBColors = false;
	bool useBBOrientations = false;
	bool useBBSizes = false;
	bool useRadialNormals = false;
	bool LodBlendIn = false;
	bool LodBlendOut = false;
	mslot.findPlug("useBBColors").getValue(useBBColors);
	mslot.findPlug("useBBSizes").getValue(useBBSizes);
	mslot.findPlug("useBBOrientations").getValue(useBBOrientations);
	mslot.findPlug("useRadialNormals").getValue(useRadialNormals);
	mslot.findPlug("LodBlendIn").getValue(LodBlendIn);
	mslot.findPlug("LodBlendOut").getValue(LodBlendOut);
	slot->Flags = 0;
	if(useBBColors)
		slot->Flags |= NL3D::CBBClusterShape::UseBBColors;
	if(useBBSizes)
		slot->Flags |= NL3D::CBBClusterShape::UseBBSizes;
	if(useBBOrientations)
		slot->Flags |= NL3D::CBBClusterShape::UseBBOrientations;
	if(useRadialNormals)
		slot->Flags |= NL3D::CBBClusterShape::UseRadialNormals;
	if(LodBlendIn)
		slot->Flags |= NL3D::CBBClusterShape::BlendIn;
	if(LodBlendOut)
		slot->Flags |= NL3D::CBBClusterShape::BlendOut;

	float LodBlendLength = 0;
	NLMISC::CVector2f BBRadius(0.5, 0.5);
	int Alignment = 1;

	mslot.findPlug("LodBlendLength").getValue(LodBlendLength);
	slot->BlendLength = LodBlendLength;

	mslot.findPlug("BBRadiusX").getValue(BBRadius.x);
	mslot.findPlug("BBRadiusZ").getValue(BBRadius.y);
	slot->BBRadius = BBRadius;

	mslot.findPlug("Alignment").getValue(Alignment);
	slot->Alignment = (NL3D::CBBClusterShape::TAlignment)Alignment;

	uint numBBs = mLODGroups[index].path.childCount();
	for(uint bb = 0; bb < numBBs; bb++) {
		MDagPath bbPath = mLODGroups[index].path;
		bbPath.push(bbPath.child(bb, &status));
		if(status != MStatus::kSuccess)
			return status;
		if(!isBB(bbPath))
			continue;
		MFnTransform bbNode(bbPath);
		MVector pos = bbNode.translation(MSpace::kTransform);
		slot->BBPositions.push_back(NLMISC::CVector((float)pos.x, (float)pos.y, (float)pos.z));
		if(useBBSizes) {
			double scale[3];
			bbNode.getScale(scale);
			slot->BBSizes.push_back(NLMISC::CVector2f((float)scale[0]/2.f, (float)scale[2]/2.f));
		}
		if(useBBOrientations) {
			float orientation = 0;
			bbNode.findPlug("Orientation").getValue(orientation);
			slot->BBOrientations.push_back(orientation);
		}
		if(useBBColors) {
			MFnMesh bbMesh(bbPath);
			MColorArray colors;
			bbMesh.getVertexColors(colors);
			NLMISC::CRGBA color(255, 255, 255, 255);
			if(colors.length() > 0)
				color = mColor2CRGBA(colors[0]);
			slot->BBColors.push_back(color);
		}
		if(slot->Alignment == NL3D::CBBClusterShape::eAxisConstrained) {
			NLMISC::CVector axis(0, 0, 1);
			bbNode.findPlug("ConstraintAxisX").getValue(axis.x);
			bbNode.findPlug("ConstraintAxisY").getValue(axis.y);
			bbNode.findPlug("ConstraintAxisZ").getValue(axis.z);
			axis.normalize();
			printf("\tGot axis %f %f %f\n", axis.x, axis.y, axis.z);
			slot->BBConstrainAxes.push_back(axis);
		}
		if(!mbHasMaterial) {
			MFnMesh bbMesh(bbPath);
			MDagPath tPath;
			bbMesh.getPath(tPath);
			MObjectArray shaders;
			MIntArray indices;
			if (MStatus::kFailure == bbMesh.getConnectedShaders(tPath.instanceNumber(), shaders, indices)) {
				MGlobal::displayError("MFnMesh::getConnectedShaders"); 
				return MStatus::kFailure;
			}
			if(shaders.length() > 0) {
				getMaterial(&(mClusterBuild->Mat), bbMesh, shaders[0]);
				mbHasMaterial = true;
			}
		}
	}
	
	if(mClusterBuild->Mat.getBlend()) {
		slot->Flags |= NL3D::CBBClusterShape::IsTransparent;
	} else {
		slot->Flags |= NL3D::CBBClusterShape::IsOpaque;
	}
	addExtraSlot(index, slot);

	mClusterBuild->addLod(slot);

	return MStatus::kSuccess;
}
