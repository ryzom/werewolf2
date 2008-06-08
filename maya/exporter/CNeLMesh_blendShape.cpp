// Local includes
#include "CNeLMesh.h"
#include "general.h"
#include "CNeLBlendShape.h"
#include "CArgs.h"

//Maya includes
#include <maya/MFnBlendShapeDeformer.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MGlobal.h>


void CNeLMesh::getNormalBlendShapes() {
	MStatus status;
	std::vector<MObject> blendShapes = findBlendShapes(mDagPath->node());
	for(unsigned int k = 0; k < blendShapes.size(); k++) {
		MFnBlendShapeDeformer fn(blendShapes[k], &status);
		if(status) {
			if(CArgs::instance().verboseMode) {
				printf("\tSaving Blend Shape %s with %i weights\n", fn.name().asChar(), fn.numWeights());
				printf("\n==============================================================================\n\n");
			}
			MIntArray weightIndices;
			fn.weightIndexList(weightIndices);
			mBuildMesh.BlendShapes.resize(fn.numWeights());
			for(unsigned int i = 0; i < fn.numWeights(); i++) {
				MObjectArray targets;
				fn.getTargets(mDagPath->node(), weightIndices[i], targets);
				// no inbetween support
				if(!targets[0].hasFn(MFn::kMesh))
					continue;
				MFnMesh target(targets[0]);
				MString name = getMorphName(targets[0]);
				if(CArgs::instance().verboseMode)
					printf("\t\tGot blend shape target: %s\n", name.asChar());


				// TODO: also get tangent space if needed by building a CMeshBuild for the target
				// get the geometric data from the target
				MDagPath path;
				target.getPath(path);
				CNeLBlendShape targetMesh(path, status, this);
				if(!status) {
					if(CArgs::instance().verboseMode)
						printf("\t\t\tERROR: Failed to build target mesh\n");
					continue;
				}
				targetMesh.extract(CArgs::instance().dumpMode);

				mBuildMesh.BlendShapes[i] = targetMesh.getBlendShape();
			}
			if(CArgs::instance().verboseMode)
				printf("\n==============================================================================\n\n");
		}
	}
}

void CNeLMesh::getMRMBlendShapes() {
	MStatus status;
	std::vector<MObject> blendShapes = findBlendShapes(mDagPath->node());
	for(unsigned int k = 0; k < blendShapes.size(); k++) {
		MFnBlendShapeDeformer fn(blendShapes[k], &status);
		if(status) {
			if(CArgs::instance().verboseMode) {
				printf("\tSaving Blend Shape %s with %i weights\n", fn.name().asChar(), fn.numWeights());
				printf("\n==============================================================================\n\n");
			}
			MIntArray weightIndices;
			fn.weightIndexList(weightIndices);
			mBsList.resize(fn.numWeights());
			for(unsigned int i = 0; i < fn.numWeights(); i++) {
				MObjectArray targets;
				fn.getTargets(mDagPath->node(), weightIndices[i], targets);
				// no inbetween support
				if(!targets[0].hasFn(MFn::kMesh))
					continue;
				MFnMesh target(targets[0]);
				MString name = getMorphName(targets[0]);
				if(CArgs::instance().verboseMode)
					printf("\t\tGot blend shape target: %s\n", name.asChar());

				MDagPath path;
				target.getPath(path);
				CNeLMesh targetMesh(path, status);
				if(!status) {
					if(CArgs::instance().verboseMode)
						printf("\t\t\tERROR: Failed to build target mesh\n");
					continue;
				}
				targetMesh.extract(CArgs::instance().dumpMode);

				mBsList[i] = new NL3D::CMesh::CMeshBuild(*targetMesh.getMeshBuild());
			}
			if(CArgs::instance().verboseMode)
				printf("\n==============================================================================\n\n");

		}
	}
}