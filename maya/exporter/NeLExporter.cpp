// Local includes
#include "NeLExporter.h"
#include "CNeLAnimWriter.h"
#include "CNeLSkeletonWriter.h"
#include "CNeLMultiLODWriter.h"
#include "CNeLZoneWriter.h"
#include "CNeLMesh.h"
#include "general.h"
#include "CNeLWaterWriter.h"
#include "CNeLBBClusterWriter.h"
#include "CNeLLeafClusterWriter.h"
#include "CArgs.h"
#include "CNeLIGWriter.h"
#include "CNeLCollisionMeshWriter.h"
#include "CNeLVegWriter.h"

// Maya includes
#include <maya/MFnPlugin.h>
#include <maya/MDagPath.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MItSelectionList.h>
#include <maya/MAnimUtil.h>
#include <maya/MPlugArray.h>
#include <maya/MFnNurbsSurface.h>

// System includes
#include <algorithm>

const char *const nelExporterDefaultOptions = 
"verbose=1;dump=0;exportMesh=1;exportSkeleton=1;exportNormals=1;exportVBA=1;exportColours=1;exportUVs=1;exportAnimation=1;exportWater=1;exportLOD=1;exportZone=1;exportIG=1;exportCollision=1;exportBillboard=1;exportLeaf=1;exportVeget=1;";
const char *const nelExporterOptionScript = "nelExportOptions";

CNeLExporter::~CNeLExporter() { 
	// does nothing
}


void* CNeLExporter::creator() {
	return new CNeLExporter();
}


MString CNeLExporter::defaultExtension () const {
	return MString("ig");
}


__declspec(dllexport) MStatus initializePlugin(MObject obj) {
	MStatus status;
	MFnPlugin plugin(obj, "Henri Kuuste aka RTSan - NeL mesh exporter test", "1.0", "Any");
	status =  plugin.registerFileTranslator("NeL",
		"",
		CNeLExporter::creator,
		(char *)nelExporterOptionScript,
		(char *)nelExporterDefaultOptions,
		true);
	if (!status) {
		status.perror("registerFileTranslator");
		return status;
	}
	return status;
}


__declspec(dllexport) MStatus uninitializePlugin(MObject obj) {
	MStatus   status;
	MFnPlugin plugin( obj );
	status =  plugin.deregisterFileTranslator("NeL");
	if (!status) {
		status.perror("deregisterFileTranslator");
		return status;
	}
	return status;
}

MStatus CNeLExporter::writer(const MFileObject& file, const MString& optionsString, MPxFileTranslator::FileAccessMode mode) {

	CArgs::instance().setFile(file);

	CArgs::instance().init(optionsString);
	if(CArgs::instance().verboseMode)
		CArgs::instance().debugOutput();


	if (MPxFileTranslator::kExportAccessMode == mode) {
		if(CArgs::instance().verboseMode)
			printf("=== Exporting All ===============================\n");

		if (MStatus::kFailure == exportAll()) {
			printf("=== Export Failed ===============================\n\n");
			std::cout << std::flush;
			return MStatus::kFailure;
		}
	} else if (MPxFileTranslator::kExportActiveAccessMode == mode) {
		if(CArgs::instance().verboseMode)
			printf("=== Exporting Selection =========================\n");

		if (MStatus::kFailure == exportSelection()) {
			printf("=== Export Failed ===============================\n\n");
			std::cout << std::flush;
			return MStatus::kFailure;
		}
	} else {
		if(CArgs::instance().verboseMode)
			printf("=== Export Failed ===============================\n\n");
		std::cout << std::flush;
		return MStatus::kFailure;
	}

	if(CArgs::instance().verboseMode)
		printf("=== Export Sucessful ============================\n\n");

	std::cout << std::flush;
	return MS::kSuccess;
}


bool CNeLExporter::haveWriteMethod() const {
	return true;
}


bool CNeLExporter::haveReadMethod() const {
	return false;
}


bool CNeLExporter::canBeOpened() const {
	return true;
}

MStatus CNeLExporter::exportAll() {
	MStatus status;
	MSelectionList selectionList;
	MItDag it(MItDag::kDepthFirst);
	for(;!it.isDone();it.next()) {
		MDagPath dagPath;
		it.getPath(dagPath);
		selectionList.add(dagPath);
	}
	return export(selectionList);
}


MStatus CNeLExporter::exportSelection() {
	MStatus status;
	MSelectionList selectionList;
	if (MStatus::kFailure == MGlobal::getActiveSelectionList(selectionList)) {
		MGlobal::displayError("MGlobal::getActiveSelectionList");
		return MStatus::kFailure;
	}
	return export(selectionList);
}

MStatus CNeLExporter::export(MSelectionList selection) {
	MStatus status;

	/*************************************
	 * Export Textures					 *
	 *************************************/
// Done in material export for now.
//	MGlobal::executeCommand("nelExportTextures " + CArgs::instance().filePath + CArgs::instance().locTextures);

	/*************************************
	 * Export Meshes					 *
	 *************************************/
	if(CArgs::instance().exportMesh) {
		// find all the meshes in the scene
		MItSelectionList itDag(selection, MFn::kMesh, &status);
		if (MStatus::kFailure == status) {
			MGlobal::displayError("MItSelectionList::MItSelectionList");
			return MStatus::kFailure;
		}
		std::vector<MDagPath> exportedMeshes;
		for(;!itDag.isDone();itDag.next()) {
			MDagPath dagPath;
			if (MStatus::kFailure == itDag.getDagPath(dagPath)) {
				continue;
			}
			if(std::find(exportedMeshes.begin(), exportedMeshes.end(), dagPath) != exportedMeshes.end()) {
//				printf("Found duplicate %s\n", dagPath.partialPathName().asChar());
				continue;
			}
			exportedMeshes.push_back(dagPath);

			// check if the mesh is visible
			MFnDagNode visTester(dagPath.transform());
			MFnDagNode intermTester(dagPath);
			MObject parent;
			if(isVisible(visTester, status) && !intermTester.isIntermediateObject() && (MStatus::kSuccess == status) &&
				!isLOD(dagPath) &&
				!isBB(dagPath) &&
				!isCollisionMesh(dagPath) &&
				!isVeg(dagPath)) {

				bool exportShape = true;
				visTester.findPlug("exportShape").getValue(exportShape);
				int type = CNeLMesh::eNormal;
				visTester.findPlug("NelType").getValue(type);
				if(exportShape && (type == CNeLMesh::eNormal)) {
					if (MStatus::kFailure == processPolyMesh(dagPath)) {
						return MStatus::kFailure;
					}
				}
			}
		}
	}

	/*************************************
	 * Export Skeletons					 *
	 *************************************/
	if(CArgs::instance().exportSkeleton) {
		// find all the skeletons in the scene
		MItSelectionList itSkeleton(selection, MFn::kJoint, &status);
		if (MStatus::kFailure == status) {
			MGlobal::displayError("MItSelectionList::MItSelectionList");
			return MStatus::kFailure;
		}
		for(;!itSkeleton.isDone();itSkeleton.next()) {
			MDagPath skelPath;
			if(MStatus::kFailure == itSkeleton.getDagPath(skelPath)) {
				continue;
			}
			MFnDagNode node(skelPath);

			MObject parent = node.parent(0);
			if(parent.hasFn(MFn::kJoint))
				break;

			if (MStatus::kFailure == processSkeleton(skelPath)) {
				return MStatus::kFailure;
			}
		}
	}

	/*************************************
	 * Export LOD Groups				 *
	 *************************************/
	if(CArgs::instance().exportLOD)	{
		// find	all	the	lod	groups in the scene
		MItSelectionList itLod(selection, MFn::kLodGroup, &status);
		if (MStatus::kFailure == status) {
			MGlobal::displayError("MItSelectionList::MItSelectionList");
			return MStatus::kFailure;
		}
		for(;!itLod.isDone();itLod.next())	{
			MDagPath lodPath;
			if(MStatus::kFailure == itLod.getDagPath(lodPath) || isBBCluster(lodPath) || isLeafCluster(lodPath)) {
				continue;
			}
			if (MStatus::kFailure == processLOD(lodPath)) {
				return MStatus::kFailure;
			}
		}
	}

	/*************************************
	 * Export Billboard Clusters		 *
	 *************************************/
	if(CArgs::instance().exportBillboard) {
		std::vector<MDagPath> exportedClusters;
		MItSelectionList itBBCluster(selection, MFn::kLodGroup, &status);
		if (MStatus::kFailure == status) {
			MGlobal::displayError("MItSelectionList::MItSelectionList");
			return MStatus::kFailure;
		}
		for(;!itBBCluster.isDone();itBBCluster.next())	{
			MDagPath BBClusterPath;
			if(MStatus::kFailure == itBBCluster.getDagPath(BBClusterPath) || !isBBCluster(BBClusterPath)) {
				continue;
			}
			MFnDagNode visTester(BBClusterPath);
			if(!isVisible(visTester, status) || status != MStatus::kSuccess) {
				continue;
			}
			if(std::find(exportedClusters.begin(), exportedClusters.end(), BBClusterPath) != exportedClusters.end()) {
				continue;
			}
			exportedClusters.push_back(BBClusterPath);
			bool exportShape = true;
			visTester.findPlug("exportShape").getValue(exportShape);
			int type = CNeLMesh::eNormal;
			visTester.findPlug("NelType").getValue(type);
			if(exportShape && (type == CNeLMesh::eNormal)) {
				if (MStatus::kFailure == processBBCluster(BBClusterPath)) {
					return MStatus::kFailure;
				}
			}
		}
	}

	/*************************************
	 * Export Leaf Clusters				 *
	 *************************************/
	if(CArgs::instance().exportLeaf) {
		std::vector<MDagPath> exportedLeafClusters;
		MItSelectionList itLeafCluster(selection, MFn::kLodGroup, &status);
		if (MStatus::kFailure == status) {
			MGlobal::displayError("MItSelectionList::MItSelectionList");
			return MStatus::kFailure;
		}
		for(;!itLeafCluster.isDone();itLeafCluster.next())	{
			MDagPath LeafClusterPath;
			if(MStatus::kFailure == itLeafCluster.getDagPath(LeafClusterPath) || !isLeafCluster(LeafClusterPath)) {
				continue;
			}
			MFnDagNode visTester(LeafClusterPath);
			if(!isVisible(visTester, status) || status != MStatus::kSuccess) {
				continue;
			}
			if(std::find(exportedLeafClusters.begin(), exportedLeafClusters.end(), LeafClusterPath) != exportedLeafClusters.end()) {
				continue;
			}
			exportedLeafClusters.push_back(LeafClusterPath);
			bool exportShape = true;
			visTester.findPlug("exportShape").getValue(exportShape);
			int type = CNeLMesh::eNormal;
			visTester.findPlug("NelType").getValue(type);
			if(exportShape && (type == CNeLMesh::eNormal)) {
				if (MStatus::kFailure == processLeafCluster(LeafClusterPath)) {
					return MStatus::kFailure;
				}
			}
		}
	}

	/*************************************
	 * Export Zones						 *
	 *************************************/
	if(CArgs::instance().exportZone) {
		// find all the bezier surfaces in the scene
		MItSelectionList itNurbs(selection, MFn::kNurbsSurface, &status);
		if (MStatus::kFailure == status) {
			MGlobal::displayError("MItSelectionList::MItSelectionList");
			return MStatus::kFailure;
		}
		std::vector<MDagPath> exportedZones;
		for(;!itNurbs.isDone();itNurbs.next()) {
			MDagPath nurbsPath;
			if(MStatus::kFailure == itNurbs.getDagPath(nurbsPath)) {
				continue;
			}
			if(std::find(exportedZones.begin(), exportedZones.end(), nurbsPath) != exportedZones.end()) {
				continue;
			}
			exportedZones.push_back(nurbsPath);

			MFnNurbsSurface surf(nurbsPath);
			if(CArgs::instance().verboseMode)
				printf("Found nurbs surface %s\n", nurbsPath.partialPathName().asChar());
			if(surf.isBezier()) {
				if(CArgs::instance().verboseMode)
					printf("\tIt's a bezier surface\n");
				if (MStatus::kFailure == processZone(nurbsPath)) {
					return MStatus::kFailure;
				}
			}
		}
	}

	/*************************************
	 * Export Instance Groups			 *
	 *************************************/
	if(CArgs::instance().exportIG) {
		CNeLIGWriter igWriter(selection, status);
		igWriter.writeToFile(CArgs::instance().filePath + CArgs::instance().locIG + CArgs::instance().fileName);
	}

	/*************************************
	 * Export Collisions				 *
	 *************************************/
	if(CArgs::instance().exportCollision) {
		// find all the meshes in the scene
		MItSelectionList itDag(selection, MFn::kMesh, &status);
		if (MStatus::kFailure == status) {
			MGlobal::displayError("MItSelectionList::MItSelectionList");
			return MStatus::kFailure;
		}
		std::vector<MDagPath> exportedMeshes;
		// finds groups that contain the selected meshes and exports each as
		// a collision mesh. Note that no export is done if just the group is
		// selected - one has to select a collision mesh inside that group.
		// Should fix this sometime.
		for(;!itDag.isDone();itDag.next()) {
			MDagPath dagPath;
			if (MStatus::kFailure == itDag.getDagPath(dagPath))
				continue;
			if (!isCollisionMesh(dagPath))
				continue;
			if(std::find(exportedMeshes.begin(), exportedMeshes.end(), dagPath) != exportedMeshes.end())
				continue;

			MFnDagNode intermTester(dagPath);
			if(intermTester.isIntermediateObject()) {
				exportedMeshes.push_back(dagPath);
				continue;
			}
			MDagPath parent = getTransformPath(dagPath, status);
			if(MStatus::kFailure == status)
				continue;
			if(parent.length() > 1 && parent.pop() == MStatus::kSuccess) {
				printf("Found collision group: %s\n", parent.partialPathName().asChar());
				std::vector<MDagPath> currentMeshes;
				exportedMeshes.push_back(parent);
				MDagPath children(parent);
				for(unsigned int i = 0; i < children.childCount(); i++) {
					children.push(children.child(i));
					if(!isCollisionMesh(children)) {
						children.pop();
						continue;
					}
					for(unsigned int j = 0; j < children.childCount(); j++) {
						if(!children.child(j).hasFn(MFn::kMesh)) {
							continue;
						}
						children.push(children.child(j));
						MFnDagNode intermChildTester(children);
						if(intermChildTester.isIntermediateObject()) {
							exportedMeshes.push_back(children);
							children.pop();
							continue;
						}
						currentMeshes.push_back(children);
						exportedMeshes.push_back(children);
						children.pop();
					}
					children.pop();
				}
				if (MStatus::kFailure == processCollisionMesh(currentMeshes, dag2FilePath(parent.partialPathName())))
					return MStatus::kFailure;		
			}
		}
	}

	/*************************************
	 * Export Vegs						 *
	 *************************************/
	if(CArgs::instance().exportVeget) {
		MItSelectionList itDag(selection, MFn::kMesh, &status);
		if (MStatus::kFailure == status) {
			MGlobal::displayError("MItSelectionList::MItSelectionList");
			return MStatus::kFailure;
		}
		std::vector<MDagPath> exportedMeshes;
		for(;!itDag.isDone();itDag.next()) {
			MDagPath dagPath;
			if(MStatus::kFailure == itDag.getDagPath(dagPath))
				continue;
			if(!isVeg(dagPath))
				continue;
			if(std::find(exportedMeshes.begin(), exportedMeshes.end(), dagPath) != exportedMeshes.end())
				continue;
			exportedMeshes.push_back(dagPath);

			// check if the mesh is visible
			MFnDagNode visTester(dagPath.transform());
			MFnDagNode intermTester(dagPath);
			if(isVisible(visTester, status) && !intermTester.isIntermediateObject() && (MStatus::kSuccess == status)) {
				if (MStatus::kFailure == processVeg(dagPath)) {
					return MStatus::kFailure;
				}
			}
		}
	}

	return MStatus::kSuccess;
}


MStatus CNeLExporter::processPolyMesh(const MDagPath dagPath) {
	if(CArgs::instance().verboseMode)
		printf("Processing poly mesh\n");

	MFnDagNode tr(dagPath.transform());

	MStatus status;
	CNeLMesh* pWriter;
	if(isWater(dagPath)) {
		if(!CArgs::instance().exportWater) {
			return MStatus::kSuccess;
		}
		pWriter = new CNeLWaterWriter(dagPath, status);
	} else {
		pWriter = new CNeLMesh(dagPath, status);
	}
	if (MStatus::kFailure == status) {
		delete pWriter;
		return MStatus::kFailure;
	}
	pWriter->extract();
	if(CArgs::instance().verboseMode)
		printf("\tWriting mesh to file\n");
	if (MStatus::kFailure == pWriter->writeToFile(CArgs::instance().filePath + CArgs::instance().locMeshes + dag2FilePath(tr.partialPathName()))) {
		delete pWriter;
		return MStatus::kFailure;
	}
	delete pWriter;

	if(CArgs::instance().exportAnimation) {
		CNeLAnimWriter* pAnimWriter = new CNeLAnimWriter();
		if (MStatus::kFailure == pAnimWriter->addMeshAnimation(dagPath)) {
			delete pAnimWriter;
			return MStatus::kFailure;
		}
		if(CArgs::instance().verboseMode)
			printf("\tWriting mesh animation to file\n");
		if (MStatus::kFailure == pAnimWriter->writeToFile(CArgs::instance().filePath + CArgs::instance().locAnimations + dag2FilePath(tr.partialPathName()))) {
			delete pAnimWriter;
			return MStatus::kFailure;
		}
		delete pAnimWriter;
	}

	if(CArgs::instance().verboseMode)
		printf("Done processing poly mesh\n");

	return MStatus::kSuccess;
}

MStatus CNeLExporter::processSkeleton(const MDagPath dagPath) {
	MStatus status;

	if(CArgs::instance().verboseMode)
		printf("Processing Skeleton %s\n", dagPath.partialPathName().asChar());

	CNeLSkeletonWriter* pWriter = new CNeLSkeletonWriter(dagPath, status);
	if (MStatus::kFailure == status) {
		delete pWriter;
		return MStatus::kFailure;
	}

	if(CArgs::instance().verboseMode)
		printf("\tWriting skeleton to file\n");
	if(MStatus::kFailure == pWriter->writeToFile(CArgs::instance().filePath + CArgs::instance().locSkeletons + dag2FilePath(dagPath.partialPathName()))) {
		delete pWriter;
		return MStatus::kFailure;
	}
	delete pWriter;

	if(CArgs::instance().exportAnimation) {
		CNeLAnimWriter* pAnimWriter = new CNeLAnimWriter();
		if (MStatus::kFailure == pAnimWriter->addSkeletonAnimation(dagPath)) {
			delete pAnimWriter;
			return MStatus::kFailure;
		}
		if(CArgs::instance().verboseMode)
			printf("\tWriting skeleton animation to file\n");
		if (MStatus::kFailure == pAnimWriter->writeToFile(CArgs::instance().filePath + CArgs::instance().locAnimations + dag2FilePath(dagPath.partialPathName()))) {
			delete pAnimWriter;
			return MStatus::kFailure;
		}
		delete pAnimWriter;
	}

	if(CArgs::instance().verboseMode)
		printf("Done processing skeleton\n");

	return MStatus::kSuccess;
}

MStatus CNeLExporter::processLOD(const MDagPath dagPath) {
	MStatus status;

	if(CArgs::instance().verboseMode)
		printf("Processing LOD %s\n", dagPath.partialPathName().asChar());

	CNeLMultiLODWriter* pWriter = new CNeLMultiLODWriter(dagPath, status);
	if (MStatus::kFailure == status) {
		delete pWriter;
		return MStatus::kFailure;
	}

	if(CArgs::instance().verboseMode)
		printf("\tWriting LOD to file\n");
	if(MStatus::kFailure == pWriter->writeToFile(CArgs::instance().filePath + CArgs::instance().locMeshes + dag2FilePath(dagPath.partialPathName()))) {
		delete pWriter;
		return MStatus::kFailure;
	}
	delete pWriter;

	if(CArgs::instance().exportAnimation) {
		CNeLAnimWriter* pAnimWriter = new CNeLAnimWriter();
		if (MStatus::kFailure == pAnimWriter->addLODAnimation(dagPath)) {
			delete pAnimWriter;
			return MStatus::kFailure;
		}
		if(CArgs::instance().verboseMode)
			printf("\tWriting LOD animation to file\n");
		if (MStatus::kFailure == pAnimWriter->writeToFile(CArgs::instance().filePath + CArgs::instance().locAnimations + dag2FilePath(dagPath.partialPathName()))) {
			delete pAnimWriter;
			return MStatus::kFailure;
		}
		delete pAnimWriter;
	}

	if(CArgs::instance().verboseMode)
		printf("Done processing LOD\n");

	return MStatus::kSuccess;
}

MStatus CNeLExporter::processBBCluster(const MDagPath dagPath) {
	MStatus status;

	if(CArgs::instance().verboseMode)
		printf("Processing Billboard Cluster %s\n", dagPath.partialPathName().asChar());

	CNeLBBClusterWriter* pWriter = new CNeLBBClusterWriter(dagPath, status);
	if (MStatus::kFailure == status) {
		delete pWriter;
		return MStatus::kFailure;
	}
	pWriter->init();

	if(CArgs::instance().verboseMode)
		printf("\tWriting Billboard Cluster to file\n");
	if(MStatus::kFailure == pWriter->writeToFile(CArgs::instance().filePath + CArgs::instance().locBillboards + dag2FilePath(dagPath.partialPathName()))) {
		delete pWriter;
		return MStatus::kFailure;
	}
	delete pWriter;


	if(CArgs::instance().verboseMode)
		printf("Done processing Billboard Cluster\n");

	return MStatus::kSuccess;
}

MStatus CNeLExporter::processLeafCluster(const MDagPath dagPath) {
	MStatus status;

	if(CArgs::instance().verboseMode)
		printf("Processing Leaf Cluster %s\n", dagPath.partialPathName().asChar());

	CNeLLeafClusterWriter* pWriter = new CNeLLeafClusterWriter(dagPath, status);
	if (MStatus::kFailure == status) {
		delete pWriter;
		return MStatus::kFailure;
	}
	pWriter->init();

	if(CArgs::instance().verboseMode)
		printf("\tWriting Leaf Cluster to file\n");
	if(MStatus::kFailure == pWriter->writeToFile(CArgs::instance().filePath + CArgs::instance().locLeaves + dag2FilePath(dagPath.partialPathName()))) {
		delete pWriter;
		return MStatus::kFailure;
	}
	delete pWriter;


	if(CArgs::instance().verboseMode)
		printf("Done processing Leaf Cluster\n");

	return MStatus::kSuccess;
}

MStatus CNeLExporter::processZone(const MDagPath dagPath) {
	MStatus status;
	CNeLZoneWriter* pWriter = new CNeLZoneWriter(dagPath);
	
	MDagPath transformPath = getTransformPath(dagPath, status);
	if(status == MStatus::kSuccess) {
		pWriter->writeToFile(CArgs::instance().filePath + CArgs::instance().locZones + dag2FilePath(transformPath.partialPathName()));
	}
	delete pWriter;
	return status;
}

MStatus CNeLExporter::processCollisionMesh(std::vector<MDagPath> &dagPath, MString name) {
	MStatus status;
	CNeLCollisionMeshWriter* pWriter = new CNeLCollisionMeshWriter(dagPath, status);
	if((status = pWriter->extract()) == MStatus::kSuccess) {
		pWriter->writeToFile(CArgs::instance().filePath + CArgs::instance().locCollisions + name);
	}
	delete pWriter;
	return status;
}

MStatus CNeLExporter::processVeg(const MDagPath dagPath) {
	MStatus status;
	printf("Found a veg: %s\n", dagPath.partialPathName().asChar());
	CNeLVegWriter* pWriter = new CNeLVegWriter(dagPath, status);
	pWriter->extract(true, status);
	if(status == MStatus::kSuccess) {
		MDagPath transformPath = getTransformPath(dagPath, status);
		if(status == MStatus::kSuccess)
			pWriter->writeToFile(CArgs::instance().filePath + CArgs::instance().locVegets + dag2FilePath(transformPath.partialPathName()));
	}
	delete pWriter;
	return status;
}