#pragma once

#include <math.h>


#include <sstream>
#include <iostream>
#include <cstdlib>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ri.h>
#include "vixo_floatPara.h"
#include "vixo_colorPara.h"
#include "vixo_clumpFreq.h"


#ifdef _WIN32
#define export __declspec(dllexport)
#define strtok_r(str,delim,saveptr) strtok((str),(delim))
#else
#define export
#endif

extern "C" {
	export RtPointer ConvertParameters(RtString paramstr);
	export RtVoid Subdivide(RtPointer data, RtFloat detail);
	export RtVoid Free(RtPointer data);
}

using namespace std;
class vixo_hairRender
{
public:
	hairParametersInfo hairParameters;
	int currentTriIdx;
	string clumpCenterInfoFileName;
	vec3 boxMin;
	vec3 boxMax;

	string clumpCenterTempFile;

	int span;

	perTriHairInfo triangleHairs;
	renderInfo renderData;

	vector<perTriHairInfo> perClumpData;
	map<int,int> hairIdMapLocalId;
	vector<hairDetail> forClumpUV;
	renderInfo clumpData;

	vixo_colorPara tipColor;
	vixo_colorPara baseColor;
	vixo_floatPara tipOpacity;
	vixo_floatPara baseOpacity;
	vixo_floatPara tipWidth;
	vixo_floatPara baseWidth;

	vixo_floatPara length;
	vixo_floatPara polar;
	vixo_floatPara inclination;
	vixo_floatPara tipCurl;
	vixo_floatPara baseCurl;

	vixo_floatPara scraggle;
	vixo_floatPara scraggleFreq;
	vixo_floatPara clump;
	vixo_floatPara clumpShape;

public:
	vixo_hairRender(void);
	~vixo_hairRender(void);
	vixo_hairRender(const char* initChars);
	vixo_hairRender(hairParametersInfo hairParameters,int triIdx,string clumpCenterFile,vec3 boxMin,vec3 boxMax);

	void doRender();
	void subdivide();
	void renderTriangle();

	void getBoundingBox(vector<vec3>& allPoints,vector<vec3>& box,int allPLen,float expand);

	void getClumpCenter();

	void getTriHairRootData(int triIdx,triangleBasicInfo& perTriBasicData,vector<hairDetail>& perHairDetailInfo);
	void getTriHairCacheData(int triIdx,forExportHairCache& perTriCacheData,vector<vec3>& perTriVertexCacheData);
	void getTriClumpCenterSet(triangleBasicInfo& triBasicData,set<hairDetailExtend>& clumpSet,vector<hairDetailExtend>& hairClump);

	void getColorOpaWidthInfo();
	void getLenPolarIncliCurlInfo(vector<float>& curlParaHair,vector<float>& curlParaClump,int clumpSetSize);
	void deformPolarIncliCurl(vector<float>& curlParaHair,vector<float>& curlParaClump,int clumpSetSize);
	void deformScraggle(int clumpSetSize);
	void doClump();
	void bound(RtBound bnd)
	{
		bnd[0]=boxMin.x;
		bnd[1]=boxMax.x;
		bnd[2]=boxMin.y;
		bnd[3]=boxMax.y;
		bnd[4]=boxMin.z;
		bnd[5]=boxMax.z;
	}
};

