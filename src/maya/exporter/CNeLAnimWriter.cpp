#include <set>

// Local includes
#include "CNeLAnimWriter.h"
#include "general.h"
#include "CArgs.h"

// NeL includes
#undef min
#undef max
#include <nel/misc/file.h>
#include <3d/transform.h>
#include <3d/key.h>
#include <3d/track_keyframer.h>

// Maya includes
#include <maya/MAnimUtil.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnDagNode.h>
#include <maya/MGlobal.h>
#include <maya/MObjectArray.h>
#include <maya/MItDag.h>
#include <maya/MFnIkJoint.h>
#include <maya/MAnimControl.h>
#include <maya/MQuaternion.h>
#include <maya/MFnTransform.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MPlugArray.h>
#include <maya/MEulerRotation.h>

CNeLAnimWriter::CNeLAnimWriter() {
// does nothing
}

CNeLAnimWriter::~CNeLAnimWriter() {
// does nothing
}

MStatus CNeLAnimWriter::writeToFile(MString f) {
	if(hasAnimation()) {
		NLMISC::COFile file;
		f += ".anim";
		if(file.open(f.asChar())) {
			try {
				anim.serial (file);
			}
			catch(...) {
				return MStatus::kFailure;
			}
			file.close ();
		}
	}
	return MStatus::kSuccess;
}

bool CNeLAnimWriter::hasAnimation() {
	std::set<std::string> trackNames;
	anim.getTrackNames(trackNames);
	if(trackNames.size() > 0) {
		return true;
	}
	return false;
}

MStatus CNeLAnimWriter::addMeshAnimation(const MDagPath path) {
	if(CArgs::instance().verboseMode) {
		printf("\tChecking mesh animation..\n");
		printf("\t\tMorph tracks...\n");
	}
	addMorphTracks(path);
	if(CArgs::instance().verboseMode)
		printf("\t\tTransform tracks...\n");
	MStatus status;
	MDagPath transformPath = getTransformPath(path, status);
	if(status != MStatus::kSuccess && CArgs::instance().verboseMode) {
		printf("\t\t\tERROR: No transform node found above %s\n", path.fullPathName().asChar());
	} else {
		addTransformTracks(transformPath);
	}
	return MStatus::kSuccess;
}

MStatus CNeLAnimWriter::addCameraAnimation(const MDagPath path) {
	return MStatus::kSuccess;
}

MStatus CNeLAnimWriter::addSkeletonAnimation(const MDagPath path) {
	if(CArgs::instance().verboseMode)
		printf("\tChecking for Skeleton animation...\n");
	MItDag itSkeleton(MItDag::kDepthFirst, MFn::kJoint);
	itSkeleton.reset(path, MItDag::kDepthFirst, MFn::kJoint);
	for(;!itSkeleton.isDone();itSkeleton.next()) {
		MDagPath bonePath;
		itSkeleton.getPath(bonePath);
		MFnIkJoint joint(bonePath);
		bool animated = false;
		joint.findPlug("NelExportAnimation").getValue(animated);
		if(MAnimUtil::isAnimated(itSkeleton.item()) || animated) {
			addBoneTracks(bonePath);
		}
	}
	return MStatus::kSuccess;
}

MStatus CNeLAnimWriter::addLODAnimation(const MDagPath path) {
	if(CArgs::instance().verboseMode)
		printf("\tChecking for LOD animation...\n");
	addTransformTracks(path);
	MFnDagNode lodNode(path.node());
	MPlug outPlug = lodNode.findPlug("output");
	for(unsigned int i = 0; i < outPlug.numElements(); i++) {
		MPlugArray conPlugs;
		outPlug[i].connectedTo(conPlugs, false, true);
		if(conPlugs.length() > 0) {
			MFnDagNode node(conPlugs[0].node());
			MDagPath transformPath;
			node.getPath(transformPath);
			MItDag it(MItDag::kDepthFirst, MFn::kMesh);
			it.reset(transformPath, MItDag::kDepthFirst, MFn::kMesh);
			for(;!it.isDone();it.next()) {
				MDagPath meshPath;
				it.getPath(meshPath);
				MFnDagNode meshNode(meshPath);
				if(meshNode.isIntermediateObject() || meshNode.object().apiType() != MFn::kMesh) {
					continue;
				}
				addMorphTracks(meshPath);
			}
		}
	}
	return MStatus::kSuccess;
}

void CNeLAnimWriter::addTransformTracks(const MDagPath path, MString prefix) {
	MStatus status;
	NL3D::ITrack *pTrack;

	MFnDagNode fDagNode(path.transform(), &status);
	bool animated = false;
	fDagNode.findPlug("NelExportAnimation").getValue(animated);
	if(status) {
		if(isCompoundAnimated(fDagNode.findPlug("translate")) || animated) {
			// temporary oversampling
			pTrack = getOversampledTranslation(path);
			if (pTrack) {
				MString name = prefix + MString (NL3D::ITransformable::getPosValueName());
				if (anim.getTrackByName (name.asChar())) {
					delete pTrack;
				} else {
					anim.addTrack (name.asChar(), pTrack);
				}
			}
		}
		if(isCompoundAnimated(fDagNode.findPlug("rotate")) || animated) {
			// temporary oversampling
			MString name = prefix;
			pTrack = getOversampledRotation(path);
			name += MString(NL3D::ITransformable::getRotQuatValueName());
			if (pTrack) {
				if (anim.getTrackByName (name.asChar())) {
					delete pTrack;
				} else {
					anim.addTrack (name.asChar(), pTrack);
				}
			}
		}
		if(isCompoundAnimated(fDagNode.findPlug("scale"))) {
			// temporary oversampling
			pTrack = getOversampledScale(path);
			if (pTrack) {
				MString name = prefix + MString(NL3D::ITransformable::getScaleValueName());
				if (anim.getTrackByName (name.asChar())) {
					delete pTrack;
				} else {
					anim.addTrack (name.asChar(), pTrack);
				}
			}
		}
	}

}

void CNeLAnimWriter::addMorphTracks(const MDagPath path) {
	MStatus status;
	NL3D::ITrack *pTrack;
	
	std::vector<MObject> blendShapes = findBlendShapes(path.node());
	for(unsigned int k = 0; k < blendShapes.size(); k++) {
		MFnBlendShapeDeformer fn(blendShapes[k], &status);
		if(status) {
			if(CArgs::instance().verboseMode)
				printf("\t\t\tChecking %s\n", fn.name().asChar());
			MIntArray weightIndices;
			fn.weightIndexList(weightIndices);
			for(unsigned int i = 0; i < fn.numWeights(); i++) {
				MObjectArray targets;
				fn.getTargets(path.node(), weightIndices[i], targets);
				// no inbetween support
				if(!targets[0].hasFn(MFn::kMesh))
					continue;
				MString name = getMorphName(targets[0]);
				if(CArgs::instance().verboseMode)
					printf("\t\t\t\tMorph target: %s...", name.asChar());
				MPlug plug = fn.findPlug("weight");
				plug = plug.elementByPhysicalIndex(weightIndices[i]);
				if(MAnimUtil::isAnimated(plug)) {
					if(CArgs::instance().verboseMode)
						printf("Found\n");
					pTrack = getOversampledMorph(fn, i);
					if(pTrack) {
						MString trackName = name + "MorphFactor";
						if(anim.getTrackByName(trackName.asChar())) {
							delete pTrack;
						} else {
							anim.addTrack(trackName.asChar(), pTrack);
						}
					}
				} else {
					if(CArgs::instance().verboseMode)
						printf("Not Found\n");
				}
			}
		}
	}
}

void CNeLAnimWriter::addBoneTracks(const MDagPath path) {
	if(CArgs::instance().verboseMode)
		printf("\t\tFound animation on %s\n", path.partialPathName().asChar());
	addTransformTracks(path, path.partialPathName() + ".");
}

void CNeLAnimWriter::addMaterialTracks(const MDagPath path) {
}

bool CNeLAnimWriter::isCompoundAnimated(MPlug plug) {
	if(MAnimUtil::isAnimated(plug))
		return true;
	for(unsigned int i = 0; i < plug.numChildren(); i++) {
		if(MAnimUtil::isAnimated(plug.child(i))) {
			return true;
		}
	}
	return false;
}

NL3D::ITrack* CNeLAnimWriter::convertAnimCurve(MPlug plug, float factor) {
//	MFnAnimCurve mCurve(plug);
// TODO: check for linear type
	return getBezierTrack(plug, factor);
}

NL3D::ITrack* CNeLAnimWriter::getBezierTrack(MPlug plug, float factor) {
	NL3D::CTrackKeyFramerBezierFloat* pTrack = new NL3D::CTrackKeyFramerBezierFloat;
	MFnAnimCurve mCurve(plug);

	if(mCurve.postInfinityType() == MFnAnimCurve::kCycle) {
		pTrack->setLoopMode(true);
	} else {
		pTrack->setLoopMode(false);
	}


	int numKeys = mCurve.numKeys();
	MTime convert(1.0, MTime::uiUnit());

	for(int i = 0; i < numKeys; i++) {
		NL3D::CKeyBezier<float> key;
		key.Value = (float)mCurve.value(i) * factor;
		float x, y;
		mCurve.getTangent(i, x, y, true);
		key.InTan = y;
		mCurve.getTangent(i, x, y, false);
		key.OutTan = y;
		if(mCurve.outTangentType(i) == MFnAnimCurve::kTangentStep) {
			key.Step = true;
		} else {
			key.Step = false;
		}
		pTrack->addKey(key, (float)(mCurve.time(i).value() * convert.as(MTime::kSeconds)));
	}
	return pTrack;
}

NL3D::ITrack* CNeLAnimWriter::getLinearTrack(MPlug plug, float factor) {
	// TODO: implement
	return new NL3D::CTrackKeyFramerLinearFloat;
}

NL3D::ITrack* CNeLAnimWriter::getOversampledRotation(const MDagPath path) {
	int time_step = 1;
	MFnTransform fn(path);
	MAnimControl animControl;

	NL3D::CTrackKeyFramerLinearQuat* pTrack = new NL3D::CTrackKeyFramerLinearQuat;
	MTime convert(1.0, MTime::uiUnit());
	MAngle convAngle(1.0, MAngle::kRadians);

	MTime start = animControl.animationStartTime();
	MTime end = animControl.animationEndTime();
	MTime current = animControl.currentTime();
	NLMISC::CQuat previous;
	for(int frame = (int)start.value(); frame <= end.value(); frame += time_step) {
		animControl.setCurrentTime(MTime((double)frame, MTime::uiUnit()));
		NL3D::CKeyQuat key;
		MQuaternion rotation;
		MTransformationMatrix m;
		if(path.node().hasFn(MFn::kJoint))
			m = path.inclusiveMatrix() * path.exclusiveMatrixInverse();
		else
			m = path.inclusiveMatrix();
		rotation = m.rotation();
		NLMISC::CMatrix mat;
		NLMISC::CQuat current = NLMISC::CQuat((float)rotation.x, (float)rotation.y, (float)rotation.z, (float)rotation.w);
		mat.rotate(current);
		key.Value = mat.getRot();
		if(frame != start.value()) {
			key.Value.makeClosest(previous);
		}
		previous = key.Value;
		pTrack->addKey(key, (float)((frame-1) * convert.as(MTime::kSeconds)));
	}
	animControl.setCurrentTime(current);
	return pTrack;
}

NL3D::ITrack* CNeLAnimWriter::getOversampledRotationEuler(const MDagPath path) {
	int time_step = 1;
	MFnTransform fn(path);
	MAnimControl animControl;

	NL3D::CTrackKeyFramerLinearVector* pTrack = new NL3D::CTrackKeyFramerLinearVector;
	MTime convert(1.0, MTime::uiUnit());

	MTime start = animControl.animationStartTime();
	MTime end = animControl.animationEndTime();
	MTime current = animControl.currentTime();
	for(int frame = (int)start.value(); frame <= end.value(); frame += time_step) {
		animControl.setCurrentTime(MTime((double)frame, MTime::uiUnit()));
		NL3D::CKeyVector key;
		MEulerRotation rotation;
		MTransformationMatrix m;
		if(path.node().hasFn(MFn::kJoint))
			m = path.inclusiveMatrix() * path.exclusiveMatrixInverse();
		else
			m = path.inclusiveMatrix();
		rotation = m.eulerRotation();
		key.Value = NLMISC::CVector((float)rotation.x, (float)rotation.y, (float)rotation.z);
		pTrack->addKey(key, (float)((frame-1) * convert.as(MTime::kSeconds)));
	}
	animControl.setCurrentTime(current);
	return pTrack;
}

NL3D::ITrack* CNeLAnimWriter::getOversampledTranslation(const MDagPath path) {
	int time_step = 1;
	MFnTransform fn(path);
	MAnimControl animControl;

	NL3D::CTrackKeyFramerLinearVector* pTrack = new NL3D::CTrackKeyFramerLinearVector;
	MTime convert(1.0, MTime::uiUnit());

	MTime start = animControl.animationStartTime();
	MTime end = animControl.animationEndTime();
	MTime current = animControl.currentTime();
	for(int frame = (int)start.value(); frame <= end.value(); frame += time_step) {
		animControl.setCurrentTime(MTime((double)frame, MTime::uiUnit()));
		NL3D::CKeyVector key;
		MVector translation;
		MSpace::Space spc;
		if(path.node().hasFn(MFn::kJoint))
			spc = MSpace::kTransform;
		else 
			spc = MSpace::kWorld;
		translation = fn.translation(spc);
		translation += fn.rotatePivot(MSpace::kTransform) - fn.scalePivot(MSpace::kTransform);
		key.Value = NLMISC::CVector((float)translation.x, (float)translation.y, (float)translation.z);
		pTrack->addKey(key, (float)((frame-1) * convert.as(MTime::kSeconds)));
	}
	animControl.setCurrentTime(current);
	return pTrack;
}

NL3D::ITrack* CNeLAnimWriter::getOversampledScale(const MDagPath path) {
	int time_step = 1;
	MFnTransform fn(path);
	MAnimControl animControl;

	NL3D::CTrackKeyFramerLinearVector* pTrack = new NL3D::CTrackKeyFramerLinearVector;
	MTime convert(1.0, MTime::uiUnit());

	MTime start = animControl.animationStartTime();
	MTime end = animControl.animationEndTime();
	MTime current = animControl.currentTime();
	for(int frame = (int)start.value(); frame <= end.value(); frame += time_step) {
		animControl.setCurrentTime(MTime((double)frame, MTime::uiUnit()));
		NL3D::CKeyVector key;
		MTransformationMatrix m;
		if(path.node().hasFn(MFn::kJoint))
			m = path.inclusiveMatrix() * path.exclusiveMatrixInverse();
		else
			m = path.inclusiveMatrix();
		double scale[3];
		m.getScale(scale, MSpace::kTransform);
		key.Value = NLMISC::CVector((float)scale[0], (float)scale[1], (float)scale[2]);
		pTrack->addKey(key, (float)((frame-1) * convert.as(MTime::kSeconds)));
	}
	animControl.setCurrentTime(current);
	return pTrack;
}

NL3D::ITrack* CNeLAnimWriter::getOversampledMorph(MFnBlendShapeDeformer fn, unsigned int i) {
	int time_step = 1;
	MAnimControl animControl;

	NL3D::CTrackKeyFramerLinearFloat* pTrack = new NL3D::CTrackKeyFramerLinearFloat;
	MTime convert(1.0, MTime::uiUnit());

	MTime start = animControl.animationStartTime();
	MTime end = animControl.animationEndTime();
	MTime current = animControl.currentTime();
	for(int frame = (int)start.value(); frame <= end.value(); frame += time_step) {
		animControl.setCurrentTime(MTime((double)frame, MTime::uiUnit()));
		NL3D::CKeyFloat key;
		key.Value = fn.weight(i) * 100;
		pTrack->addKey(key, (float)((frame-1) * convert.as(MTime::kSeconds)));
	}
	animControl.setCurrentTime(current);
	return pTrack;
}