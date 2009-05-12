#ifndef _CNELMESH_H_
#define _CNELMESH_H_

#define PI 3.14159265

// NeL includes
#undef min
#undef max
#include <3d/mesh.h>
#include <3d/mrm_parameters.h>

// Standard includes
#include <vector>

// Maya includes
#include <maya/MFnMesh.h>
#include <maya/MFnTransform.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatArray.h>
#include <maya/MIntArray.h>
#include <maya/MEulerRotation.h>
#include <maya/MPoint.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MQuaternion.h>
#include <maya/MVector.h>
#include <maya/MFnLambertShader.h>
#include <maya/MDagPath.h>
#include <maya/MStringArray.h>

class CNeLMesh {

	public:
							CNeLMesh () {};
							CNeLMesh (const MDagPath dagPath, MStatus& status, MSpace::Space vtxSpace = MSpace::kObject);
							~CNeLMesh ();
		virtual void		extract(bool verbose = true);
		NL3D::CMesh::CMeshBuild* getMeshBuild() { return &mBuildMesh; };
		NL3D::CMesh::CMeshBaseBuild* getMeshBaseBuild() { return &mBuildBaseMesh; };
		NL3D::IMeshGeom*	getMeshGeom(unsigned int materials = 0);
		MDagPath*			getDagPath() { return mDagPath; };
		MStatus				writeToFile (MString f);
		enum				TType { eNormal, eCluster, ePortal };

	protected:
		// Extraction functions. Found in CNeLMesh_extract.cpp
		MStatus				extractGeometry();
		MStatus				extractTransform(MSpace::Space spc = MSpace::kWorld);
		MStatus				extractShaders();

		// Conversion functions - that is the main functions. Found in CNeLMesh.cpp
		void				makeBuildBaseMesh();
		void				makeBuildMesh();
		void				setVertexFlags();
		void				exportVertices();
		virtual void		exportFaces();
		void				setNumTriangles(MItMeshPolygon& itPolygon);
		void				setVertexColor(unsigned int triangle, unsigned int polyCorner, unsigned int triangleCorner, MItMeshPolygon& itPolygon);
		void				setVertexUVs(unsigned int triangle, unsigned int polyCorner, unsigned int triangleCorner, MItMeshPolygon &itPolygon);
		void				buildTriangle(unsigned int triangle, MItMeshPolygon &itPolygon, MIntArray &triangleVertices, MIntArray &localVertices);
		void				getSkinning(MStatus &status);
		void				getMRMParams();
		void				getInterfaces();
		void				makeMesh();
		void				makeMRM();

		// Material conversion functions. Found in CNeLMesh_materials.cpp
		void				getShaders();
		void				buildVP();

		// BlendShape conversion function. Found in CNeLMesh_blendShape.cpp
		void				getNormalBlendShapes();
		void				getMRMBlendShapes();

		// Member variables
		NL3D::CMeshBase::CMeshBaseBuild mBuildBaseMesh;
		NL3D::CMesh::CMeshBuild			mBuildMesh;
		NL3D::CMRMParameters			mMRMParams;
		NL3D::IShape*					mShape;
		std::vector<NL3D::CMesh::CMeshBuild*> mBsList;

		MStringArray		mUVSets;
		MPointArray			mVertexArray;
		MFloatVectorArray	mNormalArray;

		MSpace::Space		mVertexSpc;
		MFnMesh*			mMesh;
		MFnTransform*		mTransform;
		MDagPath*			mDagPath;
		MObjectArray		mShaders;
		MIntArray			mShaderIndices;

		double				mScale[3];
		MQuaternion			mRot;
		MEulerRotation		mRotE;
		MVector				mTrans;
		MPoint				mPivot;
		bool				mbNeedVP;
		bool				mbNeedSpecularVP;
		float				mShininess;
		bool				mbLit;
		bool				mbIsMRM;
		bool				mbIsSkinned;
};

#endif /* _CNELMESH_H_ */
