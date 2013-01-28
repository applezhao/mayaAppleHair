#include "vixo_ctrlCurves.h"

#include <maya\MPlugArray.h>
#include <maya\MImage.h>
#include <maya\MItMeshPolygon.h>
#include <maya\MFloatArray.h>

#include <maya\MFnMesh.h>

#include <maya/MFnMessageAttribute.h>
#include <maya/MFnMatrixAttribute.h>

#include <maya/MDGModifier.h>

#include <maya/MFnDependencyNode.h>

#include <maya/MVectorArray.h>
#include <maya/MFnVectorArrayData.h>

#include <maya/MArrayDataHandle.h>

#include <maya/MFnMatrixData.h>

#include <maya/MItMeshVertex.h>



MTypeId vixo_ctrlCurves::id(0x34381);

MObject vixo_ctrlCurves::inCtrlCurveData;
MObject vixo_ctrlCurves::staticMesh;
//MObject vixo_ctrlCurves::vertexList;
MObject vixo_ctrlCurves::inMesh;
//MObject vixo_ctrlCurves::meshMatrix;
//MObject vixo_ctrlCurves::inTime;

MObject vixo_ctrlCurves::hairSysMsg;
MObject vixo_ctrlCurves::hiddenOutput;

MObject vixo_ctrlCurves::degree;

MObject vixo_ctrlCurves::showCtrl;
MObject vixo_ctrlCurves::densityMap;
MObject vixo_ctrlCurves::hiddenOutCurveNum;

MObject vixo_ctrlCurves::boundBoxExpand;
MObject vixo_ctrlCurves::miscBoundBoxExpend;

MObject vixo_ctrlCurves::hiddenOutCurveDataBase;

MObject vixo_ctrlCurves::clumpInfoFileName;
MObject vixo_ctrlCurves::parametersFileName;
MObject vixo_ctrlCurves::segment;

//follicle file
MObject vixo_ctrlCurves::hairNum;
MObject vixo_ctrlCurves::hairRootFileName;
MObject vixo_ctrlCurves::hiddenOutputBasicInfo;

//cache File
MObject vixo_ctrlCurves::hairCacheFileName;
MObject vixo_ctrlCurves::renderUseCache;

MObject vixo_ctrlCurves::tipColor;
MObject vixo_ctrlCurves::tipColorNoiseAmp;
MObject vixo_ctrlCurves::tipColorNoiseFreq;
MObject vixo_ctrlCurves::baseColor;
MObject vixo_ctrlCurves::baseColorNoiseAmp;
MObject vixo_ctrlCurves::baseColorNoiseFreq;
MObject vixo_ctrlCurves::tipOpacity;
MObject vixo_ctrlCurves::tipOpacityNoiseAmp;
MObject vixo_ctrlCurves::tipOpacityNoiseFreq;
MObject vixo_ctrlCurves::baseOpacity;
MObject vixo_ctrlCurves::baseOpacityNoiseAmp;
MObject vixo_ctrlCurves::baseOpacityNoiseFreq;
MObject vixo_ctrlCurves::tipWidth;
MObject vixo_ctrlCurves::tipWidthNoiseAmp;
MObject vixo_ctrlCurves::tipWidthNoiseFreq;
MObject vixo_ctrlCurves::baseWidth;
MObject vixo_ctrlCurves::baseWidthNoiseAmp;
MObject vixo_ctrlCurves::baseWidthNoiseFreq;
MObject vixo_ctrlCurves::lengthNoiseAmp;
MObject vixo_ctrlCurves::lengthNoiseFreq;
MObject vixo_ctrlCurves::inclinationNoiseAmp;
MObject vixo_ctrlCurves::inclinationNoiseFreq;
MObject vixo_ctrlCurves::polarNoiseAmp;
MObject vixo_ctrlCurves::polarNoiseFreq;
MObject vixo_ctrlCurves::tipCurlNoiseAmp;
MObject vixo_ctrlCurves::tipCurlNoiseFreq;
MObject vixo_ctrlCurves::baseCurlNoiseAmp;
MObject vixo_ctrlCurves::baseCurlNoiseFreq;

MObject vixo_ctrlCurves::scraggleMap;
MObject vixo_ctrlCurves::scraggleNoiseAmp;
MObject vixo_ctrlCurves::scraggleNoiseFreq;

MObject vixo_ctrlCurves::scraggleFrequencyMap;
MObject vixo_ctrlCurves::scraggleFrequencyNoiseAmp;
MObject vixo_ctrlCurves::scraggleFrequencyNoiseFreq;

MObject vixo_ctrlCurves::clumpMap;
MObject vixo_ctrlCurves::clumpNoiseAmp;
MObject vixo_ctrlCurves::clumpNoiseFreq;

MObject vixo_ctrlCurves::clumpFrequencyMap;
MObject vixo_ctrlCurves::clumpFrequencyNoiseAmp;
MObject vixo_ctrlCurves::clumpFrequencyNoiseFreq;

//MObject vixo_ctrlCurves::hiddenOutClumpCenter;

MObject vixo_ctrlCurves::clumpShapeMap;
MObject vixo_ctrlCurves::clumpShapeNoiseAmp;
MObject vixo_ctrlCurves::clumpShapeNoiseFreq;

vixo_ctrlCurves::vixo_ctrlCurves(void)
{
	maxSpan=0;
}


vixo_ctrlCurves::~vixo_ctrlCurves(void)
{
}

MStatus vixo_ctrlCurves::compute( const MPlug& plug, MDataBlock& datablock )
{ 
	//cout<<"compute"<<plug.info().asChar()<<endl;
	if(datablock.inputValue(staticMesh).asMesh().hasFn(MFn::Type::kMesh)==false)
	{
		cout<<"no inMesh00"<<endl;
		vixoDrawData.degree=0;
		vixoDrawData.curveNum=0;
		vixoDrawData.knotsSize=0;
		//datablock.outputValue(hiddenOutCurveNum).setInt(0);
		datablock.outputValue(hiddenOutCurveNum).setClean();
		//datablock.outputValue(hiddenOutput).setInt(0);
		datablock.outputValue(hiddenOutput).setClean();
		return MS::kSuccess;
	}

	if(plug==hiddenOutCurveDataBase)
	{
		
		staticMeshInitOutCurveDatatBase(datablock);
		datablock.outputValue(hiddenOutCurveDataBase).setInt(0);
		datablock.outputValue(hiddenOutCurveDataBase).setClean();
	}

	if(plug==hiddenOutputBasicInfo)
	{
		int hairNumValue=datablock.inputValue(hairNum).asInt();
		if(hairNumValue==0)
			return MS::kSuccess;
		MString hairRootFileNameValue=datablock.inputValue(hairRootFileName).asString();
		if(hairRootFileNameValue=="")
			return MS::kSuccess;
		exportFollicle(hairNumValue,hairRootFileNameValue,datablock);
		datablock.outputValue(hiddenOutputBasicInfo).setClean();
		if(datablock.inputValue(clumpInfoFileName).asString()=="")
			return MS::kSuccess;
		getClumpCenter(datablock);
		return MS::kSuccess;
	}
/*
	if(plug==hiddenOutClumpCenter)
	{
		if(datablock.inputValue(clumpInfoFileName).asString()=="")
			return MS::kSuccess;
		if(datablock.inputValue(hairNum).asInt()==0)
			return MS::kSuccess;
		if(datablock.inputValue(hairRootFileName).asString()=="")
			return MS::kSuccess;
		getClumpCenter(datablock);
	}
*/
	if(datablock.inputValue(inMesh).asMesh().hasFn(MFn::kMesh)==false)
	{
		cout<<"no inMesh11"<<endl;
		vixoDrawData.degree=0;
		vixoDrawData.curveNum=0;
		vixoDrawData.knotsSize=0;
		datablock.outputValue(hiddenOutCurveNum).setInt(0);
		datablock.outputValue(hiddenOutCurveNum).setClean();
		datablock.outputValue(hiddenOutput).setInt(0);
		datablock.outputValue(hiddenOutput).setClean();
		return MS::kSuccess;
	}

	if(plug==hiddenOutCurveNum)
	{
		curOutCurveSet.clear();
		//outVertexControlData.clear();
		MPlugArray arr;
		MPlug plugMap(thisMObject(),this->densityMap);
		plugMap.connectedTo(arr,true,false);
		if(arr.length()!=1)
		{
			cout<<"no density map00"<<endl;
			vixoDrawData.curveNum=0;
			datablock.outputValue(hiddenOutCurveNum).setInt(0);
			datablock.outputValue(hiddenOutCurveNum).setClean();
			return MS::kSuccess;
		}
		MFnDependencyNode dgFn(arr[0].node());
		MPlug fnFile=dgFn.findPlug("fileTextureName");
		if(fnFile.asString()=="")
		{
			cout<<"no density map11"<<endl;
			vixoDrawData.curveNum=0;
			datablock.outputValue(hiddenOutCurveNum).setInt(0);
			datablock.outputValue(hiddenOutCurveNum).setClean();
			return MS::kSuccess;
		}

		float showCtrlValue=datablock.inputValue(showCtrl).asInt()/100.0;

		dealDensityMapShowCtrl(showCtrlValue,arr[0].node(),datablock);

		datablock.outputValue(hiddenOutCurveNum).setInt(curOutCurveSet.size());
		datablock.outputValue(hiddenOutCurveNum).setClean();
	}

	if(outVertexDataBase.size()==0)
	{
		staticMeshInitOutCurveDatatBase(datablock);
	}
	

	//get inCurve basic set
	box.clear();
	getInCurveInfo(datablock);
	//get inCurve basic set


	//output data
	vixoDrawData.curveNum=curOutCurveSet.size();
	vixoDrawData.degree=datablock.inputValue(degree).asInt();
	vixoDrawData.knotsSize=maxSpan+1+vixoDrawData.degree+1;
	vixoDrawData.knots.resize(maxSpan+1+vixoDrawData.degree+1);
	vixoDrawData.allCurves.clear();
	vixoDrawData.allCurves.resize(vixoDrawData.curveNum*(maxSpan+1)*3,0.0);

	//cout<<vixoDrawData.curveNum<<" "<<vixoDrawData.degree<<" "<<vixoDrawData.knotsSize<<endl;

	/*//debug
	map<int,vector<outVIDCtrlInfo>>::iterator outcdIter;
	for(outcdIter=outVertexControlData.begin();outcdIter!=outVertexControlData.end();outcdIter++)
	{
		cout<<outcdIter->first<<":"<<endl;
		vector<outVIDCtrlInfo>::iterator iterCtrlData;
		for(iterCtrlData=outcdIter->second.begin();iterCtrlData!=outcdIter->second.end();iterCtrlData++)
		{
			cout<<iterCtrlData->inVID<<"-"<<iterCtrlData->percent<<'\t';
		}
		cout<<endl;
	}
	//debug*/
	
	buildKnots();

	MItMeshVertex iterMesh(datablock.inputValue(inMesh).asMesh());
	int prevIndex;
	set<int>::iterator outCurveIter;
	int i=0;
	for(outCurveIter=curOutCurveSet.begin();outCurveIter!=curOutCurveSet.end();outCurveIter++)
	{
		
		int outvid=*outCurveIter;
		//cout<<"test"<<outvid<<endl;
		iterMesh.setIndex(outvid,prevIndex);
		MVector normal;
		iterMesh.getNormal(normal,MSpace::kWorld);
		MPoint firstPnt;
		firstPnt=iterMesh.position(MSpace::kWorld);

		box.expand(firstPnt);

		vixoDrawData.allCurves[i*(maxSpan+1)*3]=firstPnt.x;
		vixoDrawData.allCurves[i*(maxSpan+1)*3+1]=firstPnt.y;
		vixoDrawData.allCurves[i*(maxSpan+1)*3+2]=firstPnt.z;

		map<int,vector<outVIDCtrlInfo>>::iterator outcdIter=outVertexControlData.find(outvid);
		//*cout*/<<outcdIter->second.size()<<endl;

		for(int ctrls=0;ctrls<outcdIter->second.size();ctrls++)
		{
			//cout<<"app:"<<outcdIter->second[ctrls].inVID<<endl;
			map<int,inVertexShapeInfo>::iterator iterInCurve=inVIDMapTangents.find(outcdIter->second[ctrls].inVID);
			MQuaternion quar=iterInCurve->second.normal.rotateTo(normal);
			//cout<<iterInCurve->second.tangent.length()<<endl;
			for(int j=0;j<maxSpan;j++)
			{
				MVector tempTan(0,0,0);
				if(j<iterInCurve->second.tangent.length())
				{
					//cout<<"apple"<<endl;
					tempTan=iterInCurve->second.tangent[j].rotateBy(quar)*outcdIter->second[ctrls].percent;
				}
				vixoDrawData.allCurves[(i*(maxSpan+1)+j+1)*3]+=tempTan.x;
				vixoDrawData.allCurves[(i*(maxSpan+1)+j+1)*3+1]+=tempTan.y;
				vixoDrawData.allCurves[(i*(maxSpan+1)+j+1)*3+2]+=tempTan.z;
			}
		}
		/*
		vector<outVIDCtrlInfo>::iterator iterCtrlData;
		for(iterCtrlData=outcdIter->second.begin();iterCtrlData!=outcdIter->second.end();iterCtrlData++)
		{
			cout<<"app:"<<iterCtrlData->inVID<<endl;
			map<int,inVertexShapeInfo>::iterator iterInCurve=inVIDMapTangents.find(iterCtrlData->inVID);
			MQuaternion quar=iterInCurve->second.normal.rotateTo(normal);
			cout<<iterInCurve->second.tangent.length()<<endl;
			for(int j=0;j<maxSpan;j++)
			{
				MVector tempTan(0,0,0);
				if(j<iterInCurve->second.tangent.length())
				{
					cout<<"apple"<<endl;
					tempTan=iterInCurve->second.tangent[j].rotateBy(quar)*iterCtrlData->percent;
				}
				vixoDrawData.allCurves[(i*(maxSpan+1)+j+1)*3]+=tempTan.x;
				vixoDrawData.allCurves[(i*(maxSpan+1)+j+1)*3+1]+=tempTan.y;
				vixoDrawData.allCurves[(i*(maxSpan+1)+j+1)*3+2]+=tempTan.z;
			}
		}*/

		
		for(int j=1;j<maxSpan+1;j++)
		{
			vixoDrawData.allCurves[(i*(maxSpan+1)+j)*3]+=vixoDrawData.allCurves[(i*(maxSpan+1)+j-1)*3];
			vixoDrawData.allCurves[(i*(maxSpan+1)+j)*3+1]+=vixoDrawData.allCurves[(i*(maxSpan+1)+j-1)*3+1];
			vixoDrawData.allCurves[(i*(maxSpan+1)+j)*3+2]+=vixoDrawData.allCurves[(i*(maxSpan+1)+j-1)*3+2];
		}
		box.expand(MPoint(vixoDrawData.allCurves[(i*(maxSpan+1)+maxSpan)*3],vixoDrawData.allCurves[(i*(maxSpan+1)+maxSpan)*3+1],vixoDrawData.allCurves[(i*(maxSpan+1)+maxSpan)*3+2]));
		i++;
	}
	//output data
	
	//box.clear();
	//MFnMesh fnMesh(datablock.inputValue(inMesh).asMesh());
	//box.expand(fnMesh.boundingBox());
	float3 &scale3=datablock.inputValue(boundBoxExpand).asFloat3();
	MTransformationMatrix tranMat;
	double scale[3]={scale3[0],scale3[1],scale3[2]};
	tranMat.setScale(scale,MSpace::kObject);
	box.transformUsing(tranMat.asScaleMatrix());
	//box.transformUsing();
	//cout<<"compute:"<<box.min().x<<" "<<box.min().y<<" "<<box.min().z<<" "<<box.max().x<<" "<<box.max().y<<" "<<box.max().z<<endl;
	datablock.outputValue(hiddenOutput).set(1);
	datablock.outputValue(hiddenOutput).setClean();

	return MS::kSuccess;
}

void vixo_ctrlCurves::getClumpCenter(MDataBlock& data)
{
	fstream fin(data.inputValue(hairRootFileName).asString().asChar(),ios_base::in|ios_base::binary);
	int hairNumTriNum[3];
	fin.read((char*)hairNumTriNum,sizeof(int)*3);
	int triNumTot=hairNumTriNum[1];
	int hairNumTot=hairNumTriNum[2];
	vector<triangleBasicInfo> triBasicData(triNumTot);
	fin.read((char*)&triBasicData[0],sizeof(triangleBasicInfo)*triNumTot);
	vector<hairDetail> hairDetailData(hairNumTot);
	fin.read((char*)&hairDetailData[0],sizeof(hairDetail)*hairNumTot);
	fin.close();

	MString texName;
	vec3 freqValue;
	getTexAndValue(texName,freqValue);

	vixo_clumpFreq clumpFreq;
	clumpFreq.init(texName.asChar(),freqValue.x,data.inputValue(clumpFrequencyNoiseAmp).asFloat(),data.inputValue(clumpFrequencyNoiseFreq).asInt());

	//getClumpCenterInfo
	vector<clumpCenterInfo> clumpCenterData;
	vector<hairDetailExtend> hairClumpRelationData(hairNumTot);
	srand(time(NULL));
	float u,v;
	float step=sqrt(1.0/(hairNumTriNum[0]/10));
	step=min(step,0.01f);
	for(u=0; u<1; u+=step) 
	{
		for(v=0; v<1; v+=step) 
		{
			float cf = clumpFreq.getValue(u, v);
			float r = rand()/(RAND_MAX+1.0f);
			if( r < cf*0.25 ) 
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

	fstream fout(data.inputValue(clumpInfoFileName).asString().asChar(),ios_base::out|ios_base::binary);
	fout.write((char*)&hairClumpRelationData[0],sizeof(hairDetailExtend)*hairNumTot);
	fout.flush();
	fout.close();
}

void vixo_ctrlCurves::getTexAndValue(MString& texName,vec3& value)
{
	MPlugArray arr;
	MPlug plugMap(thisMObject(),clumpFrequencyMap);
	plugMap.connectedTo(arr,true,false);
	if(arr.length()!=1)
	{
		value.x=plugMap.child(0).asFloat();
		value.y=plugMap.child(1).asFloat();
		value.z=plugMap.child(2).asFloat();
		texName="";
		return;
	}
	MFnDependencyNode dgFn(arr[0].node());
	MPlug fnFile=dgFn.findPlug("fileTextureName");
	if(fnFile.asString()=="")
	{
		value.x=plugMap.child(0).asFloat();
		value.y=plugMap.child(1).asFloat();
		value.z=plugMap.child(2).asFloat();
		texName="";
		return;
	}
	else
	{
		texName=fnFile.asString();
		value.x=0.0f;
		value.y=0.0f;
		value.z=0.0f;
		return;
	}
}

void vixo_ctrlCurves::staticMeshInitOutCurveDatatBase(MDataBlock & datablock)
{
	outVertexDataBase.clear();
	/*//MFnComponentListData fnVlist(datablock.inputValue(vertexList).data());
	if(fnVlist.length()>0)
	{
		for(int i=0;i<fnVlist.length();i++)
		{
			MFnSingleIndexedComponent fnsingle(fnVlist[i]);
			for(int j=0;j<fnsingle.elementCount();j++)
			{
				int vid=fnsingle.element(j);
				set<outInVertexRelationInfo> temp;
				temp.clear();
				outVertexDataBase.insert(pair<int,set<outInVertexRelationInfo>>(vid,temp));
			}
		}
	}*/
	//else
	//{
		MItMeshVertex iterMesh(datablock.inputValue(staticMesh).asMesh());
		for(int i=0;i<iterMesh.count();i++)
		{
			set<outInVertexRelationInfo> temp;
			temp.clear();
			outVertexDataBase.insert(pair<int,set<outInVertexRelationInfo>>(i,temp));
		}
	//}

	//build relation between inCurve and outCurve
	map<int,inVertexBasicInfo>::iterator inDBIter;
	for(inDBIter=inVIDMapInVertexDataBase.begin();inDBIter!=inVIDMapInVertexDataBase.end();inDBIter++)
	{
		buileRelationBetweenInOut(inDBIter->first);
	}

	//sort
	map<int,set<outInVertexRelationInfo>>::iterator outDBIter;
	for(outDBIter=outVertexDataBase.begin();outDBIter!=outVertexDataBase.end();outDBIter++)
	{
		sortControlOrder(outDBIter->first);
	}
	//sort
	//build relation between inCurve and outCurve
}


void vixo_ctrlCurves::buildKnots()
{
	for(int i=0;i<vixoDrawData.degree+1;i++)
	{
		vixoDrawData.knots[i]=0.0;
	}
	for(int i=vixoDrawData.degree+1;i<=maxSpan+1;i++)
	{
		vixoDrawData.knots[i]=i-vixoDrawData.degree;
	}
	for(int i=maxSpan+2;i<maxSpan+1+vixoDrawData.degree+1;i++)
	{
		vixoDrawData.knots[i]=vixoDrawData.knots[maxSpan+1];
	}
}

int vixo_ctrlCurves::getInCurveInfo(MDataBlock & datablock)
{
	MArrayDataHandle inCurves=datablock.inputValue(inCtrlCurveData);

	maxSpan=0;
	MItMeshVertex iterMesh(datablock.inputValue(inMesh).asMesh());
	int prevIndex;

	int newAdd=0;
	for(int i=0;i<inCurves.elementCount();i++)
	{
		MPlug inCurveElePlug(thisMObject(),inCtrlCurveData);
		if(inCurveElePlug.elementByLogicalIndex(inCurves.elementIndex()).isConnected()==false)
		{
			inCurves.next();
			continue;
		}
		MPlugArray tempArr;
		inCurveElePlug.elementByLogicalIndex(inCurves.elementIndex()).connectedTo(tempArr,true,false);
		MFnDependencyNode fnHairSystem(tempArr[0].node());
		MPlug inputHairs=fnHairSystem.findPlug("inputHair");
		tempArr.clear();
		inputHairs.elementByLogicalIndex(inCurves.elementIndex()).connectedTo(tempArr,true,false);
		MFnDependencyNode fnFolli(tempArr[0].node());

		if(plugMapInVID.count(inCurves.elementIndex())<=0)
		{
			MPlug VIDPlug=fnFolli.findPlug("vertexID");
			currentInVid.insert(VIDPlug.asInt());
			inVIDMapPlug.insert(pair<int,int>(VIDPlug.asInt(),inCurves.elementIndex()));
			plugMapInVID.insert(pair<int,int>(inCurves.elementIndex(),VIDPlug.asInt()));
			inVertexShapeInfo temp;
			inVIDMapTangents.insert(pair<int,inVertexShapeInfo>(VIDPlug.asInt(),temp));
			initBasicStepInfo(VIDPlug.asInt(),datablock);
			buileRelationBetweenInOut(VIDPlug.asInt());
			newAdd++;
		}

		//get inCurve shape set
		MObject incurve=inCurves.inputValue().data();
		MFnVectorArrayData fndata(incurve);

		int vid=plugMapInVID.find(inCurves.elementIndex())->second;

		map<int,inVertexShapeInfo>::iterator iter=inVIDMapTangents.find(vid);

		iterMesh.setIndex(vid,prevIndex);
		iterMesh.getNormal(iter->second.normal,MSpace::kWorld);

		//get matrix
		MFnMatrixData fnMatData(fnFolli.findPlug("worldMatrix[0]").asMObject());
		MMatrix mat=fnMatData.matrix();

		MVectorArray allPnts=fndata.array();
		iter->second.tangent.setLength(allPnts.length()-1);
		box.expand(allPnts[0]);
		for(int j=0;j<allPnts.length()-1;j++)
		{
			iter->second.tangent[j]=allPnts[j+1]*mat-allPnts[j]*mat;
		}
		box.expand(allPnts[allPnts.length()-1]);
		maxSpan=max(maxSpan,(int)(fndata.array().length()-1));
		//get inCurve shape set

		inCurves.next();
	}

	/*
	//debug
	//map<int,inVertexBasicInfo> inVIDMapInVertexDataBase
	map<int,inVertexBasicInfo>::iterator iterInBasic;
	for(iterInBasic=inVIDMapInVertexDataBase.begin();iterInBasic!=inVIDMapInVertexDataBase.end();iterInBasic++)
	{
		cout<<"in:"<<iterInBasic->first<<endl;
		for(int i=0;i<iterInBasic->second.stepVertex.size();i++)
		{
			set<forInVertexBasicInfo>::iterator iterTemp;
			cout<<i<<":";
			for(iterTemp=iterInBasic->second.stepVertex[i].begin();iterTemp!=iterInBasic->second.stepVertex[i].end();iterTemp++)
			{
				cout<<iterTemp->vertexID<<"_"<<iterTemp->distance<<'\t';
			}
			cout<<endl;
		}
		cout<<endl;
	}
	//debug
	*/
	if(newAdd>0)
	{
		map<int,set<outInVertexRelationInfo>>::iterator outDBIter;
		for(outDBIter=outVertexDataBase.begin();outDBIter!=outVertexDataBase.end();outDBIter++)
		{
			sortControlOrder(outDBIter->first);
		}

		//debug
		/*//map<int,set<outInVertexRelationInfo>> outVertexDataBase;
		//map<int,set<outInVertexRelationInfo>>::iterator outDBIter;
		for(outDBIter=outVertexDataBase.begin();outDBIter!=outVertexDataBase.end();outDBIter++)
		{
			cout<<outDBIter->first<<":"<<endl;
			set<outInVertexRelationInfo>::iterator iterStepDB;
			for(iterStepDB=outDBIter->second.begin();iterStepDB!=outDBIter->second.end();iterStepDB++)
			{
				cout<<iterStepDB->inVID<<"-"<<iterStepDB->step<<"-"<<iterStepDB->distance<<'\t';
			}
			cout<<endl;
		}*/
		//map<int,vector<outVIDCtrlInfo>> outVertexControlData;
		/*map<int,vector<outVIDCtrlInfo>>::iterator outcdIter;
		for(outcdIter=outVertexControlData.begin();outcdIter!=outVertexControlData.end();outcdIter++)
		{
			cout<<outcdIter->first<<":"<<endl;
			vector<outVIDCtrlInfo>::iterator iterCtrlData;
			for(iterCtrlData=outcdIter->second.begin();iterCtrlData!=outcdIter->second.end();iterCtrlData++)
			{
				cout<<iterCtrlData->inVID<<"-"<<iterCtrlData->percent<<'\t';
			}
			cout<<endl;
		}*/
		//debug
	}
	return maxSpan;
}


void vixo_ctrlCurves::dealDensityMapShowCtrl(float showCtrlValue,MObject& fileNode,MDataBlock & datablock)
{
	MImage imgFn;
	imgFn.readFromTextureNode(fileNode);

	unsigned int imageWidth,imageHeight,imageDepth;
	imgFn.getSize(imageWidth,imageHeight);
	imageDepth=4;

	unsigned char * pixels=imgFn.pixels();
	MItMeshVertex iterVertex(datablock.inputValue(staticMesh).asMesh());
	int i=0;
	for(iterVertex.reset();!iterVertex.isDone();iterVertex.next())
	{

		MFloatArray uCoord,vCoord;
		MIntArray faceID;
		iterVertex.getUVs(uCoord,vCoord,faceID);
		int pixelW=uCoord[0]*imageWidth;
		pixelW=min(pixelW,(int)imageWidth-1);
		int pixelH=vCoord[0]*imageHeight;
		pixelH=min(pixelH,(int)imageHeight-1);
		unsigned int pixel=(unsigned int)pixels[imageDepth*(pixelH*imageWidth+pixelW)];
		if(pixel>0)
		{
			if(ceil(i*showCtrlValue)<=floor((i+1)*showCtrlValue)||i==0)
				curOutCurveSet.insert(iterVertex.index());
			i++;
		}
		if(iterVertex.isDone())
			break;
	}
}

void vixo_ctrlCurves::buileRelationBetweenInOut(int inVID)
{
	map<int,inVertexBasicInfo>::iterator inDBIter=inVIDMapInVertexDataBase.find(inVID);
	for(int i=0;i<MAXSTEP;i++)
	{
		if(inDBIter->second.stepVertex[i].size()<=0)
			break;
		set<forInVertexBasicInfo>::iterator stepSetIter;
		for(stepSetIter=inDBIter->second.stepVertex[i].begin();stepSetIter!=inDBIter->second.stepVertex[i].end();stepSetIter++)
		{
			int outVID=stepSetIter->vertexID;
			outInVertexRelationInfo temp;
			temp.inVID=inDBIter->first;
			temp.distance=stepSetIter->distance;
			temp.step=i;
			outVertexDataBase.find(outVID)->second.insert(temp);
		}
	}
}

void vixo_ctrlCurves::sortControlOrder(int outVID)
{
	map<int,set<outInVertexRelationInfo>>::iterator outDBIter=outVertexDataBase.find(outVID);
	//::sort(outDBIter->second.begin(),outDBIter->second.end());
	float totalDistance=0;
	float minInfinite=0.0000001;
	int maxStepNum=0;
	int maxStepValue=0;
	set<outInVertexRelationInfo>::iterator iterStepDB;
	for(iterStepDB=outDBIter->second.begin();iterStepDB!=outDBIter->second.end();iterStepDB++)
	{
		if(iterStepDB->step > maxStepValue)
		{
			maxStepNum++;
			if(maxStepNum>MAXCONTROLSTEP)
				break;
			maxStepValue=iterStepDB->step;
		}
		totalDistance+=1.0/(iterStepDB->distance+minInfinite);
	}
	vector<outVIDCtrlInfo> temp;
	for(iterStepDB=outDBIter->second.begin();iterStepDB!=outDBIter->second.end();iterStepDB++)
	{
		if(iterStepDB->step > maxStepValue)
		{
			break;
		}
		double percent=1.0/((iterStepDB->distance+minInfinite)*totalDistance);
		if(percent<0.000001)
			continue;
		outVIDCtrlInfo ele;
		ele.inVID=iterStepDB->inVID;
		ele.percent=percent;
		temp.push_back(ele);
	}
	outVertexControlData.erase(outVID);
	outVertexControlData.insert(pair<int,vector<outVIDCtrlInfo>>(outVID,temp));
}

void vixo_ctrlCurves::initOutInVertexRelationInfoDataBase(int vid)
{
	
}

bool vixo_ctrlCurves::isBounded() const
{	 
	return true;
}

MBoundingBox vixo_ctrlCurves::boundingBox() const
{
	//MBoundingBox result;
	//result.expand(MPoint(0,0,0));
	//result.expand(MPoint(2,2,2));
	return box;
}

void* vixo_ctrlCurves::creator()
{
	return new vixo_ctrlCurves();
}

MStatus vixo_ctrlCurves::initialize()
{ 
	MStatus stat;
	MFnTypedAttribute tAttr;
	MFnNumericAttribute nAttr;
	MFnMessageAttribute mAttr;

	//MFnUnitAttribute uAttr;
	//inTime=uAttr.create("inTime","it",MFnUnitAttribute::kTime);

	clumpInfoFileName=tAttr.create("clumpInfoFileName","cifn",MFnData::kString);
	parametersFileName=tAttr.create("parametersFileName","pfn",MFnData::kString);

	hairSysMsg=mAttr.create("hairSysMsg","hsm");
	hiddenOutput=nAttr.create("hiddenOutput","hop",MFnNumericData::kInt,1);

	//hiddenOutClumpCenter=nAttr.create("hiddenOutClumpCenter","hocc",MFnNumericData::kInt,1);

	inCtrlCurveData=tAttr.create("inCtrlCurveData","icd",MFnData::kVectorArray);
	tAttr.setArray(true);
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	tAttr.setHidden(true);

	staticMesh=tAttr.create("staticInMesh","sim",MFnData::kMesh);
	tAttr.setStorable(false);
	tAttr.setHidden(true);
	inMesh=tAttr.create("dynamicInMesh","dim",MFnData::kMesh);
	tAttr.setStorable(false);
	tAttr.setHidden(true);

	showCtrl=nAttr.create("showCtrl","sc",MFnNumericData::kInt,20);
	nAttr.setMin(0);
	nAttr.setMax(100);
	densityMap=nAttr.createColor("densityMap","dm");
	nAttr.setDefault(0.0,0.0,0.0);
	degree=nAttr.create("degree","deg",MFnNumericData::kInt,2);
	nAttr.setMin(1);
	nAttr.setMax(3);
	hiddenOutCurveNum=nAttr.create("hiddenOutCurveNum","hon",MFnNumericData::kInt,0);

	hiddenOutCurveDataBase=nAttr.create("hiddenOutCurveDataBase","hodb",MFnNumericData::kInt,0);
	boundBoxExpand=nAttr.create("boundingBoxExpand","bbe",MFnNumericData::k3Float,1.2);
	miscBoundBoxExpend=nAttr.create("miscBoundBoxExpand","mbbe",MFnNumericData::kFloat,3.5);
	nAttr.setSoftMin(1.0f);
	nAttr.setSoftMax(10.0f);

	segment=nAttr.create("segment","seg",MFnNumericData::kInt,15);
	nAttr.setSoftMin(1);
	nAttr.setSoftMax(100);

	//follicle file
	hairNum=nAttr.create("hairNum","hn",MFnNumericData::kInt,0);
	hairRootFileName=tAttr.create("hairRootFileName","hrfn",MFnData::kString);
	hiddenOutputBasicInfo=nAttr.create("hiddenOutputBasicInfo","hobi",MFnNumericData::kInt,0);

	//cache File
	hairCacheFileName=tAttr.create("hairCacheFileName","hcfn",MFnData::kString);
	renderUseCache=nAttr.create("renderUseCache","ruc",MFnNumericData::kBoolean,false);

	tipColor=nAttr.createColor("tipColor","tc");
	nAttr.setDefault(0.0f,0.0f,0.0f);
	tipColorNoiseAmp=nAttr.create("tipColorNoiseAmp","tca",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	tipColorNoiseFreq=nAttr.create("tipColorNoiseFreq","tcf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);

	baseColor=nAttr.createColor("baseColor","bc");
	nAttr.setDefault(0.0f,0.0f,0.0f);
	baseColorNoiseAmp=nAttr.create("baseColorNoiseAmp","bca",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	baseColorNoiseFreq=nAttr.create("baseColorNoiseFreq","bcf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);

	tipOpacity=nAttr.createColor("tipOpacity","to");
	nAttr.setDefault(0.0f,0.0f,0.0f);
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

	tipWidth=nAttr.create("tipWidth","twv",MFnNumericData::kFloat,0.1);
	nAttr.setMin(0.0);
	nAttr.setSoftMax(0.05);
	tipWidthNoiseAmp=nAttr.create("tipWidthNoiseAmp","twa",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	tipWidthNoiseFreq=nAttr.create("tipWidthNoiseFreq","twf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);
	
	baseWidth=nAttr.create("baseWidth","bw",MFnNumericData::kFloat,1.0);
	nAttr.setMin(0.0);
	nAttr.setSoftMax(0.1);
	baseWidthNoiseAmp=nAttr.create("baseWidthNoiseAmp","bwa",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	baseWidthNoiseFreq=nAttr.create("baseWidthNoiseFreq","bwf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);

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

	scraggleMap=nAttr.createColor("scraggleMap","sm");
	nAttr.setDefault(0.0f,0.0f,0.0f);
	scraggleNoiseAmp=nAttr.create("scraggleNoiseAmp","sna",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	scraggleNoiseFreq=nAttr.create("scraggleNoiseFreq","snf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);

	scraggleFrequencyMap=nAttr.createColor("scraggleFrequencyMap","sfm");
	nAttr.setDefault(0.0f,0.0f,0.0f);
	scraggleFrequencyNoiseAmp=nAttr.create("scraggleFrequencyNoiseAmp","sfna",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	scraggleFrequencyNoiseFreq=nAttr.create("scraggleFrequencyNoiseFreq","sfnf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);

	clumpMap=nAttr.createColor("clumpMap","cm");
	nAttr.setDefault(0.0f,0.0f,0.0f);
	clumpNoiseAmp=nAttr.create("clumpNoiseAmp","cna",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	clumpNoiseFreq=nAttr.create("clumpNoiseFreq","cnf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);

	clumpFrequencyMap=nAttr.createColor("clumpFrequencyMap","cfm");
	nAttr.setDefault(0.0f,0.0f,0.0f);
	clumpFrequencyNoiseAmp=nAttr.create("clumpFrequencyNoiseAmp","cfna",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	clumpFrequencyNoiseFreq=nAttr.create("clumpFrequencyNoiseFreq","cfnf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);

	clumpShapeMap=nAttr.createColor("clumpShapeMap","csm");
	nAttr.setDefault(0.0f,0.0f,0.0f);
	clumpShapeNoiseAmp=nAttr.create("clumpShapeNoiseAmp","csna",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(1.0);
	clumpShapeNoiseFreq=nAttr.create("clumpShapeNoiseFreq","csnf",MFnNumericData::kFloat,0.0);
	nAttr.setMin(0.0);
	nAttr.setMax(100.0);


	addAttribute(staticMesh);
	addAttribute(inMesh);
	addAttribute(degree);
	addAttribute(densityMap); 
	addAttribute(showCtrl);
	addAttribute(segment);
	
	addAttribute(hairSysMsg); 
	addAttribute(boundBoxExpand);
	addAttribute(miscBoundBoxExpend);
	addAttribute(inCtrlCurveData); 
	addAttribute(hiddenOutCurveDataBase);
	addAttribute(hiddenOutCurveNum);
	addAttribute(hiddenOutput);

	addAttribute(clumpInfoFileName);
	addAttribute(parametersFileName);

	addAttribute(hairNum);
	addAttribute(hairRootFileName); 
	addAttribute(hiddenOutputBasicInfo);
	addAttribute(hairCacheFileName);
	addAttribute(renderUseCache);

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
	addAttribute(tipWidth);
	addAttribute(tipWidthNoiseAmp);
	addAttribute(tipWidthNoiseFreq);
	addAttribute(baseWidth);
	addAttribute(baseWidthNoiseAmp);
	addAttribute(baseWidthNoiseFreq);
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

	addAttribute(scraggleMap);
	addAttribute(scraggleNoiseAmp);
	addAttribute(scraggleNoiseFreq);

	addAttribute(scraggleFrequencyMap);
	addAttribute(scraggleFrequencyNoiseAmp);
	addAttribute(scraggleFrequencyNoiseFreq);

	addAttribute(clumpMap);
	addAttribute(clumpNoiseAmp);
	addAttribute(clumpNoiseFreq);

	addAttribute(clumpFrequencyMap);
	addAttribute(clumpFrequencyNoiseAmp);
	addAttribute(clumpFrequencyNoiseFreq);

	//addAttribute(hiddenOutClumpCenter);

	addAttribute(clumpShapeMap);
	addAttribute(clumpShapeNoiseAmp);
	addAttribute(clumpShapeNoiseFreq);

	attributeAffects(staticMesh,hiddenOutCurveDataBase);
	attributeAffects(densityMap,hiddenOutCurveNum);
	attributeAffects(showCtrl,hiddenOutCurveNum);
	attributeAffects(inCtrlCurveData,hiddenOutput);
	attributeAffects(inMesh,hiddenOutput);
	attributeAffects(degree,hiddenOutput);
	attributeAffects(hairSysMsg,hiddenOutput);
	attributeAffects(boundBoxExpand,hiddenOutput);
	attributeAffects(hairNum,hiddenOutputBasicInfo);
	attributeAffects(hairRootFileName,hiddenOutputBasicInfo);

	attributeAffects(clumpFrequencyMap,hiddenOutputBasicInfo);
	attributeAffects(clumpFrequencyNoiseAmp,hiddenOutputBasicInfo);
	attributeAffects(clumpFrequencyNoiseFreq,hiddenOutputBasicInfo);
	attributeAffects(clumpInfoFileName,hiddenOutputBasicInfo);
	attributeAffects(staticMesh,hiddenOutputBasicInfo);

	return MS::kSuccess;
}


MStatus vixo_ctrlCurves::setDependentsDirty(const MPlug&  plugBeingDirtied, MPlugArray&  affectedPlugs)
{
	MPlug plugOutput(thisMObject(),hiddenOutput);
	//MPlug plugSelf(thisNode,this->currentOutCurves);
	//MPlug plugSelfEle=plugSelf.elementByLogicalIndex(plugBeingDirtied.logicalIndex());
	//cout<<"dir"<<plugBeingDirtied.info().asChar()<<endl;
	if(plugBeingDirtied.array().isArray()==true&&plugBeingDirtied.array().partialName()=="icd")
	{
		//cout<<"dirty"<<endl;
		affectedPlugs.append(plugOutput);
		//affectedPlugs.append(plugSelfEle);
	}
	return MS::kSuccess;
}


vixo_ctrlCurvesData* vixo_ctrlCurves::data()
{
	return &vixoDrawData;
}

MStatus vixo_ctrlCurves::connectionMade(const MPlug& plug,const MPlug& otherPlug,bool asSrc)
{
	MStatus stat;
	if(plug==hairSysMsg)
	{
		MPlug plugStaticMesh(thisMObject(),staticMesh);
		MPlugArray arrMesh;
		plugStaticMesh.connectedTo(arrMesh,true,false);
		if(arrMesh.length()<=0)
		{
			MGlobal::displayError("no static mesh connected!");
			return MS::kSuccess;
		}

		MPlugArray arr;
		plug.connectedTo(arr,true,false);
		MFnDependencyNode fnHairSystem(arr[0].node());
		MPlug outputHairs=fnHairSystem.findPlug("outputHair",&stat);

		MDGModifier dgmod;
		
		MIntArray indexes;
		int numEle=outputHairs.getExistingArrayAttributeIndices(indexes);
		for(int i=0;i<numEle;i++)
		{
			MPlugArray tempArr;
			outputHairs.elementByLogicalIndex(indexes[i]).connectedTo(tempArr,false,true);

			if(tempArr.length()==0)
				continue;
			
			MObject thisObj=thisMObject();
			MPlug curInCurvePlug(thisObj,inCtrlCurveData);
			
			dgmod.connect(outputHairs.elementByLogicalIndex(indexes[i]),curInCurvePlug.elementByLogicalIndex(indexes[i]));

			
		}
		stat=dgmod.doIt();

		MStringArray nodesName;
		plug.info().split('.',nodesName);
		msgData.vixoNodeName=nodesName[0];
		msgData.id=MNodeMessage::addAttributeChangedCallback(arr[0].node(),hairSysMsgCallBack,&msgData);
		
	}
	
	/*else if(plug.array()==inCtrlCurveData)
	{

		MPlugArray arr;
		plug.connectedTo(arr,true,false);
		MFnDependencyNode fnHairSystem(arr[0].node());
		MPlug inputHairs=fnHairSystem.findPlug("inputHair",&stat);
		MPlugArray tempArr;
		inputHairs.elementByLogicalIndex(plug.logicalIndex()).connectedTo(tempArr,true,false);

		MFnDependencyNode fnFolli(tempArr[0].node());
		MPlug VIDPlug=fnFolli.findPlug("vertexID");
		currentInVid.insert(VIDPlug.asInt());
		inVIDMapPlug.insert(pair<int,int>(VIDPlug.asInt(),plug.logicalIndex()));
		plugMapInVID.insert(pair<int,int>(plug.logicalIndex(),VIDPlug.asInt()));

		cout<<"insert00:"<<VIDPlug.asInt()<<" "<<plug.logicalIndex()<<endl;

		MFnVectorArrayData fnArray(plug.asMObject());
		maxSpan=max(maxSpan,(int)(fnArray.array().length()-1));

		MPlug plugStaticMesh(thisMObject(),staticMesh);
		MPlugArray arrMesh;
		plugStaticMesh.connectedTo(arrMesh,true,false);
		initBasicStepInfo(VIDPlug.asInt(),arrMesh[0].node());
		return MS::kSuccess;
	}
	*/
	return MPxNode::connectionMade( plug, otherPlug, asSrc );

}

void vixo_ctrlCurves::initBasicStepInfo(int vid,MDataBlock & datablock)
{
	
	inVertexBasicInfo ele;
	//ele.stepVertex.resize(MAXSTEP);
	//map<int,inVertexBasicInfo>::iterator iter=
	inVIDMapInVertexDataBase.insert(pair<int,inVertexBasicInfo>(vid,ele));
	map<int,inVertexBasicInfo>::iterator iter=inVIDMapInVertexDataBase.find(vid);
	//map<int,inVertexBasicInfo>::iterator iter=inVIDMapInVertexDataBase.insert(vid);
	iter->second.stepVertex.resize(MAXSTEP);

	forInVertexBasicInfo subele;
	subele.vertexID=vid;
	subele.distance=0;
	iter->second.stepVertex[0].insert(subele);

	MItMeshVertex iterVertex4Record(datablock.inputValue(staticMesh).asMesh());
	MIntArray tempConnectedVertex;
	int prevIndex;

	MFnMesh fnMesh(datablock.inputValue(staticMesh).asMesh());
	MPointArray allPoints;
	fnMesh.getPoints(allPoints,MSpace::kObject);

	set<int> checkedVertex;
	checkedVertex.insert(vid);

	set<forInVertexBasicInfo>::iterator lastStepSetIter;
	for(int step=1;step<MAXSTEP;step++)
	{
		for(lastStepSetIter=iter->second.stepVertex[step-1].begin();lastStepSetIter!=iter->second.stepVertex[step-1].end();lastStepSetIter++)
		{
			iterVertex4Record.setIndex(lastStepSetIter->vertexID,prevIndex);
			iterVertex4Record.getConnectedVertices(tempConnectedVertex);
			for(int j=0;j<tempConnectedVertex.length();j++)
			{
				if(checkedVertex.count(tempConnectedVertex[j])<=0)
				{
					forInVertexBasicInfo subeleTemp;
					subeleTemp.vertexID=tempConnectedVertex[j];
					subeleTemp.distance=allPoints[vid].distanceTo(allPoints[subeleTemp.vertexID]);
					iter->second.stepVertex[step].insert(subeleTemp);
					checkedVertex.insert(tempConnectedVertex[j]);
				}
			}
		}
	}

}

void hairSysMsgCallBack(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData)
{
	//MStatus stat=MMessage::removeCallback(((messageData*)clientData)->id);
	//cout<<"test"<<stat.errorString()<<endl;
	//cout<<"aaa"<<plug.info().asChar()<<" "<<otherPlug.info().asChar()<<endl;
	if(msg&MNodeMessage::AttributeMessage::kConnectionMade)
	{
		MStringArray attrName;
		plug.array().name().split('.',attrName);
		if(attrName[1]=="inputHair")
		{
			//cout<<"conn"<<plug.info().asChar()<<endl;
			//int idx=plug.logicalIndex();
			MSelectionList slist;
			slist.clear();
			MGlobal::getSelectionListByName(attrName[0],slist);
			MDagPath dagPath;
			slist.getDagPath(0,dagPath);
			MFnDagNode fnHairSys(dagPath);
			MPlug outputHairPlug=fnHairSys.findPlug("outputHair");

			slist.clear();
			MGlobal::getSelectionListByName(((messageData*)clientData)->vixoNodeName,slist);
			MDagPath vixoPath;
			slist.getDagPath(0,vixoPath);
			MFnDagNode fnVixo(vixoPath);

			MPlug curInCurvePlug=fnVixo.findPlug("inCtrlCurveData");
			MDGModifier dgmod;
			dgmod.connect(outputHairPlug.elementByLogicalIndex(plug.logicalIndex()),curInCurvePlug.elementByLogicalIndex(plug.logicalIndex()));

			//MFnDependencyNode fnFolli(tempArr[0].node());
			//MPlug VIDPlug=fnFolli.findPlug("vertexID");
			//currentInVid.insert(VIDPlug.asInt());
			///inVIDMapPlug.insert(pair<int,int>(VIDPlug.asInt(),plug.logicalIndex()));
			//plugMapInVID.insert(pair<int,int>(plug.logicalIndex(),VIDPlug.asInt()));
			//initBasicStepInfo(VIDPlug.asInt(),arrMesh[0].node());

			dgmod.doIt();
		}
	}
	else if(msg&MNodeMessage::AttributeMessage::kConnectionBroken)
	{
		MStringArray attrName;
		plug.array().name().split('.',attrName);
		if(attrName[1]=="inputHair")
		{
			MSelectionList slist;
			slist.clear();
			MGlobal::getSelectionListByName(attrName[0],slist);
			MDagPath dagPath;
			slist.getDagPath(0,dagPath);
			MFnDagNode fnHairSys(dagPath);
			MPlug outputHairPlug=fnHairSys.findPlug("outputHair");

			slist.clear();
			MGlobal::getSelectionListByName(((messageData*)clientData)->vixoNodeName,slist);
			MDagPath vixoPath;
			slist.getDagPath(0,vixoPath);
			MFnDagNode fnVixo(vixoPath);
			MPlug curInCurvePlug=fnVixo.findPlug("inCtrlCurveData");

			MDGModifier dgmod;
			dgmod.disconnect(outputHairPlug.elementByLogicalIndex(plug.logicalIndex()),curInCurvePlug.elementByLogicalIndex(plug.logicalIndex()));
		
			dgmod.doIt();
		}
	}
}

MStatus vixo_ctrlCurves::connectionBroken(const MPlug& plug,const MPlug& otherPlug,bool asSrc)
{
	if(plug==hairSysMsg)
	{
		MMessage::removeCallback(msgData.id);
		return MS::kSuccess;
	}
	else if(plug.array()==inCtrlCurveData)
	{
		int vid=plugMapInVID.find(plug.logicalIndex())->second;
		currentInVid.erase(vid);
		inVIDMapPlug.erase(vid);
		plugMapInVID.erase(plug.logicalIndex());
		inVIDMapTangents.erase(vid);
		inVIDMapInVertexDataBase.erase(vid);
		deleteInVertex(vid);
		return MS::kSuccess;
	}
	return MPxNode::connectionBroken( plug, otherPlug, asSrc );
}

void vixo_ctrlCurves::deleteInVertex(int inVID)
{
	map<int,set<outInVertexRelationInfo>>::iterator outDBIter;
	set<outInVertexRelationInfo>::iterator relationIter;
	for(outDBIter=outVertexDataBase.begin();outDBIter!=outVertexDataBase.end();outDBIter++)
	{
		bool flagUpdate=false;
		for(relationIter=outDBIter->second.begin();relationIter!=outDBIter->second.end();relationIter++)
		{
			if(relationIter->inVID==inVID)
			{
				outDBIter->second.erase(relationIter);
				flagUpdate=true;
				break;
			}
		}
		if(flagUpdate)
			sortControlOrder(outDBIter->first);
	}
}

void vixo_ctrlCurves::exportFollicle(int hairNumValue,MString hairRootInfo,MDataBlock& data)
{
	fstream fin(hairRootInfo.asChar(),ios_base::in|ios_base::binary);
	int oldHairNum;
	fin.read((char*)&oldHairNum,sizeof(int));
	fin.close();

	if(oldHairNum==hairNumValue)
		return;


	MObject meshObj=data.inputValue(staticMesh).asMesh();
		
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

	vector<triangleBasicInfo> triBasicData(triTotNum);
	vector<hairDetail> triDetailData;
	
	//double diff=sqrt(area/hairNumValue);
	srand(time(NULL));
	int triIter=0;
	for(iterFace.reset();!iterFace.isDone();iterFace.next())
	{

		double areaTmp;
		iterFace.getArea(areaTmp);
		int hairEleNum=areaTmp/area*hairNumValue;
		double diff=sqrt(areaTmp/hairEleNum);

		MIntArray faceVertexIndex;
		iterFace.getVertices(faceVertexIndex);
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

			triBasicData[triIter].faceID=iterFace.index();
			triBasicData[triIter].triID=tri;
			triBasicData[triIter].v1ID=vertexIndex[0];
			triBasicData[triIter].v2ID=vertexIndex[1];
			triBasicData[triIter].v3ID=vertexIndex[2];
			triBasicData[triIter].hairNum=0;

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
			/*
			uvs[0]=uvs[0]+0.8/(numLine1+1)*(uvs[2]-uvs[0]);
			uvs[1]=uvs[1]+0.8/(numLine1+1)*(uvs[3]-uvs[1]);
			uvs[0]=uvs[0]+0.8/(numLine2+1)*(uvs[4]-uvs[0]);
			uvs[1]=uvs[1]+0.8/(numLine2+1)*(uvs[5]-uvs[1]);
			uvs[2]=uvs[2]-0.8/(numLine1+1)*(uvs[2]-uvs[0]);
			uvs[3]=uvs[3]-0.8/(numLine1+1)*(uvs[3]-uvs[1]);
			uvs[2]=uvs[2]+0.8/(numLine2+1)*(uvs[4]-uvs[0]);
			uvs[3]=uvs[3]+0.8/(numLine2+1)*(uvs[5]-uvs[1]);
			uvs[4]=uvs[4]-0.8/(numLine2+1)*(uvs[4]-uvs[0]);
			uvs[5]=uvs[5]-0.8/(numLine2+1)*(uvs[5]-uvs[1]);
			uvs[4]=uvs[4]+0.8/(numLine1+1)*(uvs[2]-uvs[0]);
			uvs[5]=uvs[5]+0.8/(numLine1+1)*(uvs[3]-uvs[1]);
			*/
			

			for(int i=0;i<numLine1-1;i++)
			{
				for(int j=0;j<(1-(float)i/(numLine1))*(numLine2)-1;j++)
				{
					float rand0=rand()/(RAND_MAX+1.0f);
					float rand1=rand()/(RAND_MAX+1.0f);
					hairDetail ele;
					ele.w0=((float)i+rand0)/numLine1;
					ele.w1=((float)j+rand1)/numLine2;
					ele.w2=1-ele.w0-ele.w1;
					ele.u=uvs[0]*ele.w2+uvs[2]*ele.w0+uvs[4]*ele.w1;
					ele.v=uvs[1]*ele.w2+uvs[3]*ele.w0+uvs[5]*ele.w1;
					//ele.u=uvs[0]*ele.w0+uvs[2]*ele.w1+uvs[4]*ele.w2;
					//ele.v=uvs[1]*ele.w0+uvs[3]*ele.w1+uvs[5]*ele.w2;
					ele.triIdx=triIter; 
					//cout<<rand0<<" "<<rand1<<endl;
					//if(ele.u<0||ele.v<0||ele.u>1||ele.v>0)
					//	cout<<"apple"<<endl;
					triDetailData.push_back(ele);
					triBasicData[triIter].hairNum++;
				}
			}

			triIter++;
		}
		if(iterFace.isDone())
			break;
	}

	//write to file
	triBasicData[0].beginIdx=sizeof(int)*3+sizeof(triangleBasicInfo)*triTotNum;
	triBasicData[0].hairBegin=0;
	for(int i=1;i<triTotNum;i++)
	{
		triBasicData[i].beginIdx=triBasicData[i-1].beginIdx.operator+(sizeof(hairDetail)*triBasicData[i-1].hairNum);
		triBasicData[i].hairBegin=triBasicData[i-1].hairBegin+triBasicData[i-1].hairNum;
	}
	int hairNumTriNum[3];
	hairNumTriNum[0]=hairNumValue;
	hairNumTriNum[1]=triTotNum;
	hairNumTriNum[2]=triDetailData.size();
	fstream fout(hairRootInfo.asChar(),ios_base::out|ios_base::binary);
	fout.write((char*)hairNumTriNum,sizeof(int)*3);
	fout.write((char*)&triBasicData[0],sizeof(triangleBasicInfo)*triTotNum);
	fout.write((char*)&triDetailData[0],sizeof(hairDetail)*triDetailData.size());
	fout.flush();
	fout.close();
	//~write to file
	
}

bool vixo_ctrlCurves::isRenderable()
{
	return true;
}

vixo_ctrlCurvesUI::vixo_ctrlCurvesUI() {}
vixo_ctrlCurvesUI::~vixo_ctrlCurvesUI() {}

void* vixo_ctrlCurvesUI::creator()
{
	return new vixo_ctrlCurvesUI();
}


void vixo_ctrlCurvesUI::getDrawRequests( const MDrawInfo & info, bool objectAndActiveOnly, MDrawRequestQueue & queue )
{
	MDrawData data;
	MDrawRequest request = info.getPrototype( *this );
	vixo_ctrlCurves* shapeNode = (vixo_ctrlCurves*)surfaceShape();
	vixo_ctrlCurvesData * vixoData=shapeNode->data();
	getDrawData( vixoData, data );
	request.setDrawData( data );

	request.setToken( info.displayStyle() );

	M3dView::DisplayStatus displayStatus = info.displayStatus();
	M3dView::ColorTable activeColorTable = M3dView::kTemplateColor;
	M3dView::ColorTable dormantColorTable = M3dView::kDormantColors;
	switch ( displayStatus )
	{
		case M3dView::kLead :
			request.setColor( LEAD_COLOR, activeColorTable );
			break;
		case M3dView::kActive :
			request.setColor( ACTIVE_COLOR, activeColorTable );
			break;
		case M3dView::kActiveAffected :
			request.setColor( ACTIVE_AFFECTED_COLOR, activeColorTable );
			break;
		case M3dView::kDormant :
			request.setColor( DORMANT_COLOR, activeColorTable );
			break;
		case M3dView::kHilite :
			request.setColor( HILITE_COLOR, activeColorTable );
			break;
		default:	
			break;
	}
	queue.add( request );
}

void vixo_ctrlCurvesUI::draw( const MDrawRequest & request, M3dView & view ) const
{
	//cout<<"draw"<<endl;
	
	if(request.token()==M3dView::kBoundingBox)
	{
		//cout<<"bounding"<<endl;

		MPxSurfaceShape *shape = surfaceShape();
		if ( !shape )
			return;

		// Get the bounding box	
		MBoundingBox box = shape->boundingBox();
		float w = (float) box.width();
		float h = (float) box.height();
		float d = (float) box.depth();
		view.beginGL(); 

		static MGLFunctionTable *gGLFT = 0;
		if ( 0 == gGLFT )
			gGLFT = MHardwareRenderer::theRenderer()->glFunctionTable();

		// Below we just two sides and then connect
		// the edges together

		MPoint minVertex = box.min();

		// Draw first side
		gGLFT->glBegin( GL_LINE_LOOP );
		MPoint vertex = minVertex;
		gGLFT->glVertex3f( (float)vertex[0],   (float)vertex[1],   (float)vertex[2] );
		gGLFT->glVertex3f( (float)vertex[0]+w, (float)vertex[1],   (float)vertex[2] );
		gGLFT->glVertex3f( (float)vertex[0]+w, (float)vertex[1]+h, (float)vertex[2] );
		gGLFT->glVertex3f( (float)vertex[0],   (float)vertex[1]+h, (float)vertex[2] );	
		gGLFT->glVertex3f( (float)vertex[0],   (float)vertex[1],   (float)vertex[2] );
		gGLFT->glEnd();

		// Draw second side
		MPoint sideFactor(0,0,d);
		MPoint vertex2 = minVertex + sideFactor;
		gGLFT->glBegin( GL_LINE_LOOP );
		gGLFT->glVertex3f( (float)vertex2[0],   (float)vertex2[1],   (float)vertex2[2] );	
		gGLFT->glVertex3f( (float)vertex2[0]+w, (float)vertex2[1],   (float)vertex2[2] );
		gGLFT->glVertex3f( (float)vertex2[0]+w, (float)vertex2[1]+h, (float)vertex2[2] );
		gGLFT->glVertex3f( (float)vertex2[0],   (float)vertex2[1]+h, (float)vertex2[2] );	
		gGLFT->glVertex3f( (float)vertex2[0],   (float)vertex2[1],   (float)vertex2[2] );	
		gGLFT->glEnd();

		// Connect the edges together
		gGLFT->glBegin( GL_LINES );
		gGLFT->glVertex3f( (float)vertex2[0],   (float)vertex2[1],   (float)vertex2[2] );	
		gGLFT->glVertex3f( (float)vertex[0],   (float)vertex[1],   (float)vertex[2] );

		gGLFT->glVertex3f( (float)vertex2[0]+w,   (float)vertex2[1],   (float)vertex2[2] );	
		gGLFT->glVertex3f( (float)vertex[0]+w,   (float)vertex[1],   (float)vertex[2] );

		gGLFT->glVertex3f( (float)vertex2[0]+w,   (float)vertex2[1]+h,   (float)vertex2[2] );	
		gGLFT->glVertex3f( (float)vertex[0]+w,   (float)vertex[1]+h,   (float)vertex[2] );

		gGLFT->glVertex3f( (float)vertex2[0],   (float)vertex2[1]+h,   (float)vertex2[2] );	
		gGLFT->glVertex3f( (float)vertex[0],   (float)vertex[1]+h,   (float)vertex[2] );
		gGLFT->glEnd();

		view.endGL(); 
	}
	MDrawData data = request.drawData();
	vixo_ctrlCurvesData * vixoData=(vixo_ctrlCurvesData*)data.geometry();

	static MGLFunctionTable *gGLFT = 0;
	if ( 0 == gGLFT )
		gGLFT = MHardwareRenderer::theRenderer()->glFunctionTable();
	/*

	int order=2;// degree=1
	vector<::GLfloat> knots(8);
	for(int i=order-1;i<=knots.size()-order;i++)
	{
		knots[i]=i-order+1;
	}
	knots[0]=knots[1];
	knots[knots.size()-1]=knots[knots.size()-2];

	
	int order1=3;// degree=2
	vector<::GLfloat> knots1(9);
	for(int i=order1-1;i<=knots1.size()-order1;i++)
	{
		knots1[i]=i-order1+1;
	}
	knots1[0]=knots1[2];
	knots1[1]=knots1[2];
	knots1[knots1.size()-1]=knots1[knots1.size()-3];
	knots1[knots1.size()-2]=knots1[knots1.size()-3];
	

	vector<::GLfloat> cvs(6*3);
	cvs[0]=0;
	cvs[1]=0;
	cvs[2]=0;

	cvs[3]=-0.0326143770929;
	cvs[4]=0.395816610136;
	cvs[5]=0.0475974111447;
	            
	cvs[6]=-0.108146269891;
	cvs[7]=0.773779736454;
	cvs[8]=0.15454977584;

	cvs[9]=-0.271379897529;
	cvs[10]=1.06582300166;
	cvs[11]=0.373788262554;

	cvs[12]=-0.514023289076;
	cvs[13]=1.1715518357;
	cvs[14]=0.673697579166;

	cvs[15]=-0.719565836528;
	cvs[16]=1.03183799557;
	cvs[17]=0.987250575969;
	*/

	//cout<<"draw"<<endl;

	view.beginGL();
	//gGLFT->glBegin(GL_LINE_STRIP);
	//gGLFT->glVertex3f(0.0f,0.0f,0.0f);
	//gGLFT->glVertex3f(1.0f,1.0f,0.0f);
	//gGLFT->glVertex3f(0.0f,1.0f,1.0f);
	//gGLFT->glVertex3f(1.0f,0.0f,1.0f);
	//gGLFT->glEnd();
	::GLUnurbsObj *theNurbs=::gluNewNurbsRenderer();
	/*::gluBeginCurve(theNurbs);
	::gluNurbsCurve(theNurbs,knots.size(),&knots[0],3,&cvs[0],order,GL_MAP1_VERTEX_3);
	::gluEndCurve(theNurbs);

	::gluBeginCurve(theNurbs);
	::gluNurbsCurve(theNurbs,knots1.size(),&knots1[0],3,&cvs[0],order1,GL_MAP1_VERTEX_3);
	::gluEndCurve(theNurbs);
	*/
	int strip=vixoData->knotsSize-vixoData->degree-1;
	for(int i=0;i<vixoData->curveNum;i++)
	{
		::gluBeginCurve(theNurbs);
		::gluNurbsCurve(theNurbs,vixoData->knotsSize,&(vixoData->knots[0]),3,&vixoData->allCurves[i*strip*3],vixoData->degree+1,GL_MAP1_VERTEX_3);
		::gluEndCurve(theNurbs);
	}
	view.endGL(); 
}