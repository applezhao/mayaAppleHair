#pragma once

#include <maya\MVector.h>
#include <maya\MPoint.h>
#include <maya\MPointArray.h>

#include <vector>
#include <set>
#include <fstream>
#include <math.h>
#include <time.h>
#include <string>

#include <map>

struct inCurvesInfo
{
	int plugId;
	//vector<MVector> tanget;
	MPointArray cvs;
	vector<set<int>> nearestPoints;
};

//struct vertexInfo
//{
//	int vid;
//	float2 uv;
//	MVector normal;
	
//	bool needCurve;
//};

struct relatedCurveInfo
{
	//int vid;
	//int curveID;
	int step;
	float directDistance;
};

struct outCurvesInfo
{
	//int vid;
	bool show;
	bool oriCurved;
	MPointArray pointOnCurve;
	map<int,relatedCurveInfo> relatedCurves;
	vector<int> controlVID;
	vector<float> percent;
	float distanceSum;
};