#ifndef _CNELLEAFCLUSTERWRITER_H_
#define _CNELLEAFCLUSTERWRITER_H_

// Local includes
#include "CNeLBBClusterWriter.h"

// Standard includes

// NeL includes
#undef min
#undef max
#include <Shapes/CLeafClusterShape.h>

class CNeLLeafClusterWriter : public CNeLBBClusterWriter {

public:
							CNeLLeafClusterWriter(const MDagPath path, MStatus &status);
							~CNeLLeafClusterWriter();
	virtual void			init();

protected:
	virtual void			addExtraSlot(int index, NL3D::CBBClusterShape::CBBClusterBuild::CBBBuildSlot *slot);
	virtual void			createBuild() { mClusterBuild = new NL3D::CLeafClusterShape::CLeafClusterBuild(); };
	virtual NL3D::CBBClusterShape *createShape() { return new NL3D::CLeafClusterShape(); };
	virtual NL3D::CBBClusterShape::CBBClusterBuild::CBBBuildSlot *createSlot() { return new NL3D::CLeafClusterShape::CLeafClusterBuild::CLeafBuildSlot(); };
	virtual bool			isSlot(MDagPath path) { return isLeafClusterSlot(path); };

};

#endif // _CNELLEAFCLUSTERWRITER_H_
