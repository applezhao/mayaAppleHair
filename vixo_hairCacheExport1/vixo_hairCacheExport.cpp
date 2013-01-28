#include "vixo_hairCacheExport.h"

#include <maya\MArgList.h>

#include <maya\MGlobal.h>
#include <maya\MSelectionList.h>
#include <maya\MDagPath.h>
#include <maya\MFnNurbsCurve.h>
#include <maya\MPlug.h>
#include <maya\MPlugArray.h>
#include <maya\MFnDependencyNode.h>
#include <maya\MPoint.h>

#include <maya\MStringArray.h>

#include <maya\MAnimControl.h>

#include <fstream>
#include <vector>


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
//vixo_hairCacheExport -cf "f:/test.hsc" -ccn "vixo_hairNurbsCurve1" -seg 10;
//vixo_hairCacheExport -cf "f:/test.hsc" -ccn "vixo_hairNurbsCurve1" -seg 10 -sf 1 -ef 24;
MStatus vixo_hairCacheExport::doIt(const MArgList& args)
{
	MStatus stat = MS::kSuccess;

	MString fileName,referenceFile;
	MString vixoCtrlCurveNode;
	int segment;

	int startFrame,endFrame;

	unsigned index;

	index=args.flagIndex("cf","cacheFile");
	if(MArgList::kInvalidArgIndex!=index)
	{
		args.get(index+1,fileName);
	}

	index=args.flagIndex("ccn","vixoCtrlCurveNode");
	if(MArgList::kInvalidArgIndex!=index)
	{
		args.get(index+1,vixoCtrlCurveNode);
	}

	index=args.flagIndex("seg","segment");
	if(MArgList::kInvalidArgIndex!=index)
	{
		args.get(index+1,segment);
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

	MSelectionList slist;
	MGlobal::getSelectionListByName(vixoCtrlCurveNode,slist);
	//cout<<vixoCtrlCurveNode.asChar()<<endl;
	MObject vixoCtrlCurveObj;
	slist.getDependNode(0,vixoCtrlCurveObj);

	MFnDependencyNode fnVixoCurveCtrlNode(vixoCtrlCurveObj);
	MPlug outVIDPlug=fnVixoCurveCtrlNode.findPlug(MString("outVID"));

	for(int time=startFrame;time<=endFrame;time++)
	{
		MString frameNo;
		frameNo.set(time);
		if(frameNo.length()==1)
			frameNo="000"+frameNo;
		else if(frameNo.length()==2)
			frameNo="00"+frameNo;
		else if(frameNo.length()==3)
			frameNo="0"+frameNo;
		MString cacheFileNameFrame=fileName+"."+frameNo+".hsc";

		MTime mayaTime(time,MTime::kFilm);
		MAnimControl.setCurrentTime(mayaTime);


		//exportCache
		vector<int> vertexID(outVIDPlug.numElements());
		vector<curvePoint> pointsInfo(outVIDPlug.numElements()*(segment+3));
		for(int i=0;i<outVIDPlug.numElements();i++)
		{
			MPlugArray tempArr;
			outVIDPlug.elementByLogicalIndex(i).connectedTo(tempArr,false,true);
			MFnNurbsCurve fnCurve(tempArr[0].node());
			//根据segment导出数据
			vertexID[i]=fnCurve.findPlug("vertexID").asInt();
			//vertexID[i]=i;
			double min,max;
			fnCurve.getKnotDomain(min,max);
			for(int j=1;j<=segment+1;j++)
			{
				MPoint p;
				double param=j*(max-min)/(segment+1)+min;
				fnCurve.getPointAtParam(param,p,MSpace::kWorld);
				pointsInfo[i*(segment+3)+j].x=p.x;
				pointsInfo[i*(segment+3)+j].y=p.y;
				pointsInfo[i*(segment+3)+j].z=p.z;
			}

			pointsInfo[i*(segment+3)].x=pointsInfo[i*(segment+3)+1].x;
			pointsInfo[i*(segment+3)].y=pointsInfo[i*(segment+3)+1].y;
			pointsInfo[i*(segment+3)].z=pointsInfo[i*(segment+3)+1].z;

			pointsInfo[i*(segment+3)+segment+2].x=pointsInfo[i*(segment+3)+segment+1].x;
			pointsInfo[i*(segment+3)+segment+2].y=pointsInfo[i*(segment+3)+segment+1].y;
			pointsInfo[i*(segment+3)+segment+2].z=pointsInfo[i*(segment+3)+segment+1].z;
		}
		int curveNumSegment[2];
		curveNumSegment[0]=outVIDPlug.numElements();
		curveNumSegment[1]=segment+3;
		fstream fout(fileName.asChar(),ios_base::out|ios_base::binary|ios_base::trunc);
		fout.write((char*)&curveNumSegment[0],sizeof(int)*2);
		fout.write((char*)&vertexID[0],sizeof(int)*curveNumSegment[0]);
		fout.write((char*)&pointsInfo[0].x,sizeof(curvePoint)*curveNumSegment[0]*curveNumSegment[1]);
		fout.flush();
		fout.close();
		//~exportCache

	}

	
	

	//cout<<folliclePlugs.length()<<endl;
	/*
	for(int i=0;i<outCurvePlugs.length();i++)
	{
		MFnNurbsCurve fnCurve(outCurvePlugs[i].node());
				
		//根据segment导出数据
		vertexID[i]=fnCurve.findPlug("vertexID").asInt();
		//vertexID[i]=i;
		double min,max;
		fnCurve.getKnotDomain(min,max);
		for(int j=1;j<=segment+1;j++)
		{
			MPoint p;
			double param=j*(max-min)/(segment+1)+min;
			fnCurve.getPointAtParam(param,p,MSpace::kWorld);
			pointsInfo[i*(segment+3)+j].x=p.x;
			pointsInfo[i*(segment+3)+j].y=p.y;
			pointsInfo[i*(segment+3)+j].z=p.z;
		}

		pointsInfo[i*(segment+3)].x=pointsInfo[i*(segment+3)+1].x;
		pointsInfo[i*(segment+3)].y=pointsInfo[i*(segment+3)+1].y;
		pointsInfo[i*(segment+3)].z=pointsInfo[i*(segment+3)+1].z;

		pointsInfo[i*(segment+3)+segment+2].x=pointsInfo[i*(segment+3)+segment+1].x;
		pointsInfo[i*(segment+3)+segment+2].y=pointsInfo[i*(segment+3)+segment+1].y;
		pointsInfo[i*(segment+3)+segment+2].z=pointsInfo[i*(segment+3)+segment+1].z;
	}

	int curveNumSegment[2];
	curveNumSegment[0]=outCurvePlugs.length();
	curveNumSegment[1]=segment+3;
	fstream fout(fileName.asChar(),ios_base::out|ios_base::binary|ios_base::trunc);
	fout.write((char*)&curveNumSegment[0],sizeof(int)*2);
	fout.write((char*)&vertexID[0],sizeof(int)*curveNumSegment[0]);
	fout.write((char*)&pointsInfo[0].x,sizeof(curvePoint)*curveNumSegment[0]*curveNumSegment[1]);
	fout.flush();
	fout.close();
	
	*/
	/*//MPlugArray allConn;
	//fnVixoCurveCtrlNode.getConnections(allConn);
	//cout<<allConn.length()<<endl;
	//allConn[0].
	//MObjectArray follicles;
	//for(int i=0;i<allConn.length();i++)
	//{
	//	if(!existInSet(allConn[i].node(),follicles))
	//		follicles.append(allConn[i].node());
	//}

	//cout<<follicles.length()<<endl;
	//for(int i=0;i<follicles.length();i++)
	//{
	//	MFnDependencyNode fnFollicle(follicles[i]);
	//	cout<<i<<":"<<fnFollicle.name().asChar()<<endl;
	//}
	
	MPlug outUPlug=fnVixoCurveCtrlNode.findPlug(MString("outCurves"));
	cout<<"isarr:"<<outUPlug.info().asChar()<<endl;
	int outCount=outUPlug.evaluateNumElements();
	for(int i=0;i<outCount;i++)
	{
		MPlug uplug=outUPlug.elementByLogicalIndex(i).
	}
	MPlugArray folliclePlugs;
	outUPlug.connectedTo(folliclePlugs,false,true);

	vector<int> vertexID(folliclePlugs.length());
	vector<curvePoint> pointsInfo(folliclePlugs.length()*(segment+3));

	cout<<folliclePlugs.length()<<endl;

	for(int i=0;i<folliclePlugs.length();i++)
	{
		MFnDependencyNode fnFollicle(folliclePlugs[i].node());
		MPlug outCurvePlug=fnFollicle.findPlug(MString("outCurve"));
		MPlugArray outCurvePlugs;
		outCurvePlug.connectedTo(outCurvePlugs,false,true);
		MFnNurbsCurve fnCurve(outCurvePlugs[0].node());
				
		//根据segment导出数据
		//vertexID[i]=fnCurve.findPlug("vertexID").asInt();
		vertexID[i]=i;
		double min,max;
		fnCurve.getKnotDomain(min,max);
		for(int j=1;j<=segment+1;j++)
		{
			MPoint p;
			double param=j*(max-min)/(segment+1)+min;
			fnCurve.getPointAtParam(param,p,MSpace::kWorld);
			pointsInfo[i*(segment+3)+j].x=p.x;
			pointsInfo[i*(segment+3)+j].y=p.y;
			pointsInfo[i*(segment+3)+j].z=p.z;
		}

		pointsInfo[i*(segment+3)].x=pointsInfo[i*(segment+3)+1].x;
		pointsInfo[i*(segment+3)].y=pointsInfo[i*(segment+3)+1].y;
		pointsInfo[i*(segment+3)].z=pointsInfo[i*(segment+3)+1].z;

		pointsInfo[i*(segment+3)+segment+2].x=pointsInfo[i*(segment+3)+segment+1].x;
		pointsInfo[i*(segment+3)+segment+2].y=pointsInfo[i*(segment+3)+segment+1].y;
		pointsInfo[i*(segment+3)+segment+2].z=pointsInfo[i*(segment+3)+segment+1].z;
	}
	
	int curveNumSegment[2];
	curveNumSegment[0]=folliclePlugs.length();
	curveNumSegment[1]=segment+3;
	fstream fout(fileName.asChar(),ios_base::out|ios_base::binary|ios_base::trunc);
	fout.write((char*)&curveNumSegment[0],sizeof(int)*2);
	fout.write((char*)&vertexID[0],sizeof(int)*curveNumSegment[0]);
	fout.write((char*)&pointsInfo[0].x,sizeof(curvePoint)*curveNumSegment[0]*curveNumSegment[1]);
	fout.flush();
	fout.close();
	*/
	return MS::kSuccess;
}

bool vixo_hairCacheExport::existInSet(MObject& obj,MObjectArray & set)
{
	for(int i=0;i<set.length();i++)
	{
		if(obj==set[i])
			return true;
	}
	return false;
}
