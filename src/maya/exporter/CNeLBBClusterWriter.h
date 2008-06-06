#ifndef _CNELBBCLUSTERWRITER_H_
#define _CNELBBCLUSTERWRITER_H_

// Local includes
#include "general.h"

// Standard includes
#include <vector>

// NeL includes
#undef min
#undef max
#include <Shapes/CBBClusterShape.h>

// Maya includes
#include <maya/MDagPath.h>
#include <maya/MString.h>
#include <maya/MStatus.h>
#include <maya/MGlobal.h>
#include <maya/MFnDagNode.h>


struct ClusterLODGroup {
	MDagPath	path;
	float		dist;
};
typedef std::vector<ClusterLODGroup> ClusterLODGroupList;

class CNeLBBClusterWriter {

public:
							CNeLBBClusterWriter(const MDagPath path, MStatus &status);
							~CNeLBBClusterWriter();
	MStatus					writeToFile(MString f);
	virtual void			init();

protected:
	MStatus					extractLOD();
	MStatus					makeClusterBuild();
	MStatus					addSlot(int index);
	virtual void			addExtraSlot(int index, NL3D::CBBClusterShape::CBBClusterBuild::CBBBuildSlot *slot) { /* nothing here */ };
	virtual void			createBuild() { mClusterBuild = new NL3D::CBBClusterShape::CBBClusterBuild(); };
	virtual NL3D::CBBClusterShape *createShape() { return new NL3D::CBBClusterShape(); };
	virtual NL3D::CBBClusterShape::CBBClusterBuild::CBBBuildSlot *createSlot() { return new NL3D::CBBClusterShape::CBBClusterBuild::CBBBuildSlot(); };
	virtual bool			isSlot(MDagPath path) { return isBBClusterSlot(path); };

	MDagPath				mPath;
	MFnDagNode				mLODNode;
	ClusterLODGroupList		mLODGroups;

	bool					mbHasMaterial;

	NL3D::CBBClusterShape::CBBClusterBuild *mClusterBuild;
};

#endif // _CNELBBCLUSTERWRITER_H_