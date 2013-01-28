#pragma once

#include <maya\MPxNode.h>
#include <maya\MFnPlugin.h>

#include <maya\MFnMesh.h>
#include <maya\MPlug.h>
#include <maya\MPlugArray.h>
#include <maya\MItMeshVertex.h>

#include <maya\MFnTypedAttribute.h>
#include <maya\MFnNumericAttribute.h>

#include <vector>

#include "noise.h"
/*
polySphere -r 1 -sx 20 -sy 20 -ax 0 1 0 -cuv 2 -ch 1;
createNode vixo_hairStyleMaya;
connectAttr -f pSphereShape1.outMesh vixo_hairStyleMaya1.inMesh;
setAttr -type "string" vixo_hairStyleMaya1.hairInfoDir "f:";
setAttr "vixo_hairStyleMaya1.hairNum" 500000;


createNode vixo_hairStyleMaya;
connectAttr -f |hairSystem1|hairSystemShape1.outputRenderHairs vixo_hairStyleMaya1.renderHairs;
*/

struct follicleColor
{
	float density;
	float tipOpa;
	float baseOpa;
	float tipColorR;
	float tipColorG;
	float tipColorB;
	float baseColorR;
	float baseColorG;
	float baseColorB;
};

struct follicleNoise
{
	float length;
	float inclination;
	float polar;
	float tipCurl;
	float baseCurl;
};

class vixo_hairStyleMaya : public MPxNode
{
public:
	vixo_hairStyleMaya(void);
	virtual~vixo_hairStyleMaya(void);

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );
	static  void*		creator();
	static  MStatus		initialize();

private:
	void exportFollicle(int hairNumValue,MString uvFileName,MString hairRootInfo,MDataBlock& data);
	void exportColor(MString uvInfo,MString colorInfo,MDataBlock& data);
	void exportNoise(MString uvInfo,MString noiseInfo,MDataBlock& data);

	void getTextureInfo(MString attrName,vector<float>& uvs,vector<follicleColor>& res,MDataBlock& data);


public:
	static MObject hairInfoColorDir;

	//follicle file
	static MObject inMesh;
	static MObject hairNum;
	static MObject refreshBasicInfo;
	static MObject printBasicInfo;

	//color info
	static MObject densityMap;
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
	static MObject refreshColorInfo;
	static MObject printColorInfo;

	static MObject hairInfoStyleDir;

	//style info
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

	static MObject refreshStyleInfo;
	static MObject printStyleInfo;

	static MTypeId id;
};

MStatus initializePlugin( MObject obj )

{ 
	MStatus   status;

	MFnPlugin implugin( obj, "", "2011", "Any");
	status = implugin.registerNode( "vixo_hairStyleMaya", vixo_hairStyleMaya::id, vixo_hairStyleMaya::creator,
								  vixo_hairStyleMaya::initialize );
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
	status = implugin.deregisterNode( vixo_hairStyleMaya::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}
	
	return status;
}


