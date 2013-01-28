#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <tiffio.h>
#include "noise.h"
using namespace std;
class vixo_clumpFreq
{
public:
	vixo_clumpFreq(void);
	~vixo_clumpFreq(void);
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

