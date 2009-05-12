#ifndef _CNELBLENDSHAPE_H_
#define _CNELBLENDSHAPE_H_

#include "CNeLMesh.h"

class CNeLBlendShape : public CNeLMesh {
public:
						CNeLBlendShape(MDagPath dagPath, MStatus& status, CNeLMesh* parent);
						~CNeLBlendShape();
	virtual	void		extract(bool verbose = false);
	NL3D::CBlendShape	getBlendShape() { return mBlendShape; }

protected:
	MStatus				makeBlendShape();
	virtual void		exportFaces();

	NL3D::CBlendShape	mBlendShape;
	CNeLMesh*			mParent;
};

#endif // _CNELBLENDSHAPE_H_