#include <iostream>
#include <tiffio.h>
#include <vector>
using namespace std;

int main()
{
	// ooo
	/*
	string hairRootFile="f:/hairRoot.hr";
	string hairCacheFile="f:/hairCache.0010.hcc";

	float triBoxExpand=5.0;

	vec3 tipColorBaseValue=vec3(0.078,0.667,0.150);
	string tipColorTexture="";
	float tipColorNoiseAmp=0.2;
	int tipColorNoiseFreq=25;

	vec3 baseColorBaseValue=vec3(0.0,1.0,0.0);
	string baseColorTexture="";
	float baseColorNoiseAmp=0.2;
	int baseColorNoiseFreq=25;

	float tipOpaBaseValue=0.780;
	string tipOpaTexture="";
	float tipOpaNoiseAmp=0.2;
	int tipOpaNoiseFreq=25;

	float baseOpaBaseValue=0.211;
	string baseOpaTexture="";
	float baseOpaNoiseAmp=0.2;
	int baseOpaNoiseFreq=25;

	float scraggleValue=0.780;
	string scraggleTexture="";
	float scraggleNoiseAmp=0.2;
	int scraggleNoiseFreq=25;

	float scraggleFreqValue=0.780;
	string scraggleFreqTexture="";
	float scraggleFreqNoiseAmp=0.2;
	int scraggleFreqNoiseFreq=25;

	float clumpValue=0.560;
	string clumpTexture="";
	float clumpNoiseAmp=0.2;
	int clumpNoiseFreq=25;

	float clumpFreqValue=0.800;
	string clumpFreqTexture="";
	float clumpFreqNoiseAmp=0.2;
	int clumpFreqNoiseFreq=25;

	float clumpShapeValue=0.200;
	string clumpShapeTexture="";
	float clumpShapeNoiseAmp=0.2;
	int clumpShapeNoiseFreq=25;

	float lengthNoiseAmp=0.2;
	int lengthNoiseFreq=25;

	float inclinationNoiseAmp=0.2;
	int inclinationNoiseFreq=25;

	float polarNoiseAmp=0.2;
	int polarNoiseFreq=25;

	float tipCurlNoiseAmp=0.2;
	int tipCurlNoiseFreq=25;

	float baseCurlNoiseAmp=0.2;
	int baseCurlNoiseFreq=25;

	*/
	
	TIFF* tif = TIFFOpen("f:/test", "r");
	if (tif) 
	{
		size_t npixels;
		vector<uint32> pixels;
		uint32 width;
		uint32 height;
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
		npixels = width * height;
		pixels.resize(npixels);
		TIFFReadRGBAImage(tif, width, height, &pixels[0], 0); //abgr
		TIFFClose(tif); 
	}
}
