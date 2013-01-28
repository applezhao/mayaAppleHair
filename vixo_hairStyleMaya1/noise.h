#pragma once
#define TABLE_SIZE 256

extern const int kTableMask;
#define MODPERM(x) permtable[(x)&kTableMask]

typedef struct {
	float x;
	float y;
	float z;
} pnt;

class noise {
public:

	static float	atValue( float x );
	static float	atPointUV( float u, float v);
	static float	atPoint( float x, float y, float z ); 
	static pnt		atPointUVColor(float u, float v);

	static pnt    atPointAndTime( float x, float y, float z, float t );

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

