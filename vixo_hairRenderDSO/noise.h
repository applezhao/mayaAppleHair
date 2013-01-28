#pragma once
#ifndef NOISE_H
#define NOISE_H

#include <fstream>
#include <vector>
#include <tiffio.h>
#include <string>
#include<iostream>
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
	char hairRootFile[255];
	char hairCacheFile[255];
	char hairClumpFile[255];

	float triBoxExpand;

	vec3 tipColorBaseValue;
	char tipColorTexture[255];
	float tipColorNoiseAmp;
	int tipColorNoiseFreq;

	vec3 baseColorBaseValue;
	char baseColorTexture[255];
	float baseColorNoiseAmp;
	int baseColorNoiseFreq;

	float tipOpaBaseValue;
	char tipOpaTexture[255];
	float tipOpaNoiseAmp;
	int tipOpaNoiseFreq;

	float baseOpaBaseValue;
	char baseOpaTexture[255];
	float baseOpaNoiseAmp;
	int baseOpaNoiseFreq;

	float tipWidthBaseValue;
	char tipWidthTexture[255];
	float tipWidthNoiseAmp;
	int tipWidthNoiseFreq;

	float baseWidthBaseValue;
	char baseWidthTexture[255];
	float baseWidthNoiseAmp;
	int baseWidthNoiseFreq;

	float scraggleValue;
	char scraggleTexture[255];
	float scraggleNoiseAmp;
	int scraggleNoiseFreq;

	float scraggleFreqValue;
	char scraggleFreqTexture[255];
	float scraggleFreqNoiseAmp;
	int scraggleFreqNoiseFreq;

	float clumpValue;
	char clumpTexture[255];
	float clumpNoiseAmp;
	int clumpNoiseFreq;

	float clumpFreqValue;
	char clumpFreqTexture[255];
	float clumpFreqNoiseAmp;
	int clumpFreqNoiseFreq;

	float clumpShapeValue;
	char clumpShapeTexture[255];
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
		strcpy(this->hairRootFile,hairPara.hairRootFile);
		strcpy(this->hairCacheFile,hairPara.hairCacheFile);
		strcpy(this->hairClumpFile,hairPara.hairClumpFile);
		this->triBoxExpand=hairPara.triBoxExpand;
		this->tipColorBaseValue=hairPara.tipColorBaseValue;
		strcpy(this->tipColorTexture,hairPara.tipColorTexture);
		this->tipColorNoiseAmp=hairPara.tipColorNoiseAmp;
		this->tipColorNoiseFreq=hairPara.tipColorNoiseFreq;
		this->baseColorBaseValue=hairPara.baseColorBaseValue;
		strcpy(this->baseColorTexture,hairPara.baseColorTexture);
		this->baseColorNoiseAmp=hairPara.baseColorNoiseAmp;
		this->baseColorNoiseFreq=hairPara.baseColorNoiseFreq;
		this->tipOpaBaseValue=hairPara.tipOpaBaseValue;
		strcpy(this->tipOpaTexture,hairPara.tipOpaTexture);
		this->tipOpaNoiseAmp=hairPara.tipOpaNoiseAmp;
		this->tipOpaNoiseFreq=hairPara.tipOpaNoiseFreq;
		this->baseOpaBaseValue=hairPara.baseOpaBaseValue;
		strcpy(this->baseOpaTexture,hairPara.baseOpaTexture);
		this->baseOpaNoiseAmp=hairPara.baseOpaNoiseAmp;
		this->baseOpaNoiseFreq=hairPara.baseOpaNoiseFreq;
		this->tipWidthBaseValue=hairPara.tipWidthBaseValue;
		strcpy(this->tipWidthTexture,hairPara.tipWidthTexture);
		this->tipWidthNoiseAmp=hairPara.tipWidthNoiseAmp;
		this->tipWidthNoiseFreq=hairPara.tipWidthNoiseFreq;
		this->baseWidthBaseValue=hairPara.baseWidthBaseValue;
		strcpy(this->baseWidthTexture,hairPara.baseWidthTexture);
		this->baseWidthNoiseAmp=hairPara.baseWidthNoiseAmp;
		this->baseWidthNoiseFreq=hairPara.baseWidthNoiseFreq;
		this->scraggleValue=hairPara.scraggleValue;
		strcpy(this->scraggleTexture,hairPara.scraggleTexture);
		this->scraggleNoiseAmp=hairPara.scraggleNoiseAmp;
		this->scraggleNoiseFreq=hairPara.scraggleNoiseFreq;
		this->scraggleFreqValue=hairPara.scraggleFreqValue;
		strcpy(this->scraggleFreqTexture,hairPara.scraggleFreqTexture);
		this->scraggleFreqNoiseAmp=hairPara.scraggleFreqNoiseAmp;
		this->scraggleFreqNoiseFreq=hairPara.scraggleFreqNoiseFreq;
		this->clumpValue=hairPara.clumpValue;
		strcpy(this->clumpTexture,hairPara.clumpTexture);
		this->clumpNoiseAmp=hairPara.clumpNoiseAmp;
		this->clumpNoiseFreq=hairPara.clumpNoiseFreq;
		this->clumpFreqValue=hairPara.clumpFreqValue;
		strcpy(this->clumpFreqTexture,hairPara.clumpFreqTexture);
		this->clumpFreqNoiseAmp=hairPara.clumpFreqNoiseAmp;
		this->clumpFreqNoiseFreq=hairPara.clumpFreqNoiseFreq;
		this->clumpShapeValue=hairPara.clumpShapeValue;
		strcpy(this->clumpShapeTexture,hairPara.clumpShapeTexture);
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

#define TABLE_SIZE 256

extern const int kTableMask;
#define MODPERM(x) permtable[(x)&kTableMask]

class noise {
public:

	static float	atValue( float x );
	static float	atPointUV( float u, float v);
	static float	atPoint( float x, float y, float z ); 
	static vec3		atPointUVColor(float u, float v);

	static vec3    atPointAndTime( float x, float y, float z, float t );

	static void   initTable( long seed );

private:

	static int    permtable   [256];
	static float  valueTable1 [256];
	static float  valueTable2 [256];
	static float  valueTable3 [256];
	static int    isInitialized;
	
	static float  spline( float x, float knot0, float knot1, float knot2, 
						  float knot3 );

	static float  value( int x, int y, int z, float table[] = valueTable1 );
	static float  value( int x, int y, int z, int t, float table[] = valueTable1 );
	static float  value( int x, float table[] = valueTable1 );
	static float  value( int x, int y, float table[] = valueTable1 );


};
 

inline float noise::value( int x, int y, int z, int t, float table[] ) 
{
	return table[MODPERM( x + MODPERM( y + MODPERM( z + MODPERM( t ) ) ) )];
}

inline float noise::value( int x, int y, int z, float table[] ) 
{
	return table[MODPERM( x + MODPERM( y + MODPERM( z ) ) )];
}

inline float noise::value( int x, float table[] ) 
{
	return table[MODPERM( x )];
}
inline float noise::value( int x, int y, float table[] )
{
	return table[MODPERM( x + MODPERM( y ))];
}
#endif
