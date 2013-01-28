#pragma once

#ifndef TYPE_DEF_ALL_APPLE
#define TYPE_DEF_ALL_APPLE

#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <time.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>

using namespace std;

struct vec3
{
	float x;
	float y;
	float z;

	vec3()
	{
		this->x=0.0f;
		this->y=0.0f;
		this->z=0.0f;
	}

	vec3(float x,float y,float z)
	{
		this->x=x;
		this->y=y;
		this->z=z;
	}
	vec3 operator+(const vec3& v1)
	{
		vec3 res;
		res.x=this->x+v1.x;
		res.y=this->y+v1.y;
		res.z=this->z+v1.z;
		return res;
	}
	vec3 operator-(const vec3& v1)
	{
		vec3 res;
		res.x=this->x-v1.x;
		res.y=this->y-v1.y;
		res.z=this->z-v1.z;
		return res;
	}

	vec3 operator^(const vec3& v1)
	{
		vec3 res;
		res.x=y*v1.z-z*v1.y;
		res.y=z*v1.x-x*v1.z;
		res.z=x*v1.y-y*v1.x;
		return res;
	}

	void operator=(const vec3& v1)
	{
		this->x=v1.x;
		this->y=v1.y;
		this->z=v1.z;
	}

	vec3 operator/(const float m)
	{
		vec3 res;
		res.x=this->x/m;
		res.y=this->y/m;
		res.z=this->z/m;
		return res;
	}
	vec3 operator*(const float m)
	{
		vec3 res;
		res.x=this->x*m;
		res.y=this->y*m;
		res.z=this->z*m;
		return res;
	}
	vec3 rotate(const vec3& pivot,float angle)
	{
		float mat00=cos(angle)+(1-cos(angle))*pivot.x*pivot.x;
		float mat01=(1-cos(angle))*pivot.x*pivot.y-sin(angle)*z;
		float mat02=(1-cos(angle))*pivot.x*pivot.z+sin(angle)*y;
		float mat10=(1-cos(angle))*pivot.x*pivot.y+sin(angle)*z;
		float mat11=cos(angle)+(1-cos(angle))*pivot.y*pivot.y;
		float mat12=(1-cos(angle))*pivot.y*pivot.z-sin(angle)*x;
		float mat20=(1-cos(angle))*pivot.x*pivot.z-sin(angle)*y;
		float mat21=(1-cos(angle))*pivot.y*pivot.z+sin(angle)*x;
		float mat22=cos(angle)+(1-cos(angle))*pivot.z*pivot.z;
		vec3 res;
		res.x=x*mat00+y*mat10+z*mat20;
		res.y=x*mat01+y*mat11+z*mat21;
		res.z=x*mat02+y*mat12+z*mat22;
		return res;
	}
	void normalize()
	{
		float length=sqrt(x*x+y*y+z*z);
		x=x/length;
		y=y/length;
		z=z/length;
	}

};

struct hairParametersInfo
{
	string hairRootFile;
	string hairCacheFile;

	float triBoxExpand;

	vec3 tipColorBaseValue;
	string tipColorTexture;
	float tipColorNoiseAmp;
	int tipColorNoiseFreq;

	vec3 baseColorBaseValue;
	string baseColorTexture;
	float baseColorNoiseAmp;
	int baseColorNoiseFreq;

	float tipOpaBaseValue;
	string tipOpaTexture;
	float tipOpaNoiseAmp;
	int tipOpaNoiseFreq;

	float baseOpaBaseValue;
	string baseOpaTexture;
	float baseOpaNoiseAmp;
	int baseOpaNoiseFreq;

	float tipWidthBaseValue;
	string tipWidthTexture;
	float tipWidthNoiseAmp;
	int tipWidthNoiseFreq;

	float baseWidthBaseValue;
	string baseWidthTexture;
	float baseWidthNoiseAmp;
	int baseWidthNoiseFreq;

	float scraggleValue;
	string scraggleTexture;
	float scraggleNoiseAmp;
	int scraggleNoiseFreq;

	float scraggleFreqValue;
	string scraggleFreqTexture;
	float scraggleFreqNoiseAmp;
	int scraggleFreqNoiseFreq;

	float clumpValue;
	string clumpTexture;
	float clumpNoiseAmp;
	int clumpNoiseFreq;

	float clumpFreqValue;
	string clumpFreqTexture;
	float clumpFreqNoiseAmp;
	int clumpFreqNoiseFreq;

	float clumpShapeValue;
	string clumpShapeTexture;
	float clumpShapeNoiseAmp;
	int clumpShapeNoiseFreq;

	float lengthNoiseAmp;
	int lengthNoiseFreq;

	float inclinationNoiseAmp;
	int inclinationNoiseFreq;

	float polarNoiseAmp;
	int polarNoiseFreq;

	float tipCurlNoiseAmp;
	int tipCurlNoiseFreq;

	float baseCurlNoiseAmp;
	int baseCurlNoiseFreq;

	int segment;

	void operator=(const hairParametersInfo& hairPara)
	{
		this->hairRootFile=hairPara.hairRootFile;
		this->hairCacheFile=hairPara.hairCacheFile;
		this->triBoxExpand=hairPara.triBoxExpand;
		this->tipColorBaseValue=hairPara.tipColorBaseValue;
		this->tipColorTexture=hairPara.tipColorTexture;
		this->tipColorNoiseAmp=hairPara.tipColorNoiseAmp;
		this->tipColorNoiseFreq=hairPara.tipColorNoiseFreq;
		this->baseColorBaseValue=hairPara.baseColorBaseValue;
		this->baseColorTexture=hairPara.baseColorTexture;
		this->baseColorNoiseAmp=hairPara.baseColorNoiseAmp;
		this->baseColorNoiseFreq=hairPara.baseColorNoiseFreq;
		this->tipOpaBaseValue=hairPara.tipOpaBaseValue;
		this->tipOpaTexture=hairPara.tipOpaTexture;
		this->tipOpaNoiseAmp=hairPara.tipOpaNoiseAmp;
		this->tipOpaNoiseFreq=hairPara.tipOpaNoiseFreq;
		this->baseOpaBaseValue=hairPara.baseOpaBaseValue;
		this->baseOpaTexture=hairPara.baseOpaTexture;
		this->baseOpaNoiseAmp=hairPara.baseOpaNoiseAmp;
		this->baseOpaNoiseFreq=hairPara.baseOpaNoiseFreq;
		this->tipWidthBaseValue=hairPara.tipWidthBaseValue;
		this->tipWidthTexture=hairPara.tipWidthTexture;
		this->tipWidthNoiseAmp=hairPara.tipWidthNoiseAmp;
		this->tipWidthNoiseFreq=hairPara.tipWidthNoiseFreq;
		this->baseWidthBaseValue=hairPara.baseWidthBaseValue;
		this->baseWidthTexture=hairPara.baseWidthTexture;
		this->baseWidthNoiseAmp=hairPara.baseWidthNoiseAmp;
		this->baseWidthNoiseFreq=hairPara.baseWidthNoiseFreq;
		this->scraggleValue=hairPara.scraggleValue;
		this->scraggleTexture=hairPara.scraggleTexture;
		this->scraggleNoiseAmp=hairPara.scraggleNoiseAmp;
		this->scraggleNoiseFreq=hairPara.scraggleNoiseFreq;
		this->scraggleFreqValue=hairPara.scraggleFreqValue;
		this->scraggleFreqTexture=hairPara.scraggleFreqTexture;
		this->scraggleFreqNoiseAmp=hairPara.scraggleFreqNoiseAmp;
		this->scraggleFreqNoiseFreq=hairPara.scraggleFreqNoiseFreq;
		this->clumpValue=hairPara.clumpValue;
		this->clumpTexture=hairPara.clumpTexture;
		this->clumpNoiseAmp=hairPara.clumpNoiseAmp;
		this->clumpNoiseFreq=hairPara.clumpNoiseFreq;
		this->clumpFreqValue=hairPara.clumpFreqValue;
		this->clumpFreqTexture=hairPara.clumpFreqTexture;
		this->clumpFreqNoiseAmp=hairPara.clumpFreqNoiseAmp;
		this->clumpFreqNoiseFreq=hairPara.clumpFreqNoiseFreq;
		this->clumpShapeValue=hairPara.clumpShapeValue;
		this->clumpShapeTexture=hairPara.clumpShapeTexture;
		this->clumpShapeNoiseAmp=hairPara.clumpShapeNoiseAmp;
		this->clumpShapeNoiseFreq=hairPara.clumpShapeNoiseFreq;
		this->lengthNoiseAmp=hairPara.lengthNoiseAmp;
		this->lengthNoiseFreq=hairPara.lengthNoiseFreq;
		this->inclinationNoiseAmp=hairPara.inclinationNoiseAmp;
		this->inclinationNoiseFreq=hairPara.inclinationNoiseFreq;
		this->polarNoiseAmp=hairPara.polarNoiseAmp;
		this->polarNoiseFreq=hairPara.polarNoiseFreq;
		this->tipCurlNoiseAmp=hairPara.tipCurlNoiseAmp;
		this->tipCurlNoiseFreq=hairPara.tipCurlNoiseFreq;
		this->baseCurlNoiseAmp=hairPara.baseCurlNoiseAmp;
		this->baseCurlNoiseFreq=hairPara.baseCurlNoiseFreq;
		this->segment=hairPara.segment;
	}
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

struct triangleBasicInfo
{
	int faceID;
	int triID;
	int v1ID;
	int v2ID;
	int v3ID;
	int hairNum;
	int hairBegin;
	streampos beginIdx;
};

struct hairDetail
{
	float u;
	float v;
	float w0;
	float w1;
	float w2;
	int triIdx;

	void operator=(hairDetail& a)
	{
		u=a.u;
		v=a.v;
		w0=a.w0;
		w1=a.w1;
		w2=a.w2;
		triIdx=a.triIdx;
	}
};

struct hairDetailExtend
{
	int closetClumpCenter;
	int triId;
	bool operator<(const hairDetailExtend& a)const
	{
		if(this->closetClumpCenter<a.closetClumpCenter)
			return true;
		return false;
	}
};

struct clumpCenterInfo
{
	int triid;
	int hairid;
	float u;
	float v;
	float minDist;
};

struct perTriHairInfo
{
	int hairNum; //clump=1
	int hairIdx; //for clump only
	triangleBasicInfo triBasicData;
	forExportHairCache triCacheData;
	vector<vec3> triVertexCacheData;
	vector<hairDetail> hairDetailData; //clump!=1

	vector<hairDetailExtend> hairClump;//not for clump
};

struct renderInfo
{
	vector<float> tipColor;
	vector<float> baseColor;
	vector<float> opacity;
	vector<float> width;
	vector<float> curl;
	vector<vec3> hairShapeSpan;
	vector<float> hairShapeSegment;
	//vector<float> hairShapeSegment;
};

#endif
