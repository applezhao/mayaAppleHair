#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ri.h>


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

struct hairStyleData {
    RtString follicleInfoFileName[255];
	RtString CtrlCurveFileName[255];
	RtString tipColorTexName[255];
	RtString baseColorTexName[255];
	RtString tipOpacityTexName[255];
	RtString baseOpacityTexName[255];
	RtString densityMapFileName[255];
	RtFloat tipWidth;
	RtFloat baseWidth;
	RtFloat lengthNoiseAmp;
	RtFloat lengthNoiseFreq;
	RtFloat curlNoiseAmp;
	RtFloat curlNoiseFreq;
	RtFloat scraggle;
	RtFloat scraggleNoiseAmp;
	RtFloat scraggleNoiseFreq;
	RtFloat scraggleFreq;
	RtFloat scraggleFreqNoiseAmp;
	RtFloat scraggleFreqNoiseFreq;
	RtFloat polar;
	RtFloat polarNoiseAmp;
	RtFloat polarNoiseFreq;
	RtFloat inclination;
	RtFloat inclinationNoiseAmp;
	RtFloat inclinationNoiseFreq;
};

export RtPointer ConvertParameters(RtString paramstr) {

    struct quadricsData* data;
    char* parameters;
    char* token, *tokenstate;
    int i, j;

    parameters = strdup(paramstr);

    data = reinterpret_cast<struct quadricsData*>(malloc(sizeof(struct quadricsData)));

    token = strtok_r(parameters, " ", &tokenstate);
    data->shapeType = static_cast<QuadricsType>(atoi(token));

    token = strtok_r(0, " ", &tokenstate);
    data->radius1 = atof(token);

    token = strtok_r(0, " ", &tokenstate);
    data->radius2 = atof(token);

    token = strtok_r(0, " ", &tokenstate);
    data->height = atof(token);

    token = strtok_r(0, " ", &tokenstate);
    data->startAngle = atof(token);

    token = strtok_r(0, " ", &tokenstate);
    data->sweepAngle = atof(token);    
    
    free(parameters);
    return (RtPointer)(data);
}

export RtVoid Subdivide(RtPointer datap, RtFloat detail) {
    struct hairStyleData* data = reinterpret_cast<struct hairStyleData*>(datap);
    int i, j;
	
    RiTransformBegin();

    RtPoint p1;
    RtPoint p2;
    p1[0] = data->radius1;
    p2[0] = data->radius2;
    p1[1] = 0;
    p1[2] = 0;
    p2[1] = 0;
    p2[2] = 0;

    switch (data->shapeType) {
    case kCylinder:
	p2[2] = data->height;
	RiHyperboloid(p1, p2, 360, RI_NULL);
	break;
    case kDisk:
	RiHyperboloid(p1, p2, 360, RI_NULL);
	break;
    case kPartialDisk:
	RiRotate(90, 0, 0, 1);
	RiRotate(-data->startAngle, 0, 0, 1);
	RiHyperboloid(p1, p2, -data->sweepAngle, RI_NULL);
	break;
    case kSphere:
	RiSphere(data->radius1, -data->radius1, data->radius1, 360, RI_NULL);
	break;
    }
    RiTransformEnd();
}

export RtVoid Free(RtPointer datap) {
    struct quadricsData* data = reinterpret_cast<struct quadricsData*>(datap);
    free(data);
}

