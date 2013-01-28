#pragma once
#ifndef NOISE_H
#define NOISE_H

#include <fstream>
#include <vector>
//#include <tiffio.h>
#include <string>
#include<iostream>
#include <math.h>
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
