#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "kCosmos"
#define AIRWINDOWS_DESCRIPTION "Infinite space ambient, or titanic hall."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','k','C','s' )
#define AIRWINDOWS_TAGS kNT_tagEffect | kNT_tagReverb
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
const int predelay = 15000;
const int delayA = 857; const int delayB = 1433; const int delayC = 1597; const int delayD = 1789; const int delayE = 1987; const int delayF = 373; const int delayG = 883; const int delayH = 1471; const int delayI = 1601; const int delayJ = 1973; const int delayK = 191; const int delayL = 397; const int delayM = 941; const int delayN = 1483; const int delayO = 1663; const int delayP = 149; const int delayQ = 227; const int delayR = 593; const int delayS = 1061; const int delayT = 1549; const int delayU = 137; const int delayV = 167; const int delayW = 313; const int delayX = 641; const int delayY = 1153; //38 to 188 ms, 1538 seat hall  
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Regen", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Derez", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Filter", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "EarlyRf", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Predlay", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"

	
	
	
	
	
	
	float feedbackAL;
	float feedbackBL;
	float feedbackCL;
	float feedbackDL;
	float feedbackEL;
	
	float feedbackER;
	float feedbackJR;
	float feedbackOR;
	float feedbackTR;
	float feedbackYR;
	
	int countAL;
	int countBL;
	int countCL;
	int countDL;
	int countEL;
	int countFL;
	int countGL;
	int countHL;
	int countIL;
	int countJL;
	int countKL;
	int countLL;		
	int countML;		
	int countNL;		
	int countOL;		
	int countPL;		
	int countQL;		
	int countRL;		
	int countSL;		
	int countTL;		
	int countUL;		
	int countVL;		
	int countWL;		
	int countXL;		
	int countYL;		
	
	int countAR;
	int countBR;
	int countCR;
	int countDR;
	int countER;
	int countFR;
	int countGR;
	int countHR;
	int countIR;
	int countJR;
	int countKR;
	int countLR;		
	int countMR;		
	int countNR;		
	int countOR;		
	int countPR;		
	int countQR;		
	int countRR;		
	int countSR;		
	int countTR;		
	int countUR;		
	int countVR;		
	int countWR;		
	int countXR;		
	int countYR;
	
	int countZ;
	
	enum {
		bez_AL,
		bez_AR,
		bez_BL,
		bez_BR,
		bez_CL,
		bez_CR,	
		bez_InL,
		bez_InR,
		bez_UnInL,
		bez_UnInR,
		bez_SampL,
		bez_SampR,
		bez_cycle,
		bez_total
	}; //the new undersampling. bez signifies the bezier curve reconstruction
	
	int firPosition;
	
	float earlyReflectionL;
	float earlyReflectionR;
	
	float prevAL;
	float prevBL;
	float prevCL;
	float prevDL;
	float prevEL;
	float prevER;
	float prevJR;
	float prevOR;
	float prevTR;
	float prevYR;
	
	float derezA, derezB;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float eAL[delayA+5];
	float eBL[delayB+5];
	float eCL[delayC+5];
	float eDL[delayD+5];
	float eEL[delayE+5];
	float eFL[delayF+5];
	float eGL[delayG+5];
	float eHL[delayH+5];
	float eIL[delayI+5];
	float eJL[delayJ+5];
	float eKL[delayK+5];
	float eLL[delayL+5];
	float eML[delayM+5];
	float eNL[delayN+5];
	float eOL[delayO+5];
	float ePL[delayP+5];
	float eQL[delayQ+5];
	float eRL[delayR+5];
	float eSL[delayS+5];
	float eTL[delayT+5];
	float eUL[delayU+5];
	float eVL[delayV+5];
	float eWL[delayW+5];
	float eXL[delayX+5];
	float eYL[delayY+5];
	float eAR[delayA+5];
	float eBR[delayB+5];
	float eCR[delayC+5];
	float eDR[delayD+5];
	float eER[delayE+5];
	float eFR[delayF+5];
	float eGR[delayG+5];
	float eHR[delayH+5];
	float eIR[delayI+5];
	float eJR[delayJ+5];
	float eKR[delayK+5];
	float eLR[delayL+5];
	float eMR[delayM+5];
	float eNR[delayN+5];
	float eOR[delayO+5];
	float ePR[delayP+5];
	float eQR[delayQ+5];
	float eRR[delayR+5];
	float eSR[delayS+5];
	float eTR[delayT+5];
	float eUR[delayU+5];
	float eVR[delayV+5];
	float eWR[delayW+5];
	float eXR[delayX+5];
	float eYR[delayY+5];
	float aAL[delayA+5];
	float aBL[delayB+5];
	float aCL[delayC+5];
	float aDL[delayD+5];
	float aEL[delayE+5];
	float aFL[delayF+5];
	float aGL[delayG+5];
	float aHL[delayH+5];
	float aIL[delayI+5];
	float aJL[delayJ+5];
	float aKL[delayK+5];
	float aLL[delayL+5];
	float aML[delayM+5];
	float aNL[delayN+5];
	float aOL[delayO+5];
	float aPL[delayP+5];
	float aQL[delayQ+5];
	float aRL[delayR+5];
	float aSL[delayS+5];
	float aTL[delayT+5];
	float aUL[delayU+5];
	float aVL[delayV+5];
	float aWL[delayW+5];
	float aXL[delayX+5];
	float aYL[delayY+5];
	float aAR[delayA+5];
	float aBR[delayB+5];
	float aCR[delayC+5];
	float aDR[delayD+5];
	float aER[delayE+5];
	float aFR[delayF+5];
	float aGR[delayG+5];
	float aHR[delayH+5];
	float aIR[delayI+5];
	float aJR[delayJ+5];
	float aKR[delayK+5];
	float aLR[delayL+5];
	float aMR[delayM+5];
	float aNR[delayN+5];
	float aOR[delayO+5];
	float aPR[delayP+5];
	float aQR[delayQ+5];
	float aRR[delayR+5];
	float aSR[delayS+5];
	float aTR[delayT+5];
	float aUR[delayU+5];
	float aVR[delayV+5];
	float aWR[delayW+5];
	float aXR[delayX+5];
	float aYR[delayY+5];
	float aZL[predelay+5];
	float aZR[predelay+5];
	float bez[bez_total];
	float firBufferL[32768];
	float firBufferR[32768];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();

	float regenMax = (1.0f-powf(1.0f-GetParameter( kParam_A ),3.0f))*0.000321f;
	//start this but pad it in the loop by volume of output?
	float feedbackSum = 0.0f; 
	bool applyCream = (GetParameter( kParam_B ) > 0.9999f);
	float remainder = (overallscale-1.0f)*0.0375f;
	derezA = derezB; derezB = GetParameter( kParam_B )/overallscale;
	if (applyCream) derezB = 1.0f / ((int)(1.0f/derezB));
	else derezB /= (2.0f/powf(overallscale,0.5f-remainder));
	//this hard-locks derez to exact subdivisions of 1.0f
	if (derezB < 0.0005f) derezB = 0.0005f; if (derezB > 1.0f) derezB = 1.0f;
	float freq = GetParameter( kParam_C )*M_PI_2; if (freq < 0.5f) freq = 0.5f;
	bool applyAvg = (GetParameter( kParam_C ) < 1.0f);
	float earlyLoudness = GetParameter( kParam_D );
	int adjPredelay = predelay*GetParameter( kParam_E )*derezB;	
	float wet = GetParameter( kParam_F );

	float fir[74]; fir[36] = 1.0f;
	for(int fip = 0; fip < 36; fip++) {
		fir[fip] = (fip-36)*freq;
		fir[fip] = sin(fir[fip])/fir[fip]; //sinc function
	}
	for(int fip = 37; fip < 72; fip++) {
		fir[fip] = (fip-36)*freq;
		fir[fip] = sin(fir[fip])/fir[fip]; //sinc function
	} //setting up the filter which will run inside DeRez
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		float derez = (derezA*temp)+(derezB*(1.0f-temp));
		
		dram->bez[bez_cycle] += derez;
		dram->bez[bez_SampL] += ((inputSampleL+dram->bez[bez_InL]) * derez);
		dram->bez[bez_SampR] += ((inputSampleR+dram->bez[bez_InR]) * derez);
		dram->bez[bez_InL] = inputSampleL; dram->bez[bez_InR] = inputSampleR;
		if (dram->bez[bez_cycle] > 1.0f) { //hit the end point and we do a reverb sample
			if (applyCream) dram->bez[bez_cycle] = 0.0f;
			else dram->bez[bez_cycle] -= 1.0f;
			
			//predelay
			dram->aZL[countZ] = dram->bez[bez_SampL];
			dram->aZR[countZ] = dram->bez[bez_SampR];
			countZ++; if (countZ < 0 || countZ > adjPredelay) countZ = 0;
			dram->bez[bez_SampL] = dram->aZL[countZ-((countZ > adjPredelay)?adjPredelay+1:0)];
			dram->bez[bez_SampR] = dram->aZR[countZ-((countZ > adjPredelay)?adjPredelay+1:0)];
			//end predelay

			dram->eAL[countAL] = dram->bez[bez_SampL];
			dram->eBL[countBL] = dram->bez[bez_SampL];
			dram->eCL[countCL] = dram->bez[bez_SampL];
			dram->eDL[countDL] = dram->bez[bez_SampL];
			dram->eEL[countEL] = dram->bez[bez_SampL];
			
			dram->eER[countER] = dram->bez[bez_SampR];
			dram->eJR[countJR] = dram->bez[bez_SampR];
			dram->eOR[countOR] = dram->bez[bez_SampR];
			dram->eTR[countTR] = dram->bez[bez_SampR];
			dram->eYR[countYR] = dram->bez[bez_SampR];
			
			
			
			if (firPosition < 0 || firPosition > 32767) firPosition = 32767; int firp = firPosition;		
			dram->firBufferL[firp] = earlyReflectionL; earlyReflectionL = 0.0f;
			dram->firBufferR[firp] = earlyReflectionR; earlyReflectionR = 0.0f;
			if (firp + 72 < 32767) {
				for(int fip = 1; fip < 72; fip++) {
					earlyReflectionL += dram->firBufferL[firp+fip] * fir[fip];
					earlyReflectionR += dram->firBufferR[firp+fip] * fir[fip];
				}
			} else {
				for(int fip = 1; fip < 72; fip++) {
					earlyReflectionL += dram->firBufferL[firp+fip - ((firp+fip > 32767)?32768:0)] * fir[fip];
					earlyReflectionR += dram->firBufferR[firp+fip - ((firp+fip > 32767)?32768:0)] * fir[fip];
				}
			}
			earlyReflectionL *= 0.25f; earlyReflectionR *= 0.25f;
			earlyReflectionL *= sqrt(freq); earlyReflectionR *= sqrt(freq);
			firPosition--;//here's the brickwall FIR filter, running in front of the Householder matrix
			
			feedbackSum *= 0.00001f;
			feedbackSum += fabs(earlyReflectionL);
			feedbackSum += fabs(earlyReflectionR);
			feedbackSum *= 0.00003f;
			
			float regen = fmax(regenMax - feedbackSum, 0.0f);
			if (applyAvg) regen *= 0.5f;
			else {
				prevAL = prevBL = prevCL = prevDL = prevEL = 0.0f;
				prevER = prevJR = prevOR = prevTR = prevYR = 0.0f;				
			}
			dram->aAL[countAL] = earlyReflectionL + ((feedbackAL+prevAL) * regen);
			dram->aBL[countBL] = earlyReflectionL + ((feedbackBL+prevBL) * regen);
			dram->aCL[countCL] = earlyReflectionL + ((feedbackCL+prevCL) * regen);
			dram->aDL[countDL] = earlyReflectionL + ((feedbackDL+prevDL) * regen);
			dram->aEL[countEL] = earlyReflectionL + ((feedbackEL+prevEL) * regen);
			
			dram->aER[countER] = earlyReflectionR + ((feedbackER+prevER) * regen);
			dram->aJR[countJR] = earlyReflectionR + ((feedbackJR+prevJR) * regen);
			dram->aOR[countOR] = earlyReflectionR + ((feedbackOR+prevOR) * regen);
			dram->aTR[countTR] = earlyReflectionR + ((feedbackTR+prevTR) * regen);
			dram->aYR[countYR] = earlyReflectionR + ((feedbackYR+prevYR) * regen);
			
			prevAL = feedbackAL;
			prevBL = feedbackBL;
			prevCL = feedbackCL;
			prevDL = feedbackDL;
			prevEL = feedbackEL;
			
			prevER = feedbackER;
			prevJR = feedbackJR;
			prevOR = feedbackOR;
			prevTR = feedbackTR;
			prevYR = feedbackYR;
			
			countAL++; if (countAL < 0 || countAL > delayA) countAL = 0;
			countBL++; if (countBL < 0 || countBL > delayB) countBL = 0;
			countCL++; if (countCL < 0 || countCL > delayC) countCL = 0;
			countDL++; if (countDL < 0 || countDL > delayD) countDL = 0;
			countEL++; if (countEL < 0 || countEL > delayE) countEL = 0;
			
			countER++; if (countER < 0 || countER > delayE) countER = 0;
			countJR++; if (countJR < 0 || countJR > delayJ) countJR = 0;
			countOR++; if (countOR < 0 || countOR > delayO) countOR = 0;
			countTR++; if (countTR < 0 || countTR > delayT) countTR = 0;
			countYR++; if (countYR < 0 || countYR > delayY) countYR = 0;
			
			float earlyAL = dram->eAL[countAL-((countAL > delayA)?delayA+1:0)];
			float earlyBL = dram->eBL[countBL-((countBL > delayB)?delayB+1:0)];
			float earlyCL = dram->eCL[countCL-((countCL > delayC)?delayC+1:0)];
			float earlyDL = dram->eDL[countDL-((countDL > delayD)?delayD+1:0)];
			float earlyEL = dram->eEL[countEL-((countEL > delayE)?delayE+1:0)];
			
			float earlyER = dram->eER[countER-((countER > delayE)?delayE+1:0)];
			float earlyJR = dram->eJR[countJR-((countJR > delayJ)?delayJ+1:0)];
			float earlyOR = dram->eOR[countOR-((countOR > delayO)?delayO+1:0)];
			float earlyTR = dram->eTR[countTR-((countTR > delayT)?delayT+1:0)];
			float earlyYR = dram->eYR[countYR-((countYR > delayY)?delayY+1:0)];
			
			float outAL = dram->aAL[countAL-((countAL > delayA)?delayA+1:0)];
			float outBL = dram->aBL[countBL-((countBL > delayB)?delayB+1:0)];
			float outCL = dram->aCL[countCL-((countCL > delayC)?delayC+1:0)];
			float outDL = dram->aDL[countDL-((countDL > delayD)?delayD+1:0)];
			float outEL = dram->aEL[countEL-((countEL > delayE)?delayE+1:0)];
			
			float outER = dram->aER[countER-((countER > delayE)?delayE+1:0)];
			float outJR = dram->aJR[countJR-((countJR > delayJ)?delayJ+1:0)];
			float outOR = dram->aOR[countOR-((countOR > delayO)?delayO+1:0)];
			float outTR = dram->aTR[countTR-((countTR > delayT)?delayT+1:0)];
			float outYR = dram->aYR[countYR-((countYR > delayY)?delayY+1:0)];
			
			//-------- one
			
			dram->eFL[countFL] = ((earlyAL*3.0f) - ((earlyBL + earlyCL + earlyDL + earlyEL)*2.0f));
			dram->eGL[countGL] = ((earlyBL*3.0f) - ((earlyAL + earlyCL + earlyDL + earlyEL)*2.0f));
			dram->eHL[countHL] = ((earlyCL*3.0f) - ((earlyAL + earlyBL + earlyDL + earlyEL)*2.0f));
			dram->eIL[countIL] = ((earlyDL*3.0f) - ((earlyAL + earlyBL + earlyCL + earlyEL)*2.0f));
			dram->eJL[countJL] = ((earlyEL*3.0f) - ((earlyAL + earlyBL + earlyCL + earlyDL)*2.0f));
			
			dram->eDR[countDR] = ((earlyER*3.0f) - ((earlyJR + earlyOR + earlyTR + earlyYR)*2.0f));
			dram->eIR[countIR] = ((earlyJR*3.0f) - ((earlyER + earlyOR + earlyTR + earlyYR)*2.0f));
			dram->eNR[countNR] = ((earlyOR*3.0f) - ((earlyER + earlyJR + earlyTR + earlyYR)*2.0f));
			dram->eSR[countSR] = ((earlyTR*3.0f) - ((earlyER + earlyJR + earlyOR + earlyYR)*2.0f));
			dram->eXR[countXR] = ((earlyYR*3.0f) - ((earlyER + earlyJR + earlyOR + earlyTR)*2.0f));
			
			dram->aFL[countFL] = ((outAL*3.0f) - ((outBL + outCL + outDL + outEL)*2.0f));
			dram->aGL[countGL] = ((outBL*3.0f) - ((outAL + outCL + outDL + outEL)*2.0f));
			dram->aHL[countHL] = ((outCL*3.0f) - ((outAL + outBL + outDL + outEL)*2.0f));
			dram->aIL[countIL] = ((outDL*3.0f) - ((outAL + outBL + outCL + outEL)*2.0f));
			dram->aJL[countJL] = ((outEL*3.0f) - ((outAL + outBL + outCL + outDL)*2.0f));
			
			dram->aDR[countDR] = ((outER*3.0f) - ((outJR + outOR + outTR + outYR)*2.0f));
			dram->aIR[countIR] = ((outJR*3.0f) - ((outER + outOR + outTR + outYR)*2.0f));
			dram->aNR[countNR] = ((outOR*3.0f) - ((outER + outJR + outTR + outYR)*2.0f));
			dram->aSR[countSR] = ((outTR*3.0f) - ((outER + outJR + outOR + outYR)*2.0f));
			dram->aXR[countXR] = ((outYR*3.0f) - ((outER + outJR + outOR + outTR)*2.0f));			
			
			countFL++; if (countFL < 0 || countFL > delayF) countFL = 0;
			countGL++; if (countGL < 0 || countGL > delayG) countGL = 0;
			countHL++; if (countHL < 0 || countHL > delayH) countHL = 0;
			countIL++; if (countIL < 0 || countIL > delayI) countIL = 0;
			countJL++; if (countJL < 0 || countJL > delayJ) countJL = 0;
			
			countDR++; if (countDR < 0 || countDR > delayD) countDR = 0;
			countIR++; if (countIR < 0 || countIR > delayI) countIR = 0;
			countNR++; if (countNR < 0 || countNR > delayN) countNR = 0;
			countSR++; if (countSR < 0 || countSR > delayS) countSR = 0;
			countXR++; if (countXR < 0 || countXR > delayX) countXR = 0;
			
			float earlyFL = dram->eFL[countFL-((countFL > delayF)?delayF+1:0)];
			float earlyGL = dram->eGL[countGL-((countGL > delayG)?delayG+1:0)];
			float earlyHL = dram->eHL[countHL-((countHL > delayH)?delayH+1:0)];
			float earlyIL = dram->eIL[countIL-((countIL > delayI)?delayI+1:0)];
			float earlyJL = dram->eJL[countJL-((countJL > delayJ)?delayJ+1:0)];
			
			float earlyDR = dram->eDR[countDR-((countDR > delayD)?delayD+1:0)];
			float earlyIR = dram->eIR[countIR-((countIR > delayI)?delayI+1:0)];
			float earlyNR = dram->eNR[countNR-((countNR > delayN)?delayN+1:0)];
			float earlySR = dram->eSR[countSR-((countSR > delayS)?delayS+1:0)];
			float earlyXR = dram->eXR[countXR-((countXR > delayX)?delayX+1:0)];

			float outFL = dram->aFL[countFL-((countFL > delayF)?delayF+1:0)];
			float outGL = dram->aGL[countGL-((countGL > delayG)?delayG+1:0)];
			float outHL = dram->aHL[countHL-((countHL > delayH)?delayH+1:0)];
			float outIL = dram->aIL[countIL-((countIL > delayI)?delayI+1:0)];
			float outJL = dram->aJL[countJL-((countJL > delayJ)?delayJ+1:0)];
			
			float outDR = dram->aDR[countDR-((countDR > delayD)?delayD+1:0)];
			float outIR = dram->aIR[countIR-((countIR > delayI)?delayI+1:0)];
			float outNR = dram->aNR[countNR-((countNR > delayN)?delayN+1:0)];
			float outSR = dram->aSR[countSR-((countSR > delayS)?delayS+1:0)];
			float outXR = dram->aXR[countXR-((countXR > delayX)?delayX+1:0)];
						
			//-------- two
			
			dram->eKL[countKL] = ((earlyFL*3.0f) - ((earlyGL + earlyHL + earlyIL + earlyJL)*2.0f));
			dram->eLL[countLL] = ((earlyGL*3.0f) - ((earlyFL + earlyHL + earlyIL + earlyJL)*2.0f));
			dram->eML[countML] = ((earlyHL*3.0f) - ((earlyFL + earlyGL + earlyIL + earlyJL)*2.0f));
			dram->eNL[countNL] = ((earlyIL*3.0f) - ((earlyFL + earlyGL + earlyHL + earlyJL)*2.0f));
			dram->eOL[countOL] = ((earlyJL*3.0f) - ((earlyFL + earlyGL + earlyHL + earlyIL)*2.0f));
			
			dram->eCR[countCR] = ((earlyDR*3.0f) - ((earlyIR + earlyNR + earlySR + earlyXR)*2.0f));
			dram->eHR[countHR] = ((earlyIR*3.0f) - ((earlyDR + earlyNR + earlySR + earlyXR)*2.0f));
			dram->eMR[countMR] = ((earlyNR*3.0f) - ((earlyDR + earlyIR + earlySR + earlyXR)*2.0f));
			dram->eRR[countRR] = ((earlySR*3.0f) - ((earlyDR + earlyIR + earlyNR + earlyXR)*2.0f));
			dram->eWR[countWR] = ((earlyXR*3.0f) - ((earlyDR + earlyIR + earlyNR + earlySR)*2.0f));
			
			dram->aKL[countKL] = ((outFL*3.0f) - ((outGL + outHL + outIL + outJL)*2.0f));
			dram->aLL[countLL] = ((outGL*3.0f) - ((outFL + outHL + outIL + outJL)*2.0f));
			dram->aML[countML] = ((outHL*3.0f) - ((outFL + outGL + outIL + outJL)*2.0f));
			dram->aNL[countNL] = ((outIL*3.0f) - ((outFL + outGL + outHL + outJL)*2.0f));
			dram->aOL[countOL] = ((outJL*3.0f) - ((outFL + outGL + outHL + outIL)*2.0f));
			
			dram->aCR[countCR] = ((outDR*3.0f) - ((outIR + outNR + outSR + outXR)*2.0f));
			dram->aHR[countHR] = ((outIR*3.0f) - ((outDR + outNR + outSR + outXR)*2.0f));
			dram->aMR[countMR] = ((outNR*3.0f) - ((outDR + outIR + outSR + outXR)*2.0f));
			dram->aRR[countRR] = ((outSR*3.0f) - ((outDR + outIR + outNR + outXR)*2.0f));
			dram->aWR[countWR] = ((outXR*3.0f) - ((outDR + outIR + outNR + outSR)*2.0f));
			
			countKL++; if (countKL < 0 || countKL > delayK) countKL = 0;
			countLL++; if (countLL < 0 || countLL > delayL) countLL = 0;
			countML++; if (countML < 0 || countML > delayM) countML = 0;
			countNL++; if (countNL < 0 || countNL > delayN) countNL = 0;
			countOL++; if (countOL < 0 || countOL > delayO) countOL = 0;
			
			countCR++; if (countCR < 0 || countCR > delayC) countCR = 0;
			countHR++; if (countHR < 0 || countHR > delayH) countHR = 0;
			countMR++; if (countMR < 0 || countMR > delayM) countMR = 0;
			countRR++; if (countRR < 0 || countRR > delayR) countRR = 0;
			countWR++; if (countWR < 0 || countWR > delayW) countWR = 0;
			
			float earlyKL = dram->eKL[countKL-((countKL > delayK)?delayK+1:0)];
			float earlyLL = dram->eLL[countLL-((countLL > delayL)?delayL+1:0)];
			float earlyML = dram->eML[countML-((countML > delayM)?delayM+1:0)];
			float earlyNL = dram->eNL[countNL-((countNL > delayN)?delayN+1:0)];
			float earlyOL = dram->eOL[countOL-((countOL > delayO)?delayO+1:0)];
			
			float earlyCR = dram->eCR[countCR-((countCR > delayC)?delayC+1:0)];
			float earlyHR = dram->eHR[countHR-((countHR > delayH)?delayH+1:0)];
			float earlyMR = dram->eMR[countMR-((countMR > delayM)?delayM+1:0)];
			float earlyRR = dram->eRR[countRR-((countRR > delayR)?delayR+1:0)];
			float earlyWR = dram->eWR[countWR-((countWR > delayW)?delayW+1:0)];
			
			float outKL = dram->aKL[countKL-((countKL > delayK)?delayK+1:0)];
			float outLL = dram->aLL[countLL-((countLL > delayL)?delayL+1:0)];
			float outML = dram->aML[countML-((countML > delayM)?delayM+1:0)];
			float outNL = dram->aNL[countNL-((countNL > delayN)?delayN+1:0)];
			float outOL = dram->aOL[countOL-((countOL > delayO)?delayO+1:0)];
			
			float outCR = dram->aCR[countCR-((countCR > delayC)?delayC+1:0)];
			float outHR = dram->aHR[countHR-((countHR > delayH)?delayH+1:0)];
			float outMR = dram->aMR[countMR-((countMR > delayM)?delayM+1:0)];
			float outRR = dram->aRR[countRR-((countRR > delayR)?delayR+1:0)];
			float outWR = dram->aWR[countWR-((countWR > delayW)?delayW+1:0)];
			
			//-------- three
			
			dram->ePL[countPL] = ((earlyKL*3.0f) - ((earlyLL + earlyML + earlyNL + earlyOL)*2.0f));
			dram->eQL[countQL] = ((earlyLL*3.0f) - ((earlyKL + earlyML + earlyNL + earlyOL)*2.0f));
			dram->eRL[countRL] = ((earlyML*3.0f) - ((earlyKL + earlyLL + earlyNL + earlyOL)*2.0f));
			dram->eSL[countSL] = ((earlyNL*3.0f) - ((earlyKL + earlyLL + earlyML + earlyOL)*2.0f));
			dram->eTL[countTL] = ((earlyOL*3.0f) - ((earlyKL + earlyLL + earlyML + earlyNL)*2.0f));
			
			dram->eBR[countBR] = ((earlyCR*3.0f) - ((earlyHR + earlyMR + earlyRR + earlyWR)*2.0f));
			dram->eGR[countGR] = ((earlyHR*3.0f) - ((earlyCR + earlyMR + earlyRR + earlyWR)*2.0f));
			dram->eLR[countLR] = ((earlyMR*3.0f) - ((earlyCR + earlyHR + earlyRR + earlyWR)*2.0f));
			dram->eQR[countQR] = ((earlyRR*3.0f) - ((earlyCR + earlyHR + earlyMR + earlyWR)*2.0f));
			dram->eVR[countVR] = ((earlyWR*3.0f) - ((earlyCR + earlyHR + earlyMR + earlyRR)*2.0f));
			
			dram->aPL[countPL] = ((outKL*3.0f) - ((outLL + outML + outNL + outOL)*2.0f));
			dram->aQL[countQL] = ((outLL*3.0f) - ((outKL + outML + outNL + outOL)*2.0f));
			dram->aRL[countRL] = ((outML*3.0f) - ((outKL + outLL + outNL + outOL)*2.0f));
			dram->aSL[countSL] = ((outNL*3.0f) - ((outKL + outLL + outML + outOL)*2.0f));
			dram->aTL[countTL] = ((outOL*3.0f) - ((outKL + outLL + outML + outNL)*2.0f));
			
			dram->aBR[countBR] = ((outCR*3.0f) - ((outHR + outMR + outRR + outWR)*2.0f));
			dram->aGR[countGR] = ((outHR*3.0f) - ((outCR + outMR + outRR + outWR)*2.0f));
			dram->aLR[countLR] = ((outMR*3.0f) - ((outCR + outHR + outRR + outWR)*2.0f));
			dram->aQR[countQR] = ((outRR*3.0f) - ((outCR + outHR + outMR + outWR)*2.0f));
			dram->aVR[countVR] = ((outWR*3.0f) - ((outCR + outHR + outMR + outRR)*2.0f));
			
			countPL++; if (countPL < 0 || countPL > delayP) countPL = 0;
			countQL++; if (countQL < 0 || countQL > delayQ) countQL = 0;
			countRL++; if (countRL < 0 || countRL > delayR) countRL = 0;
			countSL++; if (countSL < 0 || countSL > delayS) countSL = 0;
			countTL++; if (countTL < 0 || countTL > delayT) countTL = 0;
			
			countBR++; if (countBR < 0 || countBR > delayB) countBR = 0;
			countGR++; if (countGR < 0 || countGR > delayG) countGR = 0;
			countLR++; if (countLR < 0 || countLR > delayL) countLR = 0;
			countQR++; if (countQR < 0 || countQR > delayQ) countQR = 0;
			countVR++; if (countVR < 0 || countVR > delayV) countVR = 0;
			
			float earlyPL = dram->ePL[countPL-((countPL > delayP)?delayP+1:0)];
			float earlyQL = dram->eQL[countQL-((countQL > delayQ)?delayQ+1:0)];
			float earlyRL = dram->eRL[countRL-((countRL > delayR)?delayR+1:0)];
			float earlySL = dram->eSL[countSL-((countSL > delayS)?delayS+1:0)];
			float earlyTL = dram->eTL[countTL-((countTL > delayT)?delayT+1:0)];
			
			float earlyBR = dram->eBR[countBR-((countBR > delayB)?delayB+1:0)];
			float earlyGR = dram->eGR[countGR-((countGR > delayG)?delayG+1:0)];
			float earlyLR = dram->eLR[countLR-((countLR > delayL)?delayL+1:0)];
			float earlyQR = dram->eQR[countQR-((countQR > delayQ)?delayQ+1:0)];
			float earlyVR = dram->eVR[countVR-((countVR > delayV)?delayV+1:0)];
			
			float outPL = dram->aPL[countPL-((countPL > delayP)?delayP+1:0)];
			float outQL = dram->aQL[countQL-((countQL > delayQ)?delayQ+1:0)];
			float outRL = dram->aRL[countRL-((countRL > delayR)?delayR+1:0)];
			float outSL = dram->aSL[countSL-((countSL > delayS)?delayS+1:0)];
			float outTL = dram->aTL[countTL-((countTL > delayT)?delayT+1:0)];
			
			float outBR = dram->aBR[countBR-((countBR > delayB)?delayB+1:0)];
			float outGR = dram->aGR[countGR-((countGR > delayG)?delayG+1:0)];
			float outLR = dram->aLR[countLR-((countLR > delayL)?delayL+1:0)];
			float outQR = dram->aQR[countQR-((countQR > delayQ)?delayQ+1:0)];
			float outVR = dram->aVR[countVR-((countVR > delayV)?delayV+1:0)];
			
			//-------- four
			
			dram->eUL[countUL] = ((earlyPL*3.0f) - ((earlyQL + earlyRL + earlySL + earlyTL)*2.0f));
			dram->eVL[countVL] = ((earlyQL*3.0f) - ((earlyPL + earlyRL + earlySL + earlyTL)*2.0f));
			dram->eWL[countWL] = ((earlyRL*3.0f) - ((earlyPL + earlyQL + earlySL + earlyTL)*2.0f));
			dram->eXL[countXL] = ((earlySL*3.0f) - ((earlyPL + earlyQL + earlyRL + earlyTL)*2.0f));
			dram->eYL[countYL] = ((earlyTL*3.0f) - ((earlyPL + earlyQL + earlyRL + earlySL)*2.0f));
			
			dram->eAR[countAR] = ((earlyBR*3.0f) - ((earlyGR + earlyLR + earlyQR + earlyVR)*2.0f));
			dram->eFR[countFR] = ((earlyGR*3.0f) - ((earlyBR + earlyLR + earlyQR + earlyVR)*2.0f));
			dram->eKR[countKR] = ((earlyLR*3.0f) - ((earlyBR + earlyGR + earlyQR + earlyVR)*2.0f));
			dram->ePR[countPR] = ((earlyQR*3.0f) - ((earlyBR + earlyGR + earlyLR + earlyVR)*2.0f));
			dram->eUR[countUR] = ((earlyVR*3.0f) - ((earlyBR + earlyGR + earlyLR + earlyQR)*2.0f));
									
			dram->aUL[countUL] = ((outPL*3.0f) - ((outQL + outRL + outSL + outTL)*2.0f));
			dram->aVL[countVL] = ((outQL*3.0f) - ((outPL + outRL + outSL + outTL)*2.0f));
			dram->aWL[countWL] = ((outRL*3.0f) - ((outPL + outQL + outSL + outTL)*2.0f));
			dram->aXL[countXL] = ((outSL*3.0f) - ((outPL + outQL + outRL + outTL)*2.0f));
			dram->aYL[countYL] = ((outTL*3.0f) - ((outPL + outQL + outRL + outSL)*2.0f));
			
			dram->aAR[countAR] = ((outBR*3.0f) - ((outGR + outLR + outQR + outVR)*2.0f));
			dram->aFR[countFR] = ((outGR*3.0f) - ((outBR + outLR + outQR + outVR)*2.0f));
			dram->aKR[countKR] = ((outLR*3.0f) - ((outBR + outGR + outQR + outVR)*2.0f));
			dram->aPR[countPR] = ((outQR*3.0f) - ((outBR + outGR + outLR + outVR)*2.0f));
			dram->aUR[countUR] = ((outVR*3.0f) - ((outBR + outGR + outLR + outQR)*2.0f));
			
			countUL++; if (countUL < 0 || countUL > delayU) countUL = 0;
			countVL++; if (countVL < 0 || countVL > delayV) countVL = 0;
			countWL++; if (countWL < 0 || countWL > delayW) countWL = 0;
			countXL++; if (countXL < 0 || countXL > delayX) countXL = 0;
			countYL++; if (countYL < 0 || countYL > delayY) countYL = 0;
			
			countAR++; if (countAR < 0 || countAR > delayA) countAR = 0;
			countFR++; if (countFR < 0 || countFR > delayF) countFR = 0;
			countKR++; if (countKR < 0 || countKR > delayK) countKR = 0;
			countPR++; if (countPR < 0 || countPR > delayP) countPR = 0;
			countUR++; if (countUR < 0 || countUR > delayU) countUR = 0;
			
			float earlyUL = dram->eUL[countUL-((countUL > delayU)?delayU+1:0)];
			float earlyVL = dram->eVL[countVL-((countVL > delayV)?delayV+1:0)];
			float earlyWL = dram->eWL[countWL-((countWL > delayW)?delayW+1:0)];
			float earlyXL = dram->eXL[countXL-((countXL > delayX)?delayX+1:0)];
			float earlyYL = dram->eYL[countYL-((countYL > delayY)?delayY+1:0)];
			
			float earlyAR = dram->eAR[countAR-((countAR > delayA)?delayA+1:0)];
			float earlyFR = dram->eFR[countFR-((countFR > delayF)?delayF+1:0)];
			float earlyKR = dram->eKR[countKR-((countKR > delayK)?delayK+1:0)];
			float earlyPR = dram->ePR[countPR-((countPR > delayP)?delayP+1:0)];
			float earlyUR = dram->eUR[countUR-((countUR > delayU)?delayU+1:0)];
			
			float outUL = dram->aUL[countUL-((countUL > delayU)?delayU+1:0)];
			float outVL = dram->aVL[countVL-((countVL > delayV)?delayV+1:0)];
			float outWL = dram->aWL[countWL-((countWL > delayW)?delayW+1:0)];
			float outXL = dram->aXL[countXL-((countXL > delayX)?delayX+1:0)];
			float outYL = dram->aYL[countYL-((countYL > delayY)?delayY+1:0)];
			
			float outAR = dram->aAR[countAR-((countAR > delayA)?delayA+1:0)];
			float outFR = dram->aFR[countFR-((countFR > delayF)?delayF+1:0)];
			float outKR = dram->aKR[countKR-((countKR > delayK)?delayK+1:0)];
			float outPR = dram->aPR[countPR-((countPR > delayP)?delayP+1:0)];
			float outUR = dram->aUR[countUR-((countUR > delayU)?delayU+1:0)];
			
			//-------- five

			earlyReflectionL = (earlyUL + earlyVL + earlyWL + earlyXL + earlyYL)*0.0008f;
			earlyReflectionR = (earlyAR + earlyFR + earlyKR + earlyPR + earlyUR)*0.0008f;
			//and take the final combined sum of outputs, corrected for Householder gain
			
			feedbackAL = ((outAR*3.0f) - ((outFR + outKR + outPR + outUR)*2.0f)); feedbackSum = fabs(feedbackAL);
			feedbackER = ((outUL*3.0f) - ((outVL + outWL + outXL + outYL)*2.0f)); feedbackSum += fabs(feedbackER);
			feedbackBL = ((outVL*3.0f) - ((outUL + outWL + outXL + outYL)*2.0f)); feedbackSum += fabs(feedbackBL);
			feedbackJR = ((outFR*3.0f) - ((outAR + outKR + outPR + outUR)*2.0f)); feedbackSum += fabs(feedbackJR);
			feedbackCL = ((outWL*3.0f) - ((outUL + outVL + outXL + outYL)*2.0f)); feedbackSum += fabs(feedbackCL);
			feedbackOR = ((outKR*3.0f) - ((outAR + outFR + outPR + outUR)*2.0f)); feedbackSum += fabs(feedbackOR);
			feedbackDL = ((outXL*3.0f) - ((outUL + outVL + outWL + outYL)*2.0f)); feedbackSum += fabs(feedbackDL);
			feedbackTR = ((outPR*3.0f) - ((outAR + outFR + outKR + outUR)*2.0f)); feedbackSum += fabs(feedbackTR);
			feedbackEL = ((outYL*3.0f) - ((outUL + outVL + outWL + outXL)*2.0f)); feedbackSum += fabs(feedbackEL);
			feedbackYR = ((outUR*3.0f) - ((outAR + outFR + outKR + outPR)*2.0f)); feedbackSum += fabs(feedbackYR);
			
			inputSampleL = (outUL + outVL + outWL + outXL + outYL)*0.0008f;
			inputSampleR = (outAR + outFR + outKR + outPR + outUR)*0.0008f;
			//and take the final combined sum of outputs, corrected for Householder gain			
			
			inputSampleL += (earlyReflectionL * earlyLoudness);
			inputSampleR += (earlyReflectionR * earlyLoudness);
			
			dram->bez[bez_CL] = dram->bez[bez_BL];
			dram->bez[bez_BL] = dram->bez[bez_AL];
			dram->bez[bez_AL] = inputSampleL;
			dram->bez[bez_SampL] = 0.0f;
			
			dram->bez[bez_CR] = dram->bez[bez_BR];
			dram->bez[bez_BR] = dram->bez[bez_AR];
			dram->bez[bez_AR] = inputSampleR;
			dram->bez[bez_SampR] = 0.0f;
		}
		float CBL = (dram->bez[bez_CL]*(1.0f-dram->bez[bez_cycle]))+(dram->bez[bez_BL]*dram->bez[bez_cycle]);
		float CBR = (dram->bez[bez_CR]*(1.0f-dram->bez[bez_cycle]))+(dram->bez[bez_BR]*dram->bez[bez_cycle]);
		float BAL = (dram->bez[bez_BL]*(1.0f-dram->bez[bez_cycle]))+(dram->bez[bez_AL]*dram->bez[bez_cycle]);
		float BAR = (dram->bez[bez_BR]*(1.0f-dram->bez[bez_cycle]))+(dram->bez[bez_AR]*dram->bez[bez_cycle]);
		float CBAL = (dram->bez[bez_BL]+(CBL*(1.0f-dram->bez[bez_cycle]))+(BAL*dram->bez[bez_cycle]))*0.125f;
		float CBAR = (dram->bez[bez_BR]+(CBR*(1.0f-dram->bez[bez_cycle]))+(BAR*dram->bez[bez_cycle]))*0.125f;
		inputSampleL = CBAL;
		inputSampleR = CBAR;
		
		inputSampleL = (inputSampleL * wet)+(drySampleL * (1.0f-wet));
		inputSampleR = (inputSampleR * wet)+(drySampleR * (1.0f-wet));
		
		
		
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
	for(int count = 0; count < delayA+2; count++) {dram->eAL[count] = 0.0; dram->eAR[count] = 0.0; dram->aAL[count] = 0.0; dram->aAR[count] = 0.0;}
	for(int count = 0; count < delayB+2; count++) {dram->eBL[count] = 0.0; dram->eBR[count] = 0.0; dram->aBL[count] = 0.0; dram->aBR[count] = 0.0;}
	for(int count = 0; count < delayC+2; count++) {dram->eCL[count] = 0.0; dram->eCR[count] = 0.0; dram->aCL[count] = 0.0; dram->aCR[count] = 0.0;}
	for(int count = 0; count < delayD+2; count++) {dram->eDL[count] = 0.0; dram->eDR[count] = 0.0; dram->aDL[count] = 0.0; dram->aDR[count] = 0.0;}
	for(int count = 0; count < delayE+2; count++) {dram->eEL[count] = 0.0; dram->eER[count] = 0.0; dram->aEL[count] = 0.0; dram->aER[count] = 0.0;}
	for(int count = 0; count < delayF+2; count++) {dram->eFL[count] = 0.0; dram->eFR[count] = 0.0; dram->aFL[count] = 0.0; dram->aFR[count] = 0.0;}
	for(int count = 0; count < delayG+2; count++) {dram->eGL[count] = 0.0; dram->eGR[count] = 0.0; dram->aGL[count] = 0.0; dram->aGR[count] = 0.0;}
	for(int count = 0; count < delayH+2; count++) {dram->eHL[count] = 0.0; dram->eHR[count] = 0.0; dram->aHL[count] = 0.0; dram->aHR[count] = 0.0;}
	for(int count = 0; count < delayI+2; count++) {dram->eIL[count] = 0.0; dram->eIR[count] = 0.0; dram->aIL[count] = 0.0; dram->aIR[count] = 0.0;}
	for(int count = 0; count < delayJ+2; count++) {dram->eJL[count] = 0.0; dram->eJR[count] = 0.0; dram->aJL[count] = 0.0; dram->aJR[count] = 0.0;}
	for(int count = 0; count < delayK+2; count++) {dram->eKL[count] = 0.0; dram->eKR[count] = 0.0; dram->aKL[count] = 0.0; dram->aKR[count] = 0.0;}
	for(int count = 0; count < delayL+2; count++) {dram->eLL[count] = 0.0; dram->eLR[count] = 0.0; dram->aLL[count] = 0.0; dram->aLR[count] = 0.0;}
	for(int count = 0; count < delayM+2; count++) {dram->eML[count] = 0.0; dram->eMR[count] = 0.0; dram->aML[count] = 0.0; dram->aMR[count] = 0.0;}
	for(int count = 0; count < delayN+2; count++) {dram->eNL[count] = 0.0; dram->eNR[count] = 0.0; dram->aNL[count] = 0.0; dram->aNR[count] = 0.0;}
	for(int count = 0; count < delayO+2; count++) {dram->eOL[count] = 0.0; dram->eOR[count] = 0.0; dram->aOL[count] = 0.0; dram->aOR[count] = 0.0;}
	for(int count = 0; count < delayP+2; count++) {dram->ePL[count] = 0.0; dram->ePR[count] = 0.0; dram->aPL[count] = 0.0; dram->aPR[count] = 0.0;}
	for(int count = 0; count < delayQ+2; count++) {dram->eQL[count] = 0.0; dram->eQR[count] = 0.0; dram->aQL[count] = 0.0; dram->aQR[count] = 0.0;}
	for(int count = 0; count < delayR+2; count++) {dram->eRL[count] = 0.0; dram->eRR[count] = 0.0; dram->aRL[count] = 0.0; dram->aRR[count] = 0.0;}
	for(int count = 0; count < delayS+2; count++) {dram->eSL[count] = 0.0; dram->eSR[count] = 0.0; dram->aSL[count] = 0.0; dram->aSR[count] = 0.0;}
	for(int count = 0; count < delayT+2; count++) {dram->eTL[count] = 0.0; dram->eTR[count] = 0.0; dram->aTL[count] = 0.0; dram->aTR[count] = 0.0;}
	for(int count = 0; count < delayU+2; count++) {dram->eUL[count] = 0.0; dram->eUR[count] = 0.0; dram->aUL[count] = 0.0; dram->aUR[count] = 0.0;}
	for(int count = 0; count < delayV+2; count++) {dram->eVL[count] = 0.0; dram->eVR[count] = 0.0; dram->aVL[count] = 0.0; dram->aVR[count] = 0.0;}
	for(int count = 0; count < delayW+2; count++) {dram->eWL[count] = 0.0; dram->eWR[count] = 0.0; dram->aWL[count] = 0.0; dram->aWR[count] = 0.0;}
	for(int count = 0; count < delayX+2; count++) {dram->eXL[count] = 0.0; dram->eXR[count] = 0.0; dram->aXL[count] = 0.0; dram->aXR[count] = 0.0;}
	for(int count = 0; count < delayY+2; count++) {dram->eYL[count] = 0.0; dram->eYR[count] = 0.0; dram->aYL[count] = 0.0; dram->aYR[count] = 0.0;}
	for(int count = 0; count < predelay+2; count++) {dram->aZL[count] = 0.0; dram->aZR[count] = 0.0;}

	feedbackAL = 0.0;
	feedbackBL = 0.0;
	feedbackCL = 0.0;
	feedbackDL = 0.0;
	feedbackEL = 0.0;
	feedbackER = 0.0;
	feedbackJR = 0.0;
	feedbackOR = 0.0;
	feedbackTR = 0.0;
	feedbackYR = 0.0;
	
	countAL = 1;
	countBL = 1;
	countCL = 1;
	countDL = 1;	
	countEL = 1;
	countFL = 1;
	countGL = 1;
	countHL = 1;
	countIL = 1;
	countJL = 1;
	countKL = 1;
	countLL = 1;
	countML = 1;
	countNL = 1;
	countOL = 1;
	countPL = 1;
	countQL = 1;
	countRL = 1;
	countSL = 1;
	countTL = 1;
	countUL = 1;
	countVL = 1;
	countWL = 1;
	countXL = 1;
	countYL = 1;
	
	countAR = 1;
	countBR = 1;
	countCR = 1;
	countDR = 1;	
	countER = 1;
	countFR = 1;
	countGR = 1;
	countHR = 1;
	countIR = 1;
	countJR = 1;
	countKR = 1;
	countLR = 1;
	countMR = 1;
	countNR = 1;
	countOR = 1;
	countPR = 1;
	countQR = 1;
	countRR = 1;
	countSR = 1;
	countTR = 1;
	countUR = 1;
	countVR = 1;
	countWR = 1;
	countXR = 1;
	countYR = 1;
	
	countZ = 1;	
	
	for (int x = 0; x < bez_total; x++) dram->bez[x] = 0.0;
	dram->bez[bez_cycle] = 1.0;
	
	for(int count = 0; count < 32767; count++) {dram->firBufferL[count] = 0.0; dram->firBufferR[count] = 0.0;}
	firPosition = 0;
	
	earlyReflectionL = 0.0; earlyReflectionR = 0.0;
	prevAL = 0.0;
	prevBL = 0.0;
	prevCL = 0.0;
	prevDL = 0.0;
	prevEL = 0.0;
	prevER = 0.0;
	prevJR = 0.0;
	prevOR = 0.0;
	prevTR = 0.0;
	prevYR = 0.0;
	
	derezA = derezB = 0.0;
		
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
