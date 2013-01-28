#pragma once
#include <ri.h>
#include <maya\MFnPlugin.h>
#include <maya\MPxCommand.h>
#include <maya/MArgList.h>
#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>
#include <maya/MFnDagNode.h>


#include "noise.h"
#include "vixo_clumpFreq.h"

class vixo_hairCacheCommandDelight:public MPxCommand
{
public:
	vixo_hairCacheCommandDelight(void);
	virtual ~vixo_hairCacheCommandDelight(void);
	virtual MStatus doIt ( const MArgList& args );
	static void* creator();

	static MSyntax newSyntax();
	static MStringArray full;
	static MIntArray currentTime;
	MString printHairInfo(MString node,int frame);

	void getTexAndValue(MFnDagNode& fnDag,MString attrName,MString& texName,vec3& value);
	void getClumpCenter(hairParametersInfo& hairParameters);
	void subdivide(hairParametersInfo& hairParameters,MString paraFileName);
	void getBoundingBox(vector<vec3>& allPoints,vector<vec3>& box,int allPLen,float expand);
};

MStatus initializePlugin( MObject obj )

{ 
	MStatus   status;

	MFnPlugin explugin( obj, "", "2011", "Any");
	status = explugin.registerCommand( "vixo_hairCacheCommandDelight", vixo_hairCacheCommandDelight::creator,
		vixo_hairCacheCommandDelight::newSyntax);
	if (!status) {
		status.perror("registerCommand");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)

{
	MStatus   status;

	MFnPlugin explugin( obj );
	status = explugin.deregisterCommand( "vixo_hairCacheCommandDelight" );
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}

	return status;
}


