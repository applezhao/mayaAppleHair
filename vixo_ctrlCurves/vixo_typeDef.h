#pragma comment (lib,"C:/Program Files (x86)/Microsoft SDKs/Windows/v7.0A/Lib/x64/glu32.lib")
#include <maya\MGLFunctionTable.h>
#include <maya\MHardwareRenderer.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnEnumAttribute.h>
#include <maya\MFnTypedAttribute.h>

#include <maya/MDrawData.h>
#include <maya/MDrawRequest.h>
#include <maya/MSelectionMask.h>
#include <maya/MSelectionList.h>

#include <maya/MPointArray.h>

#include <maya/MMessage.h>
#include <maya/MNodeMessage.h>
#include <maya/MQuaternion.h>

#include <maya/MMatrix.h>

#include <maya/MGlobal.h>

#include <set>
#include <map>
#include <vector>
#include <math.h>



#include <GL/glu.h>
#include <GL\GL.H>

#include <maya/MDagPath.h>

#include <maya/MVectorArray.h>
#include <maya/MFnMeshData.h>

#include <maya/MFnMesh.h>
#include <maya/MTransformationMatrix.h>

#include <maya/MFnUnitAttribute.h>

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



//int $i=1;
///for($i=1;$i<200;$i++)
//{
//	currentTime $i;
//	dgeval |transform1|vixo_ctrlCurves1.hiddenOutput; 
//	viewCapture ("f:/paipai"+$i);
//}


#define LEAD_COLOR				17	// green
#define ACTIVE_COLOR			17	// white
#define ACTIVE_AFFECTED_COLOR	17	// purple
#define DORMANT_COLOR			17	// blue
#define HILITE_COLOR			17	// pale blue
struct vixo_ctrlCurvesData
{
	int degree;
	int curveNum;
	int knotsSize;
	vector<::GLfloat> knots;
	vector<::GLfloat> allCurves;
};
void hairSysMsgCallBack(MNodeMessage::AttributeMessage msg, MPlug &plug, MPlug &otherPlug, void *clientData);

struct messageData
{
	MString vixoNodeName;
	MCallbackId id;
};

// for inCurveMatrix connection made
const int MAXSTEP=100;
const int MAXCONTROLSTEP=5;
struct forInVertexBasicInfo
{
	int vertexID;
	float distance;
	bool operator<(const forInVertexBasicInfo &a)const
	{
		return a.vertexID<vertexID;
	}
};

struct inVertexBasicInfo
{
	int cvNum;
	vector<set<forInVertexBasicInfo>> stepVertex;
};

struct inVertexShapeInfo
{
	MVector normal;
	MVectorArray tangent;
};
// for inCurveMatrix connection made

//for out curve dataBase
struct outInVertexRelationInfo
{
	int inVID;
	int step;
	float distance;
	//float percent;
	bool operator<(const outInVertexRelationInfo &a)const
	{
		if(this->step<a.step)
			return true;
		else if(this->step==a.step)
		{
			if(this->inVID<a.inVID)
				return true;
			else return false;
		}
		else
			return false;
	}
};
//for out curve dataBase

struct outVIDCtrlInfo
{
	int inVID;
	float percent;
};

struct triangleBasicInfo
{
	int faceID;
	int triID;
	int v1ID;
	int v2ID;
	int v3ID;
	int hairNum;
	int hairBegin;
	streampos beginIdx;
};

struct hairDetail
{
	float u;
	float v;
	float w0;
	float w1;
	float w2;
	int triIdx;
};

struct hairDetailExtend
{
	int closetClumpCenter;
	int triId;
	bool operator<(const hairDetailExtend& a)const
	{
		if(this->closetClumpCenter<a.closetClumpCenter)
			return true;
		return false;
	}
};

struct clumpCenterInfo
{
	int triid;
	int hairid;
	float u;
	float v;
	float minDist;
};

