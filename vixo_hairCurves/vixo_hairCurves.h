#pragma once

#include <maya\MPxNode.h>
#include <maya\MFnPlugin.h>

#include "vixo_typedef.h"



class vixo_hairCurves: public MPxNode
{

public:
	static MObject inMesh;
	static MObject inCurves;
		static MObject curveShape;
		static MObject vertexID;

	static MObject refresh;
	static MObject densityMap;
	static MObject showControl;

	static MObject outCurvesNum;
	static MObject outCurves;
		static MObject outCurveShape;
		static MObject outU;
		static MObject outV;
		static MObject show;

	static MTypeId id;

public:
	vixo_hairCurves(void);
	virtual~vixo_hairCurves(void);

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );
	static  void*		creator();
	static  MStatus		initialize();

private:
	vector<vertexInfo> allVertexBasicInfo;
public:
	virtual MStatus connectionMade(const MPlug& plug,const MPlug& otherPlug,bool asSrc);
	virtual MStatus connectionBroken(const MPlug& plug,const MPlug& otherPlug,bool asSrc);

private:

	bool inSet(int element,vector<int> setCheck);
	void diffOldNewCurveSet(vector<int> oldSet,vector<int> newSet,MIntArray &delCurves,MIntArray &addCurves);

	vector<int> oldOutCurves;
	vector<int> newOutCurves;
	map<int,outCurvesInfo> outCurvesData;
	void getDensityMapInfo(MDataBlock& data);
	// for new added outCurvedVertex, find their nearest curves
	void prepareForCurveCompute();

	vector<int> oldCurves;
	vector<int> newCurves;
	vector<inCurvesInfo> inputCurves;
	void getAllInputCurvesInfo(MDataBlock& data);
	

	
	void deletedCurve(MIntArray delCurves);
	void initAddedCurveInfo(MIntArray addCurves,MDataBlock& data);

	//bool firstTime;

	void buildRelationShipInOut(MIntArray delCurves);

};


MStatus initializePlugin( MObject obj )

{ 
	MStatus   status;

	MFnPlugin implugin( obj, "", "2011", "Any");
	status = implugin.registerNode( "vixo_hairControlCurves", vixo_hairCurves::id, vixo_hairCurves::creator,
								  vixo_hairCurves::initialize );
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
	status = implugin.deregisterNode( vixo_hairCurves::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
	return status;
}

