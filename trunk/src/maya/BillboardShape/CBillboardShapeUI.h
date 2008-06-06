// Author: Henri Kuuste

#ifndef __CBILLBOARDSHAPEUI_H__
#define __CBILLBOARDSHAPEUI_H__

// Maya includes
#include <maya/MPxSurfaceShapeUI.h>
#include <maya/MDrawInfo.h>
#include <maya/MDrawRequest.h>
#include <maya/M3dView.h>
#include <maya/MDrawRequestQueue.h>
#include <maya/MSelectInfo.h>
#include <maya/MSelectionList.h>
#include <maya/MPointArray.h>
#include <maya/MDrawData.h>
#include <maya/MVector.h>
#include <maya/MPoint.h>
#include <maya/MDagPathArray.h>
#include <maya/MMatrix.h>

// Render includes
#include <GL/gl.h>
#include <CG/cg.h>
#include <CG/cgGL.h>

// Global includes
#include <map>

// Local includes
#include "CBillboardShape.h"

typedef std::map<unsigned int, CGprogram> cgProgMap;

/////////////////////////////////////////////////////////////////////
//
// UI class	- defines the UI part of a shape node
//
class CBillboardShapeUI : public MPxSurfaceShapeUI {
public:
	CBillboardShapeUI();
	virtual ~CBillboardShapeUI(); 

	virtual void	getDrawRequests( const MDrawInfo & info, bool objectAndActiveOnly, MDrawRequestQueue & requests );
	virtual void	draw( const MDrawRequest & request, M3dView & view ) const;
	virtual bool	select( MSelectInfo &selectInfo, MSelectionList &selectionList, MPointArray &worldSpaceSelectPts ) const;

	void		getDrawRequestsWireframe( MDrawRequest&, const MDrawInfo& );
	void		getDrawRequestsShaded( MDrawRequest&, const MDrawInfo&,  MDrawRequestQueue&, MDrawData& data );

	static void*	creator();

private:
	void			createBillboard(MMatrix &inverseWorld, M3dView &view, CBillboardGeom *geom, MVector &x, MVector &y, MVector &z) const;
	void			createQuad(MPointArray &quad, MVector &x, MVector &y, MVector &z, MPoint pos) const;
	bool			intersectQuadrilateralRay(const MPointArray &quad, MPoint origin, MVector direction, MPoint &intersection) const;

	CGprogram		composeFragmentProgram();
	CGprogram		composeVertexProgram(M3dView &view, const MDrawRequest &request, MMatrix &inverseWorld);

	unsigned int	numLights;
	CGprofile		vProfile;
	CGprofile		fProfile;

	enum {
		kAutoAlign,
		kScreenAlign,
		kViewAlign,
		kAxisConstrain
	};

	// Draw Tokens
	//
	enum {
		kDrawWireframe,
		kDrawWireframeOnShaded,
		kDrawSmoothShaded,
		kDrawFlatShaded,
		kLastToken
	};
};

#endif // __CBILLBOARDSHAPEUI_H__
