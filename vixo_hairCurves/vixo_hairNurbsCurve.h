#pragma once

#include <maya\MPxNode.h>
#include <maya\MFnPlugin.h>

#include <maya\MFnTypedAttribute.h>
#include <maya\MFnNumericAttribute.h>
#include <maya\MFnCompoundAttribute.h>
#include <maya\MFnNumericData.h>

#include <maya\MFnMesh.h>
#include <maya\MFnMeshData.h>

#include <maya\MIntArray.h>
#include <maya\MPointArray.h>
#include <maya\MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya\MFnNurbsCurve.h>

#include <maya\MPlug.h>
#include <maya\MPlugArray.h>
#include <maya\MImage.h>
#include <maya\MItMeshVertex.h>
#include <maya\MItMeshPolygon.h>
#include <maya\MFnDependencyNode.h>

#include <maya\MQuaternion.h>
#include <maya\MEulerRotation.h>


#include <maya\MTransformationMatrix.h>

#include <maya\MGlobal.h>

#include <maya\MFnNurbsCurve.h>
#include <maya\MFnNurbsCurveData.h>

#include <map>

#include <math.h>

#include <atltime.h>

#include "vixo_typedef.h"

const int MAXSTEP=100;
const int MAXCONTROLSTEP=3;

class vixo_hairNurbsCurve: public MPxNode
{

public:
	static MObject inMesh;
	static MObject inCurves;
		static MObject curveShape;
		static MObject vertexID;

	static MObject densityMapRefresh;
	static MObject densityMap;
	static MObject showControl;
	static MObject stateRefresh;

	static MObject outCurvesNum;
	static MObject outCurves;
		static MObject outCurveShape;
		static MObject outU;
		static MObject outV;
		static MObject outVID;
		static MObject show;

	static MObject inMeshUpdate;

	static MTypeId id;

public:
	vixo_hairNurbsCurve(void);
	virtual~vixo_hairNurbsCurve(void);

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );
	static  void*		creator();
	static  MStatus		initialize();

	virtual MStatus connectionMade(const MPlug& plug,const MPlug& otherPlug,bool asSrc);
	virtual MStatus connectionBroken(const MPlug& plug,const MPlug& otherPlug,bool asSrc);

	virtual MStatus setDependentsDirty(const MPlug&  plugBeingDirtied, MPlugArray&  affectedPlugs); 


private:
	//common
	void cmpOldNewSet(set<int> oldSet,set<int> newSet,MIntArray &del,MIntArray &add);
	vector<float> uvs;
	vector<MPoint> positions;

	int outCurvesPoints;
	//data

	map<int,int> outPID2VID;
	map<int,outCurvesInfo> outCurvesData;
	set<int> oldOutCurvesSet;
	set<int> newOutCurvesSet;
	void initOutCurveBasicInfo(MDataBlock& data);
	void addNewOutCurves(MIntArray add);
	void postDealDensityMapUpdate(MDataBlock& data);

	map<int,inCurvesInfo> inCurvesData;
	set<int> oldInCurvesSet;
	set<int> newInCurvesSet;
	void getInCurvesInfo(MDataBlock& data);
	void initBasicStepInfo(int vid,MDataBlock & data,vector<set<int>> &nearestPoints);
	
	void updateOutCurvesBasicInfo(MIntArray del,MIntArray add);
	//void updateOutCurvesCtrlIDPercentInfo();
	void updateOutCurveCtrlIDPercentInfo(outCurvesInfo& needToUpdate);


	void buildOutCurvesShape(int outVID,MDoubleArray knots,MObject &obj);

	void outCurveShapeUpdateCollectInfo(MDataBlock & data);
	void refreshOutCurveShape(int outPlugID,MDoubleArray knots,MDataBlock & data);
	

	int dirtyCount;
	bool computeFlag;
};

MStatus initializePlugin( MObject obj )

{ 
	MStatus   status;

	MFnPlugin implugin( obj, "", "2011", "Any");
	status = implugin.registerNode( "vixo_hairNurbsCurve", vixo_hairNurbsCurve::id, vixo_hairNurbsCurve::creator,
								  vixo_hairNurbsCurve::initialize );
	if (!status) {
		status.perror("registerNode");
		return status;
	}

	return status;
}

MStatus uninitializePlugin( MObject obj)

{
	MStatus   status;

	MFnPlugin implugin( obj );
	status = implugin.deregisterNode( vixo_hairNurbsCurve::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
	return status;
}

/*
createNode vixo_hairNurbsCurve;
*/