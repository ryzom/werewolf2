#ifndef _CNELIGWRITER_H_
#define _CNELIGWRITER_H_

// Standard includes
#include <vector>

// NeL includes
#include <3d/scene_group.h>

// Maya includes
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MDagPath.h>
#include <maya/MSelectionList.h>

class CNeLIGWriter {
	public:
			
												CNeLIGWriter(MSelectionList selection, MStatus &status);
												~CNeLIGWriter();
		virtual MStatus							writeToFile (MString f);

	protected:

		MStatus									getInstances();
		MStatus									getClusters();
		MStatus									getPortals();
		MStatus									linkInstance(NL3D::CInstanceGroup::CInstance &inst, MDagPath dagPath);
		MStatus									getLights();
		MStatus									makeIG();

		MSelectionList							m_selection;

		NL3D::CInstanceGroup					mIG;
		NL3D::CInstanceGroup::TInstanceArray	mInstances;
		std::vector<NL3D::CCluster>				mClusters;
		std::vector<NL3D::CPortal>				mPortals;
		std::vector<NL3D::CPointLightNamed>		mLights;
		
};

#endif // _CNELIGWRITER_H_