// Local includes
#include "CNeLMultiLODWriter.h"
#include "general.h"
#include "CArgs.h"

// NeL includes
#include <nel/misc/file.h>

// Maya includes
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MObject.h>
#include <maya/MItDag.h>

CNeLMultiLODWriter::CNeLMultiLODWriter(const MDagPath path, MStatus &status) {
	mPath = path;
	mLODNode.setObject(path.node());
	if(CArgs::instance().verboseMode)
		printf("Lod group: %s\n", mLODNode.partialPathName().asChar());
	extractLOD();
	makeLODBuild();
	status = MStatus::kSuccess;
}

CNeLMultiLODWriter::~CNeLMultiLODWriter() {
	mNeLMeshes.clear();
}

MStatus CNeLMultiLODWriter::writeToFile(MString f) {
	NLMISC::COFile file;
	NL3D::CMeshMultiLod multiMesh;
	multiMesh.build (mMultiLodBuild);
	f += ".shape";
	if(file.open(f.asChar())) {
		try {
			NL3D::CShapeStream streamableMesh (&multiMesh);
			streamableMesh.serial (file);
		}
		catch(...) {
			return MStatus::kFailure;
		}
		file.close ();
	}
	return MStatus::kSuccess;
}

MStatus CNeLMultiLODWriter::extractLOD() {
	MPlug outPlug = mLODNode.findPlug("output");
	for(unsigned int i = 0; i < outPlug.numElements(); i++) {
		MPlugArray conPlugs;
		outPlug[i].connectedTo(conPlugs, false, true);
		if(conPlugs.length() > 0) {
			MFnDagNode node(conPlugs[0].node());
			MDagPath transformPath;
			node.getPath(transformPath);
			MItDag it(MItDag::kDepthFirst, MFn::kMesh);
			it.reset(transformPath, MItDag::kDepthFirst, MFn::kMesh);
			for(;!it.isDone();it.next()) {
				LODMesh mesh;
				it.getPath(mesh.path);
				MFnDagNode meshNode(mesh.path);
				if(meshNode.isIntermediateObject() || meshNode.object().apiType() != MFn::kMesh) {
					continue;
				}
				if(i != (outPlug.numElements()-1)) {
					MPlug distPlug = mLODNode.findPlug("threshold");
					distPlug[i].getValue(mesh.dist);
				} else {
					mesh.dist = 100000;
				}
				if(CArgs::instance().verboseMode)
					printf("\tGot mesh: %s %f\n", it.partialPathName().asChar(), mesh.dist);
				mLODMeshes.push_back(mesh);
			}
		}
	}
	return MStatus::kSuccess;
}

MStatus CNeLMultiLODWriter::makeLODBuild() {
	mMultiLodBuild.LodMeshes.resize(mLODMeshes.size());
	bool isTransparent = false;
	mMultiLodBuild.StaticLod = false;
	for(unsigned int i = 0; i < mLODMeshes.size(); i++) {
		MStatus status;
		CNeLMesh lodMesh(mLODMeshes[i].path, status);

		if(status != MStatus::kFailure) {
			lodMesh.extract(CArgs::instance().dumpMode);
			if(i == 0) {
				mMultiLodBuild.BaseMesh = *(lodMesh.getMeshBaseBuild());
				for(unsigned int j = 0; j < mMultiLodBuild.BaseMesh.Materials.size(); j++) {
					if(mMultiLodBuild.BaseMesh.Materials[j].getBlend()) {
						isTransparent = true;
						break;
					}
				}
			}
			NL3D::CMesh::CMeshBaseBuild* cBaseMesh = lodMesh.getMeshBaseBuild();
			for(unsigned int j = 0; j < cBaseMesh->BSNames.size(); j++) {
				mMultiLodBuild.BaseMesh.BSNames.push_back(cBaseMesh->BSNames[j]);
				mMultiLodBuild.BaseMesh.DefaultBSFactors.push_back(cBaseMesh->DefaultBSFactors[j]);
			}
			mMultiLodBuild.LodMeshes[i].MeshGeom = lodMesh.getMeshGeom(mMultiLodBuild.BaseMesh.Materials.size());
			mMultiLodBuild.LodMeshes[i].DistMax = mLODMeshes[i].dist;
			mMultiLodBuild.LodMeshes[i].Flags = (isTransparent ? NL3D::CMeshMultiLod::CMeshMultiLodBuild::CBuildSlot::IsTransparent : NL3D::CMeshMultiLod::CMeshMultiLodBuild::CBuildSlot::IsOpaque);
			MFnDagNode mesh(getTransformPath(mLODMeshes[i].path, status));
			mesh.attribute("NeLLod", &status);
			if(status == MStatus::kSuccess) {
				printf("\tFound NeL LOD\n");
				mesh.findPlug("LodBlendLength").getValue(mMultiLodBuild.LodMeshes[i].BlendLength);
				bool blendIn = false;
				bool blendOut = false;
				mesh.findPlug("LodBlendIn").getValue(blendIn);
				mesh.findPlug("LodBlendOut").getValue(blendOut);
				if(blendIn)
					mMultiLodBuild.LodMeshes[i].Flags |= NL3D::CMeshMultiLod::CMeshMultiLodBuild::CBuildSlot::BlendIn;
				if(blendOut)
					mMultiLodBuild.LodMeshes[i].Flags |= NL3D::CMeshMultiLod::CMeshMultiLodBuild::CBuildSlot::BlendOut;
			}

		} else {
			MGlobal::displayError("Building CNeLMesh failed");
		}
	}
	return MStatus::kSuccess;
}
