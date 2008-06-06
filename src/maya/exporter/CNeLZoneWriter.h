#ifndef _CNELZONEWRITER_H_
#define _CNELZONEWRITER_H_

// NeL Includes
#include <3d/zone.h>
#include <3d/bezier_patch.h>

// Maya includes
#include <maya/MDagPath.h>
#include <maya/MString.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MStatus.h>
#include <maya/MPlug.h>


class CNeLZoneWriter {
	public:

							CNeLZoneWriter (const MDagPath path);
		virtual				~CNeLZoneWriter ();
		virtual MStatus		writeToFile (MString f);

	protected:
		
		void				displayCV(unsigned int uPatch, unsigned int vPatch, unsigned int u, unsigned int v);
		void				insertCV(unsigned int uPatch, unsigned int vPatch, unsigned int u, unsigned int v, NLMISC::CVector *vectorArray);
		void				extractPatches();
		void				extractPatch(unsigned int uPatch, unsigned int vPatch);
		bool				getPatchPainter();
		bool				getTilesFromPatch(unsigned int uPatch, unsigned int vPatch, MPlug &tiles, MPlug &tileColors);
		void				getTileFromPlug(unsigned int index, MPlug &tiles, int &layer0, int &layer1, int &layer2);
		NLMISC::CRGBA		getTileColorFromPlug(unsigned int u, unsigned int v, unsigned int numUTiles, MPlug &tileColors);				
		void				makeZone();

		bool				mbHasPatchPainter;
		MDagPath			mPath;
		MFnNurbsSurface		mSurface;
		MFnDependencyNode	mPatchPainter;
		MPlug				mPatchesPlug;
		std::vector<NL3D::CPatchInfo> mPatchInfos;
		NL3D::CZone			mZone;
};

#endif // _CNELZONEWRITER_H_
