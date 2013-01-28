#pragma once

#include <maya/MGlobal.h>
#include <maya/MArgList.h>

#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshVertex.h>
#include <maya/MPointArray.h>

#include <maya/MItMeshFaceVertex.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItMeshVertex.h>

#include <maya/MFnVectorArrayData.h>
#include <maya/MVectorArray.h>
#include <maya/MFnMatrixData.h>
#include <maya/MMatrix.h>
#include <maya/MQuaternion.h>

#include <maya\MFnPlugin.h>
#include <maya\MPxCommand.h>


#include <map>
#include <set>
#include <vector>

#include <fstream>

struct vec3
{
	float x;
	float y;
	float z;
};

struct hairCacheVertexInfo
{
	int vid;
	vec3 position;
	vec3 normal;
	vec3 tangent;
};

struct forExportHairCache
{
	int faceid;
	int triid;
	hairCacheVertexInfo v0;
	hairCacheVertexInfo v1;
	hairCacheVertexInfo v2;
};

const int MAXSTEP=100;
const int MAXCONTROLSTEP=5;
struct forInVertexBasicInfo
{
	int vertexID;
	float distance;
	bool operator<(const forInVertexBasicInfo &a)const
	{
		return a.vertexID<vertexID;
	}
};

struct inVertexBasicInfo
{
	int cvNum;
	vector<set<forInVertexBasicInfo>> stepVertex;
};

struct outInVertexRelationInfo
{
	int inVID;
	int step;
	float distance;
	//float percent;
	bool operator<(const outInVertexRelationInfo &a)const
	{
		if(this->step<a.step)
			return true;
		else if(this->step==a.step)
		{
			if(this->inVID<a.inVID)
				return true;
			else return false;
		}
		else
			return false;
	}
};
//for out curve dataBase

struct outVIDCtrlInfo
{
	int inVID;
	float percent;
};

class vixo_hairCacheExport:public MPxCommand
{
public:
	vixo_hairCacheExport(void);
	virtual ~vixo_hairCacheExport(void);

	virtual MStatus doIt ( const MArgList& args );
	static void* creator();

	void initBasicStepInfo(int vid,MObject & staticMesh,map<int,inVertexBasicInfo>& inVIDMapInVertexDataBase);
	void buileRelationBetweenInOut(int inVID,map<int,inVertexBasicInfo>& inVIDMapInVertexDataBase,map<int,set<outInVertexRelationInfo>>& outVertexDataBase);
	void sortControlOrder(int outVID,map<int,set<outInVertexRelationInfo>>& outVertexDataBase,map<int,vector<outVIDCtrlInfo>>& outVertexControlData);
	void exportBasicData(MObject& dynamicMeshObj,vector<forExportHairCache>& exportData);

	void getInCurveInfo(map<int,int>& plugMapVID,MString vixoHairNode,map<int,MVectorArray>& inCurveShape);
	void buildCache(int maxSpan,MObject& dynamicMeshObj,vector<vec3>& outCurveCacheData,map<int,MVectorArray>& inCurveShape,map<int,vector<outVIDCtrlInfo>>& outVertexControlData);

	MString getFileName(MString cacheFileName,int frame);
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

