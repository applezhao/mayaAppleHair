#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <tiffio.h>
#include "noise.h"
using namespace std;

class vixo_image
{
private:
	string filename;
	float noiseAmp;
	float noiseFreq;
	vector<uint32> pixels;
	uint32 width,height;
public:
	vixo_image(void);
	~vixo_image(void);
	vixo_image(string fileName,float noiseAmp,float noiseFreq);
	
	float getValue(float u,float v);
	vec3 getColor(float u,float v);
};

