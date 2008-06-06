// Local includes
#include "CNeLMesh.h"
#include "general.h"
#include "CArgs.h"

// NeL includes
#include <3d/texture_file.h>
#include <3d/meshvp_per_pixel_light.h>
#include <3d/tangent_space_build.h>

// Maya includes
#include <maya/MPlug.h>
#include <maya/MGlobal.h>
#include <maya/MFnReflectShader.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnBlinnShader.h>

void CNeLMesh::getShaders() {
	MStatus status;

	int count = mShaders.length();
	mBuildBaseMesh.Materials.clear();
	if(count < 1) {
		// there are still problems with having no materials
		if(CArgs::instance().verboseMode)
			printf("\tERROR: No materials found. Using default.\n");
		mBuildBaseMesh.Materials.resize(1);
		mBuildBaseMesh.Materials[0].initLighted();
		mBuildBaseMesh.Materials[0].setLighting(true, NLMISC::CRGBA::Black, NLMISC::CRGBA::White, NLMISC::CRGBA::White, NLMISC::CRGBA::Black);
	} else {
		mBuildBaseMesh.Materials.resize(count);
	}

	NL3D::CMaterial* mat;
	for(int i=0; i<count; i++) {
		mbLit = true;
		mBuildBaseMesh.Materials[i] = NL3D::CMaterial();
		mat = &mBuildBaseMesh.Materials[i];
		getMaterial(mat, *mMesh, mShaders[i]);
		if(mat->getShader() == NL3D::CMaterial::PerPixelLighting || mat->getShader() == NL3D::CMaterial::PerPixelLightingNoSpec) {
			mbNeedVP = true;
			if(mat->getShader() == NL3D::CMaterial::PerPixelLighting) {
				mbNeedSpecularVP = true;
			}
		}
	}
}

void CNeLMesh::buildVP() {
	if(mbNeedVP) {
		if(CArgs::instance().verboseMode)
			printf("\tBuilding vertex program and tangent space\n");
		NL3D::CMesh::CMeshBuild tempMb;
		if (NL3D::BuildTangentSpace(tempMb, mBuildMesh)) {
			if(CArgs::instance().verboseMode)
				printf("\t\tBuilt tangent space\n");
			mBuildMesh = tempMb;
			NL3D::CMeshVPPerPixelLight *vp = new NL3D::CMeshVPPerPixelLight;
			vp->SpecularLighting = mbNeedSpecularVP;
			mBuildMesh.MeshVertexProgram = vp;
		}
	}
}