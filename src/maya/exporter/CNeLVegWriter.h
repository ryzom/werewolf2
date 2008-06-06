#ifndef _CNELVEGWRITER_H_
#define _CNELVEGWRITER_H_

// Local includes
#include "CNeLMesh.h"

// NeL includes
#include <3d/vegetable_shape.h>

class CNeLVegWriter : public CNeLMesh {
public:
						CNeLVegWriter(MDagPath dagPath, MStatus& status);
						~CNeLVegWriter();
	virtual	void		extract(bool verbose, MStatus &status);
	MStatus				writeToFile (MString f);

protected:
	MStatus				makeVeg();

	NL3D::CVegetableShapeBuild	mVegShapeBuild;
	NL3D::CVegetableShape		mVegShape;
};

#endif // _CNELVEGWRITER_H_