// Author: Henri Kuuste

#ifndef __CBILLBOARDSHAPE_H__
#define __CBILLBOARDSHAPE_H__

// Maya includes
#include <maya/MStatus.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxSurfaceShape.h>
#include <maya/MBoundingBox.h>
#include <maya/MTypeId.h>
#include <maya/MDataBlock.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MColor.h>
#include <maya/MPointArray.h>
#include <maya/MDoubleArray.h>

// Render includes
#include <CG/cg.h>

/////////////////////////////////////////////////////////////////////
//
// Geometry class
//
class CBillboardGeom 
{
public:
	bool   useBBSize;
	double radiusX;
	double radiusY;
	double globalRadiusX;
	double globalRadiusY;

	bool   useOrientation;
	double orientation;

	double translucency;
	double faceNormal;
	bool   useRadialNormals;
	MPoint clusterCenter;
	double clusterRadius;
	bool   isLeaf;
	bool   useCastShadows;
	MPointArray casterPositions;
	MDoubleArray casterSizes;
	double penumbra;

	bool   useBBColor;
	MColor globalColor;
	MColor color;

	short  alignType;
	MVector constrainAxis;
};

/////////////////////////////////////////////////////////////////////
//
// Shape class - defines the non-UI part of a shape node
//
class CBillboardShape : public MPxSurfaceShape {
public:
	CBillboardShape();
	virtual ~CBillboardShape(); 

	virtual void			postConstructor();
	virtual MStatus			compute( const MPlug&, MDataBlock& );
					  
	virtual bool           	isBounded() const;
	virtual MBoundingBox   	boundingBox() const; 

	static  void *			creator();
	static  MStatus			initialize();

	CBillboardGeom*			geometry();

private:
	CBillboardGeom*			fGeometry;
	
	// Attributes
	//
	static  MObject		alignType;
	static	MObject		constrainAxis;

	static	MObject		useBBSize;
	static	MObject		radiusX;
	static	MObject		radiusY;
	static	MObject		globalRadiusX;
	static	MObject		globalRadiusY;

	static	MObject		translucency;
	static	MObject		useRadialNormals;
	static	MObject		faceNormal;
	static	MObject		clusterCenter;
	static	MObject		clusterRadius;
	static	MObject		isLeaf;
	static	MObject		useCastShadows;
	static	MObject		casterPositions;
	static	MObject		casterSizes;
	static	MObject		penumbra;

	static	MObject		useOrientation;
	static	MObject		orientation;

	static	MObject		useBBColor;
	static	MObject		globalBBColor;
	static	MObject		color;
 
public:
	// Shape type id
	//
	static	MTypeId		id;
};

#endif // __CBILLBOARDSHAPE_H__
