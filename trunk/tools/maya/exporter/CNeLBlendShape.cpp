#include "CNeLBlendShape.h"
#include "CArgs.h"
#include "general.h"

#include <maya/MGlobal.h>

CNeLBlendShape::CNeLBlendShape(MDagPath dagPath, MStatus& status, CNeLMesh *parent) : CNeLMesh(dagPath, status) {
	mParent = parent;
}

void CNeLBlendShape::extract(bool verbose) {
	if(!mTransform)
		return;
	CNeLMesh::extract(verbose);
	makeBlendShape();
}

CNeLBlendShape::~CNeLBlendShape() {
}

MStatus CNeLBlendShape::makeBlendShape() {
	MStatus status;
	NL3D::CMesh::CMeshBuild* parentBuildMesh = mParent->getMeshBuild();

	if (parentBuildMesh->Vertices.size() != mBuildMesh.Vertices.size()) {
		MGlobal::displayError("Morph target does not have matching vertex count");
		return MStatus::kFailure;
	}

	if (parentBuildMesh->Faces.size() != mBuildMesh.Faces.size()) {
		MGlobal::displayError("Morph target does not have matching polygon count");
		return MStatus::kFailure;
	}

	uint32 nNbVertVB = 0;
	for (unsigned int i = 0; i < parentBuildMesh->VertLink.size(); ++i)
		if (parentBuildMesh->VertLink[i].VertVB > nNbVertVB)
			nNbVertVB = parentBuildMesh->VertLink[i].VertVB;
	++nNbVertVB;

	mBlendShape.Name = mTransform->name().asChar();

	// Following code ripped from the max exporter
	bool bIsDeltaPos = false;
	mBlendShape.deltaPos.resize (nNbVertVB, NLMISC::CVector::Null);
	bool bIsDeltaNorm = false;
	mBlendShape.deltaNorm.resize (nNbVertVB, NLMISC::CVector::Null);
	bool bIsDeltaUV = false;
	mBlendShape.deltaUV.resize (nNbVertVB, NLMISC::CUV(0.0f,0.0f));
	bool bIsDeltaCol = false;
	mBlendShape.deltaCol.resize (nNbVertVB, NLMISC::CRGBAF(0.0f,0.0f,0.0f,0.0f));
	bool bIsDeltaTgSpace = false;			
	mBlendShape.VertRefs.resize (nNbVertVB, 0xffffffff);

	unsigned int j;
	for (j = 0; j < parentBuildMesh->VertLink.size(); ++j) {
		uint32 nFace = parentBuildMesh->VertLink[j].nFace;
		uint32 nCorner = parentBuildMesh->VertLink[j].nCorner;
		uint32 VertRef = parentBuildMesh->Faces[nFace].Corner[nCorner].Vertex;
//		uint32 VertTar = mBuildMesh.Faces[nFace].Corner[nCorner].Vertex;
		uint32 VertTar = VertRef;
		uint32 nCornerTar;
		for(int c = 0; c < 3; c++) {
			if(mBuildMesh.Faces[nFace].Corner[c].Vertex == VertTar) {
				nCornerTar = c;
				break;
			}
		}
		uint32 iVB = parentBuildMesh->VertLink[j].VertVB;

		NLMISC::CVector delta = mBuildMesh.Vertices[VertTar] - parentBuildMesh->Vertices[VertRef];
		if (delta.norm() > 0.001f) {
			mBlendShape.deltaPos[iVB] = delta;
			mBlendShape.VertRefs[iVB] = iVB;
			bIsDeltaPos = true;
		}

		if (parentBuildMesh->InterfaceVertexFlag.size() == 0 || !parentBuildMesh->InterfaceVertexFlag[VertRef])
		{	
			NLMISC::CVector NormRef = parentBuildMesh->Faces[nFace].Corner[nCorner].Normal;
			NLMISC::CVector NormTar = mBuildMesh.Faces[nFace].Corner[nCornerTar].Normal;
			delta = NormTar - NormRef;
			if (delta.norm() > 0.001f) {
				mBlendShape.deltaNorm[iVB] = delta;
				mBlendShape.VertRefs[iVB] = iVB;
				bIsDeltaNorm = true;
			}
		}
		
		NLMISC::CUV UVRef = (NLMISC::CUV) parentBuildMesh->Faces[nFace].Corner[nCorner].Uvws[0];
		NLMISC::CUV UVTar = (NLMISC::CUV) mBuildMesh.Faces[nFace].Corner[nCornerTar].Uvws[0];
		NLMISC::CUV deltaUV = UVTar - UVRef;
		if ((deltaUV.U*deltaUV.U + deltaUV.V*deltaUV.V) > 0.0001f) {
			mBlendShape.deltaUV[iVB] = deltaUV;
			mBlendShape.VertRefs[iVB] = iVB;
			bIsDeltaUV = true;
		}
		
		NLMISC::CRGBAF RGBARef = parentBuildMesh->Faces[nFace].Corner[nCorner].Color;
		NLMISC::CRGBAF RGBATar = mBuildMesh.Faces[nFace].Corner[nCornerTar].Color;
		NLMISC::CRGBAF deltaRGBA = RGBATar - RGBARef;
		if ((deltaRGBA.R*deltaRGBA.R + deltaRGBA.G*deltaRGBA.G + deltaRGBA.B*deltaRGBA.B + deltaRGBA.A*deltaRGBA.A) > 0.0001f) {
			mBlendShape.deltaCol[iVB] = deltaRGBA;
			mBlendShape.VertRefs[iVB] = iVB;
			bIsDeltaCol = true;
		}
	}

	sint32 nNbVertUsed = nNbVertVB;
	sint32 nDstPos = 0;
	for (j = 0; j < nNbVertVB; ++j) {
		if (mBlendShape.VertRefs[j] == 0xffffffff) {
			--nNbVertUsed;
		} else {
			if (nDstPos != (sint32) j) {
				mBlendShape.VertRefs[nDstPos]	= mBlendShape.VertRefs[j];
				mBlendShape.deltaPos[nDstPos]	= mBlendShape.deltaPos[j];
				mBlendShape.deltaNorm[nDstPos]	= mBlendShape.deltaNorm[j];
				mBlendShape.deltaUV[nDstPos]	= mBlendShape.deltaUV[j];
				mBlendShape.deltaCol[nDstPos]	= mBlendShape.deltaCol[j];
			}
			++nDstPos;
		}
	}

	if (bIsDeltaPos)
		mBlendShape.deltaPos.resize (nNbVertUsed);
	else
		mBlendShape.deltaPos.resize (0);

	if (bIsDeltaNorm)
		mBlendShape.deltaNorm.resize (nNbVertUsed);
	else
		mBlendShape.deltaNorm.resize (0);

	if (bIsDeltaUV)
		mBlendShape.deltaUV.resize (nNbVertUsed);
	else
		mBlendShape.deltaUV.resize (0);
	
	if (bIsDeltaCol)
		mBlendShape.deltaCol.resize (nNbVertUsed);
	else
		mBlendShape.deltaCol.resize (0);
	
	mBlendShape.VertRefs.resize (nNbVertUsed);

	return MStatus::kSuccess;
}

void CNeLBlendShape::exportFaces() {
	MStatus status;
	MItMeshPolygon	itPolygon(*mDagPath, MObject::kNullObj, &status);
	if(status != MStatus::kSuccess)  {
		MGlobal::displayError("MItMeshPolygon");
		return;
	}
	MItMeshPolygon itParentPolygon(*(mParent->getDagPath()), MObject::kNullObj, &status);
	if(status != MStatus::kSuccess) {
		MGlobal::displayError("MItMeshPolygon");
		return;
	}
	setNumTriangles(itPolygon);
	unsigned int triangle = 0;
	for ( ; !itPolygon.isDone(); itPolygon.next(), itParentPolygon.next()) {
		MIntArray polyVertices;
		itPolygon.getVertices(polyVertices);
		int numTriangles;
		itPolygon.numTriangles(numTriangles);
		if(CArgs::instance().dumpMode)
			printf("\tPolygon: %i\n", itPolygon.index());
		while(numTriangles--) {
			MPointArray nonTweaked;
			MIntArray triangleVertices;
			// make sure we use the same triangulation as our base shape
			itParentPolygon.getTriangle(numTriangles, nonTweaked, triangleVertices, MSpace::kObject);
			MIntArray localVertices;
			getLocalIndices(localVertices, triangleVertices, polyVertices);
               buildTriangle(triangle, itPolygon, triangleVertices, localVertices);
			triangle++;
		}
	}
}
