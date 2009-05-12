// Local includes
#include "CNeLSkeletonWriter.h"
#include "general.h"
#include "CArgs.h"
#include "CMayaUtils.h"

// NeL includes
#include <nel/misc/file.h>

// Maya includes
#include <maya/MItDag.h>
#include <maya/MGlobal.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnIkJoint.h>
#include <maya/MPlug.h>
#include <maya/MObject.h>
#include <maya/MFnMatrixData.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MQuaternion.h>
#include <maya/MVector.h>
#include <maya/MPoint.h>
#include <maya/MMatrix.h>
#include <maya/MEulerRotation.h>
#include <maya/MAngle.h>

CNeLSkeletonWriter::CNeLSkeletonWriter(const MDagPath root, MStatus &status) {
	mRoot = root;
	status = extractSkeleton();
}

CNeLSkeletonWriter::~CNeLSkeletonWriter() {
// does nothing
}

MStatus CNeLSkeletonWriter::writeToFile(MString f) {

	mSkeleton.build (mBones);
	
	MFnDagNode root(mRoot.node());
	f += ".skel";
	NLMISC::COFile file;
	if (file.open (f.asChar()))
	{
		try {
			NL3D::CShapeStream shapeStream (&mSkeleton);
			shapeStream.serial (file);
		}
		catch(...) {
			return MStatus::kFailure;
		}
		file.close ();
	}
	return MStatus::kSuccess;
}

MStatus CNeLSkeletonWriter::extractSkeleton() {
	std::vector<unsigned int> depthMap;
	depthMap.resize(50);
	unsigned int id = 0;
	MAngle c(1);
	MItDag itSkeleton(MItDag::kDepthFirst, MFn::kJoint);
	itSkeleton.reset(mRoot, MItDag::kDepthFirst, MFn::kJoint);
	for(;!itSkeleton.isDone();itSkeleton.next()) {
		MFnIkJoint node(itSkeleton.item());
		NL3D::CBoneBase bone;
		bone.Name = node.partialPathName().asChar();

		depthMap[itSkeleton.depth()] = id;
		if(id != 0) {
			bone.FatherId = depthMap[itSkeleton.depth()-1];
		} else {
			bone.FatherId = -1;
		}
		node.findPlug("segmentScaleCompensate").getValue(bone.UnheritScale);
		// TODO
		// Turning this off for now, because grouping and parenting is not supported.
		// Once we add support, I will turn this back on and Segment Scale Compensate will be supported again.
		bone.UnheritScale = false;
		
		if(CArgs::instance().verboseMode)
			printf("\tBone: %s depth: %i id: %i parent: %i\n", node.partialPathName().asChar(), itSkeleton.depth(), id, bone.FatherId);
		MDagPath path;
		itSkeleton.getPath(path);
	    MTransformationMatrix bindMatrix;
		MPlug bindMatrixPlug = node.findPlug("bindPose");
		MObject bindMatrixObject;
		// is it binded?
	    if( bindMatrixPlug.getValue(bindMatrixObject) != MStatus::kSuccess ) {
			// no
			bindMatrix = path.inclusiveMatrix();
		} else {
			// yes
			if(CArgs::instance().dumpMode)
				printf("\t\tIs skinned\n");

			MFnMatrixData matrixDataFn(bindMatrixObject);
			bindMatrix = matrixDataFn.transformation();
		}


		TNeLTransforms btr = CMayaUtils::getWorldNeLTransforms(bindMatrix, true);
		bone.InvBindPos = NLMISC::CMatrix();
		bone.InvBindPos.translate(btr.pos + btr.pivot);
		bone.InvBindPos.rotate(btr.rot);
		bone.InvBindPos.scale(btr.scale);
		bone.InvBindPos.translate(-btr.pivot);
		if(CArgs::instance().verboseMode) {
			MEulerRotation e(CMayaUtils::getMQuaternion(bone.InvBindPos.getRot()).asEulerRotation() * c.asDegrees());
			printf("\t\tBind rot : %f %f %f\n", e.x, e.y, e.z); 
			printf("\t\tBind trl : %s\n", bone.InvBindPos.getPos().asString().c_str()); 
		}
		bone.InvBindPos.invert();

		// For the root joint, get the world space matrix, for others, get the relative one.
		MTransformationMatrix def;
		if(itSkeleton.depth() == 0) {
			def = path.inclusiveMatrix();// * path.exclusiveMatrixInverse();
		} else {
			def = path.inclusiveMatrix() * path.exclusiveMatrixInverse();
		}
		TNeLTransforms tr = CMayaUtils::getWorldNeLTransforms(def, true);
		bone.DefaultPos = tr.pos;
		bone.DefaultRotQuat = tr.rot;
		bone.DefaultScale = tr.scale;
		bone.DefaultPivot = tr.pivot;
		if(CArgs::instance().verboseMode) {
			MEulerRotation mR(CMayaUtils::getMQuaternion(bone.DefaultRotQuat.getDefaultValue()).asEulerRotation() * c.asDegrees());
			printf("\t\tPosition: %s\n", bone.DefaultPos.getDefaultValue().asString().c_str());
			printf("\t\tRotation: %f %f %f\n", mR.x, mR.y, mR.z);
			printf("\t\tScale   : %s\n", bone.DefaultScale.getDefaultValue().asString().c_str());
			printf("\t\tPivot   : %s\n", bone.DefaultPivot.getDefaultValue().asString().c_str());
		}

		mBones.push_back(bone);

		id++;
	}
	return MStatus::kSuccess;
}