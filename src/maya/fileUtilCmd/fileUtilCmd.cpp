#include <maya/MPxCommand.h>
#include <maya/MStatus.h>
#include <maya/MArgList.h>
#include <maya/MFnPlugin.h>
#include <maya/MString.h>

#include <sys/stat.h>
#include <ctime>

class fileUtil : public MPxCommand {
public:
	fileUtil();
	virtual     ~fileUtil();

	MStatus     doIt ( const MArgList& args );
	MStatus     redoIt ();
	MStatus     undoIt ();
	int			compareTime();
	bool        isUndoable() const;

	static      void* creator();

private:
	bool		compare;
	MString		file1;
	MString		file2;

};

fileUtil::fileUtil() {}

fileUtil::~fileUtil() {}

void* fileUtil::creator() {
	return new fileUtil;
}

bool fileUtil::isUndoable() const {
	return false;
}

MStatus fileUtil::undoIt() {
	return MS::kSuccess;
}

MStatus fileUtil::doIt( const MArgList& args ) {
	for (unsigned int i = 0; i < args.length (); i++) {
		MString argStr;
		args.get (i, argStr);
		if (MString ("-ct") == argStr || MString ("-compareTime") == argStr) {
			compare = true;
			break;
		}
	}

	if(compare) {
		args.get(args.length() - 2, file1);
		args.get(args.length() - 1, file2);
	}

	return redoIt();
}

MStatus fileUtil::redoIt() {
	if(compare) {
		setResult(compareTime());
	}
	return MS::kSuccess;
}

int fileUtil::compareTime() {
	time_t time1;
	time_t time2;
	struct stat attrib;
	stat(file1.asChar(), &attrib);
	time1 = attrib.st_mtime;
	stat(file2.asChar(), &attrib);
	time2 = attrib.st_mtime;
	if(time1 < time2) {
		return -1;
	} else if(time1 == time2) {
		return 0;
	}
	return 1;
}

MStatus initializePlugin( MObject obj ) {
	MStatus   status;
	MFnPlugin plugin( obj, "Henri Kuuste aka RTSan - An utility to deal with files", "1.0", "Any");

	status = plugin.registerCommand( "fileUtil", fileUtil::creator );
	if (!status) {
		status.perror("registerCommand");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj ) {
	MStatus   status;
	MFnPlugin plugin( obj );

	status = plugin.deregisterCommand( "fileUtil" );
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}

	return status;
}
