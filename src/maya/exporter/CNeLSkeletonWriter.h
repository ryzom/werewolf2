#ifndef _CNELSKELETONWRITER_H_
#define _CNELSKELETONWRITER_H_

// Local includes

// Standard includes
#include <vector>

// NeL includes
#undef min
#undef max
#include <3d/skeleton_shape.h>

// Maya includes
#include <maya/MDagPath.h>
#include <maya/MStatus.h>
#include <maya/MString.h>

class CNeLSkeletonWriter {

	public:
			
										CNeLSkeletonWriter(const MDagPath root, MStatus &status);
										~CNeLSkeletonWriter();
		virtual MStatus					writeToFile (MString f);

	protected:

		MStatus							extractSkeleton();

		MDagPath						mRoot;
		NL3D::CSkeletonShape			mSkeleton;
		std::vector<NL3D::CBoneBase>	mBones;

};

#endif // _CNELSKELETONWRITER_H_