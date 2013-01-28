#include "vixo_hairCurves.h"

#include <maya\MFnTypedAttribute.h>
#include <maya\MFnNumericAttribute.h>
#include <maya\MFnCompoundAttribute.h>
#include <maya\MFnNumericData.h>

#include <maya\MFnMesh.h>
#include <maya\MFnMeshData.h>

#include <maya\MIntArray.h>
#include <maya\MPointArray.h>
#include <maya\MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya\MFnNurbsCurve.h>

#include <maya\MPlug.h>
#include <maya\MPlugArray.h>
#include <maya\MImage.h>
#include <maya\MItMeshVertex.h>
#include <maya\MItMeshPolygon.h>
#include <maya\MFnDependencyNode.h>

#include <maya\MQuaternion.h>
#include <maya\MEulerRotation.h>


#include <maya\MTransformationMatrix.h>

#include <maya\MGlobal.h>

#include <fstream>

MTypeId vixo_hairCurves::id(0x25897);

MObject vixo_hairCurves::inMesh;
MObject vixo_hairCurves::inCurves;
	MObject vixo_hairCurves::curveShape;
	MObject vixo_hairCurves::vertexID;

MObject vixo_hairCurves::refresh;
MObject vixo_hairCurves::densityMap;
MObject vixo_hairCurves::showControl;

MObject vixo_hairCurves::outCurvesNum;
MObject vixo_hairCurves::outCurves;
	MObject vixo_hairCurves::outCurveShape;
	MObject vixo_hairCurves::outU;
	MObject vixo_hairCurves::outV;
	MObject vixo_hairCurves::show;


vixo_hairCurves::vixo_hairCurves(void)
{
	//firstTime=true;
}


vixo_hairCurves::~vixo_hairCurves(void)
{
}

void* vixo_hairCurves::creator()
{
	return new vixo_hairCurves();
}

MStatus vixo_hairCurves::initialize()
{
	MFnTypedAttribute tAttr;
	inMesh=tAttr.create("inMesh","im",MFnData::kMesh);
	curveShape=tAttr.create("curveShape","cs",MFnData::kNurbsCurve);
	tAttr.setStorable(false);
	tAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	outCurveShape=tAttr.create("outCurveShape","ocs",MFnData::kNurbsCurve);

	MFnNumericAttribute nAttr;
	vertexID=nAttr.create("vertexID","vid",MFnNumericData::kInt,-1);
	nAttr.setStorable(false);
	nAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	refresh=nAttr.create("refresh","rf",MFnNumericData::kBoolean,false);
	densityMap=nAttr.createColor("densityMap","den");
	showControl=nAttr.create("showControl","sct",MFnNumericData::kFloat,1.0);
	outU=nAttr.create("outU","ou",MFnNumericData::kFloat,0.0);
	outV=nAttr.create("outV","ov",MFnNumericData::kFloat,0.0);
	show=nAttr.create("show","s",MFnNumericData::kBoolean,true);
	outCurvesNum=nAttr.create("outCurvesNum","ocn",MFnNumericData::kInt,0);

	MFnCompoundAttribute cAttr;
	inCurves=cAttr.create("inCurves","ic");
	cAttr.addChild(vertexID);
	cAttr.addChild(curveShape);
	cAttr.setArray(true);
	cAttr.setStorable(false);
	cAttr.setDisconnectBehavior(MFnAttribute::kDelete);

	outCurves=cAttr.create("outCurves","oc");
	cAttr.addChild(outCurveShape);
	cAttr.addChild(outU);
	cAttr.addChild(outV);
	cAttr.addChild(show);
	cAttr.setArray(true);

	addAttribute(inMesh);
	addAttribute(inCurves);
	addAttribute(vertexID);
	addAttribute(curveShape);
	addAttribute(refresh);
	addAttribute(densityMap);
	addAttribute(showControl);
	addAttribute(outCurves);
	addAttribute(outCurveShape);
	addAttribute(outU);
	addAttribute(outV);
	addAttribute(show);

	attributeAffects(vertexID,outCurveShape);
	attributeAffects(inMesh,outCurveShape);
	attributeAffects(curveShape,outCurveShape);
	attributeAffects(densityMap,outCurveShape);
	attributeAffects(showControl,show);
	attributeAffects(refresh,outCurveShape);
	return MS::kSuccess;
}

MStatus vixo_hairCurves::compute(const MPlug& plug, MDataBlock& data)
{
	getAllInputCurvesInfo(data);
	return MS::kSuccess ;
}

void vixo_hairCurves::getAllInputCurvesInfo(MDataBlock& data)
{
	MArrayDataHandle inCurvesHandle=data.inputArrayValue(inCurves);
	int inCurvesNum=inCurvesHandle.elementCount();
	inputCurves.resize(inCurvesNum);

	//curves set
	newCurves.clear();
	newCurves.resize(inCurvesNum);

	//vector<
	MObjectArray allCurveObj(inCurvesNum);;
	int maxPointOnCurve=0;
	for(int i=0;i<inCurvesNum;i++)
	{	
		allCurveObj[i]=inCurvesHandle.inputValue().child(curveShape).asNurbsCurve();
		MFnNurbsCurve fnCurve(allCurveObj[i]);
		maxPointOnCurve=max(maxPointOnCurve,fnCurve.numSpans()+1);
		inputCurves[i].plugId=inCurvesHandle.elementIndex();
		inputCurves[i].vid=inCurvesHandle.inputValue().child(vertexID).asInt();
		inputCurves[i].normal=allVertexBasicInfo[inputCurves[i].vid].normal;
		inputCurves[i].pointsNum=fnCurve.numSpans()+1;

		//curves set
		newCurves[i]=inputCurves[i].vid;

		if(!inCurvesHandle.next())
			break;
	}

	for(int i=0;i<inCurvesNum;i++)
	{
		inputCurves[i].tanget.resize(maxPointOnCurve-1);
	}

	for(int i=0;i<inCurvesNum;i++)
	{
		MFnNurbsCurve fnCurve(allCurveObj[i]);
		if(inputCurves[i].pointsNum==maxPointOnCurve)
		{
			MPointArray cvs;
			fnCurve.getCVs(cvs,MSpace::kWorld);
			for(int j=0;j<maxPointOnCurve-1;j++)
			{
				inputCurves[i].tanget[j]=cvs[j+1]-cvs[j];
			}
			continue;
		}
		double paraMin,paraMax;
		fnCurve.getKnotDomain(paraMin,paraMax);
		for(int j=0;j<maxPointOnCurve-1;j++)
		{
			MPoint p1,p2;
			fnCurve.getPointAtParam(paraMin+(paraMax-paraMin)/(maxPointOnCurve-1)*j,p1,MSpace::kWorld);
			fnCurve.getPointAtParam(paraMin+(paraMax-paraMin)/(maxPointOnCurve-1)*(j+1),p2,MSpace::kWorld);
			inputCurves[i].tanget[j]=p2-p1;
		}
	}
}

MStatus vixo_hairCurves::connectionMade(const MPlug& plug,const MPlug& otherPlug,bool asSrc)
{
	if(plug==this->inMesh)
	{
		//get uv info
		MPlugArray arr;
		plug.connectedTo(arr,true,false);
		MItMeshVertex iterVert(arr[0].node());
		allVertexBasicInfo.resize(iterVert.count());
		for(iterVert.reset();!iterVert.isDone();iterVert.next())
		{
			allVertexBasicInfo[iterVert.index()].vid=iterVert.index();
			allVertexBasicInfo[iterVert.index()].position=iterVert.position(MSpace::kWorld);
			iterVert.getNormal(allVertexBasicInfo[iterVert.index()].normal);
			iterVert.getUV(allVertexBasicInfo[iterVert.index()].uv);
			allVertexBasicInfo[iterVert.index()].needCurve=true;
			iterVert.next();
		}
		//~get uv info
		return MS::kSuccess;
	}
	//else if(plug==this->densityMap)
	//{
	//	getDensityMapInfo();
	//}
	return MPxNode::connectionMade( plug, otherPlug, asSrc );

}

MStatus vixo_hairCurves::connectionBroken(const MPlug& plug,const MPlug& otherPlug,bool asSrc)
{
	if(plug==this->inMesh)
	{
		return MS::kSuccess;
	}
	return MPxNode::connectionMade( plug, otherPlug, asSrc );
}

void vixo_hairCurves::diffOldNewCurveSet(vector<int> oldSet,vector<int> newSet,MIntArray &delCurves,MIntArray &addCurves)
{
	delCurves.clear();
	addCurves.clear();

	for(int i=0;i<oldSet.size();i++)
	{
		if(!inSet(oldSet[i],newSet))
			delCurves.append(oldSet[i]);
	}

	for(int i=0;i<newCurves.size();i++)
	{
		if(!inSet(newSet[i],oldSet))
			addCurves.append(newSet[i]);
	}
}

bool vixo_hairCurves::inSet(int element,vector<int> setCheck)
{
	for(int i=0;i<setCheck.size();i++)
	{
		if(element==setCheck[i])
			return true;
	}
	return false;
}

void vixo_hairCurves::deletedCurve(MIntArray delCurves)
{
	
}

void vixo_hairCurves::initAddedCurveInfo(MIntArray addCurves,MDataBlock& data)
{
	//get nearest vertex info for each added curve
	const int MAXSTEP=100;
	//fstream fin("f:/horse.txt",ios_base::in);
	MItMeshVertex iterVertex4Record(data.inputValue(inMesh).asMesh());
	MIntArray tempConnectedVertex;
	int prevIndex;
	//vector<vector<vector<int>>> nearestPoints( addCurves.length());
	for(int i=0;i<addCurves.length();i++)
	{
		//MIntArray connectedFaces;
		inputCurves[i].nearestPoints.resize(MAXSTEP);
		inputCurves[i].nearestPoints[0].push_back(addCurves[i]);
	}
	for(int i=0;i<addCurves.length();i++)
	{
		set<int> checkedVertex;
		checkedVertex.insert(addCurves[i]);
		vector <int> :: iterator setIter;
		for(int step=1;step<MAXSTEP;step++)
		{
			for(setIter=inputCurves[i].nearestPoints[step-1].begin();setIter!=inputCurves[i].nearestPoints[step-1].end();setIter++)
			{
				iterVertex4Record.setIndex(*setIter,prevIndex);
				iterVertex4Record.getConnectedVertices(tempConnectedVertex);
				for(int j=0;j<tempConnectedVertex.length();j++)
				{
					if(checkedVertex.find(tempConnectedVertex[j])==checkedVertex.end())
					{
						inputCurves[i].nearestPoints[step].push_back(tempConnectedVertex[j]);
						checkedVertex.insert(tempConnectedVertex[j]);
					}
					/*
					//for each vertex(should be curved),
					//	if there is vertex__tempConnectedVertex[j],delete vertex__tempConnectedVertex[j] and break; 
					//	else add  vertex__tempConnectedVertex[j] and step info; 
					*/
				}
			}
			if(inputCurves[i].nearestPoints[step].size()==0)
				break;
		}
	}
	//~get nearest vertex info for each added curve

	//	for each vertex that should be curved:
	//		


}

void vixo_hairCurves::getDensityMapInfo(MDataBlock& data)
//collect info about vertex need to be curved & build outCurves Array 
{
	newOutCurves.clear();

	MPlugArray arr;
	MPlug plug(thisMObject(),this->densityMap);
	plug.connectedTo(arr,true,false);

	outCurvesInfo tempOutCurveInfo;
	tempOutCurveInfo.show=true;
	tempOutCurveInfo.oriCurved=false;

	//not connected to file texture node, all vertex shuold be curved
	if(arr.length()!=1)
	{
		for(int i=0;i<allVertexBasicInfo.size();i++)
		{
			newOutCurves.push_back(i);
			//allVertexBasicInfo[i].needCurve=true;
			outCurvesData.insert(pair<int,outCurvesInfo>(i,tempOutCurveInfo));
		}
		return;
	}
	//~not connected to file texture node, all vertex shuold be curved

	MFnDependencyNode dgFn(arr[0].node());
	MPlug fnFile=dgFn.findPlug("fileTextureName");

	//no file is real set, all vertex shuold be curved
	if(fnFile.asString()=="")
	{
		for(int i=0;i<allVertexBasicInfo.size();i++)
		{
			newOutCurves.push_back(i);
			//allVertexBasicInfo[i].needCurve=true;
			outCurvesData.insert(pair<int,outCurvesInfo>(i,tempOutCurveInfo));
		}
		return;
	}
	//~no file is real set, all vertex shuold be curved

	//init info for mor comfort computing and map refreshing
	for(int i=0;i<allVertexBasicInfo.size();i++)
		allVertexBasicInfo[i].needCurve=false;
	//init info for mor comfort computing and map refreshing

	MImage imgFn;
	imgFn.readFromTextureNode(arr[0].node());

	unsigned int imageWidth,imageHeight,imageDepth;
	imgFn.getSize(imageWidth,imageHeight);
	imageDepth=4;

	unsigned char * pixels=imgFn.pixels();

	MItMeshPolygon iterFace(data.inputValue(inMesh).asMesh());
	//for face that has nonzero density vertex, all vertex on it should be curved
	for(iterFace.reset();!iterFace.isDone();iterFace.next())
	{
		MFloatArray us,vs;
		iterFace.getUVs(us,vs);
		unsigned int pixelSum=0;
		for(int i=0;i<us.length();i++)
		{
			int pixelW=allVertexBasicInfo[i].uv[0]*imageWidth;
			int pixelH=allVertexBasicInfo[i].uv[1]*imageHeight;
			pixelSum+=(unsigned int)pixels[imageDepth*(pixelH*imageWidth+pixelW)];
		}
		MIntArray vertexArray;
		iterFace.getVertices(vertexArray);
		if(pixelSum>0)
		{
			for(int i=0;i<vertexArray.length();i++)
			{
				allVertexBasicInfo[vertexArray[i]].needCurve=true;
			}
		}
	}
	//for face that has nonzero density vertex, all vertex on it should be curved
	
	for(int i=0;i<allVertexBasicInfo.size();i++)
	{
		if(allVertexBasicInfo[i].needCurve)
		{
			newOutCurves.push_back(i);
			outCurvesData.insert(pair<int,outCurvesInfo>(i,tempOutCurveInfo));
		}
	}
	return;
}

void vixo_hairCurves::prepareForCurveCompute()
{
	MIntArray delOutCurves,addOutCurves;
	diffOldNewCurveSet(oldOutCurves,newOutCurves,delOutCurves,addOutCurves);

	//for delOutCurveSet,delete from map<int,outCurve>
	for(int i=0;i<delOutCurves.length();i++)
	{
		outCurvesData.erase(delOutCurves[i]);
	}
	//~for delOutCurveSet,delete from map<int,outCurve>

	//for each curve in oldInCurveSet, scan their nearestPoints of each step and add the info to each vertex
	//attention: distance is not computed
	relatedCurveInfo tempRelateCurve;
	map<int,outCurvesInfo>::iterator outCurveIter;
	for(int i=0;i<oldCurves.size();i++)
	{
		tempRelateCurve.vid=oldCurves[i];
		for(int stepIter=0;stepIter<inputCurves[i].nearestPoints.size();stepIter++)
		{
			tempRelateCurve.step=stepIter;
			for(int vIter=0;vIter<inputCurves[i].nearestPoints[stepIter].size();vIter++)
			{
				//count function
				outCurveIter=outCurvesData.find(inputCurves[i].nearestPoints[stepIter][vIter]);
				if(outCurveIter!=outCurvesData.end())
				{
					//outCurveIter->second.relatedCurves.push_back(tempRelateCurve);
					outCurveIter->second.relatedCurves.insert(pair<int,relatedCurveInfo>(oldCurves[i],tempRelateCurve));
				}
			}
		}
	}
	//for each curve in oldInCurveSet, scan their nearestPoints of each step and add the info to each vertex
}

void vixo_hairCurves::buildRelationShipInOut(MIntArray delCurves)
{
	//for deleted inCurves,delete nearestPoint Info of each outCurves
	//map<int,outCurvesInfo>::iterator outCurveIter;
	//vector<relatedCurveInfo>::iterator iterRelatedCurve;
	for(int i=0;i<delCurves.length();i++)
	{
		for(int j=0;j<outCurvesData.size();j++)
		{
			if(outCurvesData[j].relatedCurves.count(delCurves[i]))
				outCurvesData[j].relatedCurves.erase(delCurves[i]);
			//for(int k=0;k<outCurvesData[j].relatedCurves.size();k++)
			//{
			//	if(delCurves[i]==outCurvesData[j].relatedCurves[k].vid)
			//	{
			//		iterRelatedCurve=outCurvesData[j].relatedCurves[k];
			//	}
					//outCurvesData[j].relatedCurves.erase(k);
			//}
			//if(inSet(delCurves[i],outCurvesData[j].))
		}
	}
}

