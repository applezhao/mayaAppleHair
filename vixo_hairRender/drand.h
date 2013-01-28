#pragma once
#ifndef DRAND48_H
#define DRAND48_H

#include <stdlib.h>
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

#endif
