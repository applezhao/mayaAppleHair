#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
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