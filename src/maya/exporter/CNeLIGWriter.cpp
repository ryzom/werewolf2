// Local includes
#include "CNeLIGWriter.h"
#include "CArgs.h"
#include "CNeLMesh.h"
#include "general.h"

// NeL includes
#include <nel/misc/file.h>

// Maya includes
#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MPlug.h>
#include <maya/MAngle.h>
#include <maya/MStatus.h>
#include <maya/MFnPointLight.h>
#include <maya/MMatrix.h>
#include <maya/MBoundingBox.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MItSelectionList.h>

// System Includes
#include <algorithm>

CNeLIGWriter::CNeLIGWriter(MSelectionList selection, MStatus &status) {
	m_selection = selection;
	getClusters();
	getLights();
	getInstances();
	getPortals();
	makeIG();
	status = MStatus::kSuccess;
}

CNeLIGWriter::~CNeLIGWriter() {
	// Does nothing
}

MStatus CNeLIGWriter::writeToFile(MString f) {
	NLMISC::COFile file;
	if (file.open (f.asChar()))
	{
		try {
			mIG.serial (file);
		}
		catch(...) {
			return MStatus::kFailure;
		}
		file.close ();
	} else {
		printf("Failed to open file for writing IG\n");
	}
	return MStatus::kSuccess;
}

MStatus CNeLIGWriter::getInstances() {
	MItSelectionList it(m_selection, MFn::kTransform);
	MDagPath dagPath;
	std::vector<MDagPath> paths;
	int count;
	for(count=0; !it.isDone(); it.next()) {
		it.getDagPath(dagPath);
		int type = CNeLMesh::eNormal;
		MFnDagNode instance(dagPath);
		instance.findPlug("NelType").getValue(type);
		if((dagPath.hasFn(MFn::kMesh) || dagPath.hasFn(MFn::kLodGroup)) &&
			(type == CNeLMesh::eNormal) &&
			!isLOD(dagPath) &&
			!isBB(dagPath) &&
			!isCollisionMesh(dagPath) &&
			!isVeg(dagPath)) {

			MDagPath origShape = dagPath;
			MObject shapeObj = dagPath.child(0);
			MFnDagNode shape(shapeObj);
			if(shape.isInstanced(false)) {
				shape.getPath(origShape);
				origShape.pop();
			}
			NL3D::CInstanceGroup::CInstance inst;
			inst.AvoidStaticLightPreCompute = inst.DontAddToScene = inst.DontCastShadow = inst.DontCastShadowForExterior = inst.DontCastShadowForInterior = false;
			instance.findPlug("DontAddToScene").getValue(inst.DontAddToScene);
			instance.findPlug("DontCastShadow").getValue(inst.DontCastShadow);
			instance.findPlug("DontCastShadowForExterior").getValue(inst.DontCastShadowForExterior);
			instance.findPlug("DontCastShadowForInterior").getValue(inst.DontCastShadowForInterior);
			instance.findPlug("AvoidStaticLightPreCompute").getValue(inst.AvoidStaticLightPreCompute);
			inst.InstanceName = dag2FilePath(dagPath.partialPathName()).asChar();
			inst.Name = dag2FilePath(origShape.partialPathName()).asChar();
			inst.nParent = -1;
/*			if(mLights.size() > 0) {
				inst.Light[0] = 0;
				for(int l = 1; l < NL3D::CInstanceGroup::NumStaticLightPerInstance; l++) {
					inst.Light[1] = 0xFF;
				}
				inst.StaticLightEnabled = true;
			}
*/
			inst.SunContribution = 255;
			MStatus status;
			instance.findPlug("visibility", &status).getValue(inst.Visible);
			double scale[3];
			MQuaternion rot;
			MEulerRotation rotE;
			MVector trans;
			MPoint pivot;
			if(getTransforms(dagPath, scale, rot, rotE, trans, pivot) == MStatus::kSuccess) {
				inst.Pos = NLMISC::CVector((float)trans[0], (float)trans[1], (float)trans[2]);
				inst.Rot = NLMISC::CQuat((float)rot[0], (float)rot[1], (float)rot[2], (float)rot[3]);
				inst.Scale = NLMISC::CVector((float)scale[0], (float)scale[1], (float)scale[2]);
			}
			mInstances.push_back(inst);
			paths.push_back(dagPath);
			count++;
		}
	}
	mInstances.resize(count);
	// this code for finding the parent could be pretty slow...so maybe think of something better
	for(int i = 0; i < count; i++) {
		MDagPath parent = paths[i];
		bool found = false;
		while((parent.length() > 1) && !found) {
			parent.pop();
			for(int j = 0; j < count; j++) {
				if(paths[j] == parent) {
					mInstances[i].nParent = j;
					found = true;
				}
			}
		}
		if(CArgs::instance().verboseMode) {
			int parent = mInstances[i].nParent;
			printf("Instance #%2i: %2i %12s: %12s %12s\n",
				i, parent,
				(parent == -1 ? "World" : mInstances[parent].InstanceName.c_str()),
				mInstances[i].InstanceName.c_str(),
				mInstances[i].Name.c_str());
		}
		linkInstance(mInstances[i], paths[i]);
	}
	if(CArgs::instance().verboseMode) {
		printf("Amount of instances: %i\n", count);
	}
	return MStatus::kSuccess;
}

MStatus CNeLIGWriter::getClusters() {
	MStatus status;
	MItSelectionList it(m_selection, MFn::kTransform);
	MDagPath dagPath;
	int count;
	for(count=0; !it.isDone(); it.next()) {
		it.getDagPath(dagPath);
		int type = CNeLMesh::eNormal;
		MFnDagNode fCluster(dagPath);
		fCluster.findPlug("NelType").getValue(type);
		if(dagPath.hasFn(MFn::kMesh) && (type == CNeLMesh::eCluster)) {
			NL3D::CCluster clust;
			MString temp = "";
			fCluster.findPlug("SoundGroup").getValue(temp);
			clust.setSoundGroup(temp.asChar());
			temp = "";
			fCluster.findPlug("EnvironmentFX").getValue(temp);
			clust.setEnvironmentFx(temp.asChar());
			CNeLMesh clustMesh(dagPath, status, MSpace::kWorld);
			// TODO: error checking
			clustMesh.extract(CArgs::instance().dumpMode);
			NL3D::CMesh::CMeshBuild *pMB = clustMesh.getMeshBuild();
			for(unsigned int j = 0; j < pMB->Faces.size(); j++) {
				if (!clust.makeVolume (pMB->Vertices[pMB->Faces[j].Corner[0].Vertex],
					pMB->Vertices[pMB->Faces[j].Corner[1].Vertex],
					pMB->Vertices[pMB->Faces[j].Corner[2].Vertex]) ) {
						printf("\tERROR: Cluster not convex!");
				}
			}
			clust.Name = dag2FilePath(dagPath.partialPathName()).asChar();
			clust.FatherVisible = clust.VisibleFromFather = clust.FatherAudible = clust.AudibleFromFather = true;
			fCluster.findPlug("ParentVisible").getValue(clust.FatherVisible);
			fCluster.findPlug("VisibleFromParent").getValue(clust.VisibleFromFather);
			fCluster.findPlug("ParentAudible").getValue(clust.FatherAudible);
			fCluster.findPlug("AudibleFromParent").getValue(clust.AudibleFromFather);

			if(CArgs::instance().verboseMode) {
				printf("Cluster #%2i: %s\n", count, dagPath.partialPathName().asChar());
				if(CArgs::instance().dumpMode) {
					printf("\tParent Visible:      %s\n", (clust.FatherVisible ? "true" : "false"));
					printf("\tVisible From Parent: %s\n", (clust.VisibleFromFather ? "true" : "false"));
					printf("\tParent Audible:      %s\n", (clust.FatherAudible ? "true" : "false"));
					printf("\tAudible From Parent: %s\n", (clust.AudibleFromFather ? "true" : "false"));
				}
			}

			mClusters.push_back(clust);
			count++;
		}
	}
	mClusters.resize(count);
	if(CArgs::instance().verboseMode) {
		printf("Amount of clusters: %i\n", count);
	}
	return MStatus::kSuccess;
}

MStatus CNeLIGWriter::getPortals() {
	MStatus status;
	MItSelectionList it(m_selection, MFn::kTransform);
	MDagPath dagPath;
	int count;
	for(count=0; !it.isDone(); it.next()) {
		it.getDagPath(dagPath);
		int type = CNeLMesh::eNormal;
		MFnDagNode fPortal(dagPath);
		fPortal.findPlug("NelType").getValue(type);
		if(dagPath.hasFn(MFn::kMesh) && (type == CNeLMesh::ePortal)) {
			NL3D::CPortal port;
			MString occlusion = "";
			fPortal.findPlug("NelOcclusionModel").getValue(occlusion);
			port.setOcclusionModel(occlusion.asChar());
			occlusion = "";
			fPortal.findPlug("NelOpenOcclusionModel").getValue(occlusion);
			port.setOpenOcclusionModel(occlusion.asChar());

			// check for dynamic portals
			bool dynamic = false;
			MString instance_name = "";
			fPortal.findPlug("NelDynamicPortal").getValue(dynamic);
			fPortal.findPlug("NelInstanceName").getValue(instance_name);
			if(dynamic && (instance_name != "")) {
				printf("Is dynamic\n");
				port.setName(std::string(instance_name.asChar()));
			}

			CNeLMesh mesh(dagPath, status, MSpace::kWorld);
			// TODO: error checking
			mesh.extract(false);
			NL3D::CMesh::CMeshBuild *pMB = mesh.getMeshBuild();

			// Ripped from the max exporter - understand what it does exactly later...
			// Seems to be constructing some kind of a single polygon
			std::vector<sint32> poly;
			std::vector<bool> facechecked;
			unsigned int j, m, k;
			facechecked.resize (pMB->Faces.size());
			for (j = 0; j < pMB->Faces.size(); ++j)
				facechecked[j] = false;

			poly.push_back(pMB->Faces[0].Corner[0].Vertex);
			poly.push_back(pMB->Faces[0].Corner[1].Vertex);
			poly.push_back(pMB->Faces[0].Corner[2].Vertex);
			facechecked[0] = true;
			for (j = 0; j < pMB->Faces.size(); ++j)
			if (!facechecked[j]) {
				bool found = false;

				for(k = 0; k < 3; ++k) {
					for(m = 0; m < poly.size(); ++m) {
						if ((pMB->Faces[j].Corner[k].Vertex == poly[m]) &&
							(pMB->Faces[j].Corner[(k+1)%3].Vertex == poly[(m+1)%poly.size()])) {
							found = true;
							break;
						}
						if ((pMB->Faces[j].Corner[(k+1)%3].Vertex == poly[m]) &&
							(pMB->Faces[j].Corner[k].Vertex == poly[(m+1)%poly.size()])) {
							found = true;
							break;
						}
					}
					if (found)
						break;
				}
				if (found) {
					// insert an empty space in poly between m and m+1
					poly.resize (poly.size()+1);
					for (uint32 a = poly.size()-2; a > m; --a)
						poly[a+1] = poly[a];
					poly[m+1] = pMB->Faces[j].Corner[(k+2)%3].Vertex;
					facechecked[j] = true;
					j = 0;
				}
			}
			std::vector<NLMISC::CVector> polyv;
			polyv.resize (poly.size());
			for (j = 0; j < poly.size(); ++j)
				polyv[j] = pMB->Vertices[poly[j]];
			
			if (!port.setPoly (polyv)) {
				printf("ERROR: The portal %s is not convex.\n", dagPath.partialPathName().asChar());
			}

			// Check if portal has 2 cluster
			int nNbCluster = 0;
			for (j = 0; j < mClusters.size(); ++j) {
				bool bPortalInCluster = true;
				for (k = 0; k < polyv.size(); ++k)
					if (!mClusters[j].isIn (polyv[k]) ) {
						bPortalInCluster = false;
						break;
					}
				if (bPortalInCluster)
					++nNbCluster;
			}
			if (nNbCluster != 2) {
				printf("ERROR: The portal %s has not 2 clusters but %d\n", dagPath.partialPathName().asChar(), nNbCluster);
			}

			printf("Portal #%2i: %s\n", count, dagPath.partialPathName().asChar());

			mPortals.push_back(port);
			count++;
		}
	}
	mPortals.resize(count);
	if(CArgs::instance().verboseMode) {
		printf("Amount of portals: %i\n", count);
	}
	return MStatus::kSuccess;
}

MStatus CNeLIGWriter::getLights() {
	MStatus status;
	MItSelectionList it(m_selection, MFn::kLight);
	MDagPath dagPath;
	int count;
	std::vector<MDagPath> paths;
	for(count=0; !it.isDone(); it.next()) {
		it.getDagPath(dagPath);
		if(std::find(paths.begin(), paths.end(), dagPath) != paths.end()) {
			continue;
		}
		paths.push_back(dagPath);
		if(dagPath.hasFn(MFn::kPointLight)){
			NL3D::CPointLightNamed pl;
			MFnPointLight fLight(dagPath);
			double scale[3];
			MQuaternion rot;
			MEulerRotation rotE;
			MVector trans;
			MPoint pivot;
			if(getTransforms(dagPath, scale, rot, rotE, trans, pivot) == MStatus::kSuccess) {
				pl.setPosition(NLMISC::CVector((float)trans[0], (float)trans[1], (float)trans[2]));
			}
			float attenuationStart = 10;
			float attenuationEnd = 40;
			fLight.findPlug("AttenuationStart").getValue(attenuationStart);
			fLight.findPlug("AttenuationEnd").getValue(attenuationEnd);
			pl.setupAttenuation(attenuationStart, attenuationEnd);
			NLMISC::CRGBA color = mColor2CRGBA(fLight.color());
			pl.setColor(color);
			pl.setDefaultAmbient(color);
			pl.setDefaultDiffuse(color);
			pl.setDiffuse(color);

			pl.LightGroup = 0;

			pl.setType(NL3D::CPointLight::PointLight);
			pl.setLightFactor(NLMISC::CRGBA(255, 255, 255, 255));
			if(CArgs::instance().verboseMode) {
				printf("Light #%2i: %s %f %f %f\n", count, dagPath.partialPathName().asChar(), trans.x, trans.y, trans.z);
			}
			mLights.push_back(pl);
			count++;
		}
	}
	mLights.resize(count);
	if(CArgs::instance().verboseMode) {
		printf("Amount of lights: %i\n", count);
	}
	return MStatus::kSuccess;
}

MStatus CNeLIGWriter::linkInstance(NL3D::CInstanceGroup::CInstance &inst, MDagPath dagPath) {
	MStatus status;
	MFnDagNode fInstance(dagPath);
	MBoundingBox bBox = fInstance.boundingBox();
	bBox.transformUsing(dagPath.exclusiveMatrix()); // take groups and other parent transforms into account

	// this constructs the vertices of an aabbox, it is not good enough for our case, so think of something better
/*	std::vector<NLMISC::CVector> bBoxVtx;

	bBoxVtx.push_back(NLMISC::CVector(bBox.min().x, bBox.min().y, bBox.min().z));
	bBoxVtx.push_back(NLMISC::CVector(bBox.max().x, bBox.min().y, bBox.min().z));
	bBoxVtx.push_back(NLMISC::CVector(bBox.max().x, bBox.min().y, bBox.max().z));
	bBoxVtx.push_back(NLMISC::CVector(bBox.min().x, bBox.min().y, bBox.max().z));

	bBoxVtx.push_back(NLMISC::CVector(bBox.min().x, bBox.max().y, bBox.min().z));
	bBoxVtx.push_back(NLMISC::CVector(bBox.max().x, bBox.max().y, bBox.min().z));
	bBoxVtx.push_back(NLMISC::CVector(bBox.max().x, bBox.max().y, bBox.max().z));
	bBoxVtx.push_back(NLMISC::CVector(bBox.min().x, bBox.max().y, bBox.max().z));
*/	std::vector<int> fullClusters; // clusters containing the object fully
	for(unsigned int i = 0; i < mClusters.size(); i++) {
		bool doExtensiveTest = true;
/*		int meshInCluster = 0;
		for(int j = 0; j < bBoxVtx.size(); j++) {
			if(mClusters[i].isIn(bBoxVtx[j])) {
				meshInCluster++;
			}
		}
		NLMISC::CVector center = mClusters[i].getBBox().getCenter();
		switch(meshInCluster) {
			case 0: // Bounding box not in cluster
				// Must check if the bounding box completely envelops the cluster
				// This would generally be a simple test.
				// because if the cluster is contained in the bounding box all the vertices of the cluster are in the bounding box.
				// and since the case where a single vertex is in but others are out is dealt with by the switch
				// we could just test one vertex. However since there is no straight forward way of getting to the vertices of a cluster
				// I have to use the center of the bounding box of the cluster. The result *should* be the same.
				// It might fail if the center of the bounding box is not in the cluster itself.
				// But as far as I can see that would be a concave cluster which is not allowed.
				if(bBox.contains(MPoint(center.x, center.y, center.z))) {
					doExtensiveTest = true;
				} else if(CArgs::instance().verboseMode) {
					printf("\tNot in cluster: #%i %s\n", i, mClusters[i].Name.c_str());
				}
				break;
			case 8: // Bounding box fully in cluster
				fullClusters.push_back(i);
				if(CArgs::instance().verboseMode) {
					printf("\tFully in cluster: #%i %s\n", i, mClusters[i].Name.c_str());
				}
				break;
			default: // Bounding box partially in cluster
				if(CArgs::instance().verboseMode) {
					printf("\tDoing extensive test for cluster: #%i %s\n", i, mClusters[i].Name.c_str());
				}
				doExtensiveTest = true;
				break;
		}
*/		
		if(doExtensiveTest) {
			bool inCluster = false;
			bool partiallyInCluster = false;
			CNeLMesh mesh(dagPath, status, MSpace::kWorld);
			// TODO: error checking
			mesh.extract(false);
			NL3D::CMesh::CMeshBuild *pMB = mesh.getMeshBuild();
			for(unsigned int j = 0; j < pMB->Vertices.size(); ++j) {
				if (mClusters[i].isIn (pMB->Vertices[j])) {
					inCluster = true;
				} else if (inCluster) { // this means it is only partially in the cluster and it is safe to add it to the cluster
					if(CArgs::instance().verboseMode) {
						printf("\tIn cluster: #%i %s\n", i, mClusters[i].Name.c_str());
					}
					partiallyInCluster = true;
					inst.Clusters.push_back(i);
					break;
				}
			}
			if(inCluster && !partiallyInCluster) {
				fullClusters.push_back(i);
			}
		}
	}
	// No two clusters can contain an object fully. If more than 2 clusters do then assign only the smallest.
	// Hopefully what this means is that if the clusters contain each other the deepest in the hierarchy is used
	if(fullClusters.size() > 0) {
		int smallest_radius = 0;
		int smallest_cluster;
		for(unsigned int i = 0; i < fullClusters.size(); i++) {
			if(smallest_radius == 0 || mClusters[fullClusters[i]].getBBox().getRadius() < smallest_radius) {
				smallest_cluster = fullClusters[i];
				smallest_radius = mClusters[fullClusters[i]].getBBox().getRadius();
			}
		}
		inst.Clusters.push_back(smallest_cluster);
		if(CArgs::instance().verboseMode) {
			printf("\tIn cluster: #%i %s\n", smallest_cluster, mClusters[smallest_cluster].Name.c_str());
		}
	}
	return MStatus::kSuccess;
}

MStatus CNeLIGWriter::makeIG() {
	mIG.build(NLMISC::CVector(0, 0, 0), mInstances, mClusters, mPortals, mLights);
	mIG.enableRealTimeSunContribution(true);
	return MStatus::kSuccess;
}