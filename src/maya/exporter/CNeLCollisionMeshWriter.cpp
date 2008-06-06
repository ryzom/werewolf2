// NeL includes
#include <3d/quad_grid.h>

// Local includes
#include "general.h"
#include "CNeLCollisionMeshWriter.h"

// Standard includes
#include <vector>
#include <string>
#include <utility>


// Maya includes
#include <maya/MPointArray.h>
#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MFnDagNode.h>

CNeLCollisionMeshWriter::CNeLCollisionMeshWriter(std::vector<MDagPath> &dagPath, MStatus &status) {
	mDagPath.reserve(dagPath.size());
	for(unsigned int i = 0; i < dagPath.size(); i++)
		mDagPath.push_back(dagPath[i]);
}

CNeLCollisionMeshWriter::~CNeLCollisionMeshWriter() {
}

MStatus CNeLCollisionMeshWriter::writeToFile(MString f) {
	NLMISC::COFile file;
	f += ".cmb";
	if(file.open(f.asChar())) {
		try {
			file.serial(mCollisionMeshBuild);
		} catch(...) {
			return MStatus::kFailure;
		}
		file.close ();
	}
	return MStatus::kSuccess;
}

typedef std::pair<unsigned int, unsigned int>	TFaceRootMeshInfo;

MStatus CNeLCollisionMeshWriter::extract() {
	unsigned int totalSurfaces = 0,
			totalVertices = 0,
			totalFaces = 0;

	std::vector<unsigned int>		rootMeshVertices;
	std::vector<TFaceRootMeshInfo>	facesRootMeshesInfo;
	std::vector<std::string>			rootMeshNames;

	/****************************************************
	 * For each mesh add faces to the collision mesh	*
	 ****************************************************/
	for(unsigned int i = 0; i < mDagPath.size(); i++) {
		MFnMesh mesh(mDagPath[i]);
		int type = 0;
		
		unsigned int meshId = rootMeshNames.size();
		rootMeshNames.push_back(std::string(mDagPath[i].partialPathName().asChar()));

		MFnDagNode node(mDagPath[i].transform());
		node.findPlug("CollisionType").getValue(type);
		printf("Found collision mesh: %s of type: %d\n", mDagPath[i].partialPathName().asChar(), type);
		MPointArray vtxArray;
		if (MStatus::kFailure == mesh.getPoints(vtxArray, MSpace::kWorld)) {
			MGlobal::displayError("MFnMesh::getPoints"); 
			return MStatus::kFailure;
		}
		/********************************************************
		 * Add the vertices										*
		 ********************************************************/
		mCollisionMeshBuild.Vertices.reserve(mCollisionMeshBuild.Vertices.size() + vtxArray.length());
		for(unsigned int j = 0; j < vtxArray.length(); j++) {
			mCollisionMeshBuild.Vertices.push_back(NLMISC::CVector((float)vtxArray[j].x, (float)vtxArray[j].y, (float)vtxArray[j].z));
			rootMeshVertices.push_back(i);
		}
		
		/********************************************************
		 * Add the faces										*
		 ********************************************************/
		MStatus status;
		MItMeshPolygon	itPolygon(mDagPath[i], MObject::kNullObj, &status);
		if(status != MStatus::kSuccess) {
			MGlobal::displayError("MItMeshPolygon");
			return MStatus::kFailure;
		}
		int triangle = mCollisionMeshBuild.Faces.size();
		setNumTriangles(itPolygon);
		for ( ; !itPolygon.isDone(); itPolygon.next()) {
			int numTriangles;
			itPolygon.numTriangles(numTriangles);
			MIntArray edges;
			itPolygon.getEdges(edges);
			/**********************************************************
			 * Get the triangles not the actual faces				  *
			 **********************************************************/
			while(numTriangles--) {
				facesRootMeshesInfo.push_back(std::make_pair(meshId, itPolygon.index()));
				MPointArray nonTweaked;
				MIntArray triangleVertices;
				itPolygon.getTriangle(numTriangles, nonTweaked, triangleVertices, MSpace::kObject);
				
				mCollisionMeshBuild.Faces[triangle].V[0] = totalVertices + triangleVertices[0];
				mCollisionMeshBuild.Faces[triangle].V[1] = totalVertices + triangleVertices[1];
				mCollisionMeshBuild.Faces[triangle].V[2] = totalVertices + triangleVertices[2];

				int2 verts;
				verts[0] = triangleVertices[1]; verts[1] = triangleVertices[2];
				mCollisionMeshBuild.Faces[triangle].Visibility[0] = getEdgeVisibility(mesh, verts, edges);
				verts[0] = triangleVertices[2]; verts[1] = triangleVertices[0];
				mCollisionMeshBuild.Faces[triangle].Visibility[1] = getEdgeVisibility(mesh, verts, edges);
				verts[0] = triangleVertices[0]; verts[1] = triangleVertices[1];
				mCollisionMeshBuild.Faces[triangle].Visibility[2] = getEdgeVisibility(mesh, verts, edges);

				mCollisionMeshBuild.Faces[triangle].Surface = (type == eExterior? -1 : totalSurfaces);
				mCollisionMeshBuild.Faces[triangle].Material = 0;

				triangle++;
			}
		}
		totalVertices = mCollisionMeshBuild.Vertices.size();
		totalFaces = mCollisionMeshBuild.Faces.size();
		totalSurfaces++;
	}

	/********************************************************
	 * Weld identical vertices								*
	 * using a grid to store indexes of vertices			*
	 * Ripped from the MAX plugin							*
	 ********************************************************/
//	printf("Vertices before weld %d\n", mCollisionMeshBuild.Vertices.size());
	const sint		GridSize = 64;
	const float		GridWidth = 1.0f;
	const float		WeldThreshold = 0.005f;

	NL3D::CQuadGrid<unsigned int>	grid;
	std::vector<unsigned int>		remapIds;
	std::vector<NLMISC::CVector>	remapVertices;
	std::vector<NLMISC::CVector>	&vertices = mCollisionMeshBuild.Vertices;

	std::vector<NLMISC::CVector>			previousVertices = mCollisionMeshBuild.Vertices;
	std::vector<NLPACS::CCollisionFace>		previousFaces = mCollisionMeshBuild.Faces;

	grid.create(GridSize, GridWidth);
	remapIds.resize(totalVertices);
	for (unsigned int i=0; i<totalVertices; ++i) {
		remapIds[i] = i;
		NLMISC::CAABBox	box;
		box.setCenter(mCollisionMeshBuild.Vertices[i]);
		box.setHalfSize(NLMISC::CVector(WeldThreshold, WeldThreshold, 0.0f));
		grid.insert(box.getMin(), box.getMax(), i);
	}
	for (unsigned int i=0; i<totalVertices; ++i) {
		if (remapIds[i] != i)
			continue;
		NLMISC::CVector		weldTo = vertices[i];
		// select close vertices
		grid.select(vertices[i], vertices[i]);
		// for each selected vertex, remaps it to the current vertex
		NL3D::CQuadGrid<unsigned int>::CIterator	it;
		for (it=grid.begin(); it!=grid.end(); ++it) {
			uint	weldedId = *it;
			NLMISC::CVector		welded = vertices[weldedId];
			if (weldedId <= i || rootMeshVertices[i] == rootMeshVertices[weldedId] ||
				remapIds[weldedId] != weldedId || (welded-weldTo).norm() > WeldThreshold)
				continue;
			remapIds[weldedId] = i;
		}
	}
	for (unsigned int i=0; i<totalVertices; ++i) {
		if (remapIds[i] > i)
			printf("\tERROR: found a greater remap id\n");
		if (remapIds[i] == i) {
			uint	newId = remapVertices.size();
			remapVertices.push_back(vertices[i]);
			remapIds[i] = newId;
		} else {
			remapIds[i] = remapIds[remapIds[i]];
		}
	}
	for (unsigned int i=0; i<totalFaces; ++i)
		for (unsigned int j=0; j<3; ++j)
			mCollisionMeshBuild.Faces[i].V[j] = remapIds[mCollisionMeshBuild.Faces[i].V[j]];

	// check for errors
	std::vector<std::string>	warnings;
	for (unsigned int i=0; i<totalFaces; ++i)
	{
		if (mCollisionMeshBuild.Faces[i].V[0] == mCollisionMeshBuild.Faces[i].V[1] ||
			mCollisionMeshBuild.Faces[i].V[1] == mCollisionMeshBuild.Faces[i].V[2] ||
			mCollisionMeshBuild.Faces[i].V[2] == mCollisionMeshBuild.Faces[i].V[0])
		{
			warnings.push_back(std::string("mesh:") + rootMeshNames[facesRootMeshesInfo[i].first] + std::string(" face:") + NLMISC::toString(facesRootMeshesInfo[i].second));
		}
	}

	// and clean up the mesh if some errors appear
	std::vector<NLPACS::CCollisionFace>::iterator	it;
	for (it=mCollisionMeshBuild.Faces.begin(); it!=mCollisionMeshBuild.Faces.end(); ) {
		if ((*it).V[0] == (*it).V[1] ||
			(*it).V[1] == (*it).V[2] ||
			(*it).V[2] == (*it).V[0]) {
			it = mCollisionMeshBuild.Faces.erase(it);
		} else {
			++it;
		}
	}

	mCollisionMeshBuild.Vertices = remapVertices;

	// check bbox size
	NLMISC::CAABBox	box;
	if (!mCollisionMeshBuild.Vertices.empty())
	{
		box.setCenter(mCollisionMeshBuild.Vertices[0]);
		for (unsigned int i=1; i<mCollisionMeshBuild.Vertices.size(); ++i)
			box.extend(mCollisionMeshBuild.Vertices[i]);
	}

	NLMISC::CVector	hs = box.getHalfSize();
	if (hs.x > 255.0f || hs.y > 255.0f) {
		MGlobal::displayError("The bounding box of the selection exceeds 512 meters large!");
		return MStatus::kFailure;
	}
		
	// report warnings
	if (!warnings.empty())
	{
		std::string	message = "Warning(s) occured during collision export\n(defective links may result) error";
		for (unsigned int i=0; i<warnings.size(); ++i)
			message += std::string("\n")+warnings[i];

		printf("%s", message.c_str());
	}

//	printf("Vertices after weld %d\n", mCollisionMeshBuild.Vertices.size());

	/********************************************************
	 * Link...whatever that does.							*
	 ********************************************************/
	std::vector<std::string>	errors;
	mCollisionMeshBuild.link(false, errors);
	mCollisionMeshBuild.link(true, errors);
	if (!errors.empty()) {
		MString	message("Error(s) occured during collision export\n(edge issues)");
		for (unsigned int i=0; i<errors.size(); ++i)
			message += "ERROR: " + MString(errors[i].c_str()) + "\n";
		MGlobal::displayError(message);
		return MStatus::kFailure;
	}

	return MStatus::kSuccess;
}

void CNeLCollisionMeshWriter::setNumTriangles(MItMeshPolygon &itPolygon) {
	unsigned int count = 0;
	int numTriangles;
	for ( ; !itPolygon.isDone(); itPolygon.next()) {
		itPolygon.numTriangles(numTriangles);
		count += numTriangles;
	}
	itPolygon.reset();
	mCollisionMeshBuild.Faces.resize(mCollisionMeshBuild.Faces.size() + count);
}

bool CNeLCollisionMeshWriter::getEdgeVisibility(const MFnMesh &mesh, const int2 &vertices, MIntArray &edges) {
	int edge = findEdge(mesh, vertices, &edges);
	if(edge == -1)
		return true;
	bool visibility = true;
	mesh.getBoolBlindData(edge, MFn::kMeshEdgeComponent, 21, "visibility", visibility);
//	printf("\tGot edge visibility %d %d\n", edge, visibility);
	return visibility;
}

