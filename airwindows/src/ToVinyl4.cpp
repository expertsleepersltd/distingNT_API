#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ToVinyl4"
#define AIRWINDOWS_DESCRIPTION "A vinyl-mastering simulator bringing several vinyl-type colors."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','o','4' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Mid Highpass", .min = 1000, .max = 30000, .def = 2200, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Side Highpass", .min = 1000, .max = 30000, .def = 4400, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "HF Limiter", .min = 0, .max = 1000, .def = 320, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Groove Wear", .min = 0, .max = 1000, .def = 64, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	Float32 ataLastOutL;
	Float32 ataLastOutR;
	
	Float32 s1L;
	Float32 s2L;
	Float32 s3L;
	Float32 o1L;
	Float32 o2L;
	Float32 o3L;
	Float32 m1L;
	Float32 m2L;
	Float32 s1R;
	Float32 s2R;
	Float32 s3R;
	Float32 o1R;
	Float32 o2R;
	Float32 o3R;
	Float32 m1R;
	Float32 m2R;
	Float32 desL;
	Float32 desR;
	
	
	Float32 midSampleA;
	Float32 midSampleB;
	Float32 midSampleC;
	Float32 midSampleD;
	Float32 midSampleE;
	Float32 midSampleF;
	Float32 midSampleG;
	Float32 midSampleH;
	Float32 midSampleI;
	Float32 midSampleJ;
	Float32 midSampleK;
	Float32 midSampleL;
	Float32 midSampleM;
	Float32 midSampleN;
	Float32 midSampleO;
	Float32 midSampleP;
	Float32 midSampleQ;
	Float32 midSampleR;
	Float32 midSampleS;
	Float32 midSampleT;
	Float32 midSampleU;
	Float32 midSampleV;
	Float32 midSampleW;
	Float32 midSampleX;
	Float32 midSampleY;
	Float32 midSampleZ;
	
	Float32 sideSampleA;
	Float32 sideSampleB;
	Float32 sideSampleC;
	Float32 sideSampleD;
	Float32 sideSampleE;
	Float32 sideSampleF;
	Float32 sideSampleG;
	Float32 sideSampleH;
	Float32 sideSampleI;
	Float32 sideSampleJ;
	Float32 sideSampleK;
	Float32 sideSampleL;
	Float32 sideSampleM;
	Float32 sideSampleN;
	Float32 sideSampleO;
	Float32 sideSampleP;
	Float32 sideSampleQ;
	Float32 sideSampleR;
	Float32 sideSampleS;
	Float32 sideSampleT;
	Float32 sideSampleU;
	Float32 sideSampleV;
	Float32 sideSampleW;
	Float32 sideSampleX;
	Float32 sideSampleY;
	Float32 sideSampleZ;
	
	Float32 aMid[11];
	Float32 bMid[11];
	Float32 fMid[11];		
	Float32 aSide[11];
	Float32 bSide[11];
	Float32 fSide[11];		
	Float32 aMidPrev;
	Float32 aSidePrev;
	Float32 bMidPrev;
	Float32 bSidePrev;
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();	
	Float32 fusswithscale = 50000; //corrected
	Float32 cutofffreq = GetParameter( kParam_One );
	Float32 resonance = 0.992f;
	
	Float32 midAmount = cutofffreq/fusswithscale;
	midAmount /= overallscale;
	
	Float32 midAmountA = midAmount * resonance;
	Float32 midAmountB = midAmountA * resonance;
	Float32 midAmountC = midAmountB * resonance;
	Float32 midAmountD = midAmountC * resonance;
	Float32 midAmountE = midAmountD * resonance;
	Float32 midAmountF = midAmountE * resonance;
	Float32 midAmountG = midAmountF * resonance;
	Float32 midAmountH = midAmountG * resonance;
	Float32 midAmountI = midAmountH * resonance;
	Float32 midAmountJ = midAmountI * resonance;
	Float32 midAmountK = midAmountJ * resonance;
	Float32 midAmountL = midAmountK * resonance;
	Float32 midAmountM = midAmountL * resonance;
	Float32 midAmountN = midAmountM * resonance;
	Float32 midAmountO = midAmountN * resonance;
	Float32 midAmountP = midAmountO * resonance;
	Float32 midAmountQ = midAmountP * resonance;
	Float32 midAmountR = midAmountQ * resonance;
	Float32 midAmountS = midAmountR * resonance;
	Float32 midAmountT = midAmountS * resonance;
	Float32 midAmountU = midAmountT * resonance;
	Float32 midAmountV = midAmountU * resonance;
	Float32 midAmountW = midAmountV * resonance;
	Float32 midAmountX = midAmountW * resonance;
	Float32 midAmountY = midAmountX * resonance;
	Float32 midAmountZ = midAmountY * resonance;
	
	
	Float32 midaltAmountA = 1.0f - midAmountA;
	Float32 midaltAmountB = 1.0f - midAmountB;
	Float32 midaltAmountC = 1.0f - midAmountC;
	Float32 midaltAmountD = 1.0f - midAmountD;
	Float32 midaltAmountE = 1.0f - midAmountE;
	Float32 midaltAmountF = 1.0f - midAmountF;
	Float32 midaltAmountG = 1.0f - midAmountG;
	Float32 midaltAmountH = 1.0f - midAmountH;
	Float32 midaltAmountI = 1.0f - midAmountI;
	Float32 midaltAmountJ = 1.0f - midAmountJ;
	Float32 midaltAmountK = 1.0f - midAmountK;
	Float32 midaltAmountL = 1.0f - midAmountL;
	Float32 midaltAmountM = 1.0f - midAmountM;
	Float32 midaltAmountN = 1.0f - midAmountN;
	Float32 midaltAmountO = 1.0f - midAmountO;
	Float32 midaltAmountP = 1.0f - midAmountP;
	Float32 midaltAmountQ = 1.0f - midAmountQ;
	Float32 midaltAmountR = 1.0f - midAmountR;
	Float32 midaltAmountS = 1.0f - midAmountS;
	Float32 midaltAmountT = 1.0f - midAmountT;
	Float32 midaltAmountU = 1.0f - midAmountU;
	Float32 midaltAmountV = 1.0f - midAmountV;
	Float32 midaltAmountW = 1.0f - midAmountW;
	Float32 midaltAmountX = 1.0f - midAmountX;
	Float32 midaltAmountY = 1.0f - midAmountY;
	Float32 midaltAmountZ = 1.0f - midAmountZ;	

	cutofffreq = GetParameter( kParam_Two );
	Float32 sideAmount = cutofffreq/fusswithscale;
	sideAmount /= overallscale;
	Float32 sideAmountA = sideAmount * resonance;
	Float32 sideAmountB = sideAmountA * resonance;
	Float32 sideAmountC = sideAmountB * resonance;
	Float32 sideAmountD = sideAmountC * resonance;
	Float32 sideAmountE = sideAmountD * resonance;
	Float32 sideAmountF = sideAmountE * resonance;
	Float32 sideAmountG = sideAmountF * resonance;
	Float32 sideAmountH = sideAmountG * resonance;
	Float32 sideAmountI = sideAmountH * resonance;
	Float32 sideAmountJ = sideAmountI * resonance;
	Float32 sideAmountK = sideAmountJ * resonance;
	Float32 sideAmountL = sideAmountK * resonance;
	Float32 sideAmountM = sideAmountL * resonance;
	Float32 sideAmountN = sideAmountM * resonance;
	Float32 sideAmountO = sideAmountN * resonance;
	Float32 sideAmountP = sideAmountO * resonance;
	Float32 sideAmountQ = sideAmountP * resonance;
	Float32 sideAmountR = sideAmountQ * resonance;
	Float32 sideAmountS = sideAmountR * resonance;
	Float32 sideAmountT = sideAmountS * resonance;
	Float32 sideAmountU = sideAmountT * resonance;
	Float32 sideAmountV = sideAmountU * resonance;
	Float32 sideAmountW = sideAmountV * resonance;
	Float32 sideAmountX = sideAmountW * resonance;
	Float32 sideAmountY = sideAmountX * resonance;
	Float32 sideAmountZ = sideAmountY * resonance;
	
	
	Float32 sidealtAmountA = 1.0f - sideAmountA;
	Float32 sidealtAmountB = 1.0f - sideAmountB;
	Float32 sidealtAmountC = 1.0f - sideAmountC;
	Float32 sidealtAmountD = 1.0f - sideAmountD;
	Float32 sidealtAmountE = 1.0f - sideAmountE;
	Float32 sidealtAmountF = 1.0f - sideAmountF;
	Float32 sidealtAmountG = 1.0f - sideAmountG;
	Float32 sidealtAmountH = 1.0f - sideAmountH;
	Float32 sidealtAmountI = 1.0f - sideAmountI;
	Float32 sidealtAmountJ = 1.0f - sideAmountJ;
	Float32 sidealtAmountK = 1.0f - sideAmountK;
	Float32 sidealtAmountL = 1.0f - sideAmountL;
	Float32 sidealtAmountM = 1.0f - sideAmountM;
	Float32 sidealtAmountN = 1.0f - sideAmountN;
	Float32 sidealtAmountO = 1.0f - sideAmountO;
	Float32 sidealtAmountP = 1.0f - sideAmountP;
	Float32 sidealtAmountQ = 1.0f - sideAmountQ;
	Float32 sidealtAmountR = 1.0f - sideAmountR;
	Float32 sidealtAmountS = 1.0f - sideAmountS;
	Float32 sidealtAmountT = 1.0f - sideAmountT;
	Float32 sidealtAmountU = 1.0f - sideAmountU;
	Float32 sidealtAmountV = 1.0f - sideAmountV;
	Float32 sidealtAmountW = 1.0f - sideAmountW;
	Float32 sidealtAmountX = 1.0f - sideAmountX;
	Float32 sidealtAmountY = 1.0f - sideAmountY;
	Float32 sidealtAmountZ = 1.0f - sideAmountZ;
	Float32 tempMid;
	Float32 tempSide;
	
	Float32 intensity = powf(GetParameter( kParam_Three ),3)*(32/overallscale);
	Float32 inputSampleL;
	Float32 inputSampleR;
	Float32 senseL;
	Float32 senseR;
	Float32 smoothL;
	Float32 smoothR;
	Float32 mid;
	Float32 side;
	
	overallscale = (GetParameter( kParam_Four )*9.0f)+1.0f;
	Float32 gain = overallscale;
	//mid groove wear
	if (gain > 1.0f) {fMid[0] = 1.0f; gain -= 1.0f;} else {fMid[0] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fMid[1] = 1.0f; gain -= 1.0f;} else {fMid[1] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fMid[2] = 1.0f; gain -= 1.0f;} else {fMid[2] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fMid[3] = 1.0f; gain -= 1.0f;} else {fMid[3] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fMid[4] = 1.0f; gain -= 1.0f;} else {fMid[4] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fMid[5] = 1.0f; gain -= 1.0f;} else {fMid[5] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fMid[6] = 1.0f; gain -= 1.0f;} else {fMid[6] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fMid[7] = 1.0f; gain -= 1.0f;} else {fMid[7] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fMid[8] = 1.0f; gain -= 1.0f;} else {fMid[8] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fMid[9] = 1.0f; gain -= 1.0f;} else {fMid[9] = gain; gain = 0.0f;}
	//there, now we have a neat little moving average with remainders
	
	if (overallscale < 1.0f) overallscale = 1.0f;
	fMid[0] /= overallscale;
	fMid[1] /= overallscale;
	fMid[2] /= overallscale;
	fMid[3] /= overallscale;
	fMid[4] /= overallscale;
	fMid[5] /= overallscale;
	fMid[6] /= overallscale;
	fMid[7] /= overallscale;
	fMid[8] /= overallscale;
	fMid[9] /= overallscale;
	//and now it's neatly scaled, too

	overallscale = (GetParameter( kParam_Four )*4.5f)+1.0f;
	gain = overallscale;
	//side groove wear
	if (gain > 1.0f) {fSide[0] = 1.0f; gain -= 1.0f;} else {fSide[0] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fSide[1] = 1.0f; gain -= 1.0f;} else {fSide[1] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fSide[2] = 1.0f; gain -= 1.0f;} else {fSide[2] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fSide[3] = 1.0f; gain -= 1.0f;} else {fSide[3] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fSide[4] = 1.0f; gain -= 1.0f;} else {fSide[4] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fSide[5] = 1.0f; gain -= 1.0f;} else {fSide[5] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fSide[6] = 1.0f; gain -= 1.0f;} else {fSide[6] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fSide[7] = 1.0f; gain -= 1.0f;} else {fSide[7] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fSide[8] = 1.0f; gain -= 1.0f;} else {fSide[8] = gain; gain = 0.0f;}
	if (gain > 1.0f) {fSide[9] = 1.0f; gain -= 1.0f;} else {fSide[9] = gain; gain = 0.0f;}
	//there, now we have a neat little moving average with remainders
	
	if (overallscale < 1.0f) overallscale = 1.0f;
	fSide[0] /= overallscale;
	fSide[1] /= overallscale;
	fSide[2] /= overallscale;
	fSide[3] /= overallscale;
	fSide[4] /= overallscale;
	fSide[5] /= overallscale;
	fSide[6] /= overallscale;
	fSide[7] /= overallscale;
	fSide[8] /= overallscale;
	fSide[9] /= overallscale;
	//and now it's neatly scaled, too
	
	Float32 tempSample;
	Float32 accumulatorSample;
	Float32 midCorrection;
	Float32 sideCorrection;
	Float32 correction;
	
	while (nSampleFrames-- > 0) {
		inputSampleL = *inputL;
		inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		s3L = s2L;
		s2L = s1L;
		s1L = inputSampleL;
		smoothL = (s3L + s2L + s1L) / 3.0f;
		m1L = (s1L-s2L)*((s1L-s2L)/1.3f);
		m2L = (s2L-s3L)*((s1L-s2L)/1.3f);
		senseL = fabs(m1L-m2L);
		senseL = (intensity*intensity*senseL);
		o3L = o2L;
		o2L = o1L;
		o1L = senseL;
		if (o2L > senseL) senseL = o2L;
		if (o3L > senseL) senseL = o3L;
		//sense on the most intense
		
		s3R = s2R;
		s2R = s1R;
		s1R = inputSampleR;
		smoothR = (s3R + s2R + s1R) / 3.0f;
		m1R = (s1R-s2R)*((s1R-s2R)/1.3f);
		m2R = (s2R-s3R)*((s1R-s2R)/1.3f);
		senseR = fabs(m1R-m2R);
		senseR = (intensity*intensity*senseR);
		o3R = o2R;
		o2R = o1R;
		o1R = senseR;
		if (o2R > senseR) senseR = o2R;
		if (o3R > senseR) senseR = o3R;
		//sense on the most intense
		
		
		if (senseL > 1.0f) senseL = 1.0f;
		if (senseR > 1.0f) senseR = 1.0f;
		
		inputSampleL *= (1.0f-senseL);
		inputSampleR *= (1.0f-senseR);
		
		inputSampleL += (smoothL*senseL);
		inputSampleR += (smoothR*senseR);
		//we need to do the de-ess before anything else, and feed the result into the antialiasing-
		//but the trigger runs off just the input samples
		
		tempMid = mid = inputSampleL + inputSampleR;
		tempSide = side = inputSampleL - inputSampleR;
		//assign mid and side.
		
		tempSample = mid;
		midSampleA = (midSampleA * midaltAmountA) + (tempSample * midAmountA); tempSample -= midSampleA; 
		midSampleB = (midSampleB * midaltAmountB) + (tempSample * midAmountB); tempSample -= midSampleB; 
		midSampleC = (midSampleC * midaltAmountC) + (tempSample * midAmountC); tempSample -= midSampleC; 
		midSampleD = (midSampleD * midaltAmountD) + (tempSample * midAmountD); tempSample -= midSampleD; 
		midSampleE = (midSampleE * midaltAmountE) + (tempSample * midAmountE); tempSample -= midSampleE; 
		midSampleF = (midSampleF * midaltAmountF) + (tempSample * midAmountF); tempSample -= midSampleF; 
		midSampleG = (midSampleG * midaltAmountG) + (tempSample * midAmountG); tempSample -= midSampleG; 
		midSampleH = (midSampleH * midaltAmountH) + (tempSample * midAmountH); tempSample -= midSampleH; 
		midSampleI = (midSampleI * midaltAmountI) + (tempSample * midAmountI); tempSample -= midSampleI; 
		midSampleJ = (midSampleJ * midaltAmountJ) + (tempSample * midAmountJ); tempSample -= midSampleJ; 
		midSampleK = (midSampleK * midaltAmountK) + (tempSample * midAmountK); tempSample -= midSampleK; 
		midSampleL = (midSampleL * midaltAmountL) + (tempSample * midAmountL); tempSample -= midSampleL; 
		midSampleM = (midSampleM * midaltAmountM) + (tempSample * midAmountM); tempSample -= midSampleM; 
		midSampleN = (midSampleN * midaltAmountN) + (tempSample * midAmountN); tempSample -= midSampleN; 
		midSampleO = (midSampleO * midaltAmountO) + (tempSample * midAmountO); tempSample -= midSampleO; 
		midSampleP = (midSampleP * midaltAmountP) + (tempSample * midAmountP); tempSample -= midSampleP; 
		midSampleQ = (midSampleQ * midaltAmountQ) + (tempSample * midAmountQ); tempSample -= midSampleQ; 
		midSampleR = (midSampleR * midaltAmountR) + (tempSample * midAmountR); tempSample -= midSampleR; 
		midSampleS = (midSampleS * midaltAmountS) + (tempSample * midAmountS); tempSample -= midSampleS; 
		midSampleT = (midSampleT * midaltAmountT) + (tempSample * midAmountT); tempSample -= midSampleT; 
		midSampleU = (midSampleU * midaltAmountU) + (tempSample * midAmountU); tempSample -= midSampleU; 
		midSampleV = (midSampleV * midaltAmountV) + (tempSample * midAmountV); tempSample -= midSampleV; 
		midSampleW = (midSampleW * midaltAmountW) + (tempSample * midAmountW); tempSample -= midSampleW; 
		midSampleX = (midSampleX * midaltAmountX) + (tempSample * midAmountX); tempSample -= midSampleX; 
		midSampleY = (midSampleY * midaltAmountY) + (tempSample * midAmountY); tempSample -= midSampleY; 
		midSampleZ = (midSampleZ * midaltAmountZ) + (tempSample * midAmountZ); tempSample -= midSampleZ; 
		correction = midCorrection = mid - tempSample;
		mid -= correction;
		
		tempSample = side;
		sideSampleA = (sideSampleA * sidealtAmountA) + (tempSample * sideAmountA); tempSample -= sideSampleA; 
		sideSampleB = (sideSampleB * sidealtAmountB) + (tempSample * sideAmountB); tempSample -= sideSampleB; 
		sideSampleC = (sideSampleC * sidealtAmountC) + (tempSample * sideAmountC); tempSample -= sideSampleC; 
		sideSampleD = (sideSampleD * sidealtAmountD) + (tempSample * sideAmountD); tempSample -= sideSampleD; 
		sideSampleE = (sideSampleE * sidealtAmountE) + (tempSample * sideAmountE); tempSample -= sideSampleE; 
		sideSampleF = (sideSampleF * sidealtAmountF) + (tempSample * sideAmountF); tempSample -= sideSampleF; 
		sideSampleG = (sideSampleG * sidealtAmountG) + (tempSample * sideAmountG); tempSample -= sideSampleG; 
		sideSampleH = (sideSampleH * sidealtAmountH) + (tempSample * sideAmountH); tempSample -= sideSampleH; 
		sideSampleI = (sideSampleI * sidealtAmountI) + (tempSample * sideAmountI); tempSample -= sideSampleI; 
		sideSampleJ = (sideSampleJ * sidealtAmountJ) + (tempSample * sideAmountJ); tempSample -= sideSampleJ; 
		sideSampleK = (sideSampleK * sidealtAmountK) + (tempSample * sideAmountK); tempSample -= sideSampleK; 
		sideSampleL = (sideSampleL * sidealtAmountL) + (tempSample * sideAmountL); tempSample -= sideSampleL; 
		sideSampleM = (sideSampleM * sidealtAmountM) + (tempSample * sideAmountM); tempSample -= sideSampleM; 
		sideSampleN = (sideSampleN * sidealtAmountN) + (tempSample * sideAmountN); tempSample -= sideSampleN; 
		sideSampleO = (sideSampleO * sidealtAmountO) + (tempSample * sideAmountO); tempSample -= sideSampleO; 
		sideSampleP = (sideSampleP * sidealtAmountP) + (tempSample * sideAmountP); tempSample -= sideSampleP; 
		sideSampleQ = (sideSampleQ * sidealtAmountQ) + (tempSample * sideAmountQ); tempSample -= sideSampleQ; 
		sideSampleR = (sideSampleR * sidealtAmountR) + (tempSample * sideAmountR); tempSample -= sideSampleR; 
		sideSampleS = (sideSampleS * sidealtAmountS) + (tempSample * sideAmountS); tempSample -= sideSampleS; 
		sideSampleT = (sideSampleT * sidealtAmountT) + (tempSample * sideAmountT); tempSample -= sideSampleT; 
		sideSampleU = (sideSampleU * sidealtAmountU) + (tempSample * sideAmountU); tempSample -= sideSampleU; 
		sideSampleV = (sideSampleV * sidealtAmountV) + (tempSample * sideAmountV); tempSample -= sideSampleV; 
		sideSampleW = (sideSampleW * sidealtAmountW) + (tempSample * sideAmountW); tempSample -= sideSampleW; 
		sideSampleX = (sideSampleX * sidealtAmountX) + (tempSample * sideAmountX); tempSample -= sideSampleX; 
		sideSampleY = (sideSampleY * sidealtAmountY) + (tempSample * sideAmountY); tempSample -= sideSampleY; 
		sideSampleZ = (sideSampleZ * sidealtAmountZ) + (tempSample * sideAmountZ); tempSample -= sideSampleZ; 
		correction = sideCorrection = side - tempSample;
		side -= correction;
		
		
		aMid[9] = aMid[8]; aMid[8] = aMid[7]; aMid[7] = aMid[6]; aMid[6] = aMid[5];
		aMid[5] = aMid[4]; aMid[4] = aMid[3]; aMid[3] = aMid[2]; aMid[2] = aMid[1];
		aMid[1] = aMid[0]; aMid[0] = accumulatorSample = (mid-aMidPrev);
		
		accumulatorSample *= fMid[0];
		accumulatorSample += (aMid[1] * fMid[1]);
		accumulatorSample += (aMid[2] * fMid[2]);
		accumulatorSample += (aMid[3] * fMid[3]);
		accumulatorSample += (aMid[4] * fMid[4]);
		accumulatorSample += (aMid[5] * fMid[5]);
		accumulatorSample += (aMid[6] * fMid[6]);
		accumulatorSample += (aMid[7] * fMid[7]);
		accumulatorSample += (aMid[8] * fMid[8]);
		accumulatorSample += (aMid[9] * fMid[9]);
		//we are doing our repetitive calculations on a separate value
		correction = (mid-aMidPrev) - accumulatorSample;
		midCorrection += correction;
		aMidPrev = mid;		
		mid -= correction;
		
		aSide[9] = aSide[8]; aSide[8] = aSide[7]; aSide[7] = aSide[6]; aSide[6] = aSide[5];
		aSide[5] = aSide[4]; aSide[4] = aSide[3]; aSide[3] = aSide[2]; aSide[2] = aSide[1];
		aSide[1] = aSide[0]; aSide[0] = accumulatorSample = (side-aSidePrev);
		
		accumulatorSample *= fSide[0];
		accumulatorSample += (aSide[1] * fSide[1]);
		accumulatorSample += (aSide[2] * fSide[2]);
		accumulatorSample += (aSide[3] * fSide[3]);
		accumulatorSample += (aSide[4] * fSide[4]);
		accumulatorSample += (aSide[5] * fSide[5]);
		accumulatorSample += (aSide[6] * fSide[6]);
		accumulatorSample += (aSide[7] * fSide[7]);
		accumulatorSample += (aSide[8] * fSide[8]);
		accumulatorSample += (aSide[9] * fSide[9]);
		//we are doing our repetitive calculations on a separate value		
		correction = (side-aSidePrev) - accumulatorSample;
		sideCorrection += correction;
		aSidePrev = side;		
		side -= correction;
		
		
		bMid[9] = bMid[8]; bMid[8] = bMid[7]; bMid[7] = bMid[6]; bMid[6] = bMid[5];
		bMid[5] = bMid[4]; bMid[4] = bMid[3]; bMid[3] = bMid[2]; bMid[2] = bMid[1];
		bMid[1] = bMid[0]; bMid[0] = accumulatorSample = (mid-bMidPrev);
		
		accumulatorSample *= fMid[0];
		accumulatorSample += (bMid[1] * fMid[1]);
		accumulatorSample += (bMid[2] * fMid[2]);
		accumulatorSample += (bMid[3] * fMid[3]);
		accumulatorSample += (bMid[4] * fMid[4]);
		accumulatorSample += (bMid[5] * fMid[5]);
		accumulatorSample += (bMid[6] * fMid[6]);
		accumulatorSample += (bMid[7] * fMid[7]);
		accumulatorSample += (bMid[8] * fMid[8]);
		accumulatorSample += (bMid[9] * fMid[9]);
		//we are doing our repetitive calculations on a separate value
		correction = (mid-bMidPrev) - accumulatorSample;
		midCorrection += correction;
		bMidPrev = mid;		
		
		bSide[9] = bSide[8]; bSide[8] = bSide[7]; bSide[7] = bSide[6]; bSide[6] = bSide[5];
		bSide[5] = bSide[4]; bSide[4] = bSide[3]; bSide[3] = bSide[2]; bSide[2] = bSide[1];
		bSide[1] = bSide[0]; bSide[0] = accumulatorSample = (side-bSidePrev);
		
		accumulatorSample *= fSide[0];
		accumulatorSample += (bSide[1] * fSide[1]);
		accumulatorSample += (bSide[2] * fSide[2]);
		accumulatorSample += (bSide[3] * fSide[3]);
		accumulatorSample += (bSide[4] * fSide[4]);
		accumulatorSample += (bSide[5] * fSide[5]);
		accumulatorSample += (bSide[6] * fSide[6]);
		accumulatorSample += (bSide[7] * fSide[7]);
		accumulatorSample += (bSide[8] * fSide[8]);
		accumulatorSample += (bSide[9] * fSide[9]);
		//we are doing our repetitive calculations on a separate value		
		correction = (side-bSidePrev) - accumulatorSample;
		sideCorrection += correction;
		bSidePrev = side;		
		
		
		mid = tempMid - midCorrection;
		side = tempSide - sideCorrection;
		inputSampleL = (mid+side)/2.0f;
		inputSampleR = (mid-side)/2.0f;
				
		senseL /= 2.0f;
		senseR /= 2.0f;
		
		
		accumulatorSample = (ataLastOutL*senseL)+(inputSampleL*(1.0f-senseL));
		ataLastOutL = inputSampleL;
		inputSampleL = accumulatorSample;

		accumulatorSample = (ataLastOutR*senseR)+(inputSampleR*(1.0f-senseR));
		ataLastOutR = inputSampleR;
		inputSampleR = accumulatorSample;
		//we just re-use accumulatorSample to do this little shuffle
				
		
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		//direct stereo out
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{	
	for(int count = 0; count < 11; count++) {
		aMid[count] = 0.0;
		bMid[count] = 0.0;
		fMid[count] = 0.0;
		aSide[count] = 0.0;
		bSide[count] = 0.0;
		fSide[count] = 0.0;
	}
	aMidPrev = 0.0; aSidePrev = 0.0;
	bMidPrev = 0.0; bSidePrev = 0.0;
	ataLastOutL = ataLastOutR = 0.0;	
	midSampleA = 0.0;
	midSampleB = 0.0;
	midSampleC = 0.0;
	midSampleD = 0.0;
	midSampleE = 0.0;
	midSampleF = 0.0;
	midSampleG = 0.0;
	midSampleH = 0.0;
	midSampleI = 0.0;
	midSampleJ = 0.0;
	midSampleK = 0.0;
	midSampleL = 0.0;
	midSampleM = 0.0;
	midSampleN = 0.0;
	midSampleO = 0.0;
	midSampleP = 0.0;
	midSampleQ = 0.0;
	midSampleR = 0.0;
	midSampleS = 0.0;
	midSampleT = 0.0;
	midSampleU = 0.0;
	midSampleV = 0.0;
	midSampleW = 0.0;
	midSampleX = 0.0;
	midSampleY = 0.0;
	midSampleZ = 0.0;
	
	sideSampleA = 0.0;
	sideSampleB = 0.0;
	sideSampleC = 0.0;
	sideSampleD = 0.0;
	sideSampleE = 0.0;
	sideSampleF = 0.0;
	sideSampleG = 0.0;
	sideSampleH = 0.0;
	sideSampleI = 0.0;
	sideSampleJ = 0.0;
	sideSampleK = 0.0;
	sideSampleL = 0.0;
	sideSampleM = 0.0;
	sideSampleN = 0.0;
	sideSampleO = 0.0;
	sideSampleP = 0.0;
	sideSampleQ = 0.0;
	sideSampleR = 0.0;
	sideSampleS = 0.0;
	sideSampleT = 0.0;
	sideSampleU = 0.0;
	sideSampleV = 0.0;
	sideSampleW = 0.0;
	sideSampleX = 0.0;
	sideSampleY = 0.0;
	sideSampleZ = 0.0;
	s1L = s2L = s3L = 0.0;
	o1L = o2L = o3L = 0.0;
	m1L = m2L = desL = 0.0;
	s1R = s2R = s3R = 0.0;
	o1R = o2R = o3R = 0.0;
	m1R = m2R = desR = 0.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	
	return noErr;
	
}

};
