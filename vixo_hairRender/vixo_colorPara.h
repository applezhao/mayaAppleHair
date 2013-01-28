#pragma once
#include <string>
#include <iostream>
#include <vector>
#include <tiffio.h>
#include "noise.h"
using namespace std;
class vixo_colorPara
{
public:
	vixo_colorPara(void);
	~vixo_colorPara(void);
public:
	vector<float> hairValue;
	//vector<float> clumpValue;
	bool hasEffect;
public:
	void init(string fileName,vec3 baseColor,float noiseAmp,int noiseFreq,vector<hairDetail>& hairUVs,int length);
	//vec3 getColor(int i);
	//vec3 getClumpValue(int i);
};


