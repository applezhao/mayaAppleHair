#pragma once
#include "vixo_typeDef.h"
#include "vixo_clumpFreq.h"
#include <maya/MFnPlugin.h>
#include <maya/MPxSurfaceShape.h>
#include <maya/MPxSurfaceShapeUI.h>

#include <fstream>
#include <time.h>

class vixo_ctrlCurves : public MPxSurfaceShape
{
public:
	vixo_ctrlCurves(void);
	virtual ~vixo_ctrlCurves(void);

	virtual MStatus			compute( const MPlug&, MDataBlock& );

	virtual bool            isBounded() const;
	virtual MBoundingBox    boundingBox() const; 
	static  void *		creator();
	static  MStatus		initialize();

	virtual MStatus connectionMade(const MPlug& plug,const MPlug& otherPlug,bool asSrc);
	virtual MStatus connectionBroken(const MPlug& plug,const MPlug& otherPlug,bool asSrc);

	virtual MStatus setDependentsDirty(const MPlug&  plugBeingDirtied, MPlugArray&  affectedPlugs);

	virtual bool isRenderable();

	static MObject inCtrlCurveData;
	static MObject staticMesh;
	static MObject inMesh;
	static MObject showCtrl;
	static MObject densityMap;

	static MObject degree;

	static MObject hairSysMsg;

	static MObject hiddenOutput; 
	static MObject hiddenOutCurveNum;
	static MObject hiddenOutCurveDataBase;

	static MObject boundBoxExpand;
	static MObject miscBoundBoxExpend;

	static MObject clumpInfoFileName;
	static MObject parametersFileName;
	static MObject segment;

	//follicle file
	static MObject hairNum;
	static MObject hairRootFileName;
	static MObject hiddenOutputBasicInfo;

	//cache File
	static MObject hairCacheFileName;
	static MObject renderUseCache;

	//TEXTURE
	static MObject tipColor;
	static MObject tipColorNoiseAmp;
	static MObject tipColorNoiseFreq;
	static MObject baseColor;
	static MObject baseColorNoiseAmp;
	static MObject baseColorNoiseFreq;
	static MObject tipOpacity;
	static MObject tipOpacityNoiseAmp;
	static MObject tipOpacityNoiseFreq;
	static MObject baseOpacity;
	static MObject baseOpacityNoiseAmp;
	static MObject baseOpacityNoiseFreq;
	static MObject tipWidth;
	static MObject tipWidthNoiseAmp;
	static MObject tipWidthNoiseFreq;
	static MObject baseWidth;
	static MObject baseWidthNoiseAmp;
	static MObject baseWidthNoiseFreq;


	static MObject lengthNoiseAmp;
	static MObject lengthNoiseFreq;
	static MObject inclinationNoiseAmp;
	static MObject inclinationNoiseFreq;
	static MObject polarNoiseAmp;
	static MObject polarNoiseFreq;
	static MObject tipCurlNoiseAmp;
	static MObject tipCurlNoiseFreq;
	static MObject baseCurlNoiseAmp;
	static MObject baseCurlNoiseFreq;

	static MObject scraggleMap;
	static MObject scraggleNoiseAmp;
	static MObject scraggleNoiseFreq;

	static MObject scraggleFrequencyMap;
	static MObject scraggleFrequencyNoiseAmp;
	static MObject scraggleFrequencyNoiseFreq;

	static MObject clumpMap;
	static MObject clumpNoiseAmp;
	static MObject clumpNoiseFreq;

	static MObject clumpFrequencyMap;
	static MObject clumpFrequencyNoiseAmp;
	static MObject clumpFrequencyNoiseFreq;

	static MObject clumpShapeMap;
	static MObject clumpShapeNoiseAmp;
	static MObject clumpShapeNoiseFreq;

	//static MObject hiddenOutClumpCenter;

	
	static MTypeId		id;

	messageData msgData;


	//inCurve connection Made or Broken
	int maxSpan;

	set<int> currentInVid;
	map<int,int> inVIDMapPlug; //useless
	map<int,int> plugMapInVID;

	map<int,inVertexBasicInfo> inVIDMapInVertexDataBase;
	
	map<int,inVertexShapeInfo> inVIDMapTangents;
	void initBasicStepInfo(int vid,MDataBlock & datablock);
	//inCurve connection Made or Broken

	int getInCurveInfo(MDataBlock & datablock);

	//densityMap refresh or showCtrl
	set<int> curOutCurveSet;
	map<int,set<outInVertexRelationInfo>> outVertexDataBase;
	map<int,vector<outVIDCtrlInfo>> outVertexControlData;
	void initOutInVertexRelationInfoDataBase(int vid);
	void deleteInVertex(int inVID);
	void dealDensityMapShowCtrl(float showCtrlValue,MObject& fileNode,MDataBlock & datablock);
	void staticMeshInitOutCurveDatatBase(MDataBlock & datablock);
	//densityMap refresh or showCtrl

	void buileRelationBetweenInOut(int inVID);
	void sortControlOrder(int outVID);
	
	//compute
	map<int,MPointArray> outVIDMapCVS;
	vixo_ctrlCurvesData vixoDrawData;
	void buildKnots();

	MBoundingBox box;

	vixo_ctrlCurvesData* data();

	void exportFollicle(int hairNumValue,MString hairRootInfo,MDataBlock& data);

	void getClumpCenter(MDataBlock& data);
	void getTexAndValue(MString& texName,vec3& value);
};

class vixo_ctrlCurvesUI : public MPxSurfaceShapeUI
{
public:
	vixo_ctrlCurvesUI();
	virtual ~vixo_ctrlCurvesUI(); 

	virtual void	getDrawRequests( const MDrawInfo & info,
									 bool objectAndActiveOnly,
									 MDrawRequestQueue & requests );
	virtual void	draw( const MDrawRequest & request,
						  M3dView & view ) const;

	static  void *  creator();
};

MStatus initializePlugin( MObject obj )
{ 
	MFnPlugin plugin( obj, PLUGIN_COMPANY, "3.0", "Any");
	return plugin.registerShape( "vixo_ctrlCurves", vixo_ctrlCurves::id,
								   &vixo_ctrlCurves::creator,
								   &vixo_ctrlCurves::initialize,
								   &vixo_ctrlCurvesUI::creator  );
}

MStatus uninitializePlugin( MObject obj)
{
	MFnPlugin plugin( obj );
	return plugin.deregisterNode( vixo_ctrlCurves::id );
}

