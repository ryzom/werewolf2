// Local includes
#include "CNeLMesh.h"
#include "general.h"
#include "CArgs.h"
#include "CMayaUtils.h"

// Standard Includes
#include <map>
#include <utility>

// NeL includes
#include <3d/tangent_space_build.h>
#include <3d/mesh_mrm.h>
#include <3d/mesh_mrm_skinned.h>
#include <nel/misc/file.h>

// Maya includes
#include <maya/MIOStream.h>
#include <maya/MGlobal.h>
#include <maya/MFnSet.h>
#include <maya/MDagPath.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MPlug.h>
#include <maya/MFnBlendShapeDeformer.h>
#include <maya/MItGeometry.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MDagPathArray.h>
#include <maya/MItMeshVertex.h>
#include <maya/MItSelectionList.h>
#include <maya/MAngle.h>


CNeLMesh::CNeLMesh(const MDagPath dagPath, MStatus& status, MSpace::Space vtxSpace) {
	mbNeedVP = mbNeedSpecularVP = mbIsSkinned = false;
	mVertexSpc = vtxSpace;
	mDagPath = new MDagPath(dagPath);
	mTransform = new MFnTransform(getTransformPath(dagPath, status), &status);
	if(status != MStatus::kSuccess)
		mTransform = NULL;
	mMesh = NULL;
}

void CNeLMesh::extract(bool verbose) {
	MStatus status;
	if(!mTransform)
		return;
	if(!verbose) {
		CArgs::instance().push_verbose();
		CArgs::instance().verboseMode = false;
	}
	mMesh = new MFnMesh(*mDagPath, &status);
	mbIsMRM = false;
	MStatus mrmStat;
	mMesh->attribute("MRM", &mrmStat);
	if(mrmStat == MStatus::kSuccess) {
		mbIsMRM = true;
		getMRMParams();
	}
	if(CArgs::instance().verboseMode) {
		printf("Full path name: %s\n", mDagPath->fullPathName().asChar());
		printf("\tTransform name: %s\n", mTransform->partialPathName().asChar());
		printf("\tMesh name     : %s\n", mMesh->name().asChar());
	}
	
	extractShaders();
	mBuildMesh.VertexFlags = 0;
	getShaders();
	getSkinning(status);
	if(status != MStatus::kSuccess)
		return;
	extractGeometry();
// TODO figure out what to do with the space input
//	int space = (int)MSpace::kWorld;
//	mTransform->findPlug("nelExportSpace").getValue(space);
//	extractTransform((MSpace::Space)space);
	makeBuildBaseMesh();
	makeBuildMesh();
	buildVP();
	if(mbIsMRM) {
		if(CArgs::instance().verboseMode)
			printf("\tIs an MRM Mesh\n");
		getInterfaces();
		makeMRM();
	} else {
		makeMesh();
	}
	if(!verbose) {
		CArgs::instance().pop_verbose();
	}
}


CNeLMesh::~CNeLMesh() {
	if (NULL != mDagPath) delete mDagPath;
	if (NULL != mMesh) delete mMesh;
	if (NULL != mTransform) delete mTransform;
	mBsList.clear();
}

MStatus CNeLMesh::writeToFile(MString f) {
	NLMISC::COFile file;
	f += ".shape";
	if(file.open(f.asChar())) {
		try {
			NL3D::CShapeStream streamableMesh (mShape);
			streamableMesh.serial (file);
		}
		catch(...) {
			return MStatus::kFailure;
		}
		file.close ();
	}
	return MStatus::kSuccess;
}

void CNeLMesh::makeBuildBaseMesh() {
	mMesh->findPlug("castsShadows").getValue(mBuildBaseMesh.bCastShadows);
	mMesh->findPlug("receiveShadows").getValue(mBuildBaseMesh.bRcvShadows);
	mBuildBaseMesh.UseLightingLocalAttenuation = false;

	TNeLTransforms tr = CMayaUtils::getWorldNeLTransforms(*mDagPath);
	if(mbIsSkinned) { // What do we do if the mesh is skinned? Set to identity?
		mBuildBaseMesh.DefaultRotQuat = NLMISC::CQuat::Identity;
		mBuildBaseMesh.DefaultScale = NLMISC::CVector(1, 1, 1);
		mBuildBaseMesh.DefaultPos  = NLMISC::CVector::Null;
		mBuildBaseMesh.DefaultPivot = NLMISC::CVector::Null;
	} else {
		mBuildBaseMesh.DefaultScale = tr.scale;
		mBuildBaseMesh.DefaultRotQuat = tr.rot;
		mBuildBaseMesh.DefaultPos = tr.pos;
		mBuildBaseMesh.DefaultPivot = tr.pivot;
	}
	if(CArgs::instance().verboseMode) {
		MAngle c(1);
		MEulerRotation mR(CMayaUtils::getMQuaternion(mBuildBaseMesh.DefaultRotQuat).asEulerRotation() * c.asDegrees());
		printf("\tPosition: %s\n", mBuildBaseMesh.DefaultPos.asString().c_str());
		printf("\tRotation: %f %f %f\n", mR.x, mR.y, mR.z);
		printf("\tScale   : %s\n", mBuildBaseMesh.DefaultScale.asString().c_str());
		printf("\tPivot   : %s\n", mBuildBaseMesh.DefaultPivot.asString().c_str());
	}


	MStatus status;
	
	std::vector<MObject> blendShapes = findBlendShapes(mDagPath->node());
	for(unsigned int k = 0; k < blendShapes.size(); k++) {
		MFnBlendShapeDeformer fn(blendShapes[k], &status);
		if(status) {
			if(CArgs::instance().verboseMode)
				printf("\tChecking %s\n", fn.name().asChar());
			MIntArray weightIndices;
			fn.weightIndexList(weightIndices);
			for(unsigned int i = 0; i < fn.numWeights(); i++) {
				MObjectArray targets;
				fn.getTargets(mDagPath->node(), weightIndices[i], targets);
				// no inbetween support
				if(!targets[0].hasFn(MFn::kMesh))
					continue;
				MString name = getMorphName(targets[0]);
				mBuildBaseMesh.DefaultBSFactors.push_back(0.f);
				mBuildBaseMesh.BSNames.push_back(name.asChar());
				if(CArgs::instance().verboseMode)
					printf("\t\tAdded blendShape target %s\n", name.asChar());
			}
		}
	}

}

void CNeLMesh::setVertexFlags() {
	unsigned int i;
	mBuildMesh.VertexFlags |= NL3D::CVertexBuffer::PositionFlag|NL3D::CVertexBuffer::NormalFlag;
	unsigned int mappingChannelUsed = 0;
	for(i = 0; i < mUVSets.length(); i++)
		mappingChannelUsed |= 1 << i;
	mBuildMesh.VertexFlags |= mappingChannelUsed<<(NL3D::CVertexBuffer::TexCoord0);
	for(i = mUVSets.length(); i < NL3D::CVertexBuffer::MaxStage; i++) {
		mBuildMesh.UVRouting[i] = mUVSets.length() - 1;
	}
}

void CNeLMesh::exportVertices() {
	unsigned int count = mVertexArray.length();
	mBuildMesh.Vertices.resize (count);
	for (unsigned int i = 0; i < count; i++) {
		mBuildMesh.Vertices[i]= CMayaUtils::getCVector(mVertexArray[i]);
	}
}

void CNeLMesh::setNumTriangles(MItMeshPolygon &itPolygon) {
	unsigned int count = 0;
	int numTriangles;
	for ( ; !itPolygon.isDone(); itPolygon.next()) {
		itPolygon.numTriangles(numTriangles);
		count += numTriangles;
	}
	itPolygon.reset();
	mBuildMesh.Faces.resize(count);
}

void CNeLMesh::setVertexColor(unsigned int triangle, unsigned int polyCorner, unsigned int triangleCorner, MItMeshPolygon &itPolygon) {
	MColor vertexColor(0,0,0);
	if(itPolygon.hasColor(polyCorner)) {
		mBuildMesh.VertexFlags |= NL3D::CVertexBuffer::PrimaryColorFlag;
		itPolygon.getColor(vertexColor, polyCorner);
	}
	mBuildMesh.Faces[triangle].Corner[triangleCorner].Color = mColor2CRGBA(vertexColor);
}

void CNeLMesh::setVertexUVs(unsigned int triangle, unsigned int polyCorner, unsigned int triangleCorner, MItMeshPolygon &itPolygon) {
	int uvID = 0;
	for(unsigned int j = 0; j < mUVSets.length(); j++) {
		itPolygon.getUVIndex(polyCorner, uvID, &(mUVSets[j]));
		if(itPolygon.hasUVs(mUVSets[j])) {
			float2 uv;
			itPolygon.getUV(polyCorner, uv, &(mUVSets[j]));
			// CAUTION: NORMALIZING UVs FOR FLIPPING PURPOSES
			float mod = fmod(uv[1],1.f);
			uv[1] = 1 - abs((mod == 0)?uv[1]/1.f:mod); // Flip it
			mod = fmod(uv[0],1.f);
			uv[0] = abs((mod == 0)?uv[0]/1.f:mod);
			mBuildMesh.Faces[triangle].Corner[triangleCorner].Uvws[j] = NLMISC::CUVW(uv[0], uv[1], 0.f);
		} else {
			mBuildMesh.Faces[triangle].Corner[triangleCorner].Uvws[j] = NLMISC::CUVW(0.f, 0.f, 0.f);
		}
	}
}
void CNeLMesh::buildTriangle(unsigned int triangle, MItMeshPolygon &itPolygon, MIntArray &triangleVertices, MIntArray &localVertices) {
	mBuildMesh.Faces[triangle].MaterialId = mShaderIndices[itPolygon.index()];
	mBuildMesh.Faces[triangle].SmoothGroup = 0;	
	for(unsigned int triangleCorner=0; triangleCorner < triangleVertices.length(); triangleCorner++) {
		if(CArgs::instance().dumpMode)
			printf("\t\tFace: %03i Corner: %i Vertex: %i\n", triangle, triangleCorner, triangleVertices[triangleCorner]);

		mBuildMesh.Faces[triangle].Corner[triangleCorner].Vertex = triangleVertices[triangleCorner];
		MVector normal = mNormalArray[itPolygon.normalIndex(localVertices[triangleCorner])];
		mBuildMesh.Faces[triangle].Corner[triangleCorner].Normal = CMayaUtils::getCVector(normal);
		setVertexUVs(triangle, localVertices[triangleCorner], triangleCorner, itPolygon);
		setVertexColor(triangle, localVertices[triangleCorner], triangleCorner, itPolygon);
	}	
}
void CNeLMesh::exportFaces() {
	MStatus status;
	MItMeshPolygon	itPolygon(*mDagPath, MObject::kNullObj, &status);
	if(status != MStatus::kSuccess) {
		MGlobal::displayError("MItMeshPolygon");
		return;
	}
	setNumTriangles(itPolygon);
	unsigned int triangle = 0;
	for ( ; !itPolygon.isDone(); itPolygon.next()) {
		MIntArray polyVertices;
		itPolygon.getVertices(polyVertices);
		int numTriangles;
		itPolygon.numTriangles(numTriangles);
		if(CArgs::instance().dumpMode)
			printf("\tPolygon: %i\n", itPolygon.index());
		while(numTriangles--) {
			MPointArray nonTweaked;
			MIntArray triangleVertices;
			itPolygon.getTriangle(numTriangles, nonTweaked, triangleVertices, MSpace::kObject);
			MIntArray localVertices;
			getLocalIndices(localVertices, triangleVertices, polyVertices);
            buildTriangle(triangle, itPolygon, triangleVertices, localVertices);
			triangle++;
		}
	}
}

void CNeLMesh::makeBuildMesh() {
	setVertexFlags();
	exportVertices();
	exportFaces();
	mBuildMesh.Interfaces.clear();
	mBuildMesh.InterfaceLinks.clear();
}


void CNeLMesh::getSkinning(MStatus &status) {
	mbIsSkinned = false;                                    
	MItDependencyNodes itSkin(MFn::kSkinClusterFilter);
	std::vector<bool> weightedVertices;
	weightedVertices.resize(mMesh->numVertices(), false);
	for( ;!itSkin.isDone() && !mbIsSkinned; itSkin.next()) {
		MObject	inObject, outObject;                    
		MObject skin = itSkin.item();
	    MFnSkinCluster fSkin(skin);
		for(unsigned int i = 0; i < fSkin.numOutputConnections(); ++i ) {
			unsigned int index = fSkin.indexForOutputConnection(i);
			inObject = fSkin.inputShapeAtIndex(index);
//			outObject = fSkin.outputShapeAtIndex(index);
			MDagPath targetPath;
			fSkin.getPathAtIndex(index, targetPath);
			if(targetPath == *mDagPath) {
				unsigned int j;
				if(CArgs::instance().verboseMode)
					printf("\tFound skin cluster %s\n", fSkin.name().asChar());
				mbIsSkinned = true;
				targetPath.pop(); // find the original shape
				targetPath.push(inObject);
				if(CArgs::instance().verboseMode)
					printf("\tFound original shape %s\n", targetPath.fullPathName().asChar());
				delete mMesh;
				mMesh = new MFnMesh(targetPath);

				// Get names
				MDagPathArray influences;
				fSkin.influenceObjects(influences, &status);
				mBuildMesh.BonesNames.resize(influences.length());
				for(j = 0; j < influences.length(); j++) {
					mBuildMesh.BonesNames[j] = influences[j].partialPathName().asChar();
					if(CArgs::instance().verboseMode)
						printf("\t\tInfluence %i: %s\n", j, influences[j].partialPathName().asChar());
				}

				// Get weights
				if(CArgs::instance().verboseMode)
					printf("\t\tGetting weights.\n");
				mBuildMesh.SkinWeights.resize(mMesh->numVertices());
				MItGeometry itGeom(inObject);
				for(j = 0; !itGeom.isDone(); itGeom.next(), ++j) {
					if(j>mBuildMesh.SkinWeights.size()) {
						if(CArgs::instance().verboseMode)
							printf("\t\tERROR: More components than verticies!\n");
						break;
					}
					if(CArgs::instance().dumpMode)
						printf("\t\tVertex %i\n", j);
					MObject comp = itGeom.component(0);
					MFloatArray weights;
					unsigned int nInfluences;
					unsigned int k;

					for(k = 0; k<NL3D_MESH_SKINNING_MAX_MATRIX; k++) {
						mBuildMesh.SkinWeights[j].MatrixId[k]=0;
						mBuildMesh.SkinWeights[j].Weights[k]=0;
					}
					fSkin.getWeights(*mDagPath, comp, weights, nInfluences);
					unsigned int nFoundBones = 0;
					for(k = 0; k < weights.length(); k++) {
						if(weights[k] > 0.001f) {
							nFoundBones++;
							if((nFoundBones-1) > NL3D_MESH_SKINNING_MAX_MATRIX) {
								if(CArgs::instance().verboseMode)
									printf("\t\tERROR: More weights per vertex than NeL can handle\n");
								break;
							}
							mBuildMesh.SkinWeights[j].MatrixId[nFoundBones-1] = k;
							mBuildMesh.SkinWeights[j].Weights[nFoundBones-1] = weights[k];
							if(CArgs::instance().dumpMode) {
								printf("\t\t\tWeight #%i: bone %s value ", (nFoundBones-1), mBuildMesh.BonesNames[k]);
								printf("%f \n", weights[k]);
							}
                        }
					}
					if(nFoundBones == 0) {
						MString error = "No influences found on vertex ";
						error += j;
						error +=  + "!\n";
						MGlobal::displayError(error);
						printf(error.asChar());
						status = MStatus::kFailure;
						return;
					}
					weightedVertices[j] = true;
				}
				mBuildMesh.VertexFlags |= NL3D::CVertexBuffer::PaletteSkinFlag;
			}
		}
	}
	if(mbIsSkinned) {
		for(unsigned int i = 0; i < weightedVertices.size(); i++) {
			if(!weightedVertices[i]) {
				MString error = "Vertex ";
				error += i;
				error += " not weighted!\n";
				MGlobal::displayError(error);
				printf(error.asChar());
				status = MStatus::kFailure;
				return;
			}
		}
	}
	status = MStatus::kSuccess;
	return;
}

void CNeLMesh::getMRMParams() {
	int divisor;
	if(mMesh->findPlug("Divisor").getValue(divisor))
		mMRMParams.Divisor = divisor;

	int nLods;
	if(mMesh->findPlug("NumberOfLods").getValue(nLods))
		mMRMParams.NLods = nLods;

	int skinReduction;
	if(mMesh->findPlug("SkindReduction").getValue(skinReduction))
		mMRMParams.SkinReduction = (NL3D::CMRMParameters::TSkinReduction)skinReduction;

	float distanceFinest;
	if(mMesh->findPlug("DistanceFinest").getValue(distanceFinest))
		mMRMParams.DistanceFinest = distanceFinest;

	float distanceMiddle;
	if(mMesh->findPlug("DistanceMiddle").getValue(distanceMiddle))
		mMRMParams.DistanceMiddle = distanceMiddle;

	float distanceCoarsest;
	if(mMesh->findPlug("DistanceCoarsest").getValue(distanceCoarsest))
		mMRMParams.DistanceCoarsest = distanceCoarsest;
}

void CNeLMesh::getInterfaces() {
	MStatus status;

	// crappy solution

	std::map<std::string, int> ifIdMap;
	typedef std::pair <std::string, int> ifMap_Pair;
	typedef std::pair <std::map<std::string, int>::iterator, bool> ifMap_return_Pair;
	int last_id = 0;
	mBuildMesh.InterfaceLinks.resize(mBuildMesh.Vertices.size());
	mBuildMesh.InterfaceVertexFlag.resize(mBuildMesh.Vertices.size());

	MItMeshVertex it(*mDagPath);
	for(;!it.isDone();it.next()) {
		if(it.onBoundary()) {
			MPoint pos = it.position();
			if(mMesh->hasBlindData(it.index(), MFn::kMeshVertComponent, 20)) {
				MString ifMesh;
				int iVCount;
				int iVId;
				mMesh->getStringBlindData(it.index(), MFn::kMeshVertComponent, 20, "interfaceMesh", ifMesh);
				mMesh->getIntBlindData(it.index(), MFn::kMeshVertComponent, 20, "interfaceVtxId", iVId);
				mMesh->getIntBlindData(it.index(), MFn::kMeshVertComponent, 20, "interfaceVtxCount", iVCount);
				MSelectionList currentSelectionList;
				if (MStatus::kFailure == MGlobal::getActiveSelectionList(currentSelectionList)) {
					MGlobal::displayError("MGlobal::getActiveSelectionList");
				}

				MVector norm;
				mMesh->getVertexNormal(it.index(), norm);
				ifMap_return_Pair pr = ifIdMap.insert(ifMap_Pair(ifMesh.asChar(), last_id));
				if(pr.second) {
					last_id++;
					mBuildMesh.Interfaces.resize(last_id);
				}
				int id = (pr.first)->second;
				NL3D::CMesh::CInterfaceVertex vtx;
				vtx.Normal = CMayaUtils::getCVector(norm);
				vtx.Pos = CMayaUtils::getCVector(pos);
				if(mBuildMesh.Interfaces[id].Vertices.size() == 0) {
					mBuildMesh.Interfaces[id].Vertices.resize(iVCount);
				}
				mBuildMesh.Interfaces[id].Vertices[iVId] = vtx;
				mBuildMesh.InterfaceLinks[it.index()].InterfaceId = id;
				mBuildMesh.InterfaceLinks[it.index()].InterfaceVertexId = mBuildMesh.Interfaces[id].Vertices.size() - 1;
				mBuildMesh.InterfaceVertexFlag.set(it.index());
			}
		}
	}
}


void CNeLMesh::makeMesh() {
	NL3D::CMesh *outMesh = new NL3D::CMesh;
	outMesh->build(mBuildBaseMesh, mBuildMesh);
	getNormalBlendShapes();
    outMesh->setBlendShapes(mBuildMesh.BlendShapes);
	mShape = outMesh;
}

void CNeLMesh::makeMRM() {
	getMRMBlendShapes();
	if (NL3D::CMeshMRMSkinned::isCompatible(mBuildMesh) && mBsList.empty()) {
		NL3D::CMeshMRMSkinned* meshMRMSkinned=new NL3D::CMeshMRMSkinned;
		meshMRMSkinned->build(mBuildBaseMesh, mBuildMesh, mMRMParams);
		mShape=meshMRMSkinned;
	} else {
		NL3D::CMeshMRM* meshMRM=new NL3D::CMeshMRM;
		meshMRM->build(mBuildBaseMesh, mBuildMesh, mBsList, mMRMParams);
		mShape=meshMRM;
	}
}

NL3D::IMeshGeom* CNeLMesh::getMeshGeom(unsigned int materials) {
	if(materials == 0) {
		materials = mBuildBaseMesh.Materials.size();
	}
	if(mbIsMRM) {
		NL3D::CMeshMRMGeom* meshMRMGeom=new NL3D::CMeshMRMGeom;
		meshMRMGeom->build(mBuildMesh, mBsList, materials, mMRMParams);
		return meshMRMGeom;
	} else {
		NL3D::CMeshGeom *meshGeom = new NL3D::CMeshGeom;
        meshGeom->build(mBuildMesh, materials);
		meshGeom->setBlendShapes(mBuildMesh.BlendShapes);
		return meshGeom;
	}
}