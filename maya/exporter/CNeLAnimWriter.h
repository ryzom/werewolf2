#ifndef _CNELANIMWRITER_H_
#define _CNELANIMWRITER_H_

// NeL includes
#undef min
#undef max
#include "3d/animation.h"
#include <3d/track.h>

//Maya includes
#include <maya/MStatus.h>
#include <maya/MDagPath.h>
#include <maya/MString.h>
#include <maya/MPlug.h>
#include <maya/MFnBlendShapeDeformer.h>

class CNeLAnimWriter {

	public:

							CNeLAnimWriter ();
		virtual				~CNeLAnimWriter ();
		virtual MStatus		writeToFile (MString f);
		MStatus				addMeshAnimation(const MDagPath path);
		MStatus				addCameraAnimation(const MDagPath path);
		MStatus				addSkeletonAnimation(const MDagPath path);
		MStatus				addLODAnimation(const MDagPath path);

	private:
		
		NL3D::CAnimation	anim;
		bool				hasAnimation();
		void				addTransformTracks(const MDagPath path, MString prefix = "");
		void				addMorphTracks(const MDagPath path);
		void				addMaterialTracks(const MDagPath path);
		void				addBoneTracks(const MDagPath path);
		bool				isCompoundAnimated(MPlug plug);
		NL3D::ITrack*		convertAnimCurve(MPlug plug, float factor = 1.f);
		NL3D::ITrack*		getBezierTrack(MPlug plug, float factor);
		NL3D::ITrack*		getLinearTrack(MPlug plug, float factor);
		NL3D::ITrack*		getOversampledRotation(const MDagPath path);
		NL3D::ITrack*		getOversampledRotationEuler(const MDagPath path);
		NL3D::ITrack*		getOversampledTranslation(const MDagPath path);
		NL3D::ITrack*		getOversampledScale(const MDagPath path);
		NL3D::ITrack*		getOversampledMorph(MFnBlendShapeDeformer fn, unsigned int i);
};

#endif /* _CNELANIMWRITER_H_ */