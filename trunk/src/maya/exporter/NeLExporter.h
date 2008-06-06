#ifndef _NELEXPORTER_H_
#define _NELEXPORTER_H_

// Standard includes
#include <vector>

// Maya includes
#include <maya/MPxFileTranslator.h>
#include <maya/MSelectionList.h>

class MDagPath;
class MFnDagNode;
class MStatus;
class MString;
class MObject;

class CNeLExporter : public MPxFileTranslator  {

	public:
								CNeLExporter() {};
		virtual					~CNeLExporter();

		static	void*			creator();
				MString			defaultExtension () const;
				MStatus			initializePlugin(MObject obj);
				MStatus			uninitializePlugin(MObject obj);
		virtual MStatus			writer (const MFileObject& file,
										const MString& optionsString,
										MPxFileTranslator::FileAccessMode mode);
		virtual bool			haveWriteMethod () const;
		virtual bool			haveReadMethod () const;
		virtual	bool			canBeOpened () const;

	private:	
		virtual	MStatus			exportAll();
		virtual	MStatus			exportSelection();
		virtual MStatus			export(MSelectionList selection);
		virtual MStatus			processPolyMesh(const MDagPath dagPath);
		virtual MStatus			processSkeleton(const MDagPath dagPath);
		virtual MStatus			processLOD(const MDagPath dagPath);
		virtual MStatus			processBBCluster(const MDagPath dagPath);
		virtual MStatus			processLeafCluster(const MDagPath dagPath);
		virtual MStatus			processZone(const MDagPath dagPath);
		virtual MStatus			processCollisionMesh(std::vector<MDagPath> &dagPath, MString name);
		virtual MStatus			processVeg(const MDagPath dagPath);
};

#endif // _NELEXPORTER_H_
