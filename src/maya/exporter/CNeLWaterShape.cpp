// Local includes
#include "CNeLWaterWriter.h"
#include "general.h"
#include "CArgs.h"

// NeL includes
#undef min
#undef max
#include <nel/misc/polygon.h>
#include <3d/texture_blend.h>
#include <3d/texture_file.h>
#include <nel/misc/file.h>
#include <nel/misc/smart_ptr.h>
#include <3d/water_shape.h>


// Maya includes
#include <maya/MFnLambertShader.h>
#include <maya/MPlug.h>
#include <maya/MString.h>
#include <maya/MGlobal.h>

CNeLWaterWriter::CNeLWaterWriter(const MDagPath dagPath, MStatus &status) {
	mDagPath = new MDagPath(dagPath);
	mTransform = new MFnTransform(getTransformPath(dagPath, status), &status);
	if(status != MStatus::kSuccess)
		mTransform = NULL;
}

CNeLWaterWriter::~CNeLWaterWriter() {
}

void CNeLWaterWriter::extract(bool verbose) {
	MStatus status;
	mMesh = new MFnMesh(*mDagPath, &status);
	if(CArgs::instance().verboseMode) {
		printf("Found some water :)\n");
		printf("Full path name: %s\n", mDagPath->fullPathName().asChar());
		printf("\tTransform name: %s\n", mTransform->partialPathName().asChar());
		printf("\tMesh name     : %s\n", mMesh->name().asChar());
	}
	extractShaders();
	extractGeometry();
	extractTransform();
	makeWater();
}

void CNeLWaterWriter::makeWater() {
	NL3D::CWaterShape* waterShape = new NL3D::CWaterShape;
	NLMISC::CPolygon   dest;
	dest.Vertices.resize(mVertexArray.length());
	for(int i = 0; i < mVertexArray.length(); i++) {
		dest.Vertices.push_back(NLMISC::CVector((float)mVertexArray[i].x, (float)mVertexArray[i].y, (float)mVertexArray[i].z));
	}

	static const float proj[] = { 1, 0, 0, 0,
								  0, 1, 0, 0,
								  0, 0, 0, 0,
								  0, 0, 0, 0 };
	NLMISC::CMatrix projMat;
	projMat.set(proj);
	NLMISC::CPolygon2D projDest(dest, projMat); // project the poly
	NLMISC::CPolygon2D convexPoly;
	projDest.buildConvexHull(convexPoly);

	waterShape->setShape(convexPoly);

	waterShape->getDefaultPos()->setDefaultValue(NLMISC::CVector((float)mTrans.x, (float)mTrans.z, (float)mTrans.y));					
	waterShape->getDefaultScale()->setDefaultValue(NLMISC::CVector((float)mScale[0], (float)mScale[1], (float)mScale[2]));
	waterShape->getDefaultRotQuat()->setDefaultValue(NLMISC::CQuat((float)mRot.x, (float)mRot.y, (float)mRot.z, (float)mRot.w));

	int count = mShaders.length();
	for(int i=0; i<count; i++) {
		MObject surfaceShader = findShader(mShaders[i]);
		if(surfaceShader != MObject::kNullObj) {
			MFnLambertShader shaderNode(surfaceShader);
			bool isWater = false;
			shaderNode.findPlug("NeLWater").getValue(isWater);
			if(isWater) {
				if(CArgs::instance().verboseMode)
					printf("\tFound NeL Water Shader %s\n", shaderNode.name().asChar());
				int poolId;
				if(shaderNode.findPlug("WaterPoolId").getValue(poolId))
					waterShape->setWaterPoolID(poolId);

				float waterHeightFactor;
				if(shaderNode.findPlug("WaterHeightFactor").getValue(waterHeightFactor))
					waterShape->setWaveHeightFactor(waterHeightFactor);

				NLMISC::CVector2f bumpMapScale;
				NLMISC::CVector2f bumpMapSpeed;
				NLMISC::CVector2f displaceMapScale;
				NLMISC::CVector2f displaceMapSpeed;

				shaderNode.findPlug("BumpMapUScale").getValue(bumpMapScale.x);
				shaderNode.findPlug("BumpMapVScale").getValue(bumpMapScale.y);
				shaderNode.findPlug("BumpMapUSpeed").getValue(bumpMapSpeed.x);
				shaderNode.findPlug("BumpMapVSpeed").getValue(bumpMapSpeed.y);

				shaderNode.findPlug("DisplacementMapUScale").getValue(displaceMapScale.x);
				shaderNode.findPlug("DisplacementMapVScale").getValue(displaceMapScale.y);
				shaderNode.findPlug("DisplacementMapUSpeed").getValue(displaceMapSpeed.x);
				shaderNode.findPlug("DisplacementMapVSpeed").getValue(displaceMapSpeed.y);

				waterShape->setHeightMapScale(0, displaceMapScale);
				waterShape->setHeightMapScale(1, bumpMapScale);
                
				waterShape->setHeightMapSpeed(0, displaceMapSpeed);
				waterShape->setHeightMapSpeed(1, bumpMapSpeed);

				NLMISC::CSmartPtr<NL3D::ITexture> pTex = NULL;
				MString tex1Name, tex2Name;

				if(shaderNode.findPlug("EnvironmentMapDay").getValue(tex1Name) && (tex1Name.length() > 0)) {
					if(CArgs::instance().verboseMode)
						printf("\tEnvironment Map Day              : %s\n", tex1Name.asChar());
					if(shaderNode.findPlug("EnvironmentMapNight").getValue(tex2Name) && (tex2Name.length() > 0)) {
						if(CArgs::instance().verboseMode)
							printf("\tEnvironment Map Night            : %s\n", tex2Name.asChar());
						NLMISC::CSmartPtr<NL3D::ITexture> tex0, tex1;
						tex0 = new NL3D::CTextureFile(tex1Name.asChar());
						tex1 = new NL3D::CTextureFile(tex2Name.asChar());;
						pTex = new NL3D::CTextureBlend;
						(static_cast<NL3D::CTextureBlend *>((NL3D::ITexture *) pTex))->setBlendTexture(0, tex0);
						(static_cast<NL3D::CTextureBlend *>((NL3D::ITexture *) pTex))->setBlendTexture(1, tex1);
					} else {
						pTex = new NL3D::CTextureFile(tex1Name.asChar());
					}
					waterShape->setEnvMap(0, (NL3D::ITexture *) pTex);
				}

				if(shaderNode.findPlug("EnvironmentMapDayUnderwater").getValue(tex1Name) && (tex1Name.length() > 0)) {
					if(CArgs::instance().verboseMode)
						printf("\tEnvironment Map Day Underwater   : %s\n", tex1Name.asChar());
					if(shaderNode.findPlug("EnvironmentMapNightUnderwater").getValue(tex2Name) && (tex2Name.length() > 0)) {
						if(CArgs::instance().verboseMode)
							printf("\tEnvironment Map Night Underwater : %s\n", tex2Name.asChar());
						NLMISC::CSmartPtr<NL3D::ITexture> tex0, tex1;
						tex0 = new NL3D::CTextureFile(tex1Name.asChar());
						tex1 = new NL3D::CTextureFile(tex2Name.asChar());;
						pTex = new NL3D::CTextureBlend;
						(static_cast<NL3D::CTextureBlend *>((NL3D::ITexture *) pTex))->setBlendTexture(0, tex0);
						(static_cast<NL3D::CTextureBlend *>((NL3D::ITexture *) pTex))->setBlendTexture(1, tex1);
					} else {
						pTex = new NL3D::CTextureFile(tex1Name.asChar());
					}
					waterShape->setEnvMap(1, (NL3D::ITexture *) pTex);
				}

				if(shaderNode.findPlug("BumpMap").getValue(tex1Name) && (tex1Name.length() > 0)) {
					if(CArgs::instance().verboseMode)
						printf("\tBump Map                         : %s\n", tex1Name.asChar());
					pTex = new NL3D::CTextureFile(tex1Name.asChar());
					waterShape->setHeightMap(1, (NL3D::ITexture *) pTex);
				}

				if(shaderNode.findPlug("DisplacementMap").getValue(tex1Name) && (tex1Name.length() > 0)) {
					if(CArgs::instance().verboseMode)
						printf("\tDisplacement Map                 : %s\n", tex1Name.asChar());
					pTex = new NL3D::CTextureFile(tex1Name.asChar());
					waterShape->setHeightMap(0, (NL3D::ITexture *) pTex);
				}

				if(shaderNode.findPlug("ColorMap").getValue(tex1Name) && (tex1Name.length() > 0)) {
					if(CArgs::instance().verboseMode)
						printf("\tColor Map                        : %s\n", tex1Name.asChar());
					pTex = new NL3D::CTextureFile(tex1Name.asChar());
					waterShape->setColorMap((NL3D::ITexture *) pTex);
				}
				break;
			}
		}
	}
	mShape = waterShape;
}