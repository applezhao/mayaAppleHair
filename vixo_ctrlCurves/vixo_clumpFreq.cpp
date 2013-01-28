#include "vixo_clumpFreq.h"
//#include <tiffio.h>

vixo_clumpFreq::vixo_clumpFreq(void)
{
}


vixo_clumpFreq::~vixo_clumpFreq(void)
{
}

void vixo_clumpFreq::init(string fileName,float baseValue,float noiseAmp,int noiseFreq)
{
	this->baseValue=baseValue;
	this->noiseAmp=noiseAmp;
	this->noiseFreq=noiseFreq;
	this->filename=fileName;
	if(fileName!=""||this->baseValue!=0)
	{
		hasEffect=true;
	}
	else
		hasEffect=false;

	if(fileName!="")
	{
		MImage img;
		img.readFromFile(MString(fileName.c_str()));
		img.getSize(width,height);
		pixels=img.pixels();
		/*TIFF* tif = TIFFOpen(fileName.c_str(), "r");
		if (tif) 
		{
			size_t npixels;
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
			npixels = width * height;
			pixels.resize(npixels);
			TIFFReadRGBAImage(tif, width, height, &pixels[0], 0); //abgr
			TIFFClose(tif); 
		}*/
	}
}


float vixo_clumpFreq::getValue(float u,float v)
{
	float noiseValue=(noise::atPointUV(u*noiseFreq,v*noiseFreq)*2-1)*noiseAmp;
	if(this->filename!="")
	{
		int line=u*width;
		int col=v*height;
		float res=(pixels[(col*width+line)*4])/255.0;
		res=min(max(res+noiseValue,0.0f),1.0f);
		return res;
	}
	else
	{
		float res=min(max(baseValue+noiseValue,0.0f),1.0f);
		return res;
	}
	
}
