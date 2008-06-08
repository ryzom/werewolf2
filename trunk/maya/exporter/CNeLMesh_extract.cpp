// Local includes
#include "CNeLMesh.h"
#include "CArgs.h"
#include "general.h"

#include <iostream>

// Maya includes
#include <maya/MGlobal.h>
#include <maya/MAngle.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MMatrix.h>

MStatus CNeLMesh::extractGeometry()  {

	MStatus status;
	if(mbIsSkinned)
		mVertexSpc = MSpace::kWorld;
	if (MStatus::kFailure == mMesh->getPoints(mVertexArray, mVertexSpc)) {
		MGlobal::displayError("MFnMesh::getPoints"); 
		return MStatus::kFailure;
	}

	if (MStatus::kFailure == mMesh->getNormals(mNormalArray, mVertexSpc)) {
		MGlobal::displayError("MFnMesh::getNormals"); 
		return MStatus::kFailure;
	}

	if (MStatus::kFailure == mMesh->getUVSetNames( mUVSets )) {
		MGlobal::displayError("MFnMesh::UVSetNames"); 
		return MStatus::kFailure;
	}

	return MStatus::kSuccess;
}

MStatus CNeLMesh::extractTransform(MSpace::Space spc)  {
	MStatus status;

	return MStatus::kSuccess;
}

MStatus CNeLMesh::extractShaders() {

	if (MStatus::kFailure == mMesh->getConnectedShaders(mDagPath->instanceNumber(), mShaders, mShaderIndices)) {
		MGlobal::displayError("MFnMesh::getConnectedShaders"); 
		return MStatus::kFailure;
	}

	return MStatus::kSuccess;
}