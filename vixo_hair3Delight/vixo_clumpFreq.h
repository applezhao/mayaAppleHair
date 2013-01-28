#pragma once
//#pragma comment (obj,"tif_open.lib")
//#pragma comment (lib,"C:/Program Files (x86)/GnuWin32/src/tiff/3.8.2/tiff-3.8.2-src/libtiff/libtiff.lib")
#include <string>
#include <iostream>
//#include <window.h>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>
#include <tiffio.h>
#include <maya/MImage.h>
#include "noise.h"
using namespace std;
class vixo_clumpFreq
{
public:
	vixo_clumpFreq(void);
	~vixo_clumpFreq(void);
public:
	//vector<uint32> pixels;
	unsigned char * pixels;
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

