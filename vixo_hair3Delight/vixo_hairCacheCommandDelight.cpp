#include "vixo_hairCacheCommandDelight.h"

#include <maya/MTime.h>
#include <maya/MSelectionList.h>
#include <maya/MDagPath.h>
#include <maya/MStringArray.h>
#include <maya/MGlobal.h>
#include <maya/MPlugArray.h>
MStringArray vixo_hairCacheCommandDelight::full;
MIntArray vixo_hairCacheCommandDelight::currentTime;
vixo_hairCacheCommandDelight::vixo_hairCacheCommandDelight(void)
{
}


vixo_hairCacheCommandDelight::~vixo_hairCacheCommandDelight(void)
{
}

void * vixo_hairCacheCommandDelight::creator()
{
	return new vixo_hairCacheCommandDelight;
}

MSyntax vixo_hairCacheCommandDelight::newSyntax()
{
	MSyntax syntax;
	syntax.addFlag("-st", "-sampleTime", MSyntax::kDouble);
	syntax.addFlag("-a", "-addstep");
	syntax.addFlag("-r", "-remove");
	syntax.addFlag("-e", "-emit");
	syntax.addFlag("-f", "-flush");
	syntax.addFlag("-c", "-contains");
	syntax.addFlag("-l", "-list");
	syntax.setObjectType(MSyntax::kSelectionList, 0, 1);
	return syntax;
}

MStatus vixo_hairCacheCommandDelight::doIt(const MArgList& args)
{
	for(int i=0;i<args.length();i++)
	{
		MString temp;
		args.get(i,temp);
	}
	//return MS::kSuccess;
	MArgDatabase argData(this->syntax(),args);
	if(argData.isFlagSet("-st"))
	{
		MSelectionList slist;
		argData.getObjects(slist);
		if(slist.length()>0)
		{
			//cout<<"apple:"<<selectList[0];
			//full.append(selectList[0]);
			MDagPath path;
			slist.getDagPath(0,path);
			full.append(path.fullPathName());
			MString temp;
			args.get(2,temp);
			currentTime.append(temp.asInt());
		}
	}
	if(argData.isFlagSet("-l"))
	{
		if(full.length()>0)
		{
			for(int i=0;i<full.length();i++)
			{
				//RiArchiveRecord -m "verbatim" -t "Procedural "DynamicLoad" ["vixo_hairRenderDSO.dll" "21"] [-3 3 -3 3 -3 3]
				//	"
				MGlobal::executeCommand("RiArchiveBegin -n "+full[i]);
				MString paraFile=printHairInfo(full[i],currentTime[i]);
				MString temp;
				temp.set(-1);
				paraFile=paraFile+" "+temp;
				//MGlobal::executeCommand("RiArchiveBegin -n "+full[i]);
				//MString sp = "Procedural \"DynamicLoad\" [\"vixo_hairRenderDSO.dll\" \"21\"] [-3 3 -3 3 -3 3]\n";
				//MString sp = "Procedural \\\"DynamicLoad\\\" [\\\"vixo_hairRenderDSO\\\" \\\""+paraFile+"\\\"] [-3 3 -3 3 -3 3]\\n";
				//MString asp = "RiArchiveRecord -m \"verbatim\" -t \""+sp+"\"";
				//cout<<asp.asChar()<<endl;
				//MString sp = "RiProcedural -l \"vixo_hairRenderDSO\" -b -10 10 -10 10 -10 10 -p \""+paraFile+"\" ";
				//MString sp = "Procedural \"DynamicLoad\" [\"vixo_hairRenderDSO\" \""+paraFile+"\"] [-3 3 -3 3 -3 3]";
				//MGlobal::executeCommand(sp);
				MGlobal::executeCommand("RiArchiveEnd");
			}
		}
	}
	if(argData.isFlagSet("-f"))
	{
		full.clear();
		currentTime.clear();
	}
	return MS::kSuccess;
}

MString vixo_hairCacheCommandDelight::printHairInfo(MString node,int frame)
{
	MSelectionList slist;
	MGlobal::getSelectionListByName(node,slist);
	MDagPath dagPath;
	slist.getDagPath(0,dagPath);

	hairParametersInfo hairPara;

	MFnDagNode fnDagNode(dagPath);
	strcpy(hairPara.hairRootFile,fnDagNode.findPlug("hairRootFileName").asString().asChar());

	MString frameStr;
	frameStr.set(frame);
	if(fnDagNode.findPlug("renderUseCache").asBool()==false)
		MGlobal::executeCommand("vixo_hairCacheExport -vhn \""+node+"\" -sf "+frameStr+" -ef "+frameStr);

	MString hairCache=fnDagNode.findPlug("hairCacheFileName").asString();
	if(frameStr.length()==1)
		frameStr="000"+frameStr;
	else if(frameStr.length()==2)
		frameStr="00"+frameStr;
	else if(frameStr.length()==3)
		frameStr="0"+frameStr;
	hairCache=hairCache+"."+frameStr+".hcc";
	strcpy(hairPara.hairCacheFile,hairCache.asChar());

	strcpy(hairPara.hairClumpFile,fnDagNode.findPlug("clumpInfoFileName").asString().asChar());

	hairPara.triBoxExpand=fnDagNode.findPlug("miscBoundBoxExpand").asFloat();
	hairPara.segment=fnDagNode.findPlug("segment").asInt();

	MString densityTex;
	vec3 temp;
	getTexAndValue(fnDagNode,"densityMap",densityTex,temp);
	strcpy(hairPara.hairDensityFile,densityTex.asChar());
	//cout<<"aaple"<<densityTex.asChar()<<endl;

	MString tipColorTex;
	getTexAndValue(fnDagNode,"tipColor",tipColorTex,hairPara.tipColorBaseValue);
	strcpy(hairPara.tipColorTexture,tipColorTex.asChar());
	//hairPara.tipColorTexture={};
	//hairPara.tipColorBaseValue=vec3(1,0,0);
	hairPara.tipColorNoiseAmp=fnDagNode.findPlug("tipColorNoiseAmp").asFloat();
	hairPara.tipColorNoiseFreq=fnDagNode.findPlug("tipColorNoiseFreq").asFloat();

	MString baseColorTex;
	getTexAndValue(fnDagNode,"baseColor",baseColorTex,hairPara.baseColorBaseValue);
	strcpy(hairPara.baseColorTexture,baseColorTex.asChar());
	//hairPara.baseColorBaseValue=vec3(0.5,0,0);
	hairPara.baseColorNoiseAmp=fnDagNode.findPlug("baseColorNoiseAmp").asFloat();
	hairPara.baseColorNoiseFreq=fnDagNode.findPlug("baseColorNoiseFreq").asFloat();

	MString tipOpacityTex;
	vec3 tipOpacityVal;
	getTexAndValue(fnDagNode,"tipOpacity",tipOpacityTex,tipOpacityVal);
	hairPara.tipOpaBaseValue=tipOpacityVal.x;
	strcpy(hairPara.tipOpaTexture,tipOpacityTex.asChar());
	hairPara.tipOpaNoiseAmp=fnDagNode.findPlug("tipOpacityNoiseAmp").asFloat();
	hairPara.tipOpaNoiseFreq=fnDagNode.findPlug("tipOpacityNoiseFreq").asFloat();

	MString baseOpacityTex;
	vec3 baseOpacityVal;
	getTexAndValue(fnDagNode,"baseOpacity",baseOpacityTex,baseOpacityVal);
	hairPara.baseOpaBaseValue=baseOpacityVal.x;
	strcpy(hairPara.baseOpaTexture,baseOpacityTex.asChar());
	hairPara.baseOpaNoiseAmp=fnDagNode.findPlug("baseOpacityNoiseAmp").asFloat();
	hairPara.baseOpaNoiseFreq=fnDagNode.findPlug("baseOpacityNoiseFreq").asFloat();

	MString tipWidthTex;
	vec3 tipWidthVal;
	getTexAndValue(fnDagNode,"tipWidth",tipWidthTex,tipWidthVal);
	hairPara.tipWidthBaseValue=tipWidthVal.x;
	strcpy(hairPara.tipWidthTexture,tipWidthTex.asChar());
	hairPara.tipWidthNoiseAmp=fnDagNode.findPlug("tipWidthNoiseAmp").asFloat();
	hairPara.tipWidthNoiseFreq=fnDagNode.findPlug("tipWidthNoiseFreq").asFloat();

	MString baseWidthTex;
	vec3 baseWidthVal;
	getTexAndValue(fnDagNode,"baseWidth",baseWidthTex,baseWidthVal);
	hairPara.baseWidthBaseValue=baseWidthVal.x;
	strcpy(hairPara.baseWidthTexture,baseWidthTex.asChar());
	hairPara.baseWidthNoiseAmp=fnDagNode.findPlug("baseWidthNoiseAmp").asFloat();
	hairPara.baseWidthNoiseFreq=fnDagNode.findPlug("baseWidthNoiseFreq").asFloat();

	hairPara.lengthNoiseAmp=fnDagNode.findPlug("lengthNoiseAmp").asFloat();
	hairPara.lengthNoiseFreq=fnDagNode.findPlug("lengthNoiseFreq").asFloat();
	hairPara.inclinationNoiseAmp=fnDagNode.findPlug("inclinationNoiseAmp").asFloat();
	hairPara.inclinationNoiseFreq=fnDagNode.findPlug("inclinationNoiseFreq").asFloat();
	hairPara.polarNoiseAmp=fnDagNode.findPlug("polarNoiseAmp").asFloat();
	hairPara.polarNoiseFreq=fnDagNode.findPlug("polarNoiseFreq").asFloat();
	hairPara.tipCurlNoiseAmp=fnDagNode.findPlug("tipCurlNoiseAmp").asFloat();
	hairPara.tipCurlNoiseFreq=fnDagNode.findPlug("tipCurlNoiseFreq").asFloat();
	hairPara.baseCurlNoiseAmp=fnDagNode.findPlug("baseCurlNoiseAmp").asFloat();
	hairPara.baseCurlNoiseFreq=fnDagNode.findPlug("baseCurlNoiseFreq").asFloat();

	MString scraggleTex;
	vec3 scraggleVal;
	getTexAndValue(fnDagNode,"scraggleMap",scraggleTex,scraggleVal);
	hairPara.scraggleValue=scraggleVal.x;
	strcpy(hairPara.scraggleTexture,scraggleTex.asChar());
	hairPara.scraggleNoiseAmp=fnDagNode.findPlug("scraggleNoiseAmp").asFloat();
	hairPara.scraggleNoiseFreq=fnDagNode.findPlug("scraggleNoiseFreq").asFloat();

	MString scraggleFreqTex;
	vec3 scraggleFreqVal;
	getTexAndValue(fnDagNode,"scraggleFrequencyMap",scraggleFreqTex,scraggleFreqVal);
	hairPara.scraggleFreqValue=scraggleFreqVal.x;
	strcpy(hairPara.scraggleFreqTexture,scraggleFreqTex.asChar());
	hairPara.scraggleFreqNoiseAmp=fnDagNode.findPlug("scraggleFrequencyNoiseAmp").asFloat();
	hairPara.scraggleFreqNoiseFreq=fnDagNode.findPlug("scraggleFrequencyNoiseFreq").asFloat();

	MString clumpTex;
	vec3 clumpVal;
	getTexAndValue(fnDagNode,"clumpMap",clumpTex,clumpVal);
	hairPara.clumpValue=clumpVal.x;
	strcpy(hairPara.clumpTexture,clumpTex.asChar());
	hairPara.clumpNoiseAmp=fnDagNode.findPlug("clumpNoiseAmp").asFloat();
	hairPara.clumpNoiseFreq=fnDagNode.findPlug("clumpNoiseFreq").asFloat();

	MString clumpFreqTex;
	vec3 clumpFreqVal;
	getTexAndValue(fnDagNode,"clumpFrequencyMap",clumpFreqTex,clumpFreqVal);
	hairPara.clumpFreqValue=clumpFreqVal.x;
	strcpy(hairPara.clumpFreqTexture,clumpFreqTex.asChar());
	hairPara.clumpFreqNoiseAmp=fnDagNode.findPlug("clumpFrequencyNoiseAmp").asFloat();
	hairPara.clumpFreqNoiseFreq=fnDagNode.findPlug("clumpFrequencyNoiseFreq").asFloat();

	MString clumpShapeTex;
	vec3 clumpShapeVal;
	getTexAndValue(fnDagNode,"clumpShapeMap",clumpShapeTex,clumpShapeVal);
	hairPara.clumpShapeValue=clumpShapeVal.x;
	strcpy(hairPara.clumpShapeTexture,clumpShapeTex.asChar());
	hairPara.clumpShapeNoiseAmp=fnDagNode.findPlug("clumpShapeNoiseAmp").asFloat();
	hairPara.clumpShapeNoiseFreq=fnDagNode.findPlug("clumpShapeNoiseFreq").asFloat();

	fstream fout(fnDagNode.findPlug("parametersFileName").asString().asChar(),ios_base::out|ios_base::binary);
	fout.write((char*)&hairPara,sizeof(hairParametersInfo));
	fout.flush();
	fout.close();
	//getClumpCenter(hairPara);
	subdivide(hairPara,fnDagNode.findPlug("parametersFileName").asString());
	return fnDagNode.findPlug("parametersFileName").asString();
}

void vixo_hairCacheCommandDelight::getTexAndValue(MFnDagNode& fnDag,MString attrName,MString& texName,vec3& value)
{
	MPlug plug=fnDag.findPlug(attrName);
	MPlugArray tempArr;
	plug.connectedTo(tempArr,true,false);
	//if(attrName=="densityMap")
	//	cout<<"test"<<tempArr.length()<<endl;
	if(tempArr.length()==0)
	{
		int childNum=plug.numChildren();
		if(childNum>0)
		{
			value.x=plug.child(0).asFloat();
			value.y=plug.child(1).asFloat();
			value.z=plug.child(2).asFloat();
		}
		else
			value.x=plug.asFloat();
		texName="";
	}
	else
	{
		MFnDependencyNode fnFile(tempArr[0].node());
		texName=fnFile.findPlug("fileTextureName").asString();
		value.x=0.0f;
		value.y=0.0f;
		value.z=0.0f;
	}
}

void vixo_hairCacheCommandDelight::getClumpCenter(hairParametersInfo& hairParameters)
{
	fstream fin(hairParameters.hairRootFile,ios_base::in|ios_base::binary);
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

	for(int i=0;i<hairNumTot;i++)
	{
		double minDist = 9999999;
		unsigned cc;
		for( cc=0; cc<clumpCenterData.size(); ++cc ) 
		{
			const float cu = clumpCenterData[cc].u;
			const float cv = clumpCenterData[cc].v;
			const float cdist = clumpCenterData[cc].minDist;
			float dist = ((cu-hairDetailData[i].u)*(cu-hairDetailData[i].u) + (cv-hairDetailData[i].v)*(cv-hairDetailData[i].v));
			if( dist < cdist ) 
			{
				clumpCenterData[cc].minDist = dist;
				clumpCenterData[cc].hairid = i;
				clumpCenterData[cc].triid=hairDetailData[i].triIdx;
			}
			if( dist < minDist ) 
			{
				minDist = dist;
				hairClumpRelationData[i].closetClumpCenter=clumpCenterData[cc].hairid;
				hairClumpRelationData[i].triId=clumpCenterData[cc].triid;
			}
		}
	}

	fstream fout(hairParameters.hairClumpFile,ios_base::out|ios_base::binary);
	fout.write((char*)&hairClumpRelationData[0],sizeof(hairDetailExtend)*hairNumTot);
	fout.flush();
	fout.close();
}

void vixo_hairCacheCommandDelight::subdivide(hairParametersInfo& hairParameters,MString paraFileName)
{
	fstream fin(hairParameters.hairCacheFile,ios_base::in|ios_base::binary);
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

		MString paraFile=paraFileName;
		MString temp;
		temp.set(i);
		paraFile=paraFile+" "+temp;
		MString boxStr,boxStrTemp;
		boxStrTemp.set(box[0].x);
		boxStr=boxStrTemp;
		boxStrTemp.set(box[1].x);
		boxStr=boxStr+" "+boxStrTemp;
		boxStrTemp.set(box[0].y);
		boxStr=boxStr+" "+boxStrTemp;
		boxStrTemp.set(box[1].y);
		boxStr=boxStr+" "+boxStrTemp;
		boxStrTemp.set(box[0].z);
		boxStr=boxStr+" "+boxStrTemp;
		boxStrTemp.set(box[1].z);
		boxStr=boxStr+" "+boxStrTemp;
		MString sp = "RiProcedural -l \"vixo_hairRenderDSO\" -b "+boxStr+" -p \""+paraFile+"\" ";
	
		MGlobal::executeCommand(sp);
	}
}

void vixo_hairCacheCommandDelight::getBoundingBox(vector<vec3>& allPoints,vector<vec3>& box,int allPLen,float expand)
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

