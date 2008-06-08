#ifndef _CARGS_H_
#define _CARGS_H_

// Standard includes
#include <map>
#include <string>
#include <stack>

// Local includes
#include "ISingleton.h"

// Maya includes
#include <maya/MString.h>
#include <maya/MFileObject.h>
#include <maya/MMatrix.h>

class CArgs : public ISingleton<CArgs> {

public:
						CArgs();
						~CArgs() { };

	void				init(MString args);

	void				reset();
	bool				isValid() {return mValid;}
	void				debugOutput();

	void				setFile(MFileObject file);
	
	void				push_verbose() { mStateStack.push(verboseMode); };
	void				pop_verbose(){ verboseMode = mStateStack.top(); mStateStack.pop(); };

	typedef	std::map<std::string, void (CArgs::*)(MString)> ParserMap;

	bool				verboseMode,
						dumpMode,
						exportMesh,
						exportSkeleton,
						exportVBA,
						exportNormals,
						exportColours,
						exportUVs,
						exportAnimation,
						exportWater,
						exportLOD,
						exportZone,
						exportIG,
						exportCollision,
						exportBillboard,
						exportLeaf,
						exportVeget;

	MString				fileName;
	MString				filePath;

	MString				locMeshes;
	MString				locZones;
	MString				locIG;
	MString				locCollisions;
	MString				locBillboards;
	MString				locLeaves;
	MString				locSkeletons;
	MString				locAnimations;
	MString				locWater;
	MString				locVegets;
	MString				locTextures;
	MMatrix				conversionMatrix;
	MMatrix				scaleConversionMatrix;
protected:

	MString				mArgs;
	MFileObject			mFile;

	bool				mValid;
	ParserMap			mParserMap;
	std::stack<bool>	mStateStack;

	void				parseVerbose(MString value);
	void				parseDump(MString value);
	void				parseMesh(MString value);
	void				parseSkeleton(MString value);
	void				parseVBA(MString value);
	void				parseNormals(MString value);
	void				parseColours(MString value);
	void				parseUVs(MString value);
	void				parseAnimation(MString value);
	void				parseWater(MString value);
	void				parseLOD(MString value);
	void				parseZone(MString value);
	void				parseIG(MString value);
	void				parseCollision(MString value);
	void				parseBillboard(MString value);
	void				parseLeaf(MString value);
	void				parseVeget(MString value);

	void				parseLocMeshes(MString value);
	void				parseLocSkeletons(MString value);
	void				parseLocAnimations(MString value);
	void				parseLocWater(MString value);
	void				parseLocZones(MString value);
	void				parseLocIG(MString value);
	void				parseLocCollisions(MString value);
	void				parseLocBillboards(MString value);
	void				parseLocLeaves(MString value);
	void				parseLocVegets(MString value);
	void				parseLocTextures(MString value);

	MString				correctPath(MString value);
	void				createFolder(MString value);
};

#endif // _CARGS_H_