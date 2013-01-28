#include "vixo_hairCacheExport.h"


vixo_hairCacheExport::vixo_hairCacheExport(void)
{
}


vixo_hairCacheExport::~vixo_hairCacheExport(void)
{
}

void * vixo_hairCacheExport::creator()
{
	return new vixo_hairCacheExport;
}

MStatus vixo_hairCacheExport::doIt(const MArgList& args)
{
	MString vixoHairNode;
	int startFrame,endFrame;
	unsigned index;
	index=args.flagIndex("vhn","vixoHairNode");
	if(MArgList::kInvalidArgIndex!=index)
	{
		args.get(index+1,vixoHairNode);
	}
	index=args.flagIndex("sf","startFrame");
	if(MArgList::kInvalidArgIndex!=index)
	{
		args.get(index+1,startFrame);
	}
	index=args.flagIndex("ef","endFrame");
	if(MArgList::kInvalidArgIndex!=index)
	{
		args.get(index+1,endFrame);
	}

	//get hairCacheFileName
	MSelectionList slist;
	MGlobal::getSelectionListByName(vixoHairNode,slist);
	MDagPath vixoHairNodeDag;
	slist.getDagPath(0,vixoHairNodeDag);
	vixoHairNodeDag.extendToShape();
	MFnDagNode fnVixoHair(vixoHairNodeDag);
	MPlug plugCacheFileName=fnVixoHair.findPlug("hairCacheFileName");
	MString cacheFileName=plugCacheFileName.asString();
	//~get hairCacheFileName

	//get staticMesh
	MPlug plugStaticMesh=fnVixoHair.findPlug("staticInMesh");
	MObject staticMeshObj=MFnMeshData(plugStaticMesh.asMObject()).object();
	//~get staticMesh

	//build out dataBase
	MFnMesh fnStaticMesh(staticMeshObj);
	map<int,set<outInVertexRelationInfo>> outVertexDataBase;
	map<int,vector<outVIDCtrlInfo>> outVertexControlData;
	for(int i=0;i<fnStaticMesh.numVertices();i++)
	{
		set<outInVertexRelationInfo> temp;
		temp.clear();
		outVertexDataBase.insert(pair<int,set<outInVertexRelationInfo>>(i,temp));
	}
	//build out dataBase

	//get inCurveVID
	map<int,int> plugMapVID;
	map<int,int> VIDMapPlug;
	map<int,inVertexBasicInfo> inVIDMapInVertexDataBase;
	MPlug plugAllInCurve=fnVixoHair.findPlug("inCtrlCurveData");
	MIntArray existIndex;
	plugAllInCurve.getExistingArrayAttributeIndices(existIndex);
	for(int i=0;i<existIndex.length();i++)
	{
		MPlugArray arr;
		plugAllInCurve.elementByLogicalIndex(existIndex[i]).connectedTo(arr,true,false);
		MFnDependencyNode fnHairSystem(arr[0].node());
		MPlug inputHairs=fnHairSystem.findPlug("inputHair");
		arr.clear();
		inputHairs.elementByLogicalIndex(existIndex[i]).connectedTo(arr,true,false);
		MFnDependencyNode fnFolli(arr[0].node());
		int vid=fnFolli.findPlug("vertexID").asInt();
		plugMapVID.insert(pair<int,int>(existIndex[i],vid));
		VIDMapPlug.insert(pair<int,int>(vid,existIndex[i]));
		initBasicStepInfo(vid,staticMeshObj,inVIDMapInVertexDataBase);
	}	
	//~get inCurveVID

	//build in out relation
	map<int,inVertexBasicInfo>::iterator inDBIter;
	for(inDBIter=inVIDMapInVertexDataBase.begin();inDBIter!=inVIDMapInVertexDataBase.end();inDBIter++)
	{
		buileRelationBetweenInOut(inDBIter->first,inVIDMapInVertexDataBase,outVertexDataBase);
	}

	map<int,set<outInVertexRelationInfo>>::iterator outDBIter;
	for(outDBIter=outVertexDataBase.begin();outDBIter!=outVertexDataBase.end();outDBIter++)
	{
		sortControlOrder(outDBIter->first,outVertexDataBase,outVertexControlData);
	}
	//~build in out relation

	for(int i=startFrame;i<=endFrame;i++)
	{
		MString currentTime;
		currentTime.set(i);
		MGlobal::executeCommand("currentTime "+currentTime);
		MGlobal::executeCommand("dgeval "+vixoHairNode+".hiddenOutput");

		//get dynamic mesh
		MPlug plugDynamicMesh=fnVixoHair.findPlug("dynamicInMesh");
		MObject dynamicMeshObj=MFnMeshData(plugDynamicMesh.asMObject()).object();
		//~get dynamic mesh

		//export cache
		//faceid triid vid position normal tangent
		vector<forExportHairCache> exportData;
		exportBasicData(dynamicMeshObj,exportData);
		//curve tangent
		//get in curve infos
		map<int,MVectorArray> inCurveShape;
		getInCurveInfo(plugMapVID,vixoHairNode,inCurveShape);
		//~get in curve infos

		vector<vec3> outCurveCacheData;
		vec3 init;
		init.x=0;
		init.y=0;
		init.z=0;
		outCurveCacheData.resize(fnStaticMesh.numVertices()*inCurveShape.begin()->second.length(),init);
		buildCache(inCurveShape.begin()->second.length(),dynamicMeshObj,outCurveCacheData,inCurveShape,outVertexControlData);
		//~export cache

		//write to file
		MString fileName=getFileName(cacheFileName,i);
		fstream fout(fileName.asChar(),ios_base::out|ios_base::binary);
		int triNumvertexNum[3];
		triNumvertexNum[0]=exportData.size();
		triNumvertexNum[1]=fnStaticMesh.numVertices();
		triNumvertexNum[2]=inCurveShape.begin()->second.length();
		fout.write((char*)triNumvertexNum,sizeof(int)*3);
		fout.write((char*)&exportData[0],sizeof(forExportHairCache)*exportData.size());
		fout.write((char*)&outCurveCacheData[0],sizeof(vec3)*outCurveCacheData.size());
		fout.flush();
		fout.close();
		/*
		MString fileNameDebug=fileName+".dbg";
		fout.open(fileNameDebug.asChar(),ios_base::out);
		for(int i=0;i<outCurveCacheData.size();i++)
		{
			fout<<outCurveCacheData[i].x<<" "<<outCurveCacheData[i].y<<" "<<outCurveCacheData[i].z<<endl;
		}
		fout.flush();
		fout.close();
		*/
		//~write to file
	}

	return MS::kSuccess;
}

void vixo_hairCacheExport::initBasicStepInfo(int vid,MObject & staticMesh,map<int,inVertexBasicInfo>& inVIDMapInVertexDataBase)
{

	inVertexBasicInfo ele;
	inVIDMapInVertexDataBase.insert(pair<int,inVertexBasicInfo>(vid,ele));
	map<int,inVertexBasicInfo>::iterator iter=inVIDMapInVertexDataBase.find(vid);
	iter->second.stepVertex.resize(MAXSTEP);

	forInVertexBasicInfo subele;
	subele.vertexID=vid;
	subele.distance=0;
	iter->second.stepVertex[0].insert(subele);

	MItMeshVertex iterVertex4Record(staticMesh);
	MIntArray tempConnectedVertex;
	int prevIndex;

	MFnMesh fnMesh(staticMesh);
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

void vixo_hairCacheExport::buileRelationBetweenInOut(int inVID,map<int,inVertexBasicInfo>& inVIDMapInVertexDataBase,map<int,set<outInVertexRelationInfo>>& outVertexDataBase)
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

void vixo_hairCacheExport::sortControlOrder(int outVID,map<int,set<outInVertexRelationInfo>>& outVertexDataBase,map<int,vector<outVIDCtrlInfo>>& outVertexControlData)
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

void vixo_hairCacheExport::exportBasicData(MObject& dynamicMeshObj,vector<forExportHairCache>& exportData)
{
	MItMeshFaceVertex itFaceVertex(dynamicMeshObj);
	MItMeshPolygon itFace(dynamicMeshObj);
	int preF,preV;
	for(itFace.reset();!itFace.isDone();itFace.next())
	{
		MIntArray forLocalIdx;
		itFace.getVertices(forLocalIdx);
		map<int,int> allMapLocal;
		for(int temp=0;temp<forLocalIdx.length();temp++)
		{
			allMapLocal.insert(pair<int,int>(forLocalIdx[temp],temp));
		}
		int numTri;
		itFace.numTriangles(numTri);
		for(int j=0;j<numTri;j++)
		{
			MPointArray positions;
			MIntArray vids;
			MVector normal,tangent;
			itFace.getTriangle(j,positions,vids,MSpace::kWorld);

			forExportHairCache ele;
			ele.faceid=itFace.index();
			ele.triid=j;

			ele.v0.vid=vids[0];
			itFaceVertex.setIndex(ele.faceid,allMapLocal.find(ele.v0.vid)->second,preF,preV);
			itFaceVertex.getNormal(normal,MSpace::kWorld);
			//tangent=itFaceVertex.getTangent(MSpace::kWorld);
			MString uvset("map1");
			tangent=itFaceVertex.getTangent(MSpace::kWorld,&uvset);
			ele.v0.normal.x=normal.x;
			ele.v0.normal.y=normal.y;
			ele.v0.normal.z=normal.z;
			ele.v0.tangent.x=tangent.x;
			ele.v0.tangent.y=tangent.y;
			ele.v0.tangent.z=tangent.z;
			//cout<<ele.v0.tangent.x<<" "<<ele.v0.tangent.y<<" "<<ele.v0.tangent.z<<endl;
			ele.v0.position.x=positions[0].x;
			ele.v0.position.y=positions[0].y;
			ele.v0.position.z=positions[0].z;

			ele.v1.vid=vids[1];
			itFaceVertex.setIndex(ele.faceid,allMapLocal.find(ele.v1.vid)->second,preF,preV);
			itFaceVertex.getNormal(normal,MSpace::kWorld);
			//tangent=itFaceVertex.getTangent(MSpace::kWorld);
			tangent=itFaceVertex.getTangent(MSpace::kWorld,&uvset);
			ele.v1.normal.x=normal.x;
			ele.v1.normal.y=normal.y;
			ele.v1.normal.z=normal.z;
			ele.v1.tangent.x=tangent.x;
			ele.v1.tangent.y=tangent.y;
			ele.v1.tangent.z=tangent.z;
			//cout<<ele.v0.tangent.x<<" "<<ele.v0.tangent.y<<" "<<ele.v0.tangent.z<<endl;
			ele.v1.position.x=positions[1].x;
			ele.v1.position.y=positions[1].y;
			ele.v1.position.z=positions[1].z;

			ele.v2.vid=vids[2];
			itFaceVertex.setIndex(ele.faceid,allMapLocal.find(ele.v2.vid)->second,preF,preV);
			itFaceVertex.getNormal(normal,MSpace::kWorld);
			//tangent=itFaceVertex.getTangent(MSpace::kWorld);
			tangent=itFaceVertex.getTangent(MSpace::kWorld,&uvset);
			ele.v2.normal.x=normal.x;
			ele.v2.normal.y=normal.y;
			ele.v2.normal.z=normal.z;
			ele.v2.tangent.x=tangent.x;
			ele.v2.tangent.y=tangent.y;
			ele.v2.tangent.z=tangent.z;
			//cout<<ele.v0.tangent.x<<" "<<ele.v0.tangent.y<<" "<<ele.v0.tangent.z<<endl;
			ele.v2.position.x=positions[2].x;
			ele.v2.position.y=positions[2].y;
			ele.v2.position.z=positions[2].z;


			exportData.push_back(ele);
		}
	}
}

void vixo_hairCacheExport::getInCurveInfo(map<int,int>& plugMapVID,MString vixoHairNode,map<int,MVectorArray>& inCurveShape)
{

	MSelectionList slist;
	MGlobal::getSelectionListByName(vixoHairNode,slist);
	MDagPath vixoHairNodeDag;
	slist.getDagPath(0,vixoHairNodeDag);
	vixoHairNodeDag.extendToShape();
	MFnDagNode fnVixoHair(vixoHairNodeDag);
	MPlug plugInCtrlCurveData=fnVixoHair.findPlug("inCtrlCurveData");

	map<int,int>::iterator iterPlugMapVID;
	for(iterPlugMapVID=plugMapVID.begin();iterPlugMapVID!=plugMapVID.end();iterPlugMapVID++)
	{
		MPlug inCurvePlug=plugInCtrlCurveData.elementByLogicalIndex(iterPlugMapVID->first);
		MFnVectorArrayData fnVector(inCurvePlug.asMObject());
		MVectorArray data=fnVector.array();
		MVectorArray tangent(data.length()-1);

		MPlugArray tempArr;
		inCurvePlug.connectedTo(tempArr,true,false);
		MFnDependencyNode fnHairSystem(tempArr[0].node());
		MPlug inputHairs=fnHairSystem.findPlug("inputHair");
		tempArr.clear();
		inputHairs.elementByLogicalIndex(iterPlugMapVID->first).connectedTo(tempArr,true,false);
		MFnDependencyNode fnFolli(tempArr[0].node());
		MFnMatrixData fnMatData(fnFolli.findPlug("worldMatrix[0]").asMObject());
		MMatrix mat=fnMatData.matrix();

		for(int i=0;i<data.length()-1;i++)
		{
			tangent[i]=data[i+1]*mat-data[i]*mat;
		}

		inCurveShape.insert(pair<int,MVectorArray>(iterPlugMapVID->second,tangent));
	}
	
}

void vixo_hairCacheExport::buildCache(int maxSpan,MObject& dynamicMeshObj,vector<vec3>& outCurveCacheData,map<int,MVectorArray>& inCurveShape,map<int,vector<outVIDCtrlInfo>>& outVertexControlData)
{
	MItMeshVertex iterMesh(dynamicMeshObj);
	int pre=0;
	for(int i=0;i<iterMesh.count();i++)
	{
		map<int,vector<outVIDCtrlInfo>>::iterator outcdIter=outVertexControlData.find(i);
		iterMesh.setIndex(i,pre);
		MVector normal;
		iterMesh.getNormal(normal,MSpace::kWorld);
		for(int ctrls=0;ctrls<outcdIter->second.size();ctrls++)
		{
			iterMesh.setIndex(outcdIter->second[ctrls].inVID,pre);
			MVector inNormal;
			iterMesh.getNormal(inNormal,MSpace::kWorld);
			map<int,MVectorArray>::iterator iterInCurve=inCurveShape.find(outcdIter->second[ctrls].inVID);
			MQuaternion quar=inNormal.rotateTo(normal);
			for(int j=0;j<maxSpan;j++)
			{
				MVector tempTan=iterInCurve->second[j].rotateBy(quar)*outcdIter->second[ctrls].percent;
				outCurveCacheData[i*maxSpan+j].x+=tempTan.x;
				outCurveCacheData[i*maxSpan+j].y+=tempTan.y;
				outCurveCacheData[i*maxSpan+j].z+=tempTan.z;
			}
		}
	}
	//output data
}

MString vixo_hairCacheExport::getFileName(MString cacheFileName,int frame)
{
	MString frameStr;
	frameStr.set(frame);
	if(frameStr.length()==1)
		frameStr="000"+frameStr;
	else if(frameStr.length()==2)
		frameStr="00"+frameStr;
	else if(frameStr.length()==3)
		frameStr="0"+frameStr;

	MString result=cacheFileName+"."+frameStr+".hcc";
	return result;
}
