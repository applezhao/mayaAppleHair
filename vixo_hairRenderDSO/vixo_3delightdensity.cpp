#include <ri.h>

#include <sstream>
#include <iostream>
//#include <cstdlib>
#include <string>
#include <map>
#include <set>
#include <vector>
#include <time.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <fstream>
#include <tiffio.h>
using namespace std;
#define M_PI 3.14159265358979323846
#if defined(_WIN32)
#define AQSIS_EXPORT __declspec(dllexport)
#else
#define AQSIS_EXPORT
#endif

extern "C" AQSIS_EXPORT RtPointer ConvertParameters(char* initialdata);
extern "C" AQSIS_EXPORT void Subdivide(RtPointer blinddata, RtFloat detailsize);
extern "C" AQSIS_EXPORT void Free(RtPointer blinddata);

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
	char hairDensityFile[255];

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
	bool operator<(const hairDetailExtend& aa)const
	{
		if(this->closetClumpCenter<aa.closetClumpCenter)
			return true;
		return false;
	}
	void operator=(const hairDetailExtend& aa)
	{
		this->closetClumpCenter=aa.closetClumpCenter;
		this->triId=aa.triId;
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

//drand
#define m 0x100000000LL
#define c 0xB16
#define a 0x5DEECE66DLL

static unsigned long long seed =1;

double drand48(void)
{
	seed=(a*seed+c)&0xffffffffffffLL;
	unsigned int x=seed >>16;
	return ((double)x/(double)m);
}

void srand48(unsigned int i)
{
	seed=(((long long int) i) <<16)| rand();
}
//drand

//noise
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


const int kTableMask = TABLE_SIZE - 1;

float noise::atValue( float x )
{
	int ix; 
	float fx; 

	if ( !isInitialized ) {
		initTable( 23479015 );
		isInitialized = 1;
	}

	ix = (int)floorf( x );
	fx = x - (float)ix; 

	return spline( fx, value( ix - 1 ),
		value( ix ),
		value( ix + 1 ),
		value( ix + 2 ) );
}

float noise::atPointUV( float u, float v)
{
	int iu, iv;
	int j, k;
	float fu, fv;
	float uknots[4], vknots[4];

	if ( !isInitialized ) {
		initTable( 23479015 );
	}

	iu = (int)floorf( u );
	fu = u - (float)iu; 

	iv = (int)floorf( v );
	fv = v - (float)iv;

	for ( k = -1; k <= 2; k++ ) 
	{
		for ( j = -1; j <= 2; j++ ) 
		{
			uknots[j+1] = value( iu + j, iu + k);
		}
		vknots[k+1] = spline( fu, uknots[0], uknots[1], uknots[2], uknots[3] );
	}

	float val = spline( fv, vknots[0], vknots[1], vknots[2], vknots[3] ); 

	return val;
}

vec3 noise::atPointUVColor(float u, float v)
{
	vec3 ret;

	int iu, iv;
	int j, k;
	float fu, fv;
	float uknots[3][4], vknots[3][4];

	if ( !isInitialized ) {
		initTable( 23479015 );
	}

	iu = (int)floorf( u );
	fu = u - (float)iu; 

	iv = (int)floorf( v );
	fv = v - (float)iv;

	for ( k = -1; k <= 2; k++ ) 
	{
		for ( j = -1; j <= 2; j++ ) 
		{
			uknots[0][j+1] = value( iu + j, iu + k,valueTable1);
			uknots[1][j+1] = value( iu + j, iu + k,valueTable2);
			uknots[2][j+1] = value( iu + j, iu + k,valueTable3);
		}
		vknots[0][k+1] = spline( fu, uknots[0][0], uknots[0][1], uknots[0][2], uknots[0][3] );
		vknots[1][k+1] = spline( fu, uknots[1][0], uknots[1][1], uknots[1][2], uknots[1][3] );
		vknots[2][k+1] = spline( fu, uknots[2][0], uknots[2][1], uknots[2][2], uknots[2][3] );
	}


	ret.x = spline( fv, vknots[0][0], vknots[0][1], vknots[0][2], vknots[0][3] ); 
	ret.y = spline( fv, vknots[1][0], vknots[1][1], vknots[1][2], vknots[1][3] ); 
	ret.z = spline( fv, vknots[2][0], vknots[2][1], vknots[2][2], vknots[2][3] ); 

	return ret;
}

float noise::atPoint( float x, float y, float z )
{
	int ix, iy, iz;
	int i, j, k;
	float fx, fy, fz;
	float xknots[4], yknots[4], zknots[4];

	if ( !isInitialized ) {
		initTable( 23479015 );
	}

	ix = (int)floorf( x );
	fx = x - (float)ix; 

	iy = (int)floorf( y );
	fy = y - (float)iy;

	iz = (int)floorf( z );
	fz = z - (float)iz;

	for ( k = -1; k <= 2; k++ ) {
		for ( j = -1; j <= 2; j++ ) {
			for ( i = -1; i <= 2 ; i++ ) {
				xknots[i+1] = value( ix + i, iy + j, iz + k );
			}
			yknots[j+1] = spline( fx, xknots[0], xknots[1], xknots[2], xknots[3] );
		}
		zknots[k+1] = spline( fy, yknots[0], yknots[1], yknots[2], yknots[3] );
	}

	float val = spline( fz, zknots[0], zknots[1], zknots[2], zknots[3] ); 

	return val;
}

vec3 noise::atPointAndTime( float x, float y, float z, float t )
{
	vec3 ret;

	int ix, iy, iz, it;
	int i, j, k, l;
	float fx, fy, fz, ft;
	float xknots[3][4], yknots[3][4], zknots[3][4], tknots[3][4];

	if ( !isInitialized ) {
		initTable( 23479015 );
	}

	ix = (int)floorf( x );
	fx = x - (float)ix; 

	iy = (int)floorf( y );
	fy = y - (float)iy;

	iz = (int)floorf( z );
	fz = z - (float)iz;

	it = (int)floorf( t);
	ft = t - (float)it;

	for ( l = -1; l <= 2; l++ ) {
		for ( k = -1; k <= 2; k++ ) {
			for ( j = -1; j <= 2; j++ ) {
				for ( i = -1; i <= 2 ; i++ ) {
					xknots[0][i+1] = value( ix + i, iy + j, iz + k, it + l,
						valueTable1 ); 
					xknots[1][i+1] = value( ix + i, iy + j, iz + k, it + l,
						valueTable2 ); 
					xknots[2][i+1] = value( ix + i, iy + j, iz + k, it + l,
						valueTable3 ); 
				}
				yknots[0][j+1] = spline( fx, xknots[0][0], xknots[0][1], 
					xknots[0][2], xknots[0][3] );
				yknots[1][j+1] = spline( fx, xknots[1][0], xknots[1][1], 
					xknots[1][2], xknots[1][3] );
				yknots[2][j+1] = spline( fx, xknots[2][0], xknots[2][1], 
					xknots[2][2], xknots[2][3] );
			}
			zknots[0][k+1] = spline( fy, yknots[0][0], yknots[0][1], 
				yknots[0][2], yknots[0][3] );
			zknots[1][k+1] = spline( fy, yknots[1][0], yknots[1][1], 
				yknots[1][2], yknots[1][3] );
			zknots[2][k+1] = spline( fy, yknots[2][0], yknots[2][1], 
				yknots[2][2], yknots[2][3] );
		}
		tknots[0][l+1] = spline( fz, zknots[0][0], zknots[0][1], 
			zknots[0][2], zknots[0][3] );
		tknots[1][l+1] = spline( fz, zknots[1][0], zknots[1][1], 
			zknots[1][2], zknots[1][3] );
		tknots[2][l+1] = spline( fz, zknots[2][0], zknots[2][1], 
			zknots[2][2], zknots[2][3] );
	} 
	ret.x = spline( ft, tknots[0][0], tknots[0][1], tknots[0][2], tknots[0][3] ); 
	ret.y = spline( ft, tknots[1][0], tknots[1][1], tknots[1][2], tknots[1][3] ); 
	ret.z = spline( ft, tknots[2][0], tknots[2][1], tknots[2][2], tknots[2][3] ); 

	return ret;
}


void  noise::initTable( long seed )
{
	srand48( seed );

	for ( int i = 0; i < TABLE_SIZE; i++ ) {
		valueTable1[i] = (float)drand48();
		valueTable2[i] = (float)drand48();
		valueTable3[i] = (float)drand48();
	} 
	isInitialized = 1; 
}



float noise::spline( float x, float knot0, float knot1, float knot2, float knot3 )
{
	float c0, c1, c2, c3;

	c3 = (-0.5F * knot0 ) + ( 1.5F * knot1 ) + (-1.5F * knot2 ) + ( 0.5F * knot3 );
	c2 = ( 1.0F * knot0 ) + (-2.5F * knot1 ) + ( 2.0F * knot2 ) + (-0.5F * knot3 );
	c1 = (-0.5F * knot0 ) + ( 0.0F * knot1 ) + ( 0.5F * knot2 ) + ( 0.0F * knot3 );
	c0 = ( 0.0F * knot0 ) + ( 1.0F * knot1 ) + ( 0.0F * knot2 ) + ( 0.0F * knot3 );

	return ( ( c3 * x + c2 ) * x + c1 ) * x + c0;;
}


int noise::isInitialized = 0;

int noise::permtable[256] = {
	254,    91,     242,    186,    90,     204,    85,     133,    233,
	50,     187,    49,     182,    224,    144,    166,    7,      51,
	20,     179,    36,     203,    114,    156,    195,    40,     24,
	60,     162,    84,     126,    102,    63,     194,    220,    161,
	72,     94,     193,    229,    140,    57,     3,      189,    106,
	54,     164,    198,    199,    44,     245,    235,    100,    87,
	25,     41,     62,     111,    13,     70,     27,     82,     69,
	53,     66,     247,    124,    67,     163,    125,    155,    228,
	122,    19,     113,    143,    121,    9,      1,      241,    171,
	200,    83,     244,    185,    170,    141,    115,    190,    154,
	48,     32,     178,    127,    167,    56,     134,    15,     160,
	238,    64,     6,      11,     196,    232,    26,     89,     0,
	219,    112,    68,     30,     215,    227,    75,     132,    71,
	239,    251,    92,     14,     104,    231,    29,     180,    150,
	226,    191,    47,     73,     37,     183,    88,     105,    42,
	22,     2,      38,     5,      119,    74,     249,    184,    52,
	8,      55,     118,    255,    206,    173,    165,    78,     31,
	123,    98,     212,    80,     139,    61,     138,    77,     177,
	45,     137,    145,    28,     168,    128,    95,     223,    35,
	205,    76,     211,    175,    81,     33,     207,    21,     131,
	58,     152,    16,     240,    18,     96,     210,    109,    214,
	216,    202,    148,    34,     146,    117,    176,    93,     246,
	172,    97,     159,    197,    218,    65,     147,    253,    221,
	217,    79,     101,    142,    23,     149,    99,     39,     12,
	135,    110,    234,    108,    153,    129,    4,      169,    174,
	116,    243,    130,    107,    222,    10,     43,     188,    46,
	213,    252,    86,     157,    192,    236,    158,    120,    17,
	103,    248,    225,    230,    250,    208,    181,    151,    237,
	201,    59,     136,    209 
};

float noise::valueTable1[256];
float noise::valueTable2[256];
float noise::valueTable3[256];

//noise

//float Para
class vixo_floatPara
{
public:
	vixo_floatPara(void){}
	~vixo_floatPara(void){}
public:
	vector<float> hairValue;
	vector<float> clumpValue;
	bool hasEffect;
public:
	void init(string fileName,float baseValue,float noiseAmp,int noiseFreq,vector<hairDetail>& hairUVs,int length,vector<hairDetail>& clumpUVs,int clumpLength);
	float getValue(int i)
	{
		return hairValue[i];
	}
	float getClumpValue(int i)
	{
		return clumpValue[i];
	}
};

void vixo_floatPara::init(string fileName,float baseValue,float noiseAmp,int noiseFreq,vector<hairDetail>& hairUVs,int length,vector<hairDetail>& clumpUVs,int clumpLength)
{
	if(fileName!=""||baseValue!=0)
	{
		hasEffect=true;
	}
	else
		hasEffect=false;

	hairValue.resize(length);
	clumpValue.resize(clumpLength);

	if(fileName!="")
	{
		TIFF* tif = TIFFOpen(fileName.c_str(), "r");
		vector<uint32> pixels;
		uint32 width,height;
		if (tif) 
		{
			size_t npixels;
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
			npixels = width * height;
			pixels.resize(npixels);
			TIFFReadRGBAImage(tif, width, height, &pixels[0], 0); //abgr
			TIFFClose(tif); 
		}

		for(int i=0;i<length;i++)
		{
			float noiseValue=(noise::atPointUV(hairUVs[i].u*noiseFreq,hairUVs[i].v*noiseFreq)*2-1)*noiseAmp;
			int line=hairUVs[i].u*width;
			int col=hairUVs[i].v*height;
			float res=((pixels[col*width+line]<<8)>>24)/255.0;
			res=min(max(res+noiseValue,0.0f),1.0f);
			hairValue[i]=res;
		}

		for(int i=0;i<clumpLength;i++)
		{
			float noiseValue=(noise::atPointUV(clumpUVs[i].u*noiseFreq,clumpUVs[i].v*noiseFreq)*2-1)*noiseAmp;
			int line=clumpUVs[i].u*width;
			int col=clumpUVs[i].v*height;
			float res=((pixels[col*width+line]<<8)>>24)/255.0;
			res=min(max(res+noiseValue,0.0f),1.0f);
			clumpValue[i]=res;
		}
	}
	else
	{
		for(int i=0;i<length;i++)
		{
			float noiseValue=(noise::atPointUV(hairUVs[i].u*noiseFreq,hairUVs[i].v*noiseFreq)*2-1)*noiseAmp;
			float res=min(max(baseValue+noiseValue,0.0f),1.0f);
			hairValue[i]=res;
		}
		for(int i=0;i<clumpLength;i++)
		{
			float noiseValue=(noise::atPointUV(clumpUVs[i].u*noiseFreq,clumpUVs[i].v*noiseFreq)*2-1)*noiseAmp;
			float res=min(max(baseValue+noiseValue,0.0f),1.0f);
			clumpValue[i]=res;
		}
	}
}
//float Para

//color Para
class vixo_colorPara
{
public:
	vixo_colorPara(void){}
	~vixo_colorPara(void){}
public:
	vector<float> hairValue;
	bool hasEffect;
public:
	void init(string fileName,vec3 baseColor,float noiseAmp,int noiseFreq,vector<hairDetail>& hairUVs,int length);
};

void vixo_colorPara::init(string fileName,vec3 baseColor,float noiseAmp,int noiseFreq,vector<hairDetail>& hairUVs,int length)
{
	if(fileName!=""||(baseColor.x!=0&&baseColor.y!=0&&baseColor.z!=0))
	{
		hasEffect=true;
	}
	else
		hasEffect=false;

	hairValue.resize(length*3);

	if (fileName!="")
	{
		TIFF* tif = TIFFOpen(fileName.c_str(), "r");
		vector<uint32> pixels;
		uint32 width,height;
		if (tif) 
		{
			size_t npixels;
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
			npixels = width * height;
			pixels.resize(npixels);
			TIFFReadRGBAImage(tif, width, height, &pixels[0], 0); //abgr
			TIFFClose(tif); 
		}
		for(int i=0;i<length;i++)
		{
			vec3 noiseValue=noise::atPointUVColor(hairUVs[i].u*noiseFreq,hairUVs[i].v*noiseFreq);
			noiseValue.x=(noiseValue.x*2-1)*noiseAmp;
			noiseValue.y=(noiseValue.y*2-1)*noiseAmp;
			noiseValue.z=(noiseValue.z*2-1)*noiseAmp;
			int line=hairUVs[i].u*width;
			int col=hairUVs[i].v*height;
			vec3 res;
			res.x=(pixels[(col*width+line)]&0xff)/255.0;
			res.y=((pixels[(col*width+line)]&0xff00)>>8)/255.0;
			res.z=((pixels[(col*width+line)]&0xff0000)>>16)/255.0;

			res.x=min(max(res.x+noiseValue.x,0.0f),1.0f);
			res.y=min(max(res.y+noiseValue.y,0.0f),1.0f);
			res.z=min(max(res.z+noiseValue.z,0.0f),1.0f);
			hairValue[3*i]=res.x;
			hairValue[3*i+1]=res.y;
			hairValue[3*i+2]=res.z;
		}
	}
	else
	{
		for(int i=0;i<length;i++)
		{
			vec3 noiseValue=noise::atPointUVColor(hairUVs[i].u*noiseFreq,hairUVs[i].v*noiseFreq);
			noiseValue.x=(noiseValue.x*2-1)*noiseAmp;
			noiseValue.y=(noiseValue.y*2-1)*noiseAmp;
			noiseValue.z=(noiseValue.z*2-1)*noiseAmp;
			vec3 res;
			res.x=min(max(baseColor.x+noiseValue.x,0.0f),1.0f);
			res.y=min(max(baseColor.y+noiseValue.y,0.0f),1.0f);
			res.z=min(max(baseColor.z+noiseValue.z,0.0f),1.0f);
			hairValue[3*i]=res.x;
			hairValue[3*i+1]=res.y;
			hairValue[3*i+2]=res.z;
		}
	}
}
//color Para

/*//clump Freq
class vixo_clumpFreq
{
public:
	vixo_clumpFreq(void){}
	~vixo_clumpFreq(void){}
public:
	vector<uint32> pixels;
	uint32 width;
	uint32 height;
	float baseValue;
	float noiseAmp;
	float noiseFreq;
	string filename;
	bool hasEffect;
public:
	void init(string fileName,float baseValue,float noiseAmp,int noiseFreq);
	float getValue(float u,float v);
};

void vixo_clumpFreq::init(string fileName,float baseValue,float noiseAmp,int noiseFreq)
{
	this->baseValue=baseValue;
	this->noiseAmp=noiseAmp;
	this->noiseFreq=noiseFreq;
	this->filename=fileName;
	if(fileName!=""||this->baseValue!=0)
	{
		hasEffect=true;
	}
	else
		hasEffect=false;

	if(fileName!="")
	{
		TIFF* tif = TIFFOpen(fileName.c_str(), "r");
		if (tif) 
		{
			size_t npixels;
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
			npixels = width * height;
			pixels.resize(npixels);
			TIFFReadRGBAImage(tif, width, height, &pixels[0], 0); //abgr
			TIFFClose(tif); 
		}
	}
}


float vixo_clumpFreq::getValue(float u,float v)
{
	float noiseValue=(noise::atPointUV(u*noiseFreq,v*noiseFreq)*2-1)*noiseAmp;
	if(this->filename!="")
	{
		int line=u*width;
		int col=v*height;
		float res=((pixels[col*width+line]<<8)>>24)/255.0;
		res=min(max(res+noiseValue,0.0f),1.0f);
		return res;
	}
	else
	{
		float res=min(max(baseValue+noiseValue,0.0f),1.0f);
		return res;
	}

}
//clump Freq
*/
//hairRender
void randomAxis(vec3& axis)
{
	float randV=rand()/(RAND_MAX+1.0f);
	axis.z=2*randV-1;
	randV=rand()/(RAND_MAX+1.0f);
	float t=2*M_PI*randV;
	float w=sqrt(1-axis.z*axis.z);
	axis.x=w*cos(t);
	axis.y=w*sin(t);
}


class vixo_hairRender
{
public:
	hairParametersInfo hairParameters;
	int currentTriIdx;
	//string clumpCenterInfoFileName;
	//vec3 boxMin;
	//vec3 boxMax;

	//string clumpCenterTempFile;

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
	vixo_hairRender(void){}
	~vixo_hairRender(void){}
	vixo_hairRender(const char* initChars);
	//vixo_hairRender(hairParametersInfo hairParameters,int triIdx);

	//void doRender();
	//void subdivide();
	void renderTriangle();

	//void getBoundingBox(vector<vec3>& allPoints,vector<vec3>& box,int allPLen,float expand);

	//void getClumpCenter();

	void getTriHairRootData(int triIdx,triangleBasicInfo& perTriBasicData,vector<hairDetail>& perHairDetailInfo);
	void getTriHairCacheData(int triIdx,forExportHairCache& perTriCacheData,vector<vec3>& perTriVertexCacheData);
	void getTriClumpCenterSet(triangleBasicInfo& triBasicData,set<hairDetailExtend>& clumpSet,vector<hairDetailExtend>& hairClump);

	void getColorOpaWidthInfo();
	void getLenPolarIncliCurlInfo(vector<float>& curlParaHair,vector<float>& curlParaClump,int clumpSetSize);
	void deformPolarIncliCurl(vector<float>& curlParaHair,vector<float>& curlParaClump,int clumpSetSize);
	void deformScraggle(int clumpSetSize);
	void doClump();
};

vixo_hairRender::vixo_hairRender(const char* initChars)
{
	string paraFile;
	istringstream(initChars) >>paraFile
		>>this->currentTriIdx;
	fstream fin(paraFile.c_str(),ios_base::in|ios_base::binary);
	fin.read((char*)&hairParameters,sizeof(hairParametersInfo));
	fin.close();
	
	//srand(time(NULL));
	renderTriangle();
}

void vixo_hairRender::renderTriangle()
{
	fstream fin(hairParameters.hairRootFile,ios_base::in|ios_base::binary);
	fin.seekp(sizeof(int)*3+sizeof(triangleBasicInfo)*currentTriIdx);
	fin.read((char*)&triangleHairs.triBasicData,sizeof(triangleBasicInfo));
	fin.seekp(triangleHairs.triBasicData.beginIdx);
	vector<hairDetail> tempHairDetailInfo(triangleHairs.triBasicData.hairNum);
	fin.read((char*)&tempHairDetailInfo[0],sizeof(hairDetail)*triangleHairs.triBasicData.hairNum);
	fin.close();
	vixo_floatPara densityMap;
	densityMap.init(hairParameters.hairDensityFile,0.0f,0.0f,0,tempHairDetailInfo,triangleHairs.triBasicData.hairNum,forClumpUV,0);
	triangleHairs.hairDetailData.clear();
	triangleHairs.hairNum=0;
	map<int,int> densitiedMapAll;
	srand(currentTriIdx);
	for(int i=0;i<triangleHairs.triBasicData.hairNum;i++)
	{
		//cout<<densityMap.hairValue[i]<<endl;
		if(rand()/(RAND_MAX+1.0f)<densityMap.hairValue[i])
		{
			triangleHairs.hairDetailData.push_back(tempHairDetailInfo[i]);
			densitiedMapAll.insert(pair<int,int>(triangleHairs.hairNum,i));
			triangleHairs.hairNum++;
		}
	}
	//cout<<currentTriIdx<<" "<<triangleHairs.hairNum<<endl;
	if(triangleHairs.hairNum<=0)
	{
		//cout<<"ret"<<endl;
		return;
	}
	getTriHairCacheData(this->currentTriIdx,this->triangleHairs.triCacheData,this->triangleHairs.triVertexCacheData);
	//cout<<hairParameters.baseWidthBaseValue<<" "<<hairParameters.tipWidthBaseValue<<endl;
	//cout<<currentTriIdx<<endl;
	//cout<<triangleHairs.triCacheData.v0.vid<<" "<<triangleHairs.triCacheData.v1.vid<<" "<<triangleHairs.triCacheData.v2.vid<<endl;
	//for(int i=0;i<span;i++)
	//{
	//	cout<<triangleHairs.triVertexCacheData[3*i].x<<" "<<triangleHairs.triVertexCacheData[3*i].y<<" "<<triangleHairs.triVertexCacheData[3*i].z<<endl;
	//	cout<<triangleHairs.triVertexCacheData[3*i+1].x<<" "<<triangleHairs.triVertexCacheData[3*i+1].y<<" "<<triangleHairs.triVertexCacheData[3*i+1].z<<endl;
	//	cout<<triangleHairs.triVertexCacheData[3*i+2].x<<" "<<triangleHairs.triVertexCacheData[3*i+2].y<<" "<<triangleHairs.triVertexCacheData[3*i+2].z<<endl;
	//}


	set<hairDetailExtend> clumpSet;
	fin.open(hairParameters.hairClumpFile,ios_base::in|ios_base::binary);
	fin.seekp(sizeof(hairDetailExtend)*triangleHairs.triBasicData.hairBegin);
	vector<hairDetailExtend> tempHairClump(triangleHairs.triBasicData.hairNum);
	fin.read((char*)&tempHairClump[0],sizeof(hairDetailExtend)*triangleHairs.triBasicData.hairNum);
	fin.close();
	triangleHairs.hairClump.resize(triangleHairs.hairNum);
	clumpSet.clear();
	for(int i=0;i<triangleHairs.hairNum;i++)
	{
		triangleHairs.hairClump[i]=tempHairClump[densitiedMapAll.find(i)->second];
		clumpSet.insert(triangleHairs.hairClump[i]);
	}
	

	//getTriClumpCenterSet(this->triangleHairs.triBasicData,clumpSet,this->triangleHairs.hairClump);
	perClumpData.resize(clumpSet.size());
	forClumpUV.resize(clumpSet.size());
	set<hairDetailExtend>::iterator clumpSetIter;
	int i=0;
	for(clumpSetIter=clumpSet.begin();clumpSetIter!=clumpSet.end();clumpSetIter++,i++)
	{
		hairIdMapLocalId.insert(pair<int,int>(clumpSetIter->closetClumpCenter,i));
		perClumpData[i].hairNum=1;
		perClumpData[i].hairIdx=clumpSetIter->closetClumpCenter;
		getTriHairRootData(clumpSetIter->triId,perClumpData[i].triBasicData,perClumpData[i].hairDetailData);
		getTriHairCacheData(clumpSetIter->triId,perClumpData[i].triCacheData,perClumpData[i].triVertexCacheData);
		forClumpUV[i]=perClumpData[i].hairDetailData[perClumpData[i].hairIdx-perClumpData[i].triBasicData.hairBegin];
	}

	//get tip/base color/opacity/width uniform
	getColorOpaWidthInfo();

	//polar, inclination, length, curl
	vector<float> curlParaHair(span*triangleHairs.hairNum);
	vector<float> curlParaClump(span*clumpSet.size());
	getLenPolarIncliCurlInfo(curlParaHair,curlParaClump,clumpSet.size());
	deformPolarIncliCurl(curlParaHair,curlParaClump,clumpSet.size());

	//cout<<currentTriIdx<<endl;
	//for(int i=0;i<span;i++)
	//{
	//	cout<<renderData.hairShapeSegment[3*i]<<" "<<renderData.hairShapeSegment[3*i+1]<<" "<<renderData.hairShapeSegment[3*i+2]<<endl;
	//}

	//scraggle
	deformScraggle(clumpSet.size());

	//clump
	clump.init(hairParameters.clumpTexture,hairParameters.clumpValue,hairParameters.clumpNoiseAmp,hairParameters.clumpNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,0);
	clumpShape.init(hairParameters.clumpShapeTexture,hairParameters.clumpShapeValue,hairParameters.clumpShapeNoiseAmp,hairParameters.clumpShapeNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,0);

	if(clump.hasEffect==true)
	{
		//cout<<"clump"<<endl;
		doClump();
	}

	//array of pointsNo each curve
	vector<int> pointNum(triangleHairs.hairNum,hairParameters.segment+3);
	//array of index
	vector<float> index(triangleHairs.hairNum);
	for(int i=0;i<triangleHairs.hairNum;i++)
		index[i]=i;

	RiBasis(RiCatmullRomBasis,1,RiCatmullRomBasis,1);
	RtToken tokens[6];
	RtPointer values[6];
	tokens[0]="vertex point P";
	values[0]=(RtPointer)&renderData.hairShapeSegment[0];
	tokens[1]="varying float width";
	values[1]=(RtPointer)&renderData.width[0];
	tokens[2]="varying color Os";
	values[2]=(RtPointer)&renderData.opacity[0];
	tokens[3]="varying color Cs";
	values[3]=(RtPointer)&renderData.tipColor[0];
	//tokens[4]="uniform color tipcolor";
	//values[4]=(RtPointer)&renderData.tipColor[0];
	tokens[4]="uniform float index";
	values[4]=(RtPointer)&index[0];
	//cout<<renderData.width[0]<<endl;
	//cout<<currentTriIdx<<endl;
	//for(int i=0;i<span;i++)
	///{
	//	cout<<renderData.hairShapeSpan[i].x<<" "<<renderData.hairShapeSpan[i].y<<" "<<renderData.hairShapeSpan[i].z<<endl;
	//}
	//cout<<currentTriIdx<<endl;
	//for(int i=0;i<=hairParameters.segment;i++)
	//{
	//	cout<<renderData.hairShapeSegment[3*(i+1)]<<" "<<renderData.hairShapeSegment[3*(i+1)+1]<<" "<<renderData.hairShapeSegment[3*(i+1)+2]<<endl;
	//}

	RiCurves((RtToken)"cubic",triangleHairs.hairNum,(RtInt*)&pointNum[0],(RtToken)"nonperiodic",
		tokens[0],values[0],
		tokens[1],values[1],
		tokens[2],values[2],
		tokens[3],values[3],
		tokens[4],values[4],RI_NULL
		);

	/*getTriHairRootData(this->currentTriIdx,this->triangleHairs.triBasicData,this->triangleHairs.hairDetailData);
	getTriHairCacheData(this->currentTriIdx,this->triangleHairs.triCacheData,this->triangleHairs.triVertexCacheData);
	triangleHairs.hairNum=triangleHairs.triBasicData.hairNum;


	if(triangleHairs.hairNum<=0)
		return;

	set<hairDetailExtend> clumpSet;
	getTriClumpCenterSet(this->triangleHairs.triBasicData,clumpSet,this->triangleHairs.hairClump);
	perClumpData.resize(clumpSet.size());
	forClumpUV.resize(clumpSet.size());
	set<hairDetailExtend>::iterator clumpSetIter;
	int i=0;
	for(clumpSetIter=clumpSet.begin();clumpSetIter!=clumpSet.end();clumpSetIter++,i++)
	{
		hairIdMapLocalId.insert(pair<int,int>(clumpSetIter->closetClumpCenter,i));
		perClumpData[i].hairNum=1;
		perClumpData[i].hairIdx=clumpSetIter->closetClumpCenter;
		getTriHairRootData(clumpSetIter->triId,perClumpData[i].triBasicData,perClumpData[i].hairDetailData);
		getTriHairCacheData(clumpSetIter->triId,perClumpData[i].triCacheData,perClumpData[i].triVertexCacheData);
		forClumpUV[i]=perClumpData[i].hairDetailData[perClumpData[i].hairIdx-perClumpData[i].triBasicData.hairBegin];
	}

	//get tip/base color/opacity/width uniform
	getColorOpaWidthInfo();

	//polar, inclination, length, curl
	vector<float> curlParaHair(span*triangleHairs.hairNum);
	vector<float> curlParaClump(span*clumpSet.size());
	getLenPolarIncliCurlInfo(curlParaHair,curlParaClump,clumpSet.size());
	deformPolarIncliCurl(curlParaHair,curlParaClump,clumpSet.size());

	//scraggle
	deformScraggle(clumpSet.size());

	//clump
	clump.init(hairParameters.clumpTexture,hairParameters.clumpValue,hairParameters.clumpNoiseAmp,hairParameters.clumpNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,0);
	clumpShape.init(hairParameters.clumpShapeTexture,hairParameters.clumpShapeValue,hairParameters.clumpShapeNoiseAmp,hairParameters.clumpShapeNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,0);

	if(clump.hasEffect==true)
	{
		//cout<<"clump"<<endl;
		doClump();
	}

	//array of pointsNo each curve
	vector<int> pointNum(triangleHairs.hairNum,hairParameters.segment+3);
	//array of index
	vector<float> index(triangleHairs.hairNum);
	for(int i=0;i<triangleHairs.hairNum;i++)
		index[i]=i;

	RiBasis(RiCatmullRomBasis,1,RiCatmullRomBasis,1);
	RtToken tokens[6];
	RtPointer values[6];
	tokens[0]="vertex point P";
	values[0]=(RtPointer)&renderData.hairShapeSegment[0];
	tokens[1]="varying float width";
	values[1]=(RtPointer)&renderData.width[0];
	tokens[2]="varying color Os";
	values[2]=(RtPointer)&renderData.opacity[0];
	tokens[3]="varying color Cs";
	values[3]=(RtPointer)&renderData.tipColor[0];
	//tokens[4]="uniform color tipcolor";
	//values[4]=(RtPointer)&renderData.tipColor[0];
	tokens[4]="uniform float index";
	values[4]=(RtPointer)&index[0];

	//for(int i=0;i<hairParameters.segment+3;i++)
	//{
	//	cout<<renderData.hairShapeSegment[i*3]<<" "
	//		<<renderData.hairShapeSegment[i*3+1]<<" "
	//		<<renderData.hairShapeSegment[i*3+2]<<endl;
	//}

	RiCurves((RtToken)"cubic",triangleHairs.hairNum,(RtInt*)&pointNum[0],(RtToken)"nonperiodic",
		tokens[0],values[0],
		tokens[1],values[1],
		tokens[2],values[2],
		tokens[3],values[3],
		tokens[4],values[4],RI_NULL
	);
	*/
}

void vixo_hairRender::getTriHairRootData(int triIdx,triangleBasicInfo& perTriBasicData,vector<hairDetail>& perHairDetailInfo)
{
	fstream fin(hairParameters.hairRootFile,ios_base::in|ios_base::binary);
	fin.seekp(sizeof(int)*3+sizeof(triangleBasicInfo)*triIdx);
	fin.read((char*)&perTriBasicData,sizeof(triangleBasicInfo));
	fin.seekp(perTriBasicData.beginIdx);
	perHairDetailInfo.resize(perTriBasicData.hairNum);
	fin.read((char*)&perHairDetailInfo[0],sizeof(hairDetail)*perTriBasicData.hairNum);
	fin.close();
}

void vixo_hairRender::getTriHairCacheData(int triIdx,forExportHairCache& perTriCacheData,vector<vec3>& perTriVertexCacheData)
{
	fstream fin(hairParameters.hairCacheFile,ios_base::in|ios_base::binary);
	int triNumvertexNumSpan[3];
	fin.read((char*)triNumvertexNumSpan,sizeof(int)*3);
	span=triNumvertexNumSpan[2];
	perTriVertexCacheData.resize(3*span);
	fin.seekp(sizeof(int)*3+sizeof(forExportHairCache)*triIdx);
	fin.read((char*)&perTriCacheData,sizeof(forExportHairCache));

	fin.seekp(sizeof(int)*3+sizeof(forExportHairCache)*triNumvertexNumSpan[0]+sizeof(vec3)*span*perTriCacheData.v0.vid);
	fin.read((char*)&perTriVertexCacheData[0],sizeof(vec3)*span);
	fin.seekp(sizeof(int)*3+sizeof(forExportHairCache)*triNumvertexNumSpan[0]+sizeof(vec3)*span*perTriCacheData.v1.vid);
	fin.read((char*)&perTriVertexCacheData[span],sizeof(vec3)*span);
	fin.seekp(sizeof(int)*3+sizeof(forExportHairCache)*triNumvertexNumSpan[0]+sizeof(vec3)*span*perTriCacheData.v2.vid);
	fin.read((char*)&perTriVertexCacheData[span*2],sizeof(vec3)*span);
	fin.close();
}

void vixo_hairRender::getTriClumpCenterSet(triangleBasicInfo& triBasicData,set<hairDetailExtend>& clumpSet,vector<hairDetailExtend>& hairClump)
{
	fstream fin(hairParameters.hairClumpFile,ios_base::in|ios_base::binary);
	fin.seekp(sizeof(hairDetailExtend)*triBasicData.hairBegin);
	hairClump.resize(triBasicData.hairNum);
	fin.read((char*)&hairClump[0],sizeof(hairDetailExtend)*triBasicData.hairNum);
	fin.close();
	clumpSet.clear();
	for(int i=0;i<triBasicData.hairNum;i++)
	{
		clumpSet.insert(hairClump[i]);
	}
}

void vixo_hairRender::getColorOpaWidthInfo()
{
	tipColor.init(hairParameters.tipColorTexture,hairParameters.tipColorBaseValue,hairParameters.tipColorNoiseAmp,hairParameters.tipColorNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum);
	//renderData.tipColor=tipColor.hairValue;
	renderData.tipColor.resize(triangleHairs.hairNum*3*(hairParameters.segment+1));
	baseColor.init(hairParameters.baseColorTexture,hairParameters.baseColorBaseValue,hairParameters.baseColorNoiseAmp,hairParameters.baseColorNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum);
	//renderData.baseColor=baseColor.hairValue;
	//renderData.baseColor.resize(triangleHairs.hairNum*3*hairParameters.segment);
	for(int i=0;i<triangleHairs.hairNum;i++)
	{
		for(int j=0;j<hairParameters.segment+1;j++)
		{
			renderData.tipColor[3*(i*(hairParameters.segment+1)+j)]=baseColor.hairValue[i*3]+j*(tipColor.hairValue[i*3]-baseColor.hairValue[i*3])/hairParameters.segment;
			renderData.tipColor[3*(i*(hairParameters.segment+1)+j)+1]=baseColor.hairValue[i*3+1]+j*(tipColor.hairValue[i*3+1]-baseColor.hairValue[i*3+1])/hairParameters.segment;
			renderData.tipColor[3*(i*(hairParameters.segment+1)+j)+2]=baseColor.hairValue[i*3+2]+j*(tipColor.hairValue[i*3+2]-baseColor.hairValue[i*3+2])/hairParameters.segment;
		}
		/*renderData.tipColor[i*3]=tipColor.hairValue[i*3];
		renderData.tipColor[i*3+1]=tipColor.hairValue[i*3+1];
		renderData.tipColor[i*3+2]=tipColor.hairValue[i*3+2];
		renderData.baseColor[i*3]=baseColor.hairValue[i*3];
		renderData.baseColor[i*3+1]=baseColor.hairValue[i*3+1];
		renderData.baseColor[i*3+2]=baseColor.hairValue[i*3+2];
		*/
	}
	//renderData.tipColor=tipColor.hairValue;
	//baseColor.init(hairParameters.baseColorTexture,hairParameters.baseColorBaseValue,hairParameters.baseColorNoiseAmp,hairParameters.baseColorNoiseFreq,
	//	triangleHairs.hairDetailData,triangleHairs.hairNum);
	//renderData.baseColor=baseColor.hairValue;
	//renderData.baseColor.resize(triangleHairs.hairNum);
	//renderData.baseColor=baseColor.hairValue;
	tipOpacity.init(hairParameters.tipOpaTexture,hairParameters.tipOpaBaseValue,hairParameters.tipOpaNoiseAmp,hairParameters.tipOpaNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,0);
	baseOpacity.init(hairParameters.baseOpaTexture,hairParameters.baseOpaBaseValue,hairParameters.baseOpaNoiseAmp,hairParameters.baseOpaNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,0);
	tipWidth.init(hairParameters.tipWidthTexture,hairParameters.tipWidthBaseValue,hairParameters.tipWidthNoiseAmp,hairParameters.tipWidthNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,0);
	baseWidth.init(hairParameters.baseWidthTexture,hairParameters.baseWidthBaseValue,hairParameters.baseWidthNoiseAmp,hairParameters.baseWidthNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,0);

	renderData.opacity.resize(3*(hairParameters.segment+1)*triangleHairs.hairNum);
	renderData.width.resize((hairParameters.segment+1)*triangleHairs.hairNum);
	for(int i=0;i<triangleHairs.hairNum;i++)
	{
		for(int j=0;j<hairParameters.segment+1;j++)
		{
			float o=baseOpacity.hairValue[i]+j*(tipOpacity.hairValue[i]-baseOpacity.hairValue[i])/hairParameters.segment;
			renderData.opacity[3*(i*(hairParameters.segment+1)+j)]=renderData.opacity[3*(i*(hairParameters.segment+1)+j)+1]=renderData.opacity[3*(i*(hairParameters.segment+1)+j)+2]=o;
			float w=baseWidth.hairValue[i]+j*(tipWidth.hairValue[i]-baseWidth.hairValue[i])/hairParameters.segment;
			renderData.width[i*(hairParameters.segment+1)+j]=w;
		}
	}
}

void vixo_hairRender::getLenPolarIncliCurlInfo(vector<float>& curlParaHair,vector<float>& curlParaClump,int clumpSetSize)
{
	length.init("",0,hairParameters.lengthNoiseAmp,hairParameters.lengthNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,clumpSetSize);
	polar.init("",0,hairParameters.polarNoiseAmp,hairParameters.polarNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,clumpSetSize);
	inclination.init("",0,hairParameters.inclinationNoiseAmp,hairParameters.inclinationNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,clumpSetSize);
	tipCurl.init("",0,hairParameters.tipCurlNoiseAmp,hairParameters.tipCurlNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,clumpSetSize);
	baseCurl.init("",0,hairParameters.baseCurlNoiseAmp,hairParameters.baseCurlNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,clumpSetSize);
	for(int i=0;i<triangleHairs.hairNum;i++)
	{
		for(int j=0;j<span;j++)
		{
			float cc=baseCurl.hairValue[i]+j*(tipCurl.hairValue[i]-baseCurl.hairValue[i])/(span-1);
			curlParaHair[i*span+j]=cc;
		}
	}
	for(int i=0;i<clumpSetSize;i++)
	{
		for(int j=0;j<span;j++)
		{
			float cc=baseCurl.clumpValue[i]+j*(tipCurl.clumpValue[i]-baseCurl.clumpValue[i])/(span-1);
			curlParaClump[i*span+j]=cc;
		}
	}
}

void vixo_hairRender::deformPolarIncliCurl(vector<float>& curlParaHair,vector<float>& curlParaClump,int clumpSetSize)
{
	renderData.hairShapeSpan.resize(triangleHairs.hairNum*span);
	for(int i=0;i<triangleHairs.hairNum;i++)
	{
		vec3 normal=triangleHairs.triCacheData.v0.normal;
			//*triangleHairs.hairDetailData[i].w0+
			//triangleHairs.triCacheData.v1.normal*triangleHairs.hairDetailData[i].w1+
			//triangleHairs.triCacheData.v2.normal*triangleHairs.hairDetailData[i].w2;
		vec3 tangent=triangleHairs.triCacheData.v0.tangent;
			//*triangleHairs.hairDetailData[i].w0+
			//triangleHairs.triCacheData.v1.tangent*triangleHairs.hairDetailData[i].w1+
			//triangleHairs.triCacheData.v2.tangent*triangleHairs.hairDetailData[i].w2;
		vec3 binormal=normal^tangent;
		normal.normalize();
		binormal.normalize();

		for(int j=0;j<span;j++)
		{
			renderData.hairShapeSpan[i*span+j]=triangleHairs.triVertexCacheData[j]*triangleHairs.hairDetailData[i].w0
				+triangleHairs.triVertexCacheData[span+j]*triangleHairs.hairDetailData[i].w1
				+triangleHairs.triVertexCacheData[2*span+j]*triangleHairs.hairDetailData[i].w2;
			renderData.hairShapeSpan[i*span+j]=renderData.hairShapeSpan[i*span+j].rotate(normal,polar.hairValue[i]*M_PI*2);
			renderData.hairShapeSpan[i*span+j]=renderData.hairShapeSpan[i*span+j].rotate(binormal,inclination.hairValue[i]*M_PI/2);
			renderData.hairShapeSpan[i*span+j]=renderData.hairShapeSpan[i*span+j].rotate(binormal,curlParaHair[i*span+j]*M_PI/2);
			renderData.hairShapeSpan[i*span+j]=renderData.hairShapeSpan[i*span+j]*(length.hairValue[i]+1);
		}
	}

	clumpData.hairShapeSpan.resize(clumpSetSize*span);
	for(int i=0;i<clumpSetSize;i++)
	{
		vec3 normal=perClumpData[i].triCacheData.v0.normal;
			//*forClumpUV[i].w0+
			//perClumpData[i].triCacheData.v1.normal*forClumpUV[i].w1+
			//perClumpData[i].triCacheData.v2.normal*forClumpUV[i].w2;
		vec3 tangent=perClumpData[i].triCacheData.v0.tangent;
			//*forClumpUV[i].w0+
			//perClumpData[i].triCacheData.v1.tangent*forClumpUV[i].w1+
			//perClumpData[i].triCacheData.v2.tangent*forClumpUV[i].w2;
		vec3 binormal=normal^tangent;
		normal.normalize();
		binormal.normalize();

		for(int j=0;j<span;j++)
		{
			clumpData.hairShapeSpan[i*span+j]=perClumpData[i].triVertexCacheData[j]*forClumpUV[i].w0+
				perClumpData[i].triVertexCacheData[span+j]*forClumpUV[i].w1+
				perClumpData[i].triVertexCacheData[2*span+j]*forClumpUV[i].w2;
			clumpData.hairShapeSpan[i*span+j]=clumpData.hairShapeSpan[i*span+j].rotate(normal,polar.clumpValue[i]*M_PI*2);
			clumpData.hairShapeSpan[i*span+j]=clumpData.hairShapeSpan[i*span+j].rotate(binormal,inclination.clumpValue[i]*M_PI/2);
			clumpData.hairShapeSpan[i*span+j]=clumpData.hairShapeSpan[i*span+j].rotate(binormal,curlParaClump[i*span+j]*M_PI/2);
			clumpData.hairShapeSpan[i*span+j]=clumpData.hairShapeSpan[i*span+j]*(length.clumpValue[i]+1);
		}
	}
}

void vixo_hairRender::deformScraggle(int clumpSetSize)
{
	scraggle.init(hairParameters.scraggleTexture,hairParameters.scraggleValue,hairParameters.scraggleNoiseAmp,hairParameters.scraggleNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,clumpSetSize);
	scraggleFreq.init(hairParameters.scraggleFreqTexture,hairParameters.scraggleFreqValue,hairParameters.scraggleFreqNoiseAmp,hairParameters.scraggleFreqNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,clumpSetSize);

	renderData.hairShapeSegment.resize(triangleHairs.hairNum*(hairParameters.segment+3)*3);
	for(int i=0;i<triangleHairs.hairNum;i++)
	{
		vec3 position=triangleHairs.triCacheData.v0.position*triangleHairs.hairDetailData[i].w0+
			triangleHairs.triCacheData.v1.position*triangleHairs.hairDetailData[i].w1+
			triangleHairs.triCacheData.v2.position*triangleHairs.hairDetailData[i].w2;
		renderData.hairShapeSegment[(i*(hairParameters.segment+3))*3]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3]=0;
		renderData.hairShapeSegment[(i*(hairParameters.segment+3))*3+1]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3+1]=0;
		renderData.hairShapeSegment[(i*(hairParameters.segment+3))*3+2]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3+2]=0;

		for(int j=0;j<hairParameters.segment;j++)
		{
			float start=(j+0.0f)*span/hairParameters.segment;
			float end=(j+1.0f)*span/hairParameters.segment;
			vec3 temp;
			int floorEnd=floor(end);
			int ceilStart=ceil(start);
			if(ceilStart==0)
				ceilStart=1;
			if(floorEnd==ceilStart)
				temp=renderData.hairShapeSpan[i*span+ceilStart-1]*(ceilStart-start)+renderData.hairShapeSpan[i*span+ceilStart]*(end-floorEnd);
			else
				temp=renderData.hairShapeSpan[i*span+ceilStart-1]*(end-start);				
			if(rand()/(RAND_MAX+1.0f)<scraggleFreq.hairValue[i])
			{
				vec3 axis;
				randomAxis(axis);
				float randV=rand()/(RAND_MAX+1.0f)*2.0f-1.0f;
				temp=temp.rotate(axis,scraggle.hairValue[i]*randV*M_PI);
			}
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]=temp.x+renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3];
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]=temp.y+renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+1];
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]=temp.z+renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+2];
		}
		renderData.hairShapeSegment[(i*(hairParameters.segment+3))*3]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3]=position.x;
		renderData.hairShapeSegment[(i*(hairParameters.segment+3))*3+1]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3+1]=position.y;
		renderData.hairShapeSegment[(i*(hairParameters.segment+3))*3+2]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3+2]=position.z;
		for(int j=0;j<hairParameters.segment;j++)
		{
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]+=position.x;
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]+=position.y;
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]+=position.z;
		}
		renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3];
		renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3+1]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3+1];
		renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3+2]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3+2];
	}
	clumpData.hairShapeSegment.resize(clumpSetSize*(hairParameters.segment+3)*3);
	for(int i=0;i<clumpSetSize;i++)
	{
		vec3 position=perClumpData[i].triCacheData.v0.position*forClumpUV[i].w0+
			perClumpData[i].triCacheData.v1.position*forClumpUV[i].w1+
			perClumpData[i].triCacheData.v2.position*forClumpUV[i].w2;
		clumpData.hairShapeSegment[(i*(hairParameters.segment+3))*3]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3]=0;
		clumpData.hairShapeSegment[(i*(hairParameters.segment+3))*3+1]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3+1]=0;
		clumpData.hairShapeSegment[(i*(hairParameters.segment+3))*3+2]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3+2]=0;
		for(int j=0;j<hairParameters.segment;j++)
		{
			float start=(j+0.0f)*span/hairParameters.segment;
			float end=(j+1.0f)*span/hairParameters.segment;
			vec3 temp;
			int floorEnd=floor(end);
			int ceilStart=ceil(start);
			if(ceilStart==0)
				ceilStart=1;
			if(floorEnd==ceilStart) //not in one span
				temp=clumpData.hairShapeSpan[i*span+ceilStart-1]*(ceilStart-start)+clumpData.hairShapeSpan[i*span+ceilStart]*(end-floorEnd);
			else
				temp=clumpData.hairShapeSpan[i*span+ceilStart-1]*(end-start);
			if(rand()/(RAND_MAX+1.0f)<scraggleFreq.clumpValue[i])
			{
				vec3 axis;
				randomAxis(axis);
				float randV=rand()/(RAND_MAX+1.0f)*2.0f-1.0f;
				temp=temp.rotate(axis,scraggle.clumpValue[i]*randV*M_PI);
			}
			clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]=temp.x+clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3];
			clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]=temp.y+clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+1];
			clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]=temp.z+clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+2];
		}

		clumpData.hairShapeSegment[(i*(hairParameters.segment+3))*3]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3]=position.x;
		clumpData.hairShapeSegment[(i*(hairParameters.segment+3))*3+1]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3+1]=position.y;
		clumpData.hairShapeSegment[(i*(hairParameters.segment+3))*3+2]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3+2]=position.z;

		for(int j=0;j<hairParameters.segment;j++)
		{
			clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]+=position.x;
			clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]+=position.y;
			clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]+=position.z;
		}

		clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3];
		clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3+1]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3+1];
		clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3+2]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3+2];
	}
}

void vixo_hairRender::doClump()
{
	for(int i=0;i<triangleHairs.hairNum;i++)
	{
		int centerIdTot=triangleHairs.hairClump[i].closetClumpCenter;
		if(centerIdTot==triangleHairs.triBasicData.hairBegin+i)
			continue;
		int centerId=hairIdMapLocalId.find(centerIdTot)->second;
		float clumpPercent=pow(clump.hairValue[i],0.1f);
		//float clumpPercent=clump.hairValue[i];
		float clumpRate=pow(clumpShape.hairValue[i],5.0f)*10.0f;
		//cout<<clumpPercent<<" "<<clumpRate<<endl;
		vector<float> origDists(hairParameters.segment);
		for(int j=0;j<hairParameters.segment;j++)
		{
			origDists[j]=sqrt((renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3])
				*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3])
				+(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+1])
				*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+1])
				+(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+2])
				*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+2]));
		}
		for(int j=0;j<hairParameters.segment;j++)
		{
			float delta=clumpPercent*((j+1.0f)/hairParameters.segment+pow((j+1.0f)/hairParameters.segment,clumpRate)*(1-(j+1.0f)/hairParameters.segment));
			//float delta=clumpPercent*((j+1.0f)/hairParameters.segment+clumpRate*(1-(j+1.0f)/hairParameters.segment));
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]+=delta*(clumpData.hairShapeSegment[(centerId*(hairParameters.segment+3)+j+2)*3]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]);
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]+=delta*(clumpData.hairShapeSegment[(centerId*(hairParameters.segment+3)+j+2)*3+1]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]);
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]+=delta*(clumpData.hairShapeSegment[(centerId*(hairParameters.segment+3)+j+2)*3+2]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]);
		}
		for(int j=0;j<hairParameters.segment;j++)
		{
			float newDist=sqrt((renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3])
				*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3])
				+(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+1])
				*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+1])
				+(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+2])
				*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+2]));

			float newDelta=1-origDists[j]/newDist;
			if(newDelta<0)
				newDelta=0;
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]+=newDelta*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]);
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]+=newDelta*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+1]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]);
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]+=newDelta*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+2]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]);
		}

		renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3];
		renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3+1]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3+1];
		renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3+2]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3+2];
	}
}
//hairRender


extern "C" RtPointer ConvertParameters(char* initialdata)
{
	//cout<<"apple"<<endl;
	vixo_hairRender* params = new vixo_hairRender(initialdata);
	return reinterpret_cast<RtPointer>(NULL);
}

extern "C" RtVoid Subdivide(RtPointer blinddata, float detailsize)
{
	vixo_hairRender* p = reinterpret_cast<vixo_hairRender*>(blinddata);
	//p->doRender();
}


extern "C" RtVoid Free(RtPointer blinddata)
{
	delete reinterpret_cast<vixo_hairRender*>(blinddata);
}
