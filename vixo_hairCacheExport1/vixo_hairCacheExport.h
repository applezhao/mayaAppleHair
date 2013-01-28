#pragma once
#include <maya\MFnPlugin.h>
#include <maya\MPxCommand.h>

#include <maya\MObjectArray.h>

//#include "vixo_hairNurbsCurve.h"

struct curvePoint
{
	float x;
	float y;
	float z;
};

class vixo_hairCacheExport:public MPxCommand
{
public:
	vixo_hairCacheExport(void);
	virtual~vixo_hairCacheExport(void);

	virtual MStatus doIt ( const MArgList& args );
    static void* creator();

private:
	bool existInSet(MObject& obj,MObjectArray & set);
};

MStatus initializePlugin( MObject obj )

{ 
	MStatus   status;

	MFnPlugin explugin( obj, "", "2011", "Any");
	status = explugin.registerCommand( "vixo_hairCacheExport", vixo_hairCacheExport::creator);
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
	status = explugin.deregisterCommand( "vixo_hairCacheExport" );
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}
	
	return status;
}

