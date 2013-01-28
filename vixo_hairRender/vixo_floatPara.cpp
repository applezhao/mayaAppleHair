#include "vixo_floatPara.h"


vixo_floatPara::vixo_floatPara(void)
{
}


vixo_floatPara::~vixo_floatPara(void)
{
}

void vixo_floatPara::init(string fileName,float baseValue,float noiseAmp,int noiseFreq,vector<hairDetail>& hairUVs,int length,vector<hairDetail>& clumpUVs,int clumpLength)
{
	if(fileName!=""||baseValue!=0)
	{
		hasEffect=true;
	}
	else
		hasEffect=false;

	hairValue.resize(length);
	clumpValue.resize(clumpLength);

	if(fileName!="")
	{
		TIFF* tif = TIFFOpen(fileName.c_str(), "r");
		vector<uint32> pixels;
		uint32 width,height;
		if (tif) 
		{
			size_t npixels;
			TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
			TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
			npixels = width * height;
			pixels.resize(npixels);
			TIFFReadRGBAImage(tif, width, height, &pixels[0], 0); //abgr
			TIFFClose(tif); 
		}

		for(int i=0;i<length;i++)
		{
			float noiseValue=(noise::atPointUV(hairUVs[i].u*noiseFreq,hairUVs[i].v*noiseFreq)*2-1)*noiseAmp;
			int line=hairUVs[i].u*width;
			int col=hairUVs[i].v*height;
			float res=((pixels[col*width+line]<<8)>>24)/255.0;
			res=min(max(res+noiseValue,0.0f),1.0f);
			hairValue[i]=res;
		}

		for(int i=0;i<clumpLength;i++)
		{
			float noiseValue=(noise::atPointUV(clumpUVs[i].u*noiseFreq,clumpUVs[i].v*noiseFreq)*2-1)*noiseAmp;
			int line=clumpUVs[i].u*width;
			int col=clumpUVs[i].v*height;
			float res=((pixels[col*width+line]<<8)>>24)/255.0;
			res=min(max(res+noiseValue,0.0f),1.0f);
			clumpValue[i]=res;
		}
	}
	else
	{
		for(int i=0;i<length;i++)
		{
			float noiseValue=(noise::atPointUV(hairUVs[i].u*noiseFreq,hairUVs[i].v*noiseFreq)*2-1)*noiseAmp;
			float res=min(max(baseValue+noiseValue,0.0f),1.0f);
			hairValue[i]=res;
		}
		for(int i=0;i<clumpLength;i++)
		{
			float noiseValue=(noise::atPointUV(clumpUVs[i].u*noiseFreq,clumpUVs[i].v*noiseFreq)*2-1)*noiseAmp;
			float res=min(max(baseValue+noiseValue,0.0f),1.0f);
			clumpValue[i]=res;
		}
	}
}

/*
float vixo_floatPara::getValue(float u,float v)
{
	float noiseValue=(noise::atPointUV(u*noiseFreq,v*noiseFreq)*2-1)*noiseAmp;
	if(this->filename!="")
	{
		int line=u*width;
		int col=v*height;
		float res=((pixels[col*width+line]<<8)>>24)/255.0;
		res=min(max(res+noiseValue,0.0f),1.0f);
		return res;
	}
	else
	{
		float res=min(max(baseValue+noiseValue,0.0f),1.0f);
		return res;
	}
	
}
*/


float vixo_floatPara::getValue(int i)
{
	return hairValue[i];
}

float vixo_floatPara::getClumpValue(int i)
{
	return clumpValue[i];
}