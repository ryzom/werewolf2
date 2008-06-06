#ifndef _CNELMULTILODWRITER_H_
#define _CNELMULTILODWRITER_H_

// Local includes
#include "CNeLMesh.h"

// Standard includes
#include <vector>

// NeL includes
#include <3d/mesh_multi_lod.h>

// Maya includes
#include <maya/MDagPath.h>
#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MGlobal.h>
#include <maya/MFnDagNode.h>


struct LODMesh {
	MDagPath	path;
	float		dist;
};
typedef std::vector<LODMesh> LODMeshList;

class CNeLMultiLODWriter {

public:
							CNeLMultiLODWriter(const MDagPath path, MStatus &status);
							~CNeLMultiLODWriter();
	MStatus					writeToFile(MString f);

protected:
	MStatus					extractLOD();
	MStatus					makeLODBuild();

	MDagPath				mPath;
	MFnDagNode				mLODNode;
	LODMeshList				mLODMeshes;
	std::vector<CNeLMesh*>	mNeLMeshes;

	NL3D::CMeshMultiLod::CMeshMultiLodBuild mMultiLodBuild;
};

#endif // _CNELMULTILODWRITER_H_