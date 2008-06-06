#ifndef _CNELCOLLISIONMESHWRITER_H_
#define _CNELCOLLISIONMESHWRITER_H_

// NeL includes
#include <pacs/collision_mesh_build.h>

// Standard includes
#include <vector>

// Maya includes
#include <maya/MDagPath.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFnMesh.h>
#include <maya/MIntArray.h>

class CNeLCollisionMeshWriter {
public:
	CNeLCollisionMeshWriter (std::vector<MDagPath> &dagPath, MStatus &status);
	~CNeLCollisionMeshWriter ();

	MStatus				extract();
	MStatus				writeToFile (MString f);

private:
	void				setNumTriangles(MItMeshPolygon &itPolygon);
	bool				getEdgeVisibility(const MFnMesh &mesh, const int2 &vertices, MIntArray &edges);
	
	enum { eInterior, eExterior };

	std::vector<MDagPath>	mDagPath;

	NLPACS::CCollisionMeshBuild mCollisionMeshBuild;

};

#endif // _CNELCOLLISIONMESHWRITER_H_