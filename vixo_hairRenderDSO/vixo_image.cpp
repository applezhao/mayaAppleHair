#include "vixo_image.h"


vixo_image::vixo_image(void)
{
}


vixo_image::~vixo_image(void)
{
}

vixo_image::vixo_image(string fileName,float noiseAmp,float noiseFreq)
{
	this->filename=fileName;
	this->noiseAmp=noiseAmp;
	this->noiseFreq=noiseFreq;

	TIFF* tif = TIFFOpen(filename.c_str(), "r");
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
}

float vixo_image::getValue(float u,float v)
{
	int line=u*width;
	int col=v*height;
	float noiseValue=(noise::atPointUV(u*noiseFreq,v*noiseFreq)*2-1)*noiseAmp;
	float res=((pixels[col*width+line]<<8)>>24)/255.0;
	res=min(max(res+noiseValue,0.0f),1.0f);
	return res;
}
vec3 vixo_image::getColor(float u,float v)
{
	vec3 res;
	int line=u*width;
	int col=v*height;
	res.x=(pixels[(col*width+line)]&0xff)/255.0;
	res.y=((pixels[(col*width+line)]&0xff00)>>8)/255.0;
	res.z=((pixels[(col*width+line)]&0xff0000)>>16)/255.0;

	vec3 noiseValue=noise::atPointUVColor(u*noiseFreq,v*noiseFreq);
	noiseValue.x=(noiseValue.x*2-1)*noiseAmp;
	noiseValue.y=(noiseValue.y*2-1)*noiseAmp;
	noiseValue.z=(noiseValue.z*2-1)*noiseAmp;

	res.x=min(max(res.x+noiseValue.x,0.0f),1.0f);
	res.y=min(max(res.y+noiseValue.y,0.0f),1.0f);
	res.z=min(max(res.z+noiseValue.z,0.0f),1.0f);
	return res;
}
