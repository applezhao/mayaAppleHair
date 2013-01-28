#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <tiffio.h>
#include "noise.h"
using namespace std;
class vixo_floatPara
{
public:
	vixo_floatPara(void);
	~vixo_floatPara(void);
public:
	vector<float> hairValue;
	vector<float> clumpValue;
	bool hasEffect;
public:
	void init(string fileName,float baseValue,float noiseAmp,int noiseFreq,vector<hairDetail>& hairUVs,int length,vector<hairDetail>& clumpUVs,int clumpLength);
	float getValue(int i);
	float getClumpValue(int i);
};
