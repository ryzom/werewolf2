#ifndef _GENERAL_H_
#define _GENERAL_H_

// NeL includes
#include <nel/misc/rgba.h>
#include <3d/material.h>

// Maya includes
#include <maya/MColor.h>
#include <maya/MString.h>
#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MQuaternion.h>
#include <maya/MEulerRotation.h>
#include <maya/MVector.h>
#include <maya/MPoint.h>
#include <maya/MFnMesh.h>
#include <maya/MFnLambertShader.h>
#include <maya/MFnDagNode.h>

NLMISC::CRGBA mColor2CRGBA(const MColor &c);

std::vector<MObject> findBlendShapes(MObject &mesh);
MString getMorphName(MObject &mesh);
MObject findShader(const MObject& setNode);
MObject findTexture (const MObject& shaderNode, const MString& channel);
bool isLOD(MDagPath path);
bool isBB(MDagPath path);
bool isBBCluster(MDagPath path);
bool isBBClusterSlot(MDagPath path);
bool isLeafCluster(MDagPath path);
bool isLeafClusterSlot(MDagPath path);
bool isCollisionMesh(MDagPath path);
bool isVeg(MDagPath path);
bool getColorAttribute(MString name, MObject object, MColor &c);
MString dag2FilePath(MString path);
bool isWater(MDagPath path);
bool isVisible(MFnDagNode& fnDag, MStatus& status);
MDagPath getTransformPath(MDagPath path, MStatus &status);
MStatus getTransforms(MDagPath path,
				   double scale[3],
				   MQuaternion &rot,
				   MEulerRotation &rotE,
				   MVector &trans,
				   MPoint &pivot,
				   MSpace::Space spc = MSpace::kWorld);
MStatus getMaterial(NL3D::CMaterial *mat, MFnMesh &mesh, MObject &shader);
void getNelShader(NL3D::CMaterial* mat, MFnLambertShader &shader, bool &bLit);
void getLocalIndices(MIntArray &localIndices, const MIntArray &objIndices, const MIntArray &polyIndices);
int findEdge(const MFnMesh &mesh, const int2 &vertices, MIntArray *edges = NULL);

#endif // _GENERAL_H_