// Local includes
#include "CNeLVegWriter.h"
#include "CArgs.h"
#include "general.h"

// Maya includes
#include <maya/MPlug.h>

// NeL includes
#include <nel/misc/file.h>


CNeLVegWriter::CNeLVegWriter(MDagPath dagPath, MStatus &status) : CNeLMesh(dagPath, status) {
}

CNeLVegWriter::~CNeLVegWriter() {
}

MStatus CNeLVegWriter::writeToFile(MString f) {
	NLMISC::COFile file;
	f += ".veget";
	if(file.open(f.asChar())) {
		try {
			mVegShape.serial(file);
		} catch(...) {
			return MStatus::kFailure;
		}
		file.close ();
	}
	return MStatus::kSuccess;
}

void CNeLVegWriter::extract(bool verbose, MStatus &status) {
	if(!mTransform)
		return;
	if(!verbose) {
		CArgs::instance().push_verbose();
		CArgs::instance().verboseMode = false;
	}
	mMesh = new MFnMesh(*mDagPath, &status);
	if(CArgs::instance().verboseMode) {
		printf("Full path name: %s\n", mDagPath->fullPathName().asChar());
		printf("\tTransform name: %s\n", mTransform->partialPathName().asChar());
		printf("\tMesh name     : %s\n", mMesh->name().asChar());
	}
	
	extractShaders();
	mBuildMesh.VertexFlags = 0;
	getShaders();
	extractGeometry();
	int space = (int)MSpace::kWorld;
	mTransform->findPlug("nelExportSpace").getValue(space);
	extractTransform((MSpace::Space)space);
	makeBuildBaseMesh();
	makeBuildMesh();
	status = makeVeg();
	if(!verbose) {
		CArgs::instance().pop_verbose();
	}
}

MStatus CNeLVegWriter::makeVeg() {
	if ((mBuildMesh.VertexFlags & NL3D::CVertexBuffer::TexCoord0Flag) == 0) {
		printf("ERROR: Must have UV coordinates and texture");
		return MStatus::kFailure;
	}

	NL3D::CMesh mesh;
	mesh.build(mBuildBaseMesh, mBuildMesh);

	if (mesh.getNbMatrixBlock () != 1) {
		printf("ERROR: The object can't be skinned");
		return MStatus::kFailure;
	}
	if (mesh.getNbRdrPass (0) != 1) {
		printf("ERROR: The object must have one material!");
		return MStatus::kFailure;
	}
	mVegShapeBuild.VB = mesh.getVertexBuffer ();
	mVegShapeBuild.PB = mesh.getRdrPassPrimitiveBlock (0, 0);

	mTransform->findPlug("AlphaBlend").getValue(mVegShapeBuild.AlphaBlend);

	if (mVegShapeBuild.AlphaBlend) {
		// Default options
		mVegShapeBuild.PreComputeLighting = true;
		mVegShapeBuild.DoubleSided = true;
		mTransform->findPlug("Lit").getValue(mVegShapeBuild.Lighted);
	}
	else {
		mTransform->findPlug("Lit").getValue(mVegShapeBuild.Lighted);
		mTransform->findPlug("PrecomputeLighting").getValue(mVegShapeBuild.PreComputeLighting);
		mMesh->findPlug("doubleSided").getValue(mVegShapeBuild.DoubleSided);
		printf("Got double sided: %d\n", mVegShapeBuild.DoubleSided);
	}

	if (mVegShapeBuild.PreComputeLighting) {
		mTransform->findPlug("BestSidedPrecomputeLighting").getValue(mVegShapeBuild.BestSidedPreComputeLighting);
	}
	mTransform->findPlug("MaxBendWeight").getValue(mVegShapeBuild.MaxBendWeight);
	int blendCenter;
	mTransform->findPlug("BlendCenter").getValue(blendCenter);
	mVegShapeBuild.BendCenterMode = (NL3D::CVegetableShapeBuild::TBendCenterMode)blendCenter;

	// Build it
	mVegShape.build(mVegShapeBuild);

	return MStatus::kSuccess;
}