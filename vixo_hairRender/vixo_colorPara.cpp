#include "vixo_colorPara.h"


vixo_colorPara::vixo_colorPara(void)
{
}


vixo_colorPara::~vixo_colorPara(void)
{
}

void vixo_colorPara::init(string fileName,vec3 baseColor,float noiseAmp,int noiseFreq,vector<hairDetail>& hairUVs,int length)
{
	if(fileName!=""||(baseColor.x!=0&&baseColor.y!=0&&baseColor.z!=0))
	{
		hasEffect=true;
	}
	else
		hasEffect=false;
	
	hairValue.resize(length*3);

	if (fileName!="")
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
			vec3 noiseValue=noise::atPointUVColor(hairUVs[i].u*noiseFreq,hairUVs[i].v*noiseFreq);
			noiseValue.x=(noiseValue.x*2-1)*noiseAmp;
			noiseValue.y=(noiseValue.y*2-1)*noiseAmp;
			noiseValue.z=(noiseValue.z*2-1)*noiseAmp;
			int line=hairUVs[i].u*width;
			int col=hairUVs[i].v*height;
			vec3 res;
			res.x=(pixels[(col*width+line)]&0xff)/255.0;
			res.y=((pixels[(col*width+line)]&0xff00)>>8)/255.0;
			res.z=((pixels[(col*width+line)]&0xff0000)>>16)/255.0;

			res.x=min(max(res.x+noiseValue.x,0.0f),1.0f);
			res.y=min(max(res.y+noiseValue.y,0.0f),1.0f);
			res.z=min(max(res.z+noiseValue.z,0.0f),1.0f);
			hairValue[3*i]=res.x;
			hairValue[3*i+1]=res.y;
			hairValue[3*i+2]=res.z;
		}
	}
	else
	{
		for(int i=0;i<length;i++)
		{
			vec3 noiseValue=noise::atPointUVColor(hairUVs[i].u*noiseFreq,hairUVs[i].v*noiseFreq);
			noiseValue.x=(noiseValue.x*2-1)*noiseAmp;
			noiseValue.y=(noiseValue.y*2-1)*noiseAmp;
			noiseValue.z=(noiseValue.z*2-1)*noiseAmp;
			vec3 res;
			res.x=min(max(baseColor.x+noiseValue.x,0.0f),1.0f);
			res.y=min(max(baseColor.y+noiseValue.y,0.0f),1.0f);
			res.z=min(max(baseColor.z+noiseValue.z,0.0f),1.0f);
			hairValue[3*i]=res.x;
			hairValue[3*i+1]=res.y;
			hairValue[3*i+2]=res.z;
		}
	}
}

/*
vec3 vixo_colorPara::getColor(int i)
{
	return hairValue[i];
}

vec3 vixo_colorPara::getClumpValue(int i)
{
	return clumpValue[i];
}

*/
