#include <ri.h>
#define _USE_MATH_DEFINES
#include <math.h>


#include <sstream>
#include <iostream>
#include <cstdlib>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vixo_floatPara.h"
#include "vixo_colorPara.h"
#include "vixo_clumpFreq.h"

#ifdef _WIN32
#define export __declspec(dllexport)
#define strtok_r(str,delim,saveptr) strtok((str),(delim))
#else
#define export
#endif

extern "C" {
	export RtPointer ConvertParameters(RtString paramstr);
	export RtVoid Subdivide(RtPointer data, RtFloat detail);
	export RtVoid Free(RtPointer data);
}

class vixo_hairRender
{
public:
	hairParametersInfo hairParameters;
	int currentTriIdx;
	string clumpCenterInfoFileName;
	vec3 boxMin;
	vec3 boxMax;

	string clumpCenterTempFile;

	int span;

	perTriHairInfo triangleHairs;
	renderInfo renderData;

	vector<perTriHairInfo> perClumpData;
	map<int,int> hairIdMapLocalId;
	vector<hairDetail> forClumpUV;
	renderInfo clumpData;

	vixo_colorPara tipColor;
	vixo_colorPara baseColor;
	vixo_floatPara tipOpacity;
	vixo_floatPara baseOpacity;
	vixo_floatPara tipWidth;
	vixo_floatPara baseWidth;

	vixo_floatPara length;
	vixo_floatPara polar;
	vixo_floatPara inclination;
	vixo_floatPara tipCurl;
	vixo_floatPara baseCurl;

	vixo_floatPara scraggle;
	vixo_floatPara scraggleFreq;
	vixo_floatPara clump;
	vixo_floatPara clumpShape;

public:
	vixo_hairRender(void);
	~vixo_hairRender(void);
	vixo_hairRender(const char* initChars);
	vixo_hairRender(hairParametersInfo hairParameters,int triIdx,string clumpCenterFile,vec3 boxMin,vec3 boxMax);

	void doRender();
	void subdivide();
	void renderTriangle();

	void getBoundingBox(vector<vec3>& allPoints,vector<vec3>& box,int allPLen,float expand);

	void getClumpCenter();

	void getTriHairRootData(int triIdx,triangleBasicInfo& perTriBasicData,vector<hairDetail>& perHairDetailInfo);
	void getTriHairCacheData(int triIdx,forExportHairCache& perTriCacheData,vector<vec3>& perTriVertexCacheData);
	void getTriClumpCenterSet(triangleBasicInfo& triBasicData,set<hairDetailExtend>& clumpSet,vector<hairDetailExtend>& hairClump);

	void getColorOpaWidthInfo();
	void getLenPolarIncliCurlInfo(vector<float>& curlParaHair,vector<float>& curlParaClump,int clumpSetSize);
	void deformPolarIncliCurl(vector<float>& curlParaHair,vector<float>& curlParaClump,int clumpSetSize);
	void deformScraggle(int clumpSetSize);
	void doClump();
	void bound(RtBound bnd)
	{
		bnd[0]=boxMin.x;
		bnd[1]=boxMax.x;
		bnd[2]=boxMin.y;
		bnd[3]=boxMax.y;
		bnd[4]=boxMin.z;
		bnd[5]=boxMax.z;
	}
};


export RtPointer ConvertParameters(char* initialdata)
{
	vixo_hairRender* params = new vixo_hairRender(initialdata);
	return reinterpret_cast<RtPointer>(params);
}

export void Subdivide(RtPointer blinddata, RtFloat detailsize)
{
	vixo_hairRender* p = reinterpret_cast<vixo_hairRender*>(blinddata);
	p->doRender();
}


export void Free(RtPointer blinddata)
{
	delete reinterpret_cast<vixo_hairRender*>(blinddata);
}

vixo_hairRender::vixo_hairRender(void)
{
}


vixo_hairRender::~vixo_hairRender(void)
{
}

vixo_hairRender::vixo_hairRender(const char* initChars)
{
	istringstream(initChars) >>hairParameters.hairRootFile
		>>hairParameters.hairCacheFile
		>>hairParameters.triBoxExpand

		>>hairParameters.tipColorBaseValue.x
		>>hairParameters.tipColorBaseValue.y
		>>hairParameters.tipColorBaseValue.z
		>>hairParameters.tipColorTexture
		>>hairParameters.tipColorNoiseAmp
		>>hairParameters.tipColorNoiseFreq

		>>hairParameters.baseColorBaseValue.x
		>>hairParameters.baseColorBaseValue.y
		>>hairParameters.baseColorBaseValue.z
		>>hairParameters.baseColorTexture
		>>hairParameters.baseColorNoiseAmp
		>>hairParameters.baseColorNoiseFreq

		>>hairParameters.tipOpaBaseValue
		>>hairParameters.tipOpaTexture
		>>hairParameters.tipOpaNoiseAmp
		>>hairParameters.tipOpaNoiseFreq

		>>hairParameters.baseOpaBaseValue
		>>hairParameters.baseOpaTexture
		>>hairParameters.baseOpaNoiseAmp
		>>hairParameters.baseOpaNoiseFreq

		>>hairParameters.tipWidthBaseValue
		>>hairParameters.tipWidthTexture
		>>hairParameters.tipWidthNoiseAmp
		>>hairParameters.tipWidthNoiseFreq

		>>hairParameters.baseWidthBaseValue
		>>hairParameters.baseWidthTexture
		>>hairParameters.baseWidthNoiseAmp
		>>hairParameters.baseWidthNoiseFreq

		>>hairParameters.scraggleValue
		>>hairParameters.scraggleTexture
		>>hairParameters.scraggleNoiseAmp
		>>hairParameters.scraggleNoiseFreq

		>>hairParameters.scraggleFreqValue
		>>hairParameters.scraggleFreqTexture
		>>hairParameters.scraggleFreqNoiseAmp
		>>hairParameters.scraggleFreqNoiseFreq

		>>hairParameters.clumpValue
		>>hairParameters.clumpTexture
		>>hairParameters.clumpNoiseAmp
		>>hairParameters.clumpNoiseFreq

		>>hairParameters.clumpFreqValue
		>>hairParameters.clumpFreqTexture
		>>hairParameters.clumpFreqNoiseAmp
		>>hairParameters.clumpFreqNoiseFreq

		>>hairParameters.clumpShapeValue
		>>hairParameters.clumpShapeTexture
		>>hairParameters.clumpShapeNoiseAmp
		>>hairParameters.clumpShapeNoiseFreq

		>>hairParameters.lengthNoiseAmp
		>>hairParameters.lengthNoiseFreq
		>>hairParameters.inclinationNoiseAmp
		>>hairParameters.inclinationNoiseFreq
		>>hairParameters.polarNoiseAmp
		>>hairParameters.polarNoiseFreq
		>>hairParameters.tipCurlNoiseAmp
		>>hairParameters.tipCurlNoiseFreq
		>>hairParameters.baseCurlNoiseAmp
		>>hairParameters.baseCurlNoiseFreq
		>>hairParameters.segment

		>>this->currentTriIdx
		>>this->clumpCenterInfoFileName
		>>this->boxMin.x >>this->boxMax.x
		>>this->boxMin.y >>this->boxMax.y
		>>this->boxMin.z >>this->boxMax.z;
}

vixo_hairRender::vixo_hairRender(hairParametersInfo hairParameters,int triIdx,string clumpCenterFile,vec3 boxMin,vec3 boxMax)
{
	this->hairParameters=hairParameters;
	this->currentTriIdx=triIdx;
	this->boxMin=boxMin;
	this->boxMax=boxMax;
	this->clumpCenterInfoFileName=clumpCenterFile;
}

void vixo_hairRender::doRender()
{
	srand(time(NULL));
	if(this->currentTriIdx==-1)
	{
		getClumpCenter();
		subdivide();
		return;
	}
	//for each triangle needing rendering
	renderTriangle();
}

void vixo_hairRender::renderTriangle()
{
	getTriHairRootData(this->currentTriIdx,this->triangleHairs.triBasicData,this->triangleHairs.hairDetailData);
	getTriHairCacheData(this->currentTriIdx,this->triangleHairs.triCacheData,this->triangleHairs.triVertexCacheData);
	triangleHairs.hairNum=triangleHairs.triBasicData.hairNum;

	set<hairDetailExtend> clumpSet;
	getTriClumpCenterSet(this->triangleHairs.triBasicData,clumpSet,this->triangleHairs.hairClump);
	perClumpData.resize(clumpSet.size());
	forClumpUV.resize(clumpSet.size());
	set<hairDetailExtend>::iterator clumpSetIter;
	int i=0;
	for(clumpSetIter=clumpSet.begin();clumpSetIter!=clumpSet.end();clumpSetIter++,i++)
	{
		hairIdMapLocalId.insert(pair<int,int>(clumpSetIter->closetClumpCenter,i));
		perClumpData[i].hairNum=1;
		perClumpData[i].hairIdx=clumpSetIter->closetClumpCenter;
		getTriHairRootData(clumpSetIter->triId,perClumpData[i].triBasicData,perClumpData[i].hairDetailData);
		getTriHairCacheData(clumpSetIter->triId,perClumpData[i].triCacheData,perClumpData[i].triVertexCacheData);
		forClumpUV[i]=perClumpData[i].hairDetailData[perClumpData[i].hairIdx-perClumpData[i].triBasicData.hairBegin];
	}

	//get tip/base color/opacity/width uniform
	getColorOpaWidthInfo();

	//polar, inclination, length, curl
	vector<float> curlParaHair(span*triangleHairs.hairNum);
	vector<float> curlParaClump(span*clumpSet.size());
	getLenPolarIncliCurlInfo(curlParaHair,curlParaClump,clumpSet.size());
	deformPolarIncliCurl(curlParaHair,curlParaClump,clumpSet.size());

	//scraggle
	deformScraggle(clumpSet.size());

	//clump
	clump.init(hairParameters.clumpTexture,hairParameters.clumpValue,hairParameters.clumpNoiseAmp,hairParameters.clumpNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,0);
	clumpShape.init(hairParameters.clumpShapeTexture,hairParameters.clumpShapeValue,hairParameters.clumpShapeNoiseAmp,hairParameters.clumpShapeNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,0);

	if(clump.hasEffect==true)
	{
		doClump();
	}

	//array of pointsNo each curve
	vector<int> pointNum(triangleHairs.hairNum,hairParameters.segment+3);
	//array of index
	vector<float> index(triangleHairs.hairNum);
	for(int i=0;i<triangleHairs.hairNum;i++)
		index[i]=i;

	RiBasis(RiCatmullRomBasis,1,RiCatmullRomBasis,1);
	RtToken tokens[5];
	RtPointer values[5];
	tokens[0]="vertex point P";
	values[0]=(RtPointer)&renderData.hairShapeSegment[0];
	tokens[1]="varying float width";
	values[1]=(RtPointer)&renderData.width[0];
	tokens[2]="varying color Os";
	values[2]=(RtPointer)&renderData.opacity[0];
	tokens[3]="uniform color rootcolor";
	values[3]=(RtPointer)&renderData.baseColor[0];
	tokens[3]="uniform color tipcolor";
	values[3]=(RtPointer)&renderData.tipColor[0];
	tokens[4]="uniform float index";
	values[4]=(RtPointer)&index[0];

	RiCurves((RtToken)"cubic",triangleHairs.hairNum,(RtInt*)&pointNum[0],(RtToken)"nonperiodic",
		tokens[0],values[0],
		tokens[1],values[1],
		tokens[2],values[2],
		tokens[3],values[3],
		tokens[4],values[4]
	);
}

void vixo_hairRender::getTriHairRootData(int triIdx,triangleBasicInfo& perTriBasicData,vector<hairDetail>& perHairDetailInfo)
{
	fstream fin(hairParameters.hairRootFile.c_str(),ios_base::in|ios_base::binary);
	fin.seekp(sizeof(int)*3+sizeof(triangleBasicInfo)*triIdx);
	fin.read((char*)&perTriBasicData,sizeof(triangleBasicInfo));
	fin.seekp(perTriBasicData.beginIdx);
	perHairDetailInfo.resize(perTriBasicData.hairNum);
	fin.read((char*)&perHairDetailInfo[0],sizeof(hairDetail)*perTriBasicData.hairNum);
	fin.close();
}

void vixo_hairRender::getTriHairCacheData(int triIdx,forExportHairCache& perTriCacheData,vector<vec3>& perTriVertexCacheData)
{
	fstream fin(hairParameters.hairCacheFile.c_str(),ios_base::in|ios_base::binary);
	int triNumvertexNumSpan[3];
	fin.read((char*)triNumvertexNumSpan,sizeof(int)*3);
	span=triNumvertexNumSpan[2];
	perTriVertexCacheData.resize(3*span);
	fin.seekp(sizeof(int)*3+sizeof(forExportHairCache)*triIdx);
	fin.read((char*)&perTriCacheData,sizeof(forExportHairCache));
	fin.seekp(sizeof(int)*3+sizeof(forExportHairCache)*triNumvertexNumSpan[0]+sizeof(vec3)*span*perTriCacheData.v0.vid);
	fin.read((char*)&perTriVertexCacheData[0],sizeof(vec3)*span);
	fin.seekp(sizeof(int)*3+sizeof(forExportHairCache)*triNumvertexNumSpan[0]+sizeof(vec3)*span*perTriCacheData.v1.vid);
	fin.read((char*)&perTriVertexCacheData[span],sizeof(vec3)*span);
	fin.seekp(sizeof(int)*3+sizeof(forExportHairCache)*triNumvertexNumSpan[0]+sizeof(vec3)*span*perTriCacheData.v2.vid);
	fin.read((char*)&perTriVertexCacheData[span*2],sizeof(vec3)*span);
	fin.close();
}

void vixo_hairRender::getTriClumpCenterSet(triangleBasicInfo& triBasicData,set<hairDetailExtend>& clumpSet,vector<hairDetailExtend>& hairClump)
{
	fstream fin(clumpCenterInfoFileName.c_str(),ios_base::in|ios_base::binary);
	fin.seekp(sizeof(hairDetailExtend)*triBasicData.hairBegin);
	hairClump.resize(triBasicData.hairNum);
	fin.read((char*)&hairClump[0],sizeof(hairDetailExtend)*triBasicData.hairNum);
	fin.close();
	clumpSet.clear();
	for(int i=0;i<triBasicData.hairNum;i++)
	{
		clumpSet.insert(hairClump[i]);
	}
}

void vixo_hairRender::subdivide()
{
	fstream fin(hairParameters.hairCacheFile.c_str(),ios_base::in|ios_base::binary);
	int triNumvertexNumSpan[3];
	fin.read((char*)triNumvertexNumSpan,sizeof(int)*3);
	int triNumTot=triNumvertexNumSpan[0];
	int vertexNumTot=triNumvertexNumSpan[1];
	int span=triNumvertexNumSpan[2];
	vector<forExportHairCache> hairVertexData(triNumTot);
	fin.read((char*)&hairVertexData[0],sizeof(forExportHairCache)*triNumTot);
	vector<vec3> hairShapeData(vertexNumTot*span);
	fin.read((char*)&hairShapeData[0],sizeof(vec3)*vertexNumTot*span);
	fin.close();

	vector<vec3> allPoints(3*(span+1));
	vector<vec3> box(2);
	for(int i=0;i<triNumTot;i++)
	{
		allPoints[0]=hairVertexData[i].v0.position;
		allPoints[1]=hairVertexData[i].v1.position;
		allPoints[2]=hairVertexData[i].v2.position;

		for(int j=0;j<span;j++)
		{
			allPoints[3*(j+1)+0]=allPoints[3*j+0]+hairShapeData[hairVertexData[i].v0.vid*span+j];
			allPoints[3*(j+1)+1]=allPoints[3*j+1]+hairShapeData[hairVertexData[i].v1.vid*span+j];
			allPoints[3*(j+1)+2]=allPoints[3*j+2]+hairShapeData[hairVertexData[i].v2.vid*span+j];
		}

		getBoundingBox(allPoints,box,(3*span+1),hairParameters.triBoxExpand);

		vixo_hairRender* pNew = new vixo_hairRender(hairParameters,i,clumpCenterTempFile,box[0],box[1]);
		RtBound bnd;
		pNew->bound(bnd);
		//RiProcedural(pNew, bnd, Subdivide, Free);
		//pNew->doRender();
		RiProcedural(pNew,bnd,Subdivide,Free);
	}
}

void vixo_hairRender::getBoundingBox(vector<vec3>& allPoints,vector<vec3>& box,int allPLen,float expand)
{
	box[0].x=allPoints[0].x;
	box[0].y=allPoints[0].y;
	box[0].z=allPoints[0].z;
	box[1].x=allPoints[0].x;
	box[1].y=allPoints[0].y;
	box[1].z=allPoints[0].z;

	for(int i=1;i<allPLen;i++)
	{
		box[0].x=min(allPoints[i].x,box[0].x);
		box[0].y=min(allPoints[i].y,box[0].y);
		box[0].z=min(allPoints[i].z,box[0].z);

		box[1].x=max(allPoints[i].x,box[1].x);
		box[1].y=max(allPoints[i].y,box[1].y);
		box[1].z=max(allPoints[i].z,box[1].z);
	}

	vec3 center=(box[0]+box[1])/2.0f;

	vec3 bv0=box[0]-center;
	bv0=bv0*expand;
	box[0]=center+bv0;

	vec3 bv1=box[1]-center;
	bv1=bv1*expand;
	box[1]=center+bv1;
}

void vixo_hairRender::getClumpCenter()
{
	fstream fin(hairParameters.hairRootFile.c_str(),ios_base::in|ios_base::binary);
	int hairNumTriNum[3];
	fin.read((char*)hairNumTriNum,sizeof(int)*3);
	int triNumTot=hairNumTriNum[1];
	int hairNumTot=hairNumTriNum[2];
	vector<triangleBasicInfo> triBasicData(triNumTot);
	fin.read((char*)&triBasicData[0],sizeof(triangleBasicInfo)*triNumTot);
	vector<hairDetail> hairDetailData(hairNumTot);
	fin.read((char*)&hairDetailData[0],sizeof(hairDetail)*hairNumTot);
	fin.close();

	vixo_clumpFreq clumpFreq;
	clumpFreq.init(hairParameters.clumpFreqTexture,hairParameters.clumpFreqValue,hairParameters.clumpFreqNoiseAmp,hairParameters.clumpFreqNoiseFreq);

	//getClumpCenterInfo
	vector<clumpCenterInfo> clumpCenterData;
	vector<hairDetailExtend> hairClumpRelationData(hairNumTot);

	float u,v;
	for(u=0; u<1; u+= .01) 
	{
		for(v=0; v<1; v+= .01) 
		{
			float cf = clumpFreq.getValue(u, v);
			float r = rand()/(RAND_MAX+1.0f);
			if( r < cf*0.0015 ) 
			{
				clumpCenterInfo ele;
				ele.hairid=0;
				ele.u= u + r*(-.5);
				ele.v= v + r*(-.5);
				ele.minDist=999999;

				clumpCenterData.push_back(ele);
			}
		}
	}

	double minDist = 9999999;
	if( clump.hasEffect==true ) 
	{
		for(int i=0;i<hairNumTot;i++)
		{
			unsigned c;
			for( c=0; c<clumpCenterData.size(); ++c ) 
			{
				const float cu = clumpCenterData[c].u;
				const float cv = clumpCenterData[c].v;
				const float cdist = clumpCenterData[c].minDist;
				float dist = ((cu-hairDetailData[i].u)*(cu-hairDetailData[i].u) + (cv-hairDetailData[i].v)*(cv-hairDetailData[i].v));
				if( dist < cdist ) 
				{
					clumpCenterData[c].minDist = dist;
					clumpCenterData[c].hairid = i;
					clumpCenterData[c].triid=hairDetailData[i].triIdx;
				}
				if( dist < minDist ) 
				{
					minDist = dist;
					hairClumpRelationData[i].closetClumpCenter=c;
					hairClumpRelationData[i].triId=clumpCenterData[c].triid;
				}
			}
		}
	}

	//write to file
	char buffer [100];
	long int ltime=time(NULL);
	_itoa(ltime,buffer,100);
	string ltimestr(buffer);

	_itoa(rand(),buffer,100);
	string randStr(buffer);

	clumpCenterTempFile="c:/windows/temp/temp"+ltimestr+"."+randStr+".cct";
	fstream fout(clumpCenterTempFile.c_str(),ios_base::out|ios_base::binary);
	fout.write((char*)&hairClumpRelationData[0],sizeof(hairDetailExtend)*hairNumTot);
	fout.flush();
	fout.close();
}

void vixo_hairRender::getColorOpaWidthInfo()
{
	tipColor.init(hairParameters.tipColorTexture,hairParameters.tipColorBaseValue,hairParameters.tipColorNoiseAmp,hairParameters.tipColorNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum);
	//renderData.tipColor=tipColor.hairValue;
	baseColor.init(hairParameters.baseColorTexture,hairParameters.baseColorBaseValue,hairParameters.baseColorNoiseAmp,hairParameters.baseColorNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum);
	//renderData.baseColor=baseColor.hairValue;
	tipOpacity.init(hairParameters.tipOpaTexture,hairParameters.tipOpaBaseValue,hairParameters.tipOpaNoiseAmp,hairParameters.tipOpaNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,0);
	baseOpacity.init(hairParameters.baseOpaTexture,hairParameters.baseOpaBaseValue,hairParameters.baseOpaNoiseAmp,hairParameters.baseOpaNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,0);
	tipWidth.init(hairParameters.tipWidthTexture,hairParameters.tipWidthBaseValue,hairParameters.tipWidthNoiseAmp,hairParameters.tipWidthNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,0);
	baseWidth.init(hairParameters.baseWidthTexture,hairParameters.baseWidthBaseValue,hairParameters.baseWidthNoiseAmp,hairParameters.baseWidthNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,0);

	renderData.opacity.resize(3*hairParameters.segment*triangleHairs.hairNum);
	renderData.width.resize(3*hairParameters.segment*triangleHairs.hairNum);
	for(int i=0;i<triangleHairs.hairNum;i++)
	{
		for(int j=0;j<hairParameters.segment;j++)
		{
			float o=baseOpacity.hairValue[i]+j*(tipOpacity.hairValue[i]-baseOpacity.hairValue[i])/(hairParameters.segment-1);
			renderData.opacity[3*(i*hairParameters.segment+j)]=renderData.opacity[3*(i*hairParameters.segment+j)+1]=renderData.opacity[3*(i*hairParameters.segment+j)+2]=o;
			float w=baseWidth.hairValue[i]+j*(tipWidth.hairValue[i]-baseWidth.hairValue[i])/(hairParameters.segment-1);
			renderData.width[3*(i*hairParameters.segment+j)]=renderData.width[3*(i*hairParameters.segment+j)+1]=renderData.width[3*(i*hairParameters.segment+j)+2]=w;
		}
	}
}

void vixo_hairRender::getLenPolarIncliCurlInfo(vector<float>& curlParaHair,vector<float>& curlParaClump,int clumpSetSize)
{
	length.init("",0,hairParameters.lengthNoiseAmp,hairParameters.lengthNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,clumpSetSize);
	polar.init("",0,hairParameters.polarNoiseAmp,hairParameters.polarNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,clumpSetSize);
	inclination.init("",0,hairParameters.inclinationNoiseAmp,hairParameters.inclinationNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,clumpSetSize);
	tipCurl.init("",0,hairParameters.tipCurlNoiseAmp,hairParameters.tipCurlNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,clumpSetSize);
	baseCurl.init("",0,hairParameters.baseCurlNoiseAmp,hairParameters.baseCurlNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,clumpSetSize);
	for(int i=0;i<triangleHairs.hairNum;i++)
	{
		for(int j=0;j<span;j++)
		{
			float c=baseCurl.hairValue[i]+j*(tipCurl.hairValue[i]-baseCurl.hairValue[i])/(span-1);
			curlParaHair[i*span+j]=c;
		}
	}
	for(int i=0;i<clumpSetSize;i++)
	{
		for(int j=0;j<span;j++)
		{
			float c=baseCurl.clumpValue[i]+j*(tipCurl.clumpValue[i]-baseCurl.clumpValue[i])/(span-1);
			curlParaClump[i*span+j]=c;
		}
	}
}

void vixo_hairRender::deformPolarIncliCurl(vector<float>& curlParaHair,vector<float>& curlParaClump,int clumpSetSize)
{
	renderData.hairShapeSpan.resize(triangleHairs.hairNum*span);
	for(int i=0;i<triangleHairs.hairNum;i++)
	{
		vec3 normal=triangleHairs.triCacheData.v0.normal*triangleHairs.hairDetailData[i].w0+
			triangleHairs.triCacheData.v1.normal*triangleHairs.hairDetailData[i].w1+
			triangleHairs.triCacheData.v2.normal*triangleHairs.hairDetailData[i].w2;
		vec3 tangent=triangleHairs.triCacheData.v0.tangent*triangleHairs.hairDetailData[i].w0+
			triangleHairs.triCacheData.v1.tangent*triangleHairs.hairDetailData[i].w1+
			triangleHairs.triCacheData.v2.tangent*triangleHairs.hairDetailData[i].w2;
		vec3 binormal=normal^tangent;
		normal.normalize();
		binormal.normalize();

		for(int j=0;j<span;j++)
		{
			renderData.hairShapeSpan[i*span+j]=triangleHairs.triVertexCacheData[j]*triangleHairs.hairDetailData[i].w0
				+triangleHairs.triVertexCacheData[span+j]*triangleHairs.hairDetailData[i].w1
				+triangleHairs.triVertexCacheData[2*span+j]*triangleHairs.hairDetailData[i].w2;
			renderData.hairShapeSpan[i*span+j].rotate(normal,polar.hairValue[i]*M_PI);
			renderData.hairShapeSpan[i*span+j].rotate(binormal,inclination.hairValue[i]*M_PI/2);
			renderData.hairShapeSpan[i*span+j].rotate(binormal,curlParaClump[i*span+j]*M_PI/2);
			renderData.hairShapeSpan[i*span+j]=renderData.hairShapeSpan[i*span+j]*length.hairValue[i];
		}
	}

	clumpData.hairShapeSpan.resize(clumpSetSize*span);
	for(int i=0;i<clumpSetSize;i++)
	{
		vec3 normal=perClumpData[i].triCacheData.v0.normal*forClumpUV[i].w0+
			perClumpData[i].triCacheData.v1.normal*forClumpUV[i].w1+
			perClumpData[i].triCacheData.v2.normal*forClumpUV[i].w2;
		vec3 tangent=perClumpData[i].triCacheData.v0.tangent*forClumpUV[i].w0+
			perClumpData[i].triCacheData.v1.tangent*forClumpUV[i].w1+
			perClumpData[i].triCacheData.v2.tangent*forClumpUV[i].w2;
		vec3 binormal=normal^tangent;
		normal.normalize();
		binormal.normalize();

		for(int j=0;j<span;j++)
		{
			clumpData.hairShapeSpan[i*span+j]=perClumpData[i].triVertexCacheData[j]*forClumpUV[i].w0+
				perClumpData[i].triVertexCacheData[span+j]*forClumpUV[i].w1+
				perClumpData[i].triVertexCacheData[2*span+j]*forClumpUV[i].w2;
			clumpData.hairShapeSpan[i*span+j].rotate(normal,polar.clumpValue[i]*M_PI);
			clumpData.hairShapeSpan[i*span+j].rotate(binormal,inclination.clumpValue[i]*M_PI/2);
			clumpData.hairShapeSpan[i*span+j].rotate(binormal,curlParaClump[i]*M_PI/2);
			clumpData.hairShapeSpan[i*span+j]=clumpData.hairShapeSpan[i*span+j]*length.clumpValue[i];
		}
	}
}

void vixo_hairRender::deformScraggle(int clumpSetSize)
{
	scraggle.init(hairParameters.scraggleTexture,hairParameters.scraggleValue,hairParameters.scraggleNoiseAmp,hairParameters.scraggleNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,clumpSetSize);
	scraggleFreq.init(hairParameters.scraggleFreqTexture,hairParameters.scraggleFreqValue,hairParameters.scraggleFreqNoiseAmp,hairParameters.scraggleFreqNoiseFreq,
		triangleHairs.hairDetailData,triangleHairs.hairNum,forClumpUV,clumpSetSize);

	renderData.hairShapeSegment.resize(triangleHairs.hairNum*(hairParameters.segment+3)*3);
	for(int i=0;i<triangleHairs.hairNum;i++)
	{
		vec3 position=triangleHairs.triCacheData.v0.position*triangleHairs.hairDetailData[i].w0+
			triangleHairs.triCacheData.v1.position*triangleHairs.hairDetailData[i].w1+
			triangleHairs.triCacheData.v2.position*triangleHairs.hairDetailData[i].w2;
		for(int j=0;j<hairParameters.segment;j++)
		{
			float start=(j+0.0f)*span/hairParameters.segment;
			float end=(j+1.0f)*span/hairParameters.segment;
			vec3 temp;
			if(floor(end)==ceil(start)) //not in one span
				temp=renderData.hairShapeSpan[i*span+ceil(start)-1]*(ceil(start)-start)+renderData.hairShapeSpan[i*span+ceil(start)]*(end-floor(end));
			else
				temp=renderData.hairShapeSpan[i*span+ceil(start)-1]*(end-start);
			if(rand()/(RAND_MAX+1.0f)<scraggleFreq.hairValue[i])
			{
				vec3 axis;
				randomAxis(axis);
				float randV=rand()/(RAND_MAX+1.0f)*2.0f-1.0f;
				temp.rotate(axis,scraggle.hairValue[i]*randV*M_PI);
			}
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]=temp.x+position.x;
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]=temp.y+position.y;
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]=temp.z+position.z;
		}
		renderData.hairShapeSegment[(i*(hairParameters.segment+3))*3]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3]=position.x;
		renderData.hairShapeSegment[(i*(hairParameters.segment+3))*3+1]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3+1]=position.x;
		renderData.hairShapeSegment[(i*(hairParameters.segment+3))*3+2]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3+2]=position.x;

		renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3];
		renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3+1]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3+1];
		renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3+2]=renderData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3+2];
	}
	clumpData.hairShapeSegment.resize(clumpSetSize*(hairParameters.segment+3)*3);
	for(int i=0;i<clumpSetSize;i++)
	{
		vec3 position=perClumpData[i].triCacheData.v0.position*forClumpUV[i].w0+
			perClumpData[i].triCacheData.v1.position*forClumpUV[i].w1+
			perClumpData[i].triCacheData.v2.position*forClumpUV[i].w2;
		for(int j=0;j<hairParameters.segment;j++)
		{
			float start=(j+0.0f)*span/hairParameters.segment;
			float end=(j+1.0f)*span/hairParameters.segment;
			vec3 temp;
			if(floor(end)==ceil(start)) //not in one span
				temp=clumpData.hairShapeSpan[i*span+ceil(start)-1]*(ceil(start)-start)+clumpData.hairShapeSpan[i*span+ceil(start)]*(end-floor(end));
			else
				temp=clumpData.hairShapeSpan[i*span+ceil(start)-1]*(end-start);
			if(rand()/(RAND_MAX+1.0f)<scraggleFreq.clumpValue[i])
			{
				vec3 axis;
				randomAxis(axis);
				float randV=rand()/(RAND_MAX+1.0f)*2.0f-1.0f;
				temp.rotate(axis,scraggle.clumpValue[i]*randV*M_PI);
			}
			clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3]=temp.x+position.x;
			clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+1]=temp.y+position.y;
			clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+2]=temp.z+position.z;
		}

		clumpData.hairShapeSegment[(i*(hairParameters.segment+3))*3]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3]=position.x;
		clumpData.hairShapeSegment[(i*(hairParameters.segment+3))*3+1]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3+1]=position.x;
		clumpData.hairShapeSegment[(i*(hairParameters.segment+3))*3+2]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+1)*3+2]=position.x;

		clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3];
		clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3+1]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3+1];
		clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+2)*3+2]=clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+hairParameters.segment+1)*3+2];
	}
}

void vixo_hairRender::doClump()
{
	for(int i=0;i<triangleHairs.hairNum;i++)
	{
		int centerIdTot=triangleHairs.hairClump[i].closetClumpCenter;
		if(centerIdTot==triangleHairs.triBasicData.hairBegin+i)
			continue;
		int centerId=hairIdMapLocalId.find(centerIdTot)->second;
		float clumpPercent=pow(clump.hairValue[i],0.1f);
		float clumpRate=pow(clumpShape.hairValue[i],5)*10;
		vector<float> origDists(hairParameters.segment);
		for(int j=0;j<hairParameters.segment;j++)
		{
			origDists[j]=sqrt((renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3])
				*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3])
				+(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+1])
				*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+1])
				+(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+2])
				*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+2]));
		}
		for(int j=0;j<hairParameters.segment;j++)
		{
			float delta=clumpPercent*((j+1.0f)/hairParameters.segment+pow((j+1.0f)/hairParameters.segment,clumpRate)*(1-(j+1.0f)/hairParameters.segment));
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]+=delta*(clumpData.hairShapeSegment[centerId*(hairParameters.segment+3)*3]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]);
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]+=delta*(clumpData.hairShapeSegment[centerId*(hairParameters.segment+3)*3+1]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]);
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]+=delta*(clumpData.hairShapeSegment[centerId*(hairParameters.segment+3)*3+2]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]);

			float newDist=sqrt((renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3])
				*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3])
				+(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+1])
				*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+1])
				+(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+2])
				*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+2]));

			float newDelta=1-origDists[j]/newDist;
			if(newDelta<0)
				newDelta=0;
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]+=delta*(renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3]);
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]+=delta*(clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+1]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+1]);
			renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]+=delta*(clumpData.hairShapeSegment[(i*(hairParameters.segment+3)+j+1)*3+2]-renderData.hairShapeSegment[(i*(hairParameters.segment+3)+j+2)*3+2]);
		}
	}
}

