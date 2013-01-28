#include "vixo_hairStyleMaya.h"

#include <maya\MFnMesh.h>
#include <maya\MItMeshPolygon.h>

#include <maya\MPointArray.h>
#include <maya\MIntArray.h>

#include <maya\MAnimControl.h>
#include <maya\MFnNurbsCurve.h>

#include <map>

#include <vector>

#include <fstream>

#include <maya\MFnUnitAttribute.h>
#include <maya/MFnGenericAttribute.h>

#include <maya\MGlobal.h>

#include <maya\MFloatVectorArray.h>

#include <maya\MImage.h>


MTypeId vixo_hairStyleMaya::id(0x23576);

MObject vixo_hairStyleMaya::hairInfoColorDir;

//follicle info
MObject vixo_hairStyleMaya::inMesh;
MObject vixo_hairStyleMaya::hairNum;

MObject vixo_hairStyleMaya::refreshBasicInfo;
MObject vixo_hairStyleMaya::printBasicInfo;

//color info
MObject vixo_hairStyleMaya::densityMap;
MObject vixo_hairStyleMaya::tipColor;
MObject vixo_hairStyleMaya::tipColorNoiseAmp;
MObject vixo_hairStyleMaya::tipColorNoiseFreq;
MObject vixo_hairStyleMaya::baseColor;
MObject vixo_hairStyleMaya::baseColorNoiseAmp;
MObject vixo_hairStyleMaya::baseColorNoiseFreq;
MObject vixo_hairStyleMaya::tipOpacity;
MObject vixo_hairStyleMaya::tipOpacityNoiseAmp;
MObject vixo_hairStyleMaya::tipOpacityNoiseFreq;
MObject vixo_hairStyleMaya::baseOpacity;
MObject vixo_hairStyleMaya::baseOpacityNoiseAmp;
MObject vixo_hairStyleMaya::baseOpacityNoiseFreq;

MObject vixo_hairStyleMaya::refreshColorInfo;
MObject vixo_hairStyleMaya::printColorInfo;

MObject vixo_hairStyleMaya::hairInfoStyleDir;

//style info
//MObject vixo_hairStyleMaya::noiseSeed;
	
MObject vixo_hairStyleMaya::lengthNoiseAmp;
MObject vixo_hairStyleMaya::lengthNoiseFreq;
MObject vixo_hairStyleMaya::inclinationNoiseAmp;
MObject vixo_hairStyleMaya::inclinationNoiseFreq;
MObject vixo_hairStyleMaya::polarNoiseAmp;
MObject vixo_hairStyleMaya::polarNoiseFreq;
MObject vixo_hairStyleMaya::tipCurlNoiseAmp;
MObject vixo_hairStyleMaya::tipCurlNoiseFreq;
MObject vixo_hairStyleMaya::baseCurlNoiseAmp;
MObject vixo_hairStyleMaya::baseCurlNoiseFreq;

MObject vixo_hairStyleMaya::refreshStyleInfo;
MObject vixo_hairStyleMaya::printStyleInfo;


vixo_hairStyleMaya::vixo_hairStyleMaya(void)
{
}


vixo_hairStyleMaya::~vixo_hairStyleMaya(void)
{
}

void* vixo_hairStyleMaya::creator()
{
	return new vixo_hairStyleMaya();
}

MStatus vixo_hairStyleMaya::initialize()
{
	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;

	hairInfoColorDir=tAttr.create("hairInfoColorDir","hcd",MFnData::kString);

	//follicle
	inMesh=tAttr.create("inMesh","im",MFnData::kMesh);
	hairNum=nAttr.create("hairNum","hn",MFnNumericData::kInt,0);
	refreshBasicInfo=nAttr.create("refreshBasicInfo","rbi",MFnNumericData::kBoolean,0);
	printBasicInfo=nAttr.create("printBasicInfo","pbi",MFnNumericData::kInt,1);
	//follicle

	//color
	refreshColorInfo=nAttr.create("refreshColorInfo","rci",MFnNumericData::kBoolean,0);
	printColorInfo=nAttr.create("printColorInfo","pci",MFnNumericData::kInt,1);
	densityMap=nAttr.createColor("densityMap","den");
	nAttr.setDefault(1.0f,1.0f,1.0f);

	tipColor=nAttr.createColor("tipColor","tc");
	nAttr.setDefault(1.0f,1.0f,1.0f);
	tipColorNoiseAmp=nAttr.create("tipColorNoiseAmp","tca",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	tipColorNoiseFreq=nAttr.create("tipColorNoiseFreq","tcf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);

	baseColor=nAttr.createColor("baseColor","bc");
	nAttr.setDefault(1.0f,1.0f,1.0f);
	baseColorNoiseAmp=nAttr.create("baseColorNoiseAmp","bca",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	baseColorNoiseFreq=nAttr.create("baseColorNoiseFreq","bcf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);

	tipOpacity=nAttr.createColor("tipOpacity","to");
	nAttr.setDefault(1.0f,1.0f,1.0f);
	tipOpacityNoiseAmp=nAttr.create("tipOpacityNoiseAmp","toa",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	tipOpacityNoiseFreq=nAttr.create("tipOpacityNoiseFreq","tof",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);

	baseOpacity=nAttr.createColor("baseOpacity","bo");
	nAttr.setDefault(1.0f,1.0f,1.0f);
	baseOpacityNoiseAmp=nAttr.create("baseOpacityNoiseAmp","boa",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	baseOpacityNoiseFreq=nAttr.create("baseOpacityNoiseFreq","bof",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);
	//color

	hairInfoStyleDir=tAttr.create("hairInfoStyleDir","hsd",MFnData::kString);

	//style
	//noiseSeed=nAttr.create("noiseSeed","ns",MFnNumericData::kInt,1);
	
	lengthNoiseAmp=nAttr.create("lengthNoiseAmp","lna",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	lengthNoiseFreq=nAttr.create("lengthNoiseFreq","lnf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);
	inclinationNoiseAmp=nAttr.create("inclinationNoiseAmp","ina",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	inclinationNoiseFreq=nAttr.create("inclinationNoiseFreq","inf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);
	polarNoiseAmp=nAttr.create("polarNoiseAmp","pna",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	polarNoiseFreq=nAttr.create("polarNoiseFreq","pnf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);
	tipCurlNoiseAmp=nAttr.create("tipCurlNoiseAmp","tcna",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	tipCurlNoiseFreq=nAttr.create("tipCurlNoiseFreq","tcnf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);
	baseCurlNoiseAmp=nAttr.create("baseCurlNoiseAmp","bcna",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	baseCurlNoiseFreq=nAttr.create("baseCurlNoiseFreq","bcnf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);

	refreshStyleInfo=nAttr.create("refreshStyleInfo","rsi",MFnNumericData::kBoolean,0);
	printStyleInfo=nAttr.create("printStyleInfo","psi",MFnNumericData::kInt,1);
	//style


	addAttribute(hairInfoColorDir);

	addAttribute(inMesh);
	addAttribute(hairNum);
	addAttribute(refreshBasicInfo);
	addAttribute(printBasicInfo);

	addAttribute(densityMap);
	addAttribute(tipColor);
	addAttribute(tipColorNoiseAmp);
	addAttribute(tipColorNoiseFreq);
	addAttribute(baseColor);
	addAttribute(baseColorNoiseAmp);
	addAttribute(baseColorNoiseFreq);
	addAttribute(tipOpacity);
	addAttribute(tipOpacityNoiseAmp);
	addAttribute(tipOpacityNoiseFreq);
	addAttribute(baseOpacity);
	addAttribute(baseOpacityNoiseAmp);
	addAttribute(baseOpacityNoiseFreq);
	addAttribute(refreshColorInfo);
	addAttribute(printColorInfo);

	addAttribute(hairInfoStyleDir);

	addAttribute(lengthNoiseAmp);
	addAttribute(lengthNoiseFreq);
	addAttribute(inclinationNoiseAmp);
	addAttribute(inclinationNoiseFreq);
	addAttribute(polarNoiseAmp);
	addAttribute(polarNoiseFreq);
	addAttribute(tipCurlNoiseAmp);
	addAttribute(tipCurlNoiseFreq);
	addAttribute(baseCurlNoiseAmp);
	addAttribute(baseCurlNoiseFreq);

	addAttribute(refreshStyleInfo);
	addAttribute(printStyleInfo);

	//attributeAffects(inMesh,printBasicInfo);
	//attributeAffects(hairNum,printBasicInfo);
	//attributeAffects(follicleFileName,printBasicInfo);
	attributeAffects(inMesh,printBasicInfo);
	attributeAffects(hairNum,printBasicInfo);
	attributeAffects(refreshBasicInfo,printBasicInfo);
	attributeAffects(refreshColorInfo,printColorInfo);
	attributeAffects(refreshStyleInfo,printStyleInfo);

	return MS::kSuccess;
}

void vixo_hairStyleMaya::getTextureInfo(MString attrName,vector<float>& uvs,vector<follicleColor>& res,MDataBlock& data)
{
	MPlug plug(thisMObject(),this->densityMap);
	if(attrName=="tipColor")
		plug.setAttribute(tipColor);
	else if(attrName=="baseColor")
		plug.setAttribute(baseColor);
	else if(attrName=="tipOpacity")
		plug.setAttribute(tipOpacity);
	else if(attrName=="baseOpacity")
		plug.setAttribute(baseOpacity);

	MPlugArray arr;
	plug.connectedTo(arr,true,false);

	if(arr.length()!=1)
	{
		MFloatVector color=data.inputValue(plug).asFloatVector();
		if(attrName=="densityMap")
			for(int i=0;i<uvs.size()/2;i++)
				res[i].density=color.x;
		else if(attrName=="tipOpacity")
		{
			float noiseAmp=data.inputValue(tipOpacityNoiseAmp).asFloat();
			float noiseFreq=data.inputValue(tipOpacityNoiseFreq).asFloat();
			for(int i=0;i<uvs.size()/2;i++)
			{
				float ton=noise::atPointUV(uvs[2*i]*noiseFreq,uvs[2*i+1]*noiseFreq)*2-1;
				res[i].tipOpa=min(max(color.x+ton*noiseAmp,0.0),1.0);
			}
		}
		else if(attrName=="baseOpacity")
		{
			float noiseAmp=data.inputValue(baseOpacityNoiseAmp).asFloat();
			float noiseFreq=data.inputValue(baseOpacityNoiseFreq).asFloat();
			for(int i=0;i<uvs.size()/2;i++)
			{
				float bon=noise::atPointUV(uvs[2*i]*noiseFreq,uvs[2*i+1]*noiseFreq)*2-1;
				res[i].baseOpa=min(max(color.x+bon*noiseAmp,0.0),1.0);
			}
		}
		else if(attrName=="baseColor")
		{
			float noiseAmp=data.inputValue(baseColorNoiseAmp).asFloat();
			float noiseFreq=data.inputValue(baseColorNoiseFreq).asFloat();
			for(int i=0;i<uvs.size()/2;i++)
			{
				pnt bcn=noise::atPointUVColor(uvs[2*i]*noiseFreq,uvs[2*i+1]*noiseFreq);
				bcn.x=(bcn.x*2-1)*noiseAmp;
				bcn.y=(bcn.y*2-1)*noiseAmp;
				bcn.z=(bcn.z*2-1)*noiseAmp;

				res[i].baseColorR=min(max(color.x+bcn.x,0.0),1.0);
				res[i].baseColorG=min(max(color.y+bcn.y,0.0),1.0);
				res[i].baseColorB=min(max(color.z+bcn.z,0.0),1.0);
			}
		}
		else if(attrName=="tipColor")
		{
			float noiseAmp=data.inputValue(tipColorNoiseAmp).asFloat();
			float noiseFreq=data.inputValue(tipColorNoiseFreq).asFloat();
			for(int i=0;i<uvs.size()/2;i++)
			{
				pnt tcn=noise::atPointUVColor(uvs[2*i]*noiseFreq,uvs[2*i+1]*noiseFreq);
				tcn.x=(tcn.x*2-1)*noiseAmp;
				tcn.y=(tcn.y*2-1)*noiseAmp;
				tcn.z=(tcn.z*2-1)*noiseAmp;
				res[i].tipColorR=min(max(color.x+tcn.x,0.0),1.0);
				res[i].tipColorG=min(max(color.y+tcn.y,0.0),1.0);
				res[i].tipColorB=min(max(color.z+tcn.z,0.0),1.0);
			}
		}
		return;
	}

	MFnDependencyNode dgFn(arr[0].node());
	MPlug fnFile=dgFn.findPlug("fileTextureName");
	if(fnFile.asString()=="")
	{
		MFloatVector color=data.inputValue(plug).asFloatVector();
		if(attrName=="densityMap")
			for(int i=0;i<uvs.size()/2;i++)
				res[i].density=color.x;
		else if(attrName=="tipOpacity")
		{
			float noiseAmp=data.inputValue(tipOpacityNoiseAmp).asFloat();
			float noiseFreq=data.inputValue(tipOpacityNoiseFreq).asFloat();
			for(int i=0;i<uvs.size()/2;i++)
			{
				float ton=noise::atPointUV(uvs[2*i]*noiseFreq,uvs[2*i+1]*noiseFreq)*2-1;
				res[i].tipOpa=min(max(color.x+ton*noiseAmp,0.0),1.0);
			}
		}
		else if(attrName=="baseOpacity")
		{
			float noiseAmp=data.inputValue(baseOpacityNoiseAmp).asFloat();
			float noiseFreq=data.inputValue(baseOpacityNoiseFreq).asFloat();
			for(int i=0;i<uvs.size()/2;i++)
			{
				float bon=noise::atPointUV(uvs[2*i]*noiseFreq,uvs[2*i+1]*noiseFreq)*2-1;
				res[i].baseOpa=min(max(color.x+bon*noiseAmp,0.0),1.0);
			}
		}
		else if(attrName=="baseColor")
		{
			float noiseAmp=data.inputValue(baseColorNoiseAmp).asFloat();
			float noiseFreq=data.inputValue(baseColorNoiseFreq).asFloat();
			for(int i=0;i<uvs.size()/2;i++)
			{
				pnt bcn=noise::atPointUVColor(uvs[2*i]*noiseFreq,uvs[2*i+1]*noiseFreq);
				bcn.x=(bcn.x*2-1)*noiseAmp;
				bcn.y=(bcn.y*2-1)*noiseAmp;
				bcn.z=(bcn.z*2-1)*noiseAmp;

				res[i].baseColorR=min(max(color.x+bcn.x,0.0),1.0);
				res[i].baseColorG=min(max(color.y+bcn.y,0.0),1.0);
				res[i].baseColorB=min(max(color.z+bcn.z,0.0),1.0);
			}
		}
		else if(attrName=="tipColor")
		{
			float noiseAmp=data.inputValue(tipColorNoiseAmp).asFloat();
			float noiseFreq=data.inputValue(tipColorNoiseFreq).asFloat();
			for(int i=0;i<uvs.size()/2;i++)
			{
				pnt tcn=noise::atPointUVColor(uvs[2*i]*noiseFreq,uvs[2*i+1]*noiseFreq);
				tcn.x=(tcn.x*2-1)*noiseAmp;
				tcn.y=(tcn.y*2-1)*noiseAmp;
				tcn.z=(tcn.z*2-1)*noiseAmp;
				res[i].tipColorR=min(max(color.x+tcn.x,0.0),1.0);
				res[i].tipColorG=min(max(color.y+tcn.y,0.0),1.0);
				res[i].tipColorB=min(max(color.z+tcn.z,0.0),1.0);
			}
		}
		return;
	}

	MImage imgFn;
	imgFn.readFromTextureNode(arr[0].node());

	unsigned int imageWidth,imageHeight,imageDepth;
	imgFn.getSize(imageWidth,imageHeight);
	imageDepth=4;

	unsigned char * pixels=imgFn.pixels();

	if(attrName=="densityMap")
		for(int i=0;i<uvs.size()/2;i++)
		{
			int pixelW=uvs[2*i]*imageWidth;
			int pixelH=uvs[2*i+1]*imageHeight;
			res[i].density=((unsigned int)pixels[imageDepth*(pixelH*imageWidth+pixelW)])/255.0;
		}
	else if(attrName=="tipOpacity")
	{
		float noiseAmp=data.inputValue(tipOpacityNoiseAmp).asFloat();
		float noiseFreq=data.inputValue(tipOpacityNoiseFreq).asFloat();
		for(int i=0;i<uvs.size()/2;i++)
		{
			float ton=noise::atPointUV(uvs[2*i]*noiseFreq,uvs[2*i+1]*noiseFreq)*2-1;
			int pixelW=uvs[2*i]*imageWidth;
			int pixelH=uvs[2*i+1]*imageHeight;
			res[i].tipOpa=((unsigned int)pixels[imageDepth*(pixelH*imageWidth+pixelW)])/255.0;
			res[i].tipOpa=min(max(res[i].tipOpa+ton*noiseAmp,0.0),1.0);
		}
	}
	else if(attrName=="baseOpacity")
	{
		float noiseAmp=data.inputValue(baseOpacityNoiseAmp).asFloat();
		float noiseFreq=data.inputValue(baseOpacityNoiseFreq).asFloat();
		for(int i=0;i<uvs.size()/2;i++)
		{
			float bon=noise::atPointUV(uvs[2*i]*noiseFreq,uvs[2*i+1]*noiseFreq)*2-1;
			int pixelW=uvs[2*i]*imageWidth;
			int pixelH=uvs[2*i+1]*imageHeight;
			res[i].baseOpa=((unsigned int)pixels[imageDepth*(pixelH*imageWidth+pixelW)])/255.0;
			res[i].baseOpa=min(max(res[i].baseOpa+bon*noiseAmp,0.0),1.0);
		}
	}
	else if(attrName=="baseColor")
	{
		float noiseAmp=data.inputValue(baseColorNoiseAmp).asFloat();
		float noiseFreq=data.inputValue(baseColorNoiseFreq).asFloat();
		for(int i=0;i<uvs.size()/2;i++)
		{
			pnt bcn=noise::atPointUVColor(uvs[2*i]*noiseFreq,uvs[2*i+1]*noiseFreq);
			bcn.x=(bcn.x*2-1)*noiseAmp;
			bcn.y=(bcn.y*2-1)*noiseAmp;
			bcn.z=(bcn.z*2-1)*noiseAmp;

			int pixelW=uvs[2*i]*imageWidth;
			int pixelH=uvs[2*i+1]*imageHeight;
			res[i].baseColorR=((unsigned int)pixels[imageDepth*(pixelH*imageWidth+pixelW)])/255.0;
			res[i].baseColorG=((unsigned int)pixels[imageDepth*(pixelH*imageWidth+pixelW)+1])/255.0;
			res[i].baseColorB=((unsigned int)pixels[imageDepth*(pixelH*imageWidth+pixelW)+2])/255.0;

			res[i].baseColorR=min(max(res[i].baseColorR+bcn.x,0.0),1.0);
			res[i].baseColorG=min(max(res[i].baseColorG+bcn.y,0.0),1.0);
			res[i].baseColorB=min(max(res[i].baseColorB+bcn.z,0.0),1.0);
		}
	}
	else if(attrName=="tipColor")
	{
		float noiseAmp=data.inputValue(tipColorNoiseAmp).asFloat();
		float noiseFreq=data.inputValue(tipColorNoiseFreq).asFloat();
		for(int i=0;i<uvs.size()/2;i++)
		{
			pnt tcn=noise::atPointUVColor(uvs[2*i]*noiseFreq,uvs[2*i+1]*noiseFreq);
			tcn.x=(tcn.x*2-1)*noiseAmp;
			tcn.y=(tcn.y*2-1)*noiseAmp;
			tcn.z=(tcn.z*2-1)*noiseAmp;
			
			int pixelW=uvs[2*i]*imageWidth;
			int pixelH=uvs[2*i+1]*imageHeight;
			res[i].tipColorR=((unsigned int)pixels[imageDepth*(pixelH*imageWidth+pixelW)])/255.0;
			res[i].tipColorG=((unsigned int)pixels[imageDepth*(pixelH*imageWidth+pixelW)+1])/255.0;
			res[i].tipColorB=((unsigned int)pixels[imageDepth*(pixelH*imageWidth+pixelW)+2])/255.0;

			res[i].tipColorR=min(max(res[i].tipColorR+tcn.x,0.0),1.0);
			res[i].tipColorG=min(max(res[i].tipColorG+tcn.y,0.0),1.0);
			res[i].tipColorB=min(max(res[i].tipColorB+tcn.z,0.0),1.0);
		}
	}
}

MStatus vixo_hairStyleMaya::compute(const MPlug& plug, MDataBlock& data)
{
	if(plug==printBasicInfo)
	{
		if(data.inputValue(hairNum).asInt()==0)
		{
			return MS::kSuccess;
		}
		if(data.inputValue(inMesh).asMesh().hasFn(MFn::kMesh)==false)
		{
			return MS::kSuccess;
		}
		if(data.inputValue(hairInfoColorDir).asString()=="")
		{
			return MS::kSuccess;
		}
		MString hairInfoDirValue=data.inputValue(hairInfoColorDir).asString();
		int hairNumValue=data.inputValue(hairNum).asInt();
		//cout<<"hair"<<hairNumValue<<endl;
		MString follicleFileName=hairInfoDirValue+"/hairRoot.info";
		MString uvFileName=hairInfoDirValue+"/uv.info";
		exportFollicle(hairNumValue,uvFileName,follicleFileName,data);

		data.outputValue(printBasicInfo).setClean();

		return MS::kSuccess;
	}
	if(plug==printColorInfo)
	{
		if(data.inputValue(refreshColorInfo).asBool()==false)
			return MS::kSuccess;
		MString hairInfoDirValue=data.inputValue(hairInfoColorDir).asString();
		
		MString uvFileName=hairInfoDirValue+"/uv.info";
		fstream fileExist(uvFileName.asChar(),ios_base::in|ios_base::binary);
		if(!fileExist)
		{
			data.outputValue(refreshColorInfo).set(false);
			return MS::kSuccess;
		}
		MString colorInfoFileName=hairInfoDirValue+"/hairColor.info";
		exportColor(uvFileName,colorInfoFileName,data);

		data.outputValue(refreshColorInfo).set(false);
		data.outputValue(printColorInfo).setClean();

		return MS::kSuccess;
	}
	if(plug==printStyleInfo)
	{
		if(data.inputValue(refreshStyleInfo).asBool()==false)
			return MS::kSuccess;
		//follicle未导出 
		MString hairInfoColorDirValue=data.inputValue(hairInfoColorDir).asString();
		MString hairInfoStyleDirValue=data.inputValue(hairInfoStyleDir).asString();

		if(hairInfoStyleDirValue=="")
		{
			data.outputValue(refreshStyleInfo).set(false);
			return MS::kSuccess;
		}

		MString uvFileName=hairInfoColorDirValue+"/uv.info";
		fstream fileExist(uvFileName.asChar(),ios_base::in|ios_base::binary);
		if(!fileExist)
			return MS::kSuccess;
		MString noiseInfoFileName=hairInfoStyleDirValue+"/hairStyle.info";
		exportNoise(uvFileName,noiseInfoFileName,data);

		data.outputValue(refreshStyleInfo).set(false);
		data.outputValue(printStyleInfo).setClean();
		return MS::kSuccess;
	}
	return MS::kSuccess;
}

void vixo_hairStyleMaya::exportFollicle(int hairNumValue,MString uvFileName,MString hairRootInfo,MDataBlock& data)
{
	MObject meshObj=data.inputValue(inMesh).asMesh();
		
	MItMeshPolygon iterFace(meshObj);
	double area=0.0;
	int triTotNum=0;
	for(iterFace.reset();!iterFace.isDone();iterFace.next())
	{
		double areaTmp;
		iterFace.getArea(areaTmp);
		area+=areaTmp;
		int triTotNumTmp;
		iterFace.numTriangles(triTotNumTmp);
		triTotNum+=triTotNumTmp;

		if(iterFace.isDone())
			break;
	}
	MFnMesh fnMesh(meshObj);

	vector<vector<float>> uv(triTotNum);
	vector<int> triInfoIndex(triTotNum,0); //numFollicle
	vector<streampos> triInfoPosIndex(triTotNum);
	vector<int> vertexCtrl(triTotNum*3);
	vector<vector<float>> vertexWeight(triTotNum);
	
	double diff=sqrt(area/hairNumValue);

	int hairIter=0;
	int triIter=0;
	for(iterFace.reset();!iterFace.isDone();iterFace.next())
	{
		MIntArray faceVertexIndex;
		iterFace.getVertices(faceVertexIndex);

		MVectorArray faceNormals;
		iterFace.getNormals(faceNormals,MSpace::kWorld);

		MFloatVectorArray faceTangents;
		fnMesh.getFaceVertexTangents(iterFace.index(),faceTangents,MSpace::kWorld);

		map<int,int> vertexIndex2faceIndex;
		for(int i=0;i<faceVertexIndex.length();i++)
		{
			vertexIndex2faceIndex.insert(pair<int,int>(faceVertexIndex[i],i));
		}
		int triNum;
		iterFace.numTriangles(triNum);
		MString uvset("map1");
		for(int tri=0;tri<triNum;tri++)
		{
			MPointArray vertex;
			MIntArray vertexIndex;
			iterFace.getTriangle(tri,vertex,vertexIndex);
			int numLine1=MVector(vertex[1]-vertex[0]).length()/diff;
			int numLine2=MVector(vertex[2]-vertex[0]).length()/diff;

			MFloatArray uvs(6);
			float2 tempUV;
			iterFace.getUV(vertexIndex2faceIndex.find(vertexIndex[0])->second,tempUV,&uvset);
			uvs[0]=tempUV[0];
			uvs[1]=tempUV[1];
			iterFace.getUV(vertexIndex2faceIndex.find(vertexIndex[1])->second,tempUV,&uvset);
			uvs[2]=tempUV[0];
			uvs[3]=tempUV[1];
			iterFace.getUV(vertexIndex2faceIndex.find(vertexIndex[2])->second,tempUV,&uvset);
			uvs[4]=tempUV[0];
			uvs[5]=tempUV[1];

			vertexCtrl[3*triIter]=vertexIndex[1];
			vertexCtrl[3*triIter+1]=vertexIndex[2];
			vertexCtrl[3*triIter+2]=vertexIndex[0];

			

			for(int i=0;i<numLine1;i++)
			{
				for(int j=0;j<(1-(float)i/numLine1)*numLine2;j++)
				{
					vertexWeight[triIter].push_back((float)i/numLine1);
					vertexWeight[triIter].push_back((float)j/numLine2);
					vertexWeight[triIter].push_back(1-(float)i/numLine1-(float)j/numLine2);

					int localHairIter=triInfoIndex[triIter];

					uv[triIter].push_back(uvs[0]*vertexWeight[triIter][3*localHairIter+2]+uvs[2]*vertexWeight[triIter][3*localHairIter]+uvs[4]*vertexWeight[triIter][3*localHairIter+1]);
					uv[triIter].push_back(uvs[1]*vertexWeight[triIter][3*localHairIter+2]+uvs[3]*vertexWeight[triIter][3*localHairIter]+uvs[5]*vertexWeight[triIter][3*localHairIter+1]);
					
					hairIter++;
					triInfoIndex[triIter]++;
				}
			}

			triIter++;
		}
		if(iterFace.isDone())
			break;
	}

	//print uv
	fstream fout(uvFileName.asChar(),ios_base::out|ios_base::binary);
	fout.write((char*)&triTotNum,sizeof(int));
	fout.write((char*)&triInfoIndex[0],sizeof(int)*triTotNum);
	fout.write((char*)&vertexCtrl[0],sizeof(int)*3*triTotNum); //for cache export
	for(int i=0;i<triTotNum;i++) //for color info
	{
		fout.write((char*)&uv[i][0],sizeof(float)*2*triInfoIndex[i]);
	}
	fout.flush();
	fout.close();
	//~print uv

	/*
	//debug uv
	MString uvDebug=uvFileName+"debug";
	fout.open(uvDebug.asChar(),ios_base::out);
	fout<<triTotNum<<endl;
	for(int i=0;i<triTotNum;i++)
		fout<<"tri"<<i<<":"<<'\t'<<triInfoIndex[i]<<'\t'<<vertexCtrl[3*i]<<'\t'<<vertexCtrl[3*i+1]<<'\t'<<vertexCtrl[3*i+2]<<endl;
	for(int i=0;i<triTotNum;i++)
		for(int j=0;j<triInfoIndex[i];j++)
			fout<<"tri"<<i<<":"<<'\t'<<j<<'\t'<<uv[i][2*j]<<'\t'<<uv[i][2*j+1]<<endl;
	fout.flush();
	fout.close();
	//~debug uv
	*/
	//prepare streampos
	triInfoPosIndex[0]=sizeof(int)+sizeof(streampos)*triTotNum;
	for(int i=1;i<triTotNum;i++)
	{
		triInfoPosIndex[i]=triInfoPosIndex[i-1].operator+(sizeof(int)+sizeof(float)*3*triInfoIndex[i-1]);
	}
	//~prepare streampos
	
	//print infos
	fout.open(hairRootInfo.asChar(),ios_base::out|ios_base::binary);
	fout.write((char*)&triTotNum,sizeof(int));
	fout.write((char*)&triInfoPosIndex[0],sizeof(streampos)*triTotNum);
	for(int i=0;i<triTotNum;i++) //for color info
	{
		fout.write((char*)&triInfoIndex[i],sizeof(int));
		fout.write((char*)&vertexWeight[i][0],sizeof(float)*3*triInfoIndex[i]);
	}
	fout.flush();
	fout.close();
	//~print infos
	/*
	//debug info
	MString hairRootInfoDebug=hairRootInfo+"debug";
	fout.open(hairRootInfoDebug.asChar(),ios_base::out);
	fout<<triTotNum<<endl;
	for(int i=0;i<triTotNum;i++)
		fout<<"tri"<<i<<":"<<'\t'<<triInfoIndex[i]<<'\t'<<triInfoPosIndex[i]<<endl;
	for(int i=0;i<triTotNum;i++)
		for(int j=0;j<triInfoIndex[i];j++)
			fout<<"tri"<<i<<":"<<'\t'<<j<<'\t'<<vertexWeight[i][3*j]<<'\t'<<vertexWeight[i][3*j+1]<<'\t'<<vertexWeight[i][3*j+2]<<endl;
	fout.flush();
	fout.close();
	//~debug info
	*/
}

void vixo_hairStyleMaya::exportColor(MString uvInfo,MString colorInfo,MDataBlock& data)
{
	//getUVinfo
	ifstream fin(uvInfo.asChar(),ios_base::in|ios_base::binary);

	int triNum;
	fin.read((char*)&triNum,sizeof(int));

	vector<int> hairNumPerTri(triNum);
	fin.read((char*)&hairNumPerTri[0],sizeof(int)*triNum);

	vector<int> ctrlVert(triNum*3);
	fin.read((char*)&ctrlVert[0],sizeof(int)*triNum*3);

	vector<int> hairStartIdxPerTri(triNum,0);
	int hairNum=0;
	for(int i=1;i<triNum;i++)
	{
		hairStartIdxPerTri[i]=hairStartIdxPerTri[i-1]+hairNumPerTri[i-1];
	}
	hairNum=hairStartIdxPerTri[triNum-1]+hairNumPerTri[triNum-1];

	vector<float> uvs(hairNum*2);
	fin.read((char*)&uvs[0],sizeof(float)*hairNum*2);
	fin.close();


	//getTextureInfo
	vector<follicleColor> follicleInfo(hairNum);
	getTextureInfo("densityMap",uvs,follicleInfo,data);
	getTextureInfo("tipColor",uvs,follicleInfo,data);
	getTextureInfo("baseColor",uvs,follicleInfo,data);
	getTextureInfo("tipOpacity",uvs,follicleInfo,data);
	getTextureInfo("baseOpacity",uvs,follicleInfo,data);

	vector<streampos> startPos(triNum);
	startPos[0]=sizeof(int)+sizeof(streampos)*triNum;
	for(int i=1;i<triNum;i++)
	{
		startPos[i]=startPos[i-1].operator+(sizeof(int)+sizeof(follicleColor)*hairNumPerTri[i-1]);
	}

	//输出
	fstream fout(colorInfo.asChar(),ios_base::out|ios_base::trunc|ios_base::binary);
	fout.write((char*)&triNum,sizeof(int));
	fout.write((char*)&startPos[0],sizeof(streampos)*triNum);
	for(int i=0;i<triNum;i++)
	{
		fout.write((char*)&hairNumPerTri[i],sizeof(int));
		fout.write((char*)&follicleInfo[hairStartIdxPerTri[i]],sizeof(follicleColor)*hairNumPerTri[i]);
	}
	fout.flush();
	fout.close();

	//输出
	/*
	colorInfo=colorInfo+"debug";
	fout.open(colorInfo.asChar(),ios_base::out|ios_base::trunc);
	fout<<triNum<<endl;
	for(int i=0;i<triNum;i++)
	{
		fout<<i<<":"<<startPos[i]<<'\t'<<hairNumPerTri[i]<<endl;
		for(int j=0;j<hairNumPerTri[i];j++)
		{
			fout<<'\t'<<j<<":"<<follicleInfo[hairStartIdxPerTri[i]+j].density<<'\t'<<follicleInfo[hairStartIdxPerTri[i]+j].tipColorB<<endl;
		}
	}
	fout.flush();
	fout.close();
	*/
}

void vixo_hairStyleMaya::exportNoise(MString uvInfo,MString noiseInfo,MDataBlock& data)
{
	//getUVinfo
	ifstream fin(uvInfo.asChar(),ios_base::in|ios_base::binary);

	int triNum;
	fin.read((char*)&triNum,sizeof(int));

	vector<int> hairNumPerTri(triNum);
	fin.read((char*)&hairNumPerTri[0],sizeof(int)*triNum);

	vector<int> ctrlVert(triNum*3);
	fin.read((char*)&ctrlVert[0],sizeof(int)*triNum*3);

	vector<int> hairStartIdxPerTri(triNum,0);
	int hairNum=0;
	for(int i=1;i<triNum;i++)
	{
		hairStartIdxPerTri[i]=hairStartIdxPerTri[i-1]+hairNumPerTri[i-1];
	}
	hairNum=hairStartIdxPerTri[triNum-1]+hairNumPerTri[triNum-1];

	vector<float> uvs(hairNum*2);
	fin.read((char*)&uvs[0],sizeof(float)*hairNum*2);
	fin.close();

	//get noise data
	float lengthNoiseAmpValue=data.inputValue(lengthNoiseAmp).asFloat();
	float lengthNoiseFreqValue=data.inputValue(lengthNoiseFreq).asFloat();
	float inclinationNoiseAmpValue=data.inputValue(inclinationNoiseAmp).asFloat();
	float inclinationNoiseFreqValue=data.inputValue(inclinationNoiseFreq).asFloat();
	float polarNoiseAmpValue=data.inputValue(polarNoiseAmp).asFloat();
	float polarNoiseFreqValue=data.inputValue(polarNoiseFreq).asFloat();
	float tipCurlNoiseAmpValue=data.inputValue(tipCurlNoiseAmp).asFloat();
	float tipCurlNoiseFreqValue=data.inputValue(tipCurlNoiseFreq).asFloat();
	float baseCurlNoiseAmpValue=data.inputValue(baseCurlNoiseAmp).asFloat();
	float baseCurlNoiseFreqValue=data.inputValue(baseCurlNoiseFreq).asFloat();
	//~get noise data
	const float PI=3.1415926;
	vector<follicleNoise> hairNoise(hairNum);
	for(int i=0;i<hairNum;i++)
	{
		float lenn=noise::atPointUV(uvs[2*i]*lengthNoiseFreqValue,uvs[2*i+1]*lengthNoiseFreqValue);
		hairNoise[i].length=(lenn*2-1)*lengthNoiseAmpValue+1;
		hairNoise[i].inclination=(noise::atPointUV(uvs[2*i]*inclinationNoiseFreqValue,uvs[2*i+1]*inclinationNoiseFreqValue)*2-1)*lengthNoiseAmpValue*PI/2;
		hairNoise[i].polar=(noise::atPointUV(uvs[2*i]*polarNoiseFreqValue,uvs[2*i+1]*polarNoiseFreqValue)*2-1)*polarNoiseAmpValue*PI;
		hairNoise[i].tipCurl=(noise::atPointUV(uvs[2*i]*tipCurlNoiseFreqValue,uvs[2*i+1]*tipCurlNoiseFreqValue)*2-1)*tipCurlNoiseAmpValue*PI/6;
		hairNoise[i].baseCurl=(noise::atPointUV(uvs[2*i]*baseCurlNoiseFreqValue,uvs[2*i+1]*baseCurlNoiseFreqValue)*2-1)*baseCurlNoiseAmpValue*PI/6;
	}

	vector<streampos> startPos(triNum);
	startPos[0]=sizeof(int)+sizeof(streampos)*triNum;
	for(int i=1;i<triNum;i++)
	{
		startPos[i]=startPos[i-1].operator+(sizeof(int)+sizeof(follicleNoise)*hairNumPerTri[i-1]);
	}

	//print noise info
	fstream fout(noiseInfo.asChar(),ios_base::out|ios_base::trunc|ios_base::binary);
	fout.write((char*)&triNum,sizeof(int));
	fout.write((char*)&startPos[0],sizeof(streampos)*triNum);
	for(int i=0;i<triNum;i++)
	{
		fout.write((char*)&hairNumPerTri[i],sizeof(int));
		fout.write((char*)&hairNoise[hairStartIdxPerTri[i]],sizeof(follicleNoise)*hairNumPerTri[i]);
	}
	fout.flush();
	fout.close();
	//~print noise info

	//debug
	/*
	noiseInfo=noiseInfo+"debug";
	fout.open(noiseInfo.asChar(),ios_base::out|ios_base::trunc);
	fout<<triNum<<endl;
	for(int i=0;i<triNum;i++)
	{
		fout<<i<<":"<<startPos[i]<<'\t'<<hairNumPerTri[i]<<endl;
		for(int j=0;j<hairNumPerTri[i];j++)
		{
			fout<<'\t'<<j<<":"<<hairNoise[hairStartIdxPerTri[i]+j].length
				<<'\t'<<hairNoise[hairStartIdxPerTri[i]+j].inclination
				<<'\t'<<hairNoise[hairStartIdxPerTri[i]+j].polar
				<<'\t'<<hairNoise[hairStartIdxPerTri[i]+j].tipCurl
				<<'\t'<<hairNoise[hairStartIdxPerTri[i]+j].baseCurl<<endl;
		}
	}
	fout.flush();
	fout.close();
	*/
	//~debug
}
