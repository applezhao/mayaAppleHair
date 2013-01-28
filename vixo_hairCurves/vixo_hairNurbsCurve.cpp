#include "vixo_hairNurbsCurve.h"

#include <maya\MArrayDataBuilder.h>


MTypeId vixo_hairNurbsCurve::id(0x25897);

MObject vixo_hairNurbsCurve::inMesh;
MObject vixo_hairNurbsCurve::inCurves;
	MObject vixo_hairNurbsCurve::curveShape;
	MObject vixo_hairNurbsCurve::vertexID;

MObject vixo_hairNurbsCurve::densityMapRefresh;
MObject vixo_hairNurbsCurve::densityMap;
MObject vixo_hairNurbsCurve::showControl;
MObject vixo_hairNurbsCurve::stateRefresh;

MObject vixo_hairNurbsCurve::outCurvesNum;
MObject vixo_hairNurbsCurve::outVID;
MObject vixo_hairNurbsCurve::outCurves;
	MObject vixo_hairNurbsCurve::outCurveShape;
	MObject vixo_hairNurbsCurve::outU;
	MObject vixo_hairNurbsCurve::outV;
	
	MObject vixo_hairNurbsCurve::show;

MObject vixo_hairNurbsCurve::inMeshUpdate;

vixo_hairNurbsCurve::vixo_hairNurbsCurve(void)
{
	dirtyCount=0;
	computeFlag=true;
}


vixo_hairNurbsCurve::~vixo_hairNurbsCurve(void)
{
}

void* vixo_hairNurbsCurve::creator()
{
	return new vixo_hairNurbsCurve();
}

MStatus vixo_hairNurbsCurve::initialize()
{
	MFnTypedAttribute tAttr;
	inMesh=tAttr.create("inMesh","im",MFnData::kMesh);
	tAttr.setHidden(true);
	curveShape=tAttr.create("curveShape","cs",MFnData::kNurbsCurve);
	tAttr.setStorable(false);
	tAttr.setHidden(true);
	tAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	outCurveShape=tAttr.create("outCurveShape","ocs",MFnData::kNurbsCurve);
	tAttr.setHidden(true);

	MFnNumericAttribute nAttr;
	vertexID=nAttr.create("vertexID","vid",MFnNumericData::kInt,-1);
	nAttr.setStorable(false);
	nAttr.setHidden(true);
	nAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	densityMapRefresh=nAttr.create("densityMapRefresh","dmr",MFnNumericData::kBoolean,false);
	densityMap=nAttr.createColor("densityMap","den");
	showControl=nAttr.create("showControl","sct",MFnNumericData::kFloat,1.0);
	stateRefresh=nAttr.create("stateRefresh","sr",MFnNumericData::kBoolean,true);
	outU=nAttr.create("outU","ou",MFnNumericData::kFloat,0.0);
	nAttr.setHidden(true);
	outV=nAttr.create("outV","ov",MFnNumericData::kFloat,0.0);
	nAttr.setHidden(true);
	outVID=nAttr.create("outVID","ovi",MFnNumericData::kInt,-1);
	nAttr.setHidden(true);
	nAttr.setArray(true);
	show=nAttr.create("show","vis",MFnNumericData::kBoolean,true);
	nAttr.setHidden(true);
	outCurvesNum=nAttr.create("outCurvesNum","ocn",MFnNumericData::kInt,0);

	inMeshUpdate=nAttr.create("inMeshUpdate","imu",MFnNumericData::kBoolean,false);
	//nAttr.setHidden(true);
	//nAttr.setArray(true);

	MFnCompoundAttribute cAttr;
	inCurves=cAttr.create("inCurves","ic");
	cAttr.addChild(vertexID);
	cAttr.addChild(curveShape);
	cAttr.setArray(true);
	cAttr.setStorable(false);
	cAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	cAttr.setHidden(true);

	outCurves=cAttr.create("outCurves","oc");
	cAttr.addChild(outCurveShape);
	cAttr.addChild(outU);
	cAttr.addChild(outV);
	//cAttr.addChild(outVID);
	cAttr.addChild(show);
	cAttr.setArray(true);
	cAttr.setHidden(true);

	addAttribute(inMesh);
	addAttribute(inMeshUpdate);
	addAttribute(inCurves);
	addAttribute(vertexID);
	addAttribute(curveShape);
	addAttribute(densityMapRefresh);
	addAttribute(densityMap);
	addAttribute(showControl);
	addAttribute(stateRefresh);
	addAttribute(outCurves);
	addAttribute(outCurveShape);
	addAttribute(outU);
	addAttribute(outV);
	addAttribute(outVID);
	addAttribute(show);
	addAttribute(outCurvesNum);

	

	
	//attributeAffects(densityMapRefresh,outU);
	//attributeAffects(densityMapRefresh,outV);
	//attributeAffects(densityMap,outU);
	//attributeAffects(densityMap,outV);
	//attributeAffects(inMeshUpdate,outU);
	//attributeAffects(inMeshUpdate,outV);

	attributeAffects(densityMapRefresh,outCurvesNum);
	attributeAffects(densityMap,outCurvesNum);
	attributeAffects(inMeshUpdate,outCurvesNum);
	attributeAffects(densityMapRefresh,outVID);
	attributeAffects(densityMap,outVID);
	attributeAffects(inMeshUpdate,outVID);
	attributeAffects(stateRefresh,inCurves);
	attributeAffects(stateRefresh,outCurveShape);
	attributeAffects(stateRefresh,outU);
	attributeAffects(stateRefresh,outV);
	attributeAffects(stateRefresh,show);
	
	attributeAffects(showControl,show);

	//attributeAffects(inMeshUpdate,outU);
	//attributeAffects(inMeshUpdate,outV);
	//attributeAffects(inMeshUpdate,outCurvesNum);

	return MS::kSuccess;
}

MStatus vixo_hairNurbsCurve::compute(const MPlug& plug, MDataBlock& data)
{
	bool inMeshUpdateValue=data.inputValue(inMeshUpdate).asBool();
	bool densityMapRefreshValue=data.inputValue(densityMapRefresh).asBool();
	if(uvs.size()==0||outPID2VID.size()==0||(inMeshUpdateValue||densityMapRefreshValue)==true)
	{
		//刷新inMesh信息
		MObject meshObj=data.inputValue(inMesh).asMesh();
		if(meshObj.hasFn(MFn::kMesh)==false)
			return MS::kSuccess;

		MItMeshVertex iterVert(meshObj);
		uvs.clear();
		uvs.resize(iterVert.count()*2);
		positions.clear();
		positions.resize(iterVert.count());
		for(iterVert.reset();!iterVert.isDone();iterVert.next())
		{
			float2 uvsTemp;
			iterVert.getUV(uvsTemp);
			uvs[iterVert.index()*2]=uvsTemp[0];
			uvs[iterVert.index()*2+1]=uvsTemp[1];
			positions[iterVert.index()]=iterVert.position(MSpace::kWorld);
		}
		data.outputValue(inMeshUpdate).setBool(false);
		data.outputValue(inMeshUpdate).setClean();
		//cout<<"test uv refresh"<<endl;
		//~刷新inMesh信息

		//刷新densityMap信息
		//得到新的outcurveID集合,只是处理set<int> newOutCurvesSet;
		initOutCurveBasicInfo(data);

		//刷新数据输出
		data.outputValue(densityMapRefresh).set(false);
		data.setClean(densityMapRefresh);
		data.outputValue(outCurvesNum).setInt(newOutCurvesSet.size());
		data.setClean(outCurvesNum);

		MIntArray del,add;
		cmpOldNewSet(oldOutCurvesSet,newOutCurvesSet,del,add);

		// DELETED OUT CURVES
		for(int i=0;i<del.length();i++)
			outCurvesData.erase(del[i]);
		
		//ADDED OUT CURVES
		addNewOutCurves(add);

		//后续处理：outCurveID与outCurveVertexID的mapping + old=new
		//if(add.length()+del.length()>0)
		//{
			postDealDensityMapUpdate(data);
			//cout<<"test001"<<endl;
			//更新state，触发shape更新~确认getAttr不会触发compute计算？？？
			//data.outputValue(stateRefresh).set(true);
			//data.setClean(stateRefresh);
		//}
		//~刷新densityMap信息
	}
	//更新曲线的vis信息
	if(plug.partialName().substring(plug.partialName().length()-3,plug.partialName().length())=="vis")
	{
		MArrayDataHandle outCurveHandle=data.outputArrayValue(outCurves);
		float showCtrlValue=min(data.inputValue(showControl).asFloat(),1.0);

		int i=plug.parent().logicalIndex();
		MStatus stat=outCurveHandle.jumpToElement(i);

		//规则：
		//1 top(i*showValue)==floor((i+1)*showValue) -> show=true
		//2 没有在原始控制曲线集合中 -> show=true
		int outVIDValue=outPID2VID.find(i)->second;
		bool showFlag=true;
		if(showCtrlValue<0.98)
		{
			if(ceil(i*showCtrlValue)>floor((i+1)*showCtrlValue))
				showFlag=false;
		}

		if(newInCurvesSet.count(outVIDValue)>0)
			showFlag=false;

		if(i==0)
			showFlag=true;
		outCurveHandle.outputValue().child(show).setBool(showFlag);

		outCurveHandle.outputValue().child(show).setClean();
	}

	//更新曲线的shape,uv信息
	//cout<<plug.info().asChar()<<endl;
	if(plug.partialName().substring(plug.partialName().length()-3,plug.partialName().length())=="ocs")
	{
		if(data.inputValue(stateRefresh).asBool()==false&&data.outputValue(plug).asNurbsCurve().hasFn(MFn::kNurbsCurve)==true)
			return MS::kSuccess;
		else if(data.inputValue(stateRefresh).asBool()==false&&data.outputValue(plug).asNurbsCurve().hasFn(MFn::kNurbsCurve)==false)
		{
			//单个曲线信息更新
			int outPluginId=plug.parent().logicalIndex();
			if(inCurvesData.size()==0||outPluginId==0)
			{
				outCurveShapeUpdateCollectInfo(data);
			}
			//int outPluginId=plug.parent().logicalIndex();
			MDoubleArray knots(outCurvesPoints);
			for(int i=0;i<outCurvesPoints;i++)
				knots[i]=i;
			refreshOutCurveShape(outPluginId,knots,data);
		}
		else //刷新全部曲线的信息
		{
			//cout<<"all"<<plug.info().asChar()<<" "<<outPID2VID.size()<<endl;;
			outCurveShapeUpdateCollectInfo(data);

			MDoubleArray knots(outCurvesPoints);
			for(int i=0;i<outCurvesPoints;i++)
				knots[i]=i;

			//MArrayDataHandle outCurveHandle=data.inputArrayValue(outCurves);
			for(int i=0;i<outPID2VID.size();i++)
			{
				refreshOutCurveShape(i,knots,data);
			}
		}
	}
	return MS::kSuccess ;
}

void vixo_hairNurbsCurve::outCurveShapeUpdateCollectInfo(MDataBlock & data)
{
	//得到新的incurveID集合,处理set<int> newInCurvesSet,map<int,inCurvesInfo> inCurvesData(信息完整);
	getInCurvesInfo(data);

	//cmp old& new
	MIntArray del,add;
	cmpOldNewSet(oldInCurvesSet,newInCurvesSet,del,add);

	//更新outCurvesInfo的controlID和percent信息
	if(del.length()+add.length()>0)
	{
		//cout<<"test"<<endl;
		updateOutCurvesBasicInfo(del,add);
	}

	//后续处理：old=new
	if(add.length()+del.length()>0)
	{
		oldInCurvesSet.clear();
		set<int>::iterator iterSet;
		for(iterSet=newInCurvesSet.begin();iterSet!=newInCurvesSet.end();iterSet++)
			oldInCurvesSet.insert(*iterSet);
	}

	//后续处理：stateRefresh=false
	data.outputValue(stateRefresh).set(false);
	data.setClean(stateRefresh);
}

void vixo_hairNurbsCurve::refreshOutCurveShape(int outPlugID,MDoubleArray knots,MDataBlock & data)
{
	//cout<<"jump:"<<outPlugID<<endl;
	MArrayDataHandle outCurveHandle=data.outputArrayValue(outCurves);
	//if(outPlugID>0)
	//{
		MStatus stat=outCurveHandle.jumpToElement(outPlugID);
		//if(stat!=MS::kSuccess)
		//{
			//cout<<"test"<<endl;
		//	return ;
		//}
	//}
	MFnNurbsCurveData fnCurveData;

	int outVIDValue=outPID2VID.find(outPlugID)->second;
	//cout<<outPlugID<<" "<<outVIDValue<<endl;
	MObject outCurveObj=fnCurveData.create();

	buildOutCurvesShape(outVIDValue,knots,outCurveObj);
	outCurveHandle.outputValue().child(outU).setFloat(uvs[2*outVIDValue]);
	outCurveHandle.outputValue().child(outV).setFloat(uvs[2*outVIDValue+1]);
	//outCurveHandle.outputValue().child(outVID).setInt(outVIDValue);
	outCurveHandle.outputValue().child(outCurveShape).setMObject(outCurveObj);

	if(inCurvesData.count(outVIDValue)>0)
	{
		outCurveHandle.outputValue().child(show).setBool(false);
		outCurveHandle.outputValue().child(show).setClean();
	}

	outCurveHandle.outputValue().child(outU).setClean();
	outCurveHandle.outputValue().child(outV).setClean();
	//outCurveHandle.outputValue().child(outVID).setClean();
	outCurveHandle.outputValue().child(outCurveShape).setClean();
}

MStatus vixo_hairNurbsCurve::connectionMade(const MPlug& plug,const MPlug& otherPlug,bool asSrc)
{
	if(plug==this->inMesh)
	{
		//cout<<"inmesh"<<endl;
		//get uv info
		MPlugArray arr;
		plug.connectedTo(arr,true,false);
		MItMeshVertex iterVert(arr[0].node());
		uvs.resize(iterVert.count()*2);
		positions.resize(iterVert.count());
		for(iterVert.reset();!iterVert.isDone();iterVert.next())
		{
			float2 uvsTemp;
			iterVert.getUV(uvsTemp);
			uvs[iterVert.index()*2]=uvsTemp[0];
			uvs[iterVert.index()*2+1]=uvsTemp[1];
			positions[iterVert.index()]=iterVert.position(MSpace::kWorld);
		}
		//~get uv info
		return MS::kSuccess;
	}
	return MPxNode::connectionMade( plug, otherPlug, asSrc );

}

MStatus vixo_hairNurbsCurve::connectionBroken(const MPlug& plug,const MPlug& otherPlug,bool asSrc)
{
	if(plug==this->inMesh)
	{
		return MS::kSuccess;
	}
	return MPxNode::connectionMade( plug, otherPlug, asSrc );
}

void vixo_hairNurbsCurve::initOutCurveBasicInfo(MDataBlock& data)
{
	newOutCurvesSet.clear();

	MPlugArray arr;
	MPlug plug(thisMObject(),this->densityMap);
	plug.connectedTo(arr,true,false);

	//not connected to file texture node, all vertex shuold be curved
	if(arr.length()!=1)
	{
		for(int i=0;i<uvs.size()/2;i++)
		{
			newOutCurvesSet.insert(i);
		}
		return;
	}
	//~not connected to file texture node, all vertex shuold be curved

	MFnDependencyNode dgFn(arr[0].node());
	MPlug fnFile=dgFn.findPlug("fileTextureName");

	//no file is real set, all vertex shuold be curved
	if(fnFile.asString()=="")
	{
		for(int i=0;i<uvs.size()/2;i++)
		{
			newOutCurvesSet.insert(i);
		}
		return;
	}
	//~no file is real set, all vertex shuold be curved

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
		MIntArray vertexArray;
		iterFace.getVertices(vertexArray);
		unsigned int pixelSum=0;
		for(int i=0;i<us.length();i++)
		{
			int pixelW=us[i]*imageWidth;
			int pixelH=vs[i]*imageHeight;
			pixelSum+=(unsigned int)pixels[imageDepth*(pixelH*imageWidth+pixelW)];
		}
		
		if(pixelSum>0)
		{
			for(int i=0;i<vertexArray.length();i++)
			{
				newOutCurvesSet.insert(vertexArray[i]);
			}
		}
	}
	//for face that has nonzero density vertex, all vertex on it should be curved
}

void vixo_hairNurbsCurve::cmpOldNewSet(set<int> oldSet,set<int> newSet,MIntArray &del,MIntArray &add)
{
	del.clear();
	add.clear();

	set<int>::iterator iter;
	for(iter=oldSet.begin();iter!=oldSet.end();iter++)
	{
		if(newSet.count(*iter)<=0)
			del.append(*iter);
	}

	for(iter=newSet.begin();iter!=newSet.end();iter++)
	{
		if(oldSet.count(*iter)<=0)
			add.append(*iter);
	}
}

void vixo_hairNurbsCurve::addNewOutCurves(MIntArray add)
{
	//ADDED OUT CURVES
	outCurvesInfo temp;
	temp.show=true;
	temp.oriCurved=false;
	temp.pointOnCurve.clear();
	temp.relatedCurves.clear();
	for(int i=0;i<add.length();i++)
	{
		outCurvesData.insert(pair<int,outCurvesInfo>(add[i],temp));
	}

	//遍历incurves，对每一个step，对每一个add，如果找到了add中点，对该点添加relatedCurves信息
	relatedCurveInfo tempRelated;
	map<int,inCurvesInfo>::iterator itInCurves;
	map<int,outCurvesInfo>::iterator iterOutCurves;
	for(itInCurves=inCurvesData.begin();itInCurves!=inCurvesData.end();itInCurves++)
	{
		for(int j=0;j<itInCurves->second.nearestPoints.size();j++)
		{
			for(int k=0;k<add.length();k++)
			{
				iterOutCurves=outCurvesData.find(add[k]);
				if(itInCurves->second.nearestPoints[j].count(add[k])>0)
				{
					tempRelated.step=j;
					tempRelated.directDistance=positions[add[k]].distanceTo(positions[itInCurves->first]);
					iterOutCurves->second.relatedCurves.insert(pair<int,relatedCurveInfo>(itInCurves->first,tempRelated));
				}
			}
		}
	}

	//per
	for(int i=0;i<add.length();i++)
	{
		updateOutCurveCtrlIDPercentInfo(outCurvesData.find(add[i])->second);
	}
}

void vixo_hairNurbsCurve::postDealDensityMapUpdate(MDataBlock& data)
{
	MStatus test;
	MArrayDataHandle outVIDHandle=data.outputArrayValue(outVID);
	MArrayDataBuilder outVIDBuilder(&data,outVID,newOutCurvesSet.size(),&test);
	for(int i=0;i<newOutCurvesSet.size();i++)
		outVIDBuilder.addElement(i);
	outVIDHandle.set(outVIDBuilder);
	//outVIDHandle.
	//build a map
	outPID2VID.clear();
	set<int>::iterator newOutCurveSetIter;
	int i=0;
	for(newOutCurveSetIter=newOutCurvesSet.begin();newOutCurveSetIter!=newOutCurvesSet.end();newOutCurveSetIter++)
	{
		outPID2VID.insert(pair<int,int>(i,*newOutCurveSetIter));
		outVIDHandle.jumpToElement(i);
		outVIDHandle.outputValue().set(*newOutCurveSetIter);
		outVIDHandle.outputValue().setClean();
		i++;
	}

	//oldOutSet=newOutSet
	oldOutCurvesSet.clear();
	set<int>::iterator iterSet;
	for(iterSet=newOutCurvesSet.begin();iterSet!=newOutCurvesSet.end();iterSet++)
		oldOutCurvesSet.insert(*iterSet);
}

void vixo_hairNurbsCurve::getInCurvesInfo(MDataBlock& data)
{
	newInCurvesSet.clear();

	MArrayDataHandle inCurvesHandle=data.inputArrayValue(inCurves);
	int inCurvesNum=inCurvesHandle.elementCount();
	//inputCurves.resize(inCurvesNum);
	outCurvesPoints=0;
	//MObjectArray allCurveObj;

	for(int i=0;i<inCurvesNum;i++)
	{	
		MObject curveObj=inCurvesHandle.inputValue().child(curveShape).asNurbsCurve();
		int vid=inCurvesHandle.inputValue().child(vertexID).asInt();
		if(vid==-1||curveObj.isNull()==true)
			continue;
		MFnNurbsCurve fnCurve(curveObj);
		//allCurveObj.append(curveObj);
		outCurvesPoints=max(outCurvesPoints,fnCurve.numSpans()+1);
		newInCurvesSet.insert(vid);
		//cout<<"debug"<<vid<<endl;

		map<int,inCurvesInfo>::iterator iterInCurves=inCurvesData.find(vid);
		if(iterInCurves!=inCurvesData.end())
		{
			//cout<<"here0"<<endl;
			iterInCurves->second.plugId=inCurvesHandle.elementIndex();
			iterInCurves->second.cvs.clear();
		}
		else
		{
			//cout<<"here1"<<endl;
			inCurvesInfo tempInElement;
			//tempInElement.tanget.clear();
			tempInElement.cvs.clear();
			tempInElement.nearestPoints.clear();
			tempInElement.plugId=inCurvesHandle.elementIndex();
			initBasicStepInfo(vid,data,tempInElement.nearestPoints);
			//cout<<"test para"<<tempInElement.nearestPoints.size()<<endl;
			inCurvesData.insert(pair<int,inCurvesInfo>(vid,tempInElement));
		}

		if(!inCurvesHandle.next())
			break;
	}

	//get shape info
	set<int>::iterator newInCurveSetIter;
	for(newInCurveSetIter=newInCurvesSet.begin();newInCurveSetIter!=newInCurvesSet.end();newInCurveSetIter++)
	{
		map<int,inCurvesInfo>::iterator iterInCurves=inCurvesData.find(*newInCurveSetIter);
		int plugId=iterInCurves->second.plugId;
		iterInCurves->second.cvs.clear();
		iterInCurves->second.cvs.setLength(outCurvesPoints);
		inCurvesHandle.jumpToElement(plugId);
		MFnNurbsCurve fnCurve(inCurvesHandle.inputValue().child(curveShape).asNurbsCurve());
		if(fnCurve.numSpans()+1==outCurvesPoints)
		{
			fnCurve.getCVs(iterInCurves->second.cvs,MSpace::kObject);
		}
		else
		{
			double paraMin,paraMax;
			fnCurve.getKnotDomain(paraMin,paraMax);
			for(int j=0;j<outCurvesPoints;j++)
			{
				fnCurve.getPointAtParam(paraMin+(paraMax-paraMin)/(outCurvesPoints-1)*j,iterInCurves->second.cvs[j],MSpace::kObject);
			}
		}
	}	
}

void vixo_hairNurbsCurve::initBasicStepInfo(int vid,MDataBlock & data,vector<set<int>>& nearestPoints)
{
	//get nearest vertex info for each added curve
	//cout<<"debug init"<<vid<<endl;
	nearestPoints.resize(MAXSTEP);
	nearestPoints[0].insert(vid);

	MItMeshVertex iterVertex4Record(data.inputValue(inMesh).asMesh());
	MIntArray tempConnectedVertex;
	int prevIndex;

	set<int> checkedVertex;
	checkedVertex.insert(vid);
	set<int> :: iterator lastStepSetIter;
	for(int step=1;step<MAXSTEP;step++)
	{
		for(lastStepSetIter=nearestPoints[step-1].begin();lastStepSetIter!=nearestPoints[step-1].end();lastStepSetIter++)
		{
			iterVertex4Record.setIndex(*lastStepSetIter,prevIndex);
			iterVertex4Record.getConnectedVertices(tempConnectedVertex);
			for(int j=0;j<tempConnectedVertex.length();j++)
			{
				if(checkedVertex.count(tempConnectedVertex[j])<=0)
				{
					nearestPoints[step].insert(tempConnectedVertex[j]);
					checkedVertex.insert(tempConnectedVertex[j]);
				}
			}
		}
		if(nearestPoints[step].size()==0)
			break;
	}
}

void vixo_hairNurbsCurve::buildOutCurvesShape(int outVID,MDoubleArray knots,MObject &obj)
{
	map<int,outCurvesInfo>::iterator outCurvesElement=outCurvesData.find(outVID);
	MPointArray cvs(outCurvesPoints,MPoint(0,0,0));
	for(int i=0;i<outCurvesElement->second.controlVID.size();i++)
	{
		//cout<<outCurvesElement->second.controlVID[i]<<":";
		//float percent=1.0/((outCurvesElement->second.relatedCurves.find(outCurvesElement->second.controlVID[i])->second.directDistance+0.0000000001)*outCurvesElement->second.distanceSum);
		//percent=min(percent,1.0);
		//if(percent<0.0001)
		//	continue;
		//cout<<percent<<endl;
		//float percent=0.2;
		map<int,inCurvesInfo>::iterator iterInCurveSet=inCurvesData.find(outCurvesElement->second.controlVID[i]);
		for(int j=0;j<outCurvesPoints;j++)
		{
			cvs[j]+=iterInCurveSet->second.cvs[j]*outCurvesElement->second.percent[i];
		}
	}

	MFnNurbsCurve fnCurve;
	fnCurve.create(cvs,knots,1,MFnNurbsCurve::kOpen,false,false,obj);
}

void vixo_hairNurbsCurve::updateOutCurveCtrlIDPercentInfo(outCurvesInfo& needToUpdate)
{
	needToUpdate.controlVID.clear();

	vector<vector<int>> nearestPointsOrder;
	nearestPointsOrder.clear();
	nearestPointsOrder.resize(MAXSTEP);

	map<int,relatedCurveInfo>::iterator relatedCurveSetIter;
	for(relatedCurveSetIter=needToUpdate.relatedCurves.begin();relatedCurveSetIter!=needToUpdate.relatedCurves.end();relatedCurveSetIter++)
	{
		nearestPointsOrder[relatedCurveSetIter->second.step].push_back(relatedCurveSetIter->first);
	}

	int controlStep=0;
	for(int i=0;i<MAXSTEP&&controlStep<MAXCONTROLSTEP;i++)
	{
		if(nearestPointsOrder[i].size()>0)
		{
			for(int j=0;j<nearestPointsOrder[i].size();j++)
				needToUpdate.controlVID.push_back(nearestPointsOrder[i][j]);
			controlStep++;
		}
	}

	//compute distance sum
	needToUpdate.distanceSum=0.0;
	for(int i=0;i<needToUpdate.controlVID.size();i++)
	{
		needToUpdate.distanceSum+=1.0/(needToUpdate.relatedCurves.find(needToUpdate.controlVID[i])->second.directDistance+0.0000001);
	}

	//compute percent
	needToUpdate.percent.clear();
	needToUpdate.percent.resize(needToUpdate.controlVID.size());
	for(int i=0;i<needToUpdate.controlVID.size();i++)
	{
		//cout<<outCurvesElement->second.controlVID[i]<<":";
		float percent=1.0/((needToUpdate.relatedCurves.find(needToUpdate.controlVID[i])->second.directDistance+0.0000000001)*needToUpdate.distanceSum);
		needToUpdate.percent[i]=min(percent,1.0);
		if(percent<0.0001)
			needToUpdate.percent[i]=0;
	}
}

/*
void vixo_hairNurbsCurve::updateOutCurvesCtrlIDPercentInfo()
{
	map<int,outCurvesInfo>::iterator outCurvesSetIter;
	map<int,inCurvesInfo>::iterator itInCurves;
	for(outCurvesSetIter=outCurvesData.begin();outCurvesSetIter!=outCurvesData.end();outCurvesSetIter++)
	{
		//make up the relatedCurves info to get the real control inCurve for each outCurve
		outCurvesSetIter->second.controlVID.clear();

		map<int,relatedCurveInfo>::iterator relatedCurveSetIter;
		vector<vector<int>> nearestPointsOrder;
		nearestPointsOrder.clear();
		nearestPointsOrder.resize(MAXSTEP);

		for(relatedCurveSetIter=outCurvesSetIter->second.relatedCurves.begin();relatedCurveSetIter!=outCurvesSetIter->second.relatedCurves.end();relatedCurveSetIter++)
		{
			nearestPointsOrder[relatedCurveSetIter->second.step].push_back(relatedCurveSetIter->first);
		}

		int controlStep=0;
		for(int i=0;i<MAXSTEP&&controlStep<MAXCONTROLSTEP;i++)
		{
			if(nearestPointsOrder[i].size()>0)
			{
				for(int j=0;j<nearestPointsOrder[i].size();j++)
					outCurvesSetIter->second.controlVID.push_back(nearestPointsOrder[i][j]);
				controlStep++;
			}
		}

		//compute distance sum
		outCurvesSetIter->second.distanceSum=0.0;
		for(int i=0;i<outCurvesSetIter->second.controlVID.size();i++)
		{
			outCurvesSetIter->second.distanceSum+=1.0/(outCurvesSetIter->second.relatedCurves.find(outCurvesSetIter->second.controlVID[i])->second.directDistance+0.0000001);
		}

		//compute percent
		outCurvesSetIter->second.percent.clear();
		outCurvesSetIter->second.percent.resize(outCurvesSetIter->second.controlVID.size());
		for(int i=0;i<outCurvesSetIter->second.controlVID.size();i++)
		{
			//cout<<outCurvesElement->second.controlVID[i]<<":";
			float percent=1.0/((outCurvesSetIter->second.relatedCurves.find(outCurvesSetIter->second.controlVID[i])->second.directDistance+0.0000000001)*outCurvesSetIter->second.distanceSum);
			outCurvesSetIter->second.percent[i]=min(percent,1.0);
			if(percent<0.0001)
				outCurvesSetIter->second.percent[i]=0;
		}
	}
}
*/
void vixo_hairNurbsCurve::updateOutCurvesBasicInfo(MIntArray del,MIntArray add)
{
	map<int,outCurvesInfo>::iterator outCurvesSetIter;
	map<int,inCurvesInfo>::iterator itInCurves;
	relatedCurveInfo tempRelated;
	for(outCurvesSetIter=outCurvesData.begin();outCurvesSetIter!=outCurvesData.end();outCurvesSetIter++)
	{
		for(int i=0;i<del.length();i++)
		{
			if(outCurvesSetIter->second.relatedCurves.count(del[i])>0)
			{
				outCurvesSetIter->second.relatedCurves.erase(del[i]);
			}
		}

		for(int i=0;i<add.length();i++)
		{
			itInCurves=inCurvesData.find(add[i]);
			for(int step=0;step<itInCurves->second.nearestPoints.size();step++)
			{
				if(itInCurves->second.nearestPoints[step].count(outCurvesSetIter->first)>0)
				{
					tempRelated.step=step;
					tempRelated.directDistance=positions[outCurvesSetIter->first].distanceTo(positions[add[i]]);
					outCurvesSetIter->second.relatedCurves.insert(pair<int,relatedCurveInfo>(add[i],tempRelated));
				}
			}
		}


		updateOutCurveCtrlIDPercentInfo(outCurvesSetIter->second);
		/*
		//make up the relatedCurves info to get the real control inCurve for each outCurve
		outCurvesSetIter->second.controlVID.clear();

		map<int,relatedCurveInfo>::iterator relatedCurveSetIter;
		vector<vector<int>> nearestPointsOrder;
		nearestPointsOrder.clear();
		nearestPointsOrder.resize(MAXSTEP);

		for(relatedCurveSetIter=outCurvesSetIter->second.relatedCurves.begin();relatedCurveSetIter!=outCurvesSetIter->second.relatedCurves.end();relatedCurveSetIter++)
		{
			nearestPointsOrder[relatedCurveSetIter->second.step].push_back(relatedCurveSetIter->first);
		}

		int controlStep=0;
		for(int i=0;i<MAXSTEP&&controlStep<MAXCONTROLSTEP;i++)
		{
			if(nearestPointsOrder[i].size()>0)
			{
				for(int j=0;j<nearestPointsOrder[i].size();j++)
					outCurvesSetIter->second.controlVID.push_back(nearestPointsOrder[i][j]);
				controlStep++;
			}
		}

		//compute distance sum
		outCurvesSetIter->second.distanceSum=0.0;
		for(int i=0;i<outCurvesSetIter->second.controlVID.size();i++)
		{
			outCurvesSetIter->second.distanceSum+=1.0/(outCurvesSetIter->second.relatedCurves.find(outCurvesSetIter->second.controlVID[i])->second.directDistance+0.0000001);
		}

		//compute percent
		outCurvesSetIter->second.percent.clear();
		outCurvesSetIter->second.percent.resize(outCurvesSetIter->second.controlVID.size());
		for(int i=0;i<outCurvesSetIter->second.controlVID.size();i++)
		{
			//cout<<outCurvesElement->second.controlVID[i]<<":";
			float percent=1.0/((outCurvesSetIter->second.relatedCurves.find(outCurvesSetIter->second.controlVID[i])->second.directDistance+0.0000000001)*outCurvesSetIter->second.distanceSum);
			outCurvesSetIter->second.percent[i]=min(percent,1.0);
			if(percent<0.0001)
				outCurvesSetIter->second.percent[i]=0;
		}
		*/
	}
}


MStatus vixo_hairNurbsCurve::setDependentsDirty(const MPlug&  plugBeingDirtied, MPlugArray&  affectedPlugs)
{

	//attributeAffects(densityMapRefresh,outCurvesNum);
	//attributeAffects(densityMap,outCurvesNum);
	//attributeAffects(inMeshUpdate,outCurvesNum);
	//attributeAffects(stateRefresh,inCurves);
	//attributeAffects(stateRefresh,outU);
	//attributeAffects(stateRefresh,outV);
	//attributeAffects(stateRefresh,show);
	
	//attributeAffects(showControl,show);
	
	
	//attributeAffects(showControl,show);

	MObject thisNode=thisMObject();
	
	MPlug plugOutNum(thisNode,this->outCurvesNum);
	if(plugBeingDirtied==this->densityMapRefresh||plugBeingDirtied==this->densityMap||plugBeingDirtied==this->inMeshUpdate)
	{
		affectedPlugs.append(plugOutNum);
	}
	MPlug arrayOutCurve(thisNode,this->outCurveShape);
	MPlug arrayOutU(thisNode,this->outU);
	MPlug arrayOutV(thisNode,this->outV);
	MPlug arrayOutshow(thisNode,this->show);
	//if(arrayOutCurve.numElements()>0)
	//{

		//if(plugBeingDirtied.partialName().substring(plugBeingDirtied.partialName().length()-2,plugBeingDirtied.partialName().length())=="cs")
		//{
			//affectedPlugs.append(arrayOutCurve);
		//}

		if(plugBeingDirtied.partialName()=="sr")
		{
			//cout<<"dit"<<plugBeingDirtied.info().asChar()<<endl;
			affectedPlugs.append(arrayOutCurve);
			affectedPlugs.append(arrayOutU);
			affectedPlugs.append(arrayOutV);
			affectedPlugs.append(arrayOutshow);
		}

	//}
	return MS::kSuccess;
}
