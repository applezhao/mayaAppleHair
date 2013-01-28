#include "noise.h"
#include "drand.h"
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

vec3	noise::atPointUVColor(float u, float v)
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
