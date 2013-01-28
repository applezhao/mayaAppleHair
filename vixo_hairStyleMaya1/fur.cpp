#include <stdio.h>
#include <stdlib.h>
#if defined (_WIN32)
#include <strstream>
#else
#include <unistd.h>
#include <strstream>
#endif
#include <math.h>
#include <sys/stat.h>
#include <string>

#include <map>
#include <algorithm>
using namespace std;

#include "util.h"
#include "noise.h"
#include "RIBGen.h"
#include "RIBContext.h"
#include "IFFImage.h"
#include "furBase.h"

#ifndef M_PI
const double M_PI = 3.14159265358979323846;
#endif
#ifdef _WIN32
#define unlink _unlink
#define stat _stat
#endif

//////////////////////////////////////////////////
FurFloatParameter::FurFloatParameter() :
base(0), noiseFrequency(1), noiseMultiplier(0),
	mapImage(0), mapMultiplier(1) {
}

FurFloatParameter::~FurFloatParameter() {
	delete(mapImage);
}

void FurFloatParameter::reset() {
	base = 0;
	noiseFrequency = 1;
	noiseMultiplier = 0;
	delete(mapImage);
	mapImage = 0;
	mapMultiplier = 1;
}

void FurFloatParameter::parse(string& s) {
	reset();

	// Parse the value string into a value, a noise, and a map if
	// applicable. This is done in a fairly brain-dead fashion, by
	// splitting it up based on '+', and then making some guesses
	bool quit = false;
	while (!quit) {
		string token = trim(tokenize(s, '+'));
		if (token.empty()) {
			quit = true;
			token = s;
		}
		// noise
		if (token.find("noise") != string::npos) {
			// This should be of the form: ( 0 * noise(10,-1,1) )
			// The only relevant information is the first number (the
			// multiplier) and the second number (the frequency)
			istrstream is((char *)token.c_str());
			is.ignore(2); // ( 
			is >> noiseMultiplier;
			is.ignore(9); // * noise(
			is >> noiseFrequency;
		}
		// map
		else if (token.find("image") != string::npos) {
			// map name is the first quote
			// delimited string
			string mapName = quoteDelimited(token);
			if (!mapName.empty()) {
				mapImage = new IFFImage(mapName);
				if (!mapImage) {
					delete mapImage;
					mapImage = 0;
				}
			}
			// There may be a map multiplier
			if (token.find('*') != string::npos) {
				tokenize(token, '*');
				mapMultiplier = atof(token.c_str());
			}
		}
		// number
		else {
			base = atof(token.c_str());
		}
	}
}

RtFloat FurFloatParameter::getValue(RtFloat u, RtFloat v) const {
	RtFloat val = base;
	if (noiseMultiplier != 0) {
		val += noiseMultiplier * noise(u * noiseFrequency, v * noiseFrequency);
	}
	if (mapImage) {
		val += mapImage->getLuminance(u, v) * mapMultiplier;
	}
	return val;
}

//////////////////////////////////////////////////
FurColorParameter::FurColorParameter() :
noiseFrequency(1), mapImage(0) {
	base[0] = base[1] = base[2] = 0;
	noiseMultiplier[0] = noiseMultiplier[1] = noiseMultiplier[2] = 0;
}

FurColorParameter::~FurColorParameter() {
	delete mapImage;
}

void FurColorParameter::reset() {
	base[0] = base[1] = base[2] = 0;
	noiseFrequency = 1;
	noiseMultiplier[0] = noiseMultiplier[1] = noiseMultiplier[2] = 0;
	delete(mapImage);
	mapImage = 0;
}

void FurColorParameter::parse(string& s) {

	reset();

	// Parse the value string into a value, a noise, and a map if
	// applicable. This is done in a fairly brain-dead fashion, by
	// splitting it up based on '+', and then making some guesses
	bool quit = false;
	while (!quit) {
		string token = trim(tokenize(s, '+'));
		if (token.empty()) {
			quit = true;
			token = s;
		}
		// noise
		if (token.find("noise") != string::npos) {
			// This should be of the form:
			// ( <0,0,0> * noise(10,<-1,-1,-1>,<1,1,1>) )
			// The only relevant information is the first triple (the
			// multiplier) and the second number (the frequency)
			istrstream is((char *)token.c_str());
			is.ignore(3); // ( <
			is >> noiseMultiplier[0];
			is.ignore(1);
			is >> noiseMultiplier[1];
			is.ignore(1);
			is >> noiseMultiplier[2];
			is.ignore(10); // > * noise(
			is >> noiseFrequency;
		}
		// map
		else if (token.find("image") != string::npos) {
			// map name is the first quote
			// delimited string
			string mapName = quoteDelimited(token);
			if (!mapName.empty()) {
				mapImage = new IFFImage(mapName);
			}
		}
		// number
		else {
			// Parse colors
			if (token[0] == '<') {
				char c;
				RtFloat tmp[3];
				istrstream ss((char *)token.c_str());
				ss >> c >> tmp[0]
				>> c >> tmp[1]
				>> c >> tmp[2];
				base[0] += tmp[0];
				base[1] += tmp[1];
				base[2] += tmp[2];
			}
		}
	}
}

RtFloat Attractor::distance(RtFloat x, RtFloat y, RtFloat z) const {
	return sqrt((basePoint[0] - x) * (basePoint[0] - x) +
		(basePoint[1] - y) * (basePoint[1] - y) +
		(basePoint[2] - z) * (basePoint[2] - z));

}

// Calculates basePoint, midPoint, and endPoint from the matrices
void Attractor::calculatePoints(const RtMatrix objToWorld) {
	int i, j;
	RtFloat hairTransform[4][4];

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			hairTransform[i][j] = transform[i][j];
		}
	}
	RtPoint tmp;
	for (i = 0; i < 3; i++) {
		tmp[i] =
			hairTransform[3][0] * baseTransform[0][i] +
			hairTransform[3][1] * baseTransform[1][i] +
			hairTransform[3][2] * baseTransform[2][i] + 
			hairTransform[3][3] * baseTransform[3][i];
	}
	for (i = 0; i < 3; i++) {
		basePoint[i] = objToWorld[i][0] * tmp[0] +
			objToWorld[i][1] * tmp[1] +
			objToWorld[i][2] * tmp[2];
	}
	for (i = 0; i < 3; i++) {
		tmp[i] =
			hairTransform[3][0] * midTransform[0][i] +
			hairTransform[3][1] * midTransform[1][i] +
			hairTransform[3][2] * midTransform[2][i] + 
			hairTransform[3][3] * midTransform[3][i];
	}
	for (i = 0; i < 3; i++) {
		midPoint[i] = objToWorld[i][0] * tmp[0] +
			objToWorld[i][1] * tmp[1] +
			objToWorld[i][2] * tmp[2];
	}
	for (i = 0; i < 3; i++) {
		tmp[i] =
			hairTransform[3][0] * endTransform[0][i] +
			hairTransform[3][1] * endTransform[1][i] +
			hairTransform[3][2] * endTransform[2][i] + 
			hairTransform[3][3] * endTransform[3][i];
	}
	for (i = 0; i < 3; i++) {
		endPoint[i] = objToWorld[i][0] * tmp[0] +
			objToWorld[i][1] * tmp[1] +
			objToWorld[i][2] * tmp[2];
	}

}


void Attractor::getPoint(RtFloat t, RtPoint p) const {
	// Quadratic bezier curve evaluation
	RtPoint p1 = {
		basePoint[0] + t * (midPoint[0] - basePoint[0]),
		basePoint[1] + t * (midPoint[1] - basePoint[1]),
		basePoint[2] + t * (midPoint[2] - basePoint[2])
	};

	RtPoint p2 = {
		midPoint[0] + t * (endPoint[0] - midPoint[0]),
		midPoint[1] + t * (endPoint[1] - midPoint[1]),
		midPoint[2] + t * (endPoint[2] - midPoint[2])
	};
	p[0] = p1[0] + t * (p2[0] - p1[0]);
	p[1] = p1[1] + t * (p2[1] - p1[1]);
	p[2] = p1[2] + t * (p2[2] - p1[2]);
}

void FurColorParameter::getValue(RtFloat u, RtFloat v, RtColor c) const {

	c[0] = base[0];
	c[1] = base[1];    
	c[2] = base[2];
	if (mapImage) {
		RtColor mapValue;
		mapImage->getColor(u, v, mapValue);
		c[0] += mapValue[0];
		c[1] += mapValue[1];
		c[2] += mapValue[2];
	};
	if (noiseMultiplier[0] != 0) {
		c[0] += noiseMultiplier[0] * noise(u * noiseFrequency, v * noiseFrequency);
	}
	if (noiseMultiplier[1] != 0) {
		c[1] += noiseMultiplier[1] * noise((u + 17) * noiseFrequency, (v + 13) * noiseFrequency);
	}
	if (noiseMultiplier[2] != 0) {
		c[2] += noiseMultiplier[2] * noise((u + 31) * noiseFrequency, (v + 43) * noiseFrequency);
	}
}

FurBase::FurBase() : furNumAttractors(0), furAttractors(0), attractorsPerHair(0)  {
}

FurBase::~FurBase() {
	if (furAttractors) {
		delete[] furAttractors;
	}
}

// Specializations used when sorting attractors based on distance
bool operator<(const pair<RtFloat, Attractor*>& x,
	const pair<RtFloat, Attractor*>& y) {
		return x.first < y.first;
}

// Specializations used when sorting attractors based on distance
bool operator==(const pair<RtFloat, Attractor*>& x,
	const pair<RtFloat, Attractor*>& y) {
		return x.first == y.first;
}

void
	FurBase::Bound(RIBContext *, RtBound) {
		// do nothing
}

int
	FurBase::GenRIB(RIBContext * c) {

		noiseinit();

		// Initialize the jittering 
		srand(furJitterSeed);

		RIBContext::RenderingPass p;
		RtBoolean decl;

		c->GetRenderingPass(&p, &decl);
		if (decl) {
			return 0;
		} else if (p == RIBContext::rpShadow) {
			if (!castShadows)
				return 0;		// don't output fur during shadow pass
		}
		if (!c->GetObjName()) {
			return 0;
		}
		const string objName(c->GetObjName());
		if (objName.empty()) {
			return 0;
		}

		// Call the virtual prepass function
		preGenRIB(c);

		// We ask Maya (via a mtor Mel command) for the CVs, normals, and
		// tangents of the surface given by objName. Since this is a lot
		// of data, it's more efficient to tell the Maya-side routine to
		// output to file, and return the filename as the result.
		if (fileName.empty()) {
			c->ReportError(RIBContext::reSevere, "You must specify a filename to output to file.");
			return -1;
		}
		if (!(lazyEvaluate && fileExists(fileName))) {
			char cmd[512];
			sprintf(cmd, "mtor geom getInfo %s -uvs -pts -normals -tangents -samples %d %d %f -seed %d -trim -tofile \"%s\"",
				objName.c_str(), furInU, furInV, furPlacementJitter,
				furJitterSeed,
				fileName.c_str());

			string errorString;
			const RIBContextResult* result = c->ExecuteHostCmd(cmd, errorString);

			if (result->ResultType() != RIBContextResult::kString) {
				c->ReportError(RIBContext::reSevere,
					"mtorFur:unexpected return type (%s)", errorString.c_str());
				return -1;
			}
		} else {
			c->ReportError(RIBContext::reInfo, "mtorFur:lazy on %s\n", fileName.c_str());
		}
		vector <RtFloat> us, vs, pts, normals, tangentU, tangentV;
		if (!readSurfaceGeom(fileName, us, vs, pts, normals, tangentU, tangentV)) {
			c->ReportError(RIBContext::reSevere, "mtorFur:can't open %s", fileName.c_str());
			return -1;
		}
		const int maxHairs = us.size();
		const int ptsPerCurve = 3 + furSegments;
		unsigned actualHairs = 0;

		RtPoint *cvs;
		RtInt *nverts;
		RtFloat *widths;
		RtColor *opacities;
		RtColor *basecolors;
		RtColor *tipcolors;    
		RtColor *speculars;    
		nverts = new RtInt[maxHairs];
		if (!nverts) {
			return -2;
		}
		cvs = new RtPoint[ptsPerCurve * maxHairs];
		if (!cvs) {
			return -2;
		}
		widths = new RtFloat[(furSegments + 1) * maxHairs];
		if (!widths) {
			return -2;
		}
		opacities = new RtColor[(furSegments + 1) * maxHairs];
		if (!opacities) {
			return -2;
		}

		// This relies on 3.9 semantics for uniform variables on curves,
		// which are bound one per curve (and not one per segment).
		basecolors = new RtColor[maxHairs];
		if (!basecolors) {
			return -2;
		}
		tipcolors = new RtColor[maxHairs];
		if (!tipcolors) {
			return -2;
		}
		speculars = new RtColor[maxHairs];
		if (!speculars) {
			return -2;
		}

		RtFloat l;
		RtFloat n[3], invn[3], t[3];
		int i = 0, j = 0, k = 0;

		// Multiplier for segments - used to scale the opacity, width,
		// curl per segment
		const RtFloat sFactor = 1.0 / furSegments;

		for (i = 0; i < maxHairs; i++) {

			const RtFloat u = us[i];
			const RtFloat v = vs[i];

			l = furLength.getValue(u, v);

			// Degeneracy check: if the tangents or Ns are wacky, we can't
			// use it. Maya doesn't seem to make much of an effort to
			// prevent degenerate tangents. Also check the length,
			// and baldness; we generate random floating point values
			// between 0 and 1, and compare them with the baldness
			// to see if we generate this hair or not
			if ((l > 0) &&
				(randomize() < furBaldness.getValue(u, v)) &&	    
				(!(
				(floatIsSmall(normals[j]) &&
				floatIsSmall(normals[j + 1]) &&
				floatIsSmall(normals[j + 2])) ||
				(floatIsSmall(tangentU[j]) &&
				floatIsSmall(tangentU[j + 1]) &&
				floatIsSmall(tangentU[j + 2])) ||
				(floatIsSmall(tangentV[j]) &&
				floatIsSmall(tangentV[j + 1]) &&
				floatIsSmall(tangentV[j + 2]))))) {

					// Calculate some basic angles from the curl and scraggle
					const RtFloat furBaseAdjustedCurl = -M_PI * (2 * furBaseCurl.getValue(u, v) - 1);
					const RtFloat furTipAdjustedCurl = -M_PI * (2 * furTipCurl.getValue(u, v) - 1);
					// This was done empirically to match Maya's scraggle
					const RtFloat furAdjustedScraggle = M_PI * furScraggle.getValue(u, v) / 2.0;

					// Get things which will not change over the hair
					RtColor baseColor, tipColor;
					furBaseColor.getValue(u, v, baseColor);

					furTipColor.getValue(u, v, tipColor);
					const RtFloat baseOpacity = furBaseOpacity.getValue(u, v);
					const RtFloat tipOpacity = furTipOpacity.getValue(u, v);
					const RtFloat baseWidth = furBaseWidth.getValue(u, v);
					const RtFloat tipWidth = furTipWidth.getValue(u, v);

					// Look for the attractors which are going to affect this
					// hair, based solely on the radius
					vector<pair<RtFloat, Attractor*> > attractors;
					RtFloat radius = attractorRadius.getValue(u, v);
					for (int m = 0; m < furNumAttractors; m++) {
						RtFloat distance = furAttractors[m].distance(pts[j], pts[j+1], pts[j+2]);
						if (distance < radius) {
							attractors.push_back(make_pair(distance, furAttractors + m));
						}
					}

					// Use only the closest hairs - sort the vector and trim
					// the irrelevant attractors
					sort(attractors.begin(), attractors.end());
					if (attractors.size() > attractorsPerHair) {
						attractors.erase(attractors.begin() + attractorsPerHair, attractors.end());
					}

					// We can now calculate the sum distance of all attractors
					// to the hair; we'll need this result later
					RtFloat sumAttractorDistance = 0;
					vector<pair<RtFloat, Attractor*> >::iterator a;		
					for (a = attractors.begin(); a != attractors.end(); a++) {
						sumAttractorDistance += a->first;
					}

					// Normal
					if (flipNormals) {
						n[0] = -normals[j];
						n[1] = -normals[j + 1];
						n[2] = -normals[j + 2];
					} else {
						n[0] = normals[j];
						n[1] = normals[j + 1];
						n[2] = normals[j + 2];
					}

					// Figure out the tangent vector to use, based on polar
					// information
					t[0] = tangentU[j];
					t[1] = tangentU[j + 1];
					t[2] = tangentU[j + 2];

					RtFloat polar = furPolar.getValue(u, v);
					if (polar != 0.5) {
						RtFloat theta = M_PI * (2 * polar - 1);
						rotateVector(theta, n, t);
					}

					// The axis of rotation for the curl will be N x T
					RtFloat axis[3];
					axis[0] = n[1] * t[2] - n[2] * t[1];
					axis[1] = n[2] * t[0] - n[0] * t[2];
					axis[2] = n[0] * t[1] - n[1] * t[0];
					normalize(axis);

					// Calculate the initial direction of the hair from the
					// geometric normal, tangent, and the inclination
					RtFloat incline = furInclination.getValue(u, v);
					if (incline < 0) {
						n[0] = (1 - fabs(incline)) * n[0] +
							incline * t[0];
						n[1] = (1 - fabs(incline)) * n[1] +
							incline * t[1];
						n[2] = (1 - fabs(incline)) * n[2] +
							incline * t[2];
					} else {
						n[0] = (1 - incline) * n[0] +
							incline * t[0];
						n[1] = (1 - incline) * n[1] +
							incline * t[1];
						n[2] = (1 - incline) * n[2] +
							incline * t[2];
					}

					normalize(n);


					//clump apple
					// Calculate the effect of all attractors
					if (attractorType == "local") {
						// Mimic the closest hairs.
						for (vector<pair<RtFloat, Attractor*> >::iterator a = attractors.begin(); a != attractors.end(); a++) {
							Attractor* attractor = a->second;

							RtPoint p0, p1;
							attractor->getPoint(0, p0);
							attractor->getPoint(sFactor, p1);
							RtFloat an[3] = {
								p1[0] - p0[0],
								p1[1] - p0[1],
								p1[2] - p0[2]
							};
							normalize(an);
							RtFloat weight = furAttraction.getValue(u, v);

							// The attraction factor seems to be controlled by
							// how far we are from the base of the hair, and
							// is also weighed by how far away from the hair
							// we are
							weight *= sFactor * a->first / sumAttractorDistance;

							n[0] = n[0] * (1 - weight) + an[0] * weight;
							n[1] = n[1] * (1 - weight) + an[1] * weight;
							n[2] = n[2] * (1 - weight) + an[2] * weight;
							normalize(n);
						}
					} else {
						// Grow toward the nearest hairs
						for (vector<pair<RtFloat, Attractor*> >::iterator a = attractors.begin(); a != attractors.end(); a++) {
							Attractor* attractor = a->second;

							RtPoint p1;
							attractor->getPoint(sFactor, p1);
							RtFloat an[3] = {
								p1[0] - pts[j],
								p1[1] - pts[j+1],
								p1[2] - pts[j+2]
							};
							normalize(an);		    
							RtFloat weight = furAttraction.getValue(u, v);

							// The attraction factor seems to be controlled by
							// how far we are from the base of the hair, and
							// is also weighed by how far away from the hair
							// we are
							weight *= sFactor * a->first / sumAttractorDistance;

							n[0] = n[0] * (1 - weight) + an[0] * weight;
							n[1] = n[1] * (1 - weight) + an[1] * weight;
							n[2] = n[2] * (1 - weight) + an[2] * weight;
							normalize(n);
						}
					}

					// Reversed direction, used to calculate the first point
					invn[0] = -n[0];
					invn[1] = -n[1];
					invn[2] = -n[2];

					// Scraggle the initial hair directions
					if (furAdjustedScraggle > 0) {
						RtFloat sAxis[3];
						randomAxis(sAxis);
						rotateVector(furAdjustedScraggle, sAxis, n);
						randomAxis(sAxis);
						rotateVector(furAdjustedScraggle, sAxis, invn);
					}


					// Length of each "segment" of the cubic hull. This is
					// only approximate and causes the actual arc length of
					// the cubic spline to be wrong, but with more segments
					// the accuracy is better.
					const RtFloat segmentLength = l / (furSegments);


					// Issue uniform data for entire hair
					nverts[actualHairs] = ptsPerCurve;
					furSpecColor.getValue(u, v, speculars[actualHairs]);
					furBaseColor.getValue(u, v, basecolors[actualHairs]);
					furTipColor.getValue(u, v, tipcolors[actualHairs]);	    

					// Issue data for base of the hair.
					int ioffset = actualHairs * (1 + furSegments);
					int joffset = actualHairs * ptsPerCurve;
					int ii = 0;
					int jj = 0;

					opacities[ii + ioffset][0] = baseOpacity;
					opacities[ii + ioffset][1] = baseOpacity;
					opacities[ii + ioffset][2] = baseOpacity;

					widths[ii + ioffset] = baseWidth;


					// Second point of the spline (the actual base point of
					// the hair)
					cvs[1 + joffset][0] = pts[j];
					cvs[1 + joffset][1] = pts[j + 1];
					cvs[1 + joffset][2] = pts[j + 2];

					// First point of Catmull-Rom spline, which is "behind" the
					// base. We'll calculate this by going backwards from the 2nd
					// pt with appropriate curl applied so the tangents at the
					// base are set up nicely.
					rotateVector(-furBaseAdjustedCurl / furSegments / 2.0, axis, invn);
					cvs[joffset][0] =
						cvs[1 + joffset][0] + segmentLength * invn[0];
					cvs[joffset][1] =
						cvs[1 + joffset][1] + segmentLength * invn[1];
					cvs[joffset][2] =
						cvs[1 + joffset][2] + segmentLength * invn[2];

					jj = 1;

					// Apply initial curl, which will just be the base curl
					// applied at full strength. The division by two should be
					// obvious if you draw a picture.
					rotateVector(furBaseAdjustedCurl / furSegments / 2.0, axis, n);

					if (furAdjustedScraggle > 0) {
						RtFloat sAxis[3];
						randomAxis(sAxis);
						rotateVector(furAdjustedScraggle * (2 * randomize() - 1.0), sAxis, n);
					}

					// Segment loop
					for (k = 0; k < furSegments; k++) {

						// Add one new point
						jj++;
						cvs[jj + joffset][0] =
							cvs[jj + joffset - 1][0] + segmentLength * n[0];
						cvs[jj + joffset][1] =
							cvs[jj + joffset - 1][1] + segmentLength * n[1];
						cvs[jj + joffset][2] =
							cvs[jj + joffset - 1][2] + segmentLength * n[2];

						// Add per-segment information
						ii++;

						// Calculate the (next) curl as a lerp of the base and tip
						RtFloat curl = (furBaseAdjustedCurl + ii * sFactor * (furTipAdjustedCurl - furBaseAdjustedCurl)) / furSegments;

						rotateVector(curl, axis, n);
						if (furAdjustedScraggle > 0) {
							RtFloat sAxis[3];

							randomAxis(sAxis);
							rotateVector(furAdjustedScraggle * (2 * randomize() - 1.0), sAxis, n);
						}

						// Calculate the effect of all attractors again
						if (attractorType == "local") {

							for (vector<pair<RtFloat, Attractor*> >::iterator a = attractors.begin(); a != attractors.end(); a++) {
								Attractor* attractor = a->second;
								RtPoint p0, p1;
								attractor->getPoint((ii - 1) * sFactor, p0);
								attractor->getPoint(ii * sFactor, p1);
								RtFloat an[3] = {
									p1[0] - p0[0],
									p1[1] - p0[1],
									p1[2] - p0[2]
								};
								normalize(an);
								RtFloat weight = furAttraction.getValue(u, v);
								weight *= sFactor * a->first / sumAttractorDistance;
								n[0] = n[0] * (1 - weight) + an[0] * weight;
								n[1] = n[1] * (1 - weight) + an[1] * weight;
								n[2] = n[2] * (1 - weight) + an[2] * weight;
								normalize(n);
							}
						} else {
							// Grow toward the nearest hairs
							for (vector<pair<RtFloat, Attractor*> >::iterator a = attractors.begin(); a != attractors.end(); a++) {
								Attractor* attractor = a->second;

								RtPoint p1;
								attractor->getPoint(ii * (sFactor + 1), p1);
								RtFloat an[3] = {
									p1[0] - cvs[jj + joffset][0],
									p1[1] - cvs[jj + joffset][1],
									p1[2] - cvs[jj + joffset][2]
								};
								normalize(an);
								RtFloat weight = furAttraction.getValue(u, v);

								// The attraction factor seems to be controlled by
								// how far we are from the base of the hair, and
								// is also weighed by how far away from the hair
								// we are
								weight *= sFactor * a->first / sumAttractorDistance;

								n[0] = n[0] * (1 - weight) + an[0] * weight;
								n[1] = n[1] * (1 - weight) + an[1] * weight;
								n[2] = n[2] * (1 - weight) + an[2] * weight;
								normalize(n);
							}
						}
						// Add opacity
						opacities[ii + ioffset][0] = opacities[ii + ioffset][1] = opacities[ii + ioffset][2] = baseOpacity + ii * sFactor * (tipOpacity - baseOpacity);
						// Add width
						widths[ii + ioffset] = baseWidth + ii * sFactor * (tipWidth - baseWidth);

					}			// end segment loop

					// Last point of the curve - just continue adding the
					// extra point with the adjusted normal as we normally do,
					// even though the hair won't go through this point.
					jj++;
					cvs[jj + joffset][0] =
						cvs[jj + joffset - 1][0] + segmentLength * n[0];
					cvs[jj + joffset][1] =
						cvs[jj + joffset - 1][1] + segmentLength * n[1];
					cvs[jj + joffset][2] =
						cvs[jj + joffset - 1][2] + segmentLength * n[2];

					actualHairs++;
			}
			j += 3;
		}

		if (actualHairs > 0) {
			c->AttributeBegin();
			if (useRatFuzz) {
				c->Surface("ratFuzz",
					"Ka", &furKa,
					"Kd", &furKd,
					"Ks", &furKs,
					"roughness", &furRoughness,
					RI_NULL);
			}
			RtToken names[6] = {
				"P",
				"Os",
				"uniform color rootcolor",
				"uniform color tipcolor",
				"width",
				"uniform color specularcolor"
			};
			RtPointer values[6] = {
				cvs,
				opacities,
				basecolors,
				tipcolors,
				widths,
				speculars
			};

			RtBasis catmullRom = {
				{ -0.5,  1.5, -1.5,  0.5 },
				{  1.0, -2.5,  2.0, -0.5 },
				{ -0.5,  0.0,  0.5,  0.0 },
				{  0.0,  1.0,  0.0,  0.0 }
			};
			c->Basis(catmullRom, 1, catmullRom, 1);
			c->CurvesV("cubic", actualHairs, nverts, "nonperiodic",
				6, names, values);

			c->AttributeEnd();
		}
		delete[] cvs;
		delete[] nverts;
		delete[] widths;
		delete[] opacities;
		delete[] basecolors;
		delete[] tipcolors;
		delete[] speculars;    
		return 0;
}

bool FurBase::floatIsSmall(const float f) const {
	return (fabs(f) < 0.001);
}

// Generates a random normalized axis of rotation
void FurBase::randomAxis(RtFloat axis[3]) const {
	axis[2] = 2.0 * randomize() - 1.0;
	RtFloat t = 2.0 * M_PI * randomize();
	RtFloat w = sqrt(1 - axis[2] * axis[2]);

	axis[0] = w * cos(t);
	axis[1] = w * sin(t);
}

bool FurBase::readSurfaceGeom(const string & filename,
	vector<RtFloat> &u,
	vector<RtFloat> &v,
	vector<RtFloat> &pts,
	vector<RtFloat> &normals,
	vector<RtFloat> &tangentU,
	vector<RtFloat> &tangentV) const {
		FILE *f = fopen(filename.c_str(), "r");
		if (!f)
			return false;
		double s, t, x, y, z, nx, ny, nz, ux, uy, uz, vx, vy, vz;
		char buf[256];

		u.clear();
		v.clear();
		pts.clear();
		normals.clear();
		tangentU.clear();
		tangentV.clear();

		while (fgets(buf, 256, f)) {
			// line contains a nan? throw it out
			if (strchr(buf, 'n')) {
				continue;
			}

			if (14 == sscanf(buf, "%lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg %lg",
				&s, &t, &x, &y, &z, &nx, &ny, &nz,
				&ux, &uy, &uz,
				&vx, &vy, &vz)) {
					u.push_back(s);
					v.push_back(t);
					pts.push_back(x);
					pts.push_back(y);
					pts.push_back(z);
					normals.push_back(nx);
					normals.push_back(ny);
					normals.push_back(nz);
					tangentU.push_back(ux);
					tangentU.push_back(uy);
					tangentU.push_back(uz);
					tangentV.push_back(vx);
					tangentV.push_back(vy);
					tangentV.push_back(vz);
			}
		}
		fclose(f);
		if (cleanupFile) {
			unlink(filename.c_str());
		}
		return true;
}

bool FurBase::fileExists(const string & filename) const {
	struct stat sbuf;
	return (stat(filename.c_str(), &sbuf) == 0);
}

void FurBase::rotateVector(const RtFloat angle, const RtFloat axis[3], RtFloat normal[3]) const {
	// A quaternion rotation. See Shoemake.
	RtFloat halfangle = angle / 2.0;
	RtFloat sinHA = sin(halfangle);
	RtFloat q[4] = {
		axis[0] * sinHA,
		axis[1] * sinHA,
		axis[2] * sinHA,
		cos(halfangle)
	};
	RtFloat xs = 2 * q[0], ys = 2 * q[1], zs = 2 * q[2];
	RtFloat wx = q[3] * xs, wy = q[3] * ys, wz = q[3] * zs, xx = q[0] * xs,
		xy = q[0] * ys, xz = q[0] * zs, yy = q[1] * ys, yz = q[1] * zs, zz = q[2] * zs;
	RtFloat rotMat[3][3] = {
		{1 - (yy + zz), xy - wz, xz + wy},
		{xy + wz, 1 - (xx + zz), yz - wx},
		{xz - wy, yz + wx, 1 - (xx + yy)}
	};
	RtFloat oldN[3] = {
		normal[0], normal[1], normal[2]
	};

	normal[0] = rotMat[0][0] * oldN[0] +
		rotMat[0][1] * oldN[1] +
		rotMat[0][2] * oldN[2];
	normal[1] = rotMat[1][0] * oldN[0] +
		rotMat[1][1] * oldN[1] +
		rotMat[1][2] * oldN[2];
	normal[2] = rotMat[2][0] * oldN[0] +
		rotMat[2][1] * oldN[1] +
		rotMat[2][2] * oldN[2];
}

void FurBase::normalize(RtFloat n[3]) const {
	RtFloat length = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
	if (!floatIsSmall(length)) {
		length = 1.0 / length;
		n[0] *= length;
		n[1] *= length;
		n[2] *= length;
	}
}
