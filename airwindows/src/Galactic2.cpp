#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Galactic2"
#define AIRWINDOWS_DESCRIPTION "A super-reverb REdesigned specially for pads and space ambient."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','a','2' )
#define AIRWINDOWS_TAGS kNT_tagEffect | kNT_tagReverb
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
const int shortA = 683; const int shortB = 2339; const int shortC = 2381; const int shortD = 887; const int shortE = 743; const int shortF = 1823; const int shortG = 1151; const int shortH = 2833; const int shortI = 521; const int shortJ = 3331; const int shortK = 2851; const int shortL = 1747; const int shortM = 3389; const int shortN = 83; const int shortO = 443; const int shortP = 3221; //290 ms, 10004 seat stadium. Scarcity, 1 in 55796
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
{ .name = "Drive", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Sustain", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Darken", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Wetness", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	
	float feedbackA;
	float feedbackB;
	float feedbackC;
	float feedbackD;
	
	float iirA;
	float iirB;
	float iirC;
	float iirD;
	float iirE;
	float iirF;
	float iirG;
	float iirH;
	
	
	
	float feedbackAL;
	float feedbackBL;
	float feedbackCL;
	float feedbackDL;
	
	float feedbackDR;
	float feedbackHR;
	float feedbackLR;
	float feedbackPR;
	
	float previousAL;
	float previousBL;
	float previousCL;
	float previousDL;
	float previousEL;
	
	float lastRefL[7];
	
	float previousAR;
	float previousBR;
	float previousCR;
	float previousDR;
	float previousER;
	
	float lastRefR[7];
	
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
	
	int countZ;		
	
	int cycle;
	
	float prevMulchAL;
	float prevMulchAR;
	float prevMulchBL;
	float prevMulchBR;
	float prevMulchCL;
	float prevMulchCR;
	float prevMulchDL;
	float prevMulchDR;
	float prevMulchEL;
	float prevMulchER;
	
	float prevOutAL;
	float prevOutAR;
	float prevOutBL;
	float prevOutBR;
	float prevOutCL;
	float prevOutCR;
	float prevOutDL;
	float prevOutDR;
	float prevOutEL;
	float prevOutER;
	
	float finalOutAL;
	float finalOutAR;
	float finalOutBL;
	float finalOutBR;
	float finalOutCL;
	float finalOutCR;
	float finalOutDL;
	float finalOutDR;
	float finalOutEL;
	float finalOutER;
	
	float prevInAL;
	float prevInAR;
	float prevInBL;
	float prevInBR;
	float prevInCL;
	float prevInCR;
	float prevInDL;
	float prevInDR;
	float prevInEL;
	float prevInER;
	
	float inTrimA;
	float inTrimB;
	float wetA;
	float wetB;	
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float aAL[shortA+5];
	float aBL[shortB+5];
	float aCL[shortC+5];
	float aDL[shortD+5];
	float aEL[shortE+5];
	float aFL[shortF+5];
	float aGL[shortG+5];
	float aHL[shortH+5];
	float aIL[shortI+5];
	float aJL[shortJ+5];
	float aKL[shortK+5];
	float aLL[shortL+5];
	float aML[shortM+5];
	float aNL[shortN+5];
	float aOL[shortO+5];
	float aPL[shortP+5];
	float aAR[shortA+5];
	float aBR[shortB+5];
	float aCR[shortC+5];
	float aDR[shortD+5];
	float aER[shortE+5];
	float aFR[shortF+5];
	float aGR[shortG+5];
	float aHR[shortH+5];
	float aIR[shortI+5];
	float aJR[shortJ+5];
	float aKR[shortK+5];
	float aLR[shortL+5];
	float aMR[shortM+5];
	float aNR[shortN+5];
	float aOR[shortO+5];
	float aPR[shortP+5];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1f or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check
	
	inTrimA = inTrimB;
	inTrimB = powf(GetParameter( kParam_One ),4);
	float regen = 1.0f-(powf(1.0f-GetParameter( kParam_Two ),4)); regen *= 0.063f;
	float stages = GetParameter( kParam_Three );
	wetA = wetB;
	wetB = 1.0f-(powf(1.0f-GetParameter( kParam_Four ),4));
			
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		float inputGain = (inTrimA*temp)+(inTrimB*(1.0f-temp));
		float wet = (wetA*temp)+(wetB*(1.0f-temp));
		float outSample;

		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do a reverb sample
			if (inputGain < 1.0f) {
				inputSampleL *= inputGain;
				inputSampleR *= inputGain;
			}
			
			if (stages > 0.858f) {
				outSample = (inputSampleL + prevInAL)*0.5f;
				prevInAL = inputSampleL; inputSampleL = outSample;
				outSample = (inputSampleR + prevInAR)*0.5f;
				prevInAR = inputSampleR; inputSampleR = outSample;
			} else {prevInAL = inputSampleL; prevInAR = inputSampleR;}
			if (stages > 0.660f) {
				outSample = (inputSampleL + prevInBL)*0.5f;
				prevInBL = inputSampleL; inputSampleL = outSample;
				outSample = (inputSampleR + prevInBR)*0.5f;
				prevInBR = inputSampleR; inputSampleR = outSample;
			} else {prevInBL = inputSampleL; prevInBR = inputSampleR;}
			if (stages > 0.462f) {
				outSample = (inputSampleL + prevInCL)*0.5f;
				prevInCL = inputSampleL; inputSampleL = outSample;
				outSample = (inputSampleR + prevInCR)*0.5f;
				prevInCR = inputSampleR; inputSampleR = outSample;
			} else {prevInCL = inputSampleL; prevInCR = inputSampleR;}
			if (stages > 0.264f) {
				outSample = (inputSampleL + prevInDL)*0.5f;
				prevInDL = inputSampleL; inputSampleL = outSample;
				outSample = (inputSampleR + prevInDR)*0.5f;
				prevInDR = inputSampleR; inputSampleR = outSample;
			} else {prevInDL = inputSampleL; prevInDR = inputSampleR;}
			if (stages > 0.066f) {
				outSample = (inputSampleL + prevInEL)*0.5f;
				prevInEL = inputSampleL; inputSampleL = outSample;
				outSample = (inputSampleR + prevInER)*0.5f;
				prevInER = inputSampleR; inputSampleR = outSample;
			} else {prevInEL = inputSampleL; prevInER = inputSampleR;}
									
			feedbackCL *= 0.0625f; feedbackLR *= 0.0625f;
			if (feedbackA < 0.0078125f) feedbackA = 0.0078125f; if (feedbackA > 1.0f) feedbackA = 1.0f;
			if (feedbackB < 0.0078125f) feedbackB = 0.0078125f; if (feedbackB > 1.0f) feedbackB = 1.0f;
			feedbackCL *= feedbackA; feedbackLR *= feedbackB;
			feedbackA += sin((fabs(feedbackCL*4)>1)?4:fabs(feedbackCL*4))*powf(feedbackCL,4);
			feedbackB += sin((fabs(feedbackLR*4)>1)?4:fabs(feedbackLR*4))*powf(feedbackLR,4);
			feedbackCL *= 16.0f; feedbackLR *= 16.0f;
			
			feedbackDL *= 0.0625f; feedbackPR *= 0.0625f;
			if (feedbackC < 0.0078125f) feedbackC = 0.0078125f; if (feedbackC > 1.0f) feedbackC = 1.0f;
			if (feedbackD < 0.0078125f) feedbackD = 0.0078125f; if (feedbackD > 1.0f) feedbackD = 1.0f;
			feedbackDL *= feedbackC; feedbackPR *= feedbackD;
			feedbackC += sin((fabs(feedbackDL*4)>1)?4:fabs(feedbackDL*4))*powf(feedbackDL,4);
			feedbackD += sin((fabs(feedbackPR*4)>1)?4:fabs(feedbackPR*4))*powf(feedbackPR,4);
			feedbackDL *= 16.0f; feedbackPR *= 16.0f;
			
			float iirAmount = ((feedbackA-1.0f) * -0.00007f) + 0.00001f; //kick in highpass
			iirA = (iirA*(1.0f-iirAmount)) + (feedbackCL*iirAmount); feedbackCL -= iirA;
			iirE = (iirE*(1.0f-iirAmount)) + (feedbackAL*iirAmount); feedbackAL -= iirE;
			iirAmount = ((feedbackB-1.0f) * -0.00007f) + 0.00001f; //kick in highpass
			iirB = (iirB*(1.0f-iirAmount)) + (feedbackLR*iirAmount); feedbackLR -= iirB;
			iirF = (iirF*(1.0f-iirAmount)) + (feedbackDR*iirAmount); feedbackDR -= iirF;
			iirAmount = ((feedbackC-1.0f) * -0.00007f) + 0.00001f; //kick in highpass
			iirC = (iirC*(1.0f-iirAmount)) + (feedbackDL*iirAmount); feedbackDL -= iirC;
			iirG = (iirG*(1.0f-iirAmount)) + (feedbackBL*iirAmount); feedbackBL -= iirG;
			iirAmount = ((feedbackD-1.0f) * -0.00007f) + 0.00001f; //kick in highpass
			iirD = (iirD*(1.0f-iirAmount)) + (feedbackPR*iirAmount); feedbackPR -= iirD;
			iirH = (iirH*(1.0f-iirAmount)) + (feedbackHR*iirAmount); feedbackHR -= iirH;
						
			dram->aAL[countAL] = inputSampleL + (feedbackAL * regen);
			dram->aBL[countBL] = inputSampleL + (feedbackBL * regen);
			dram->aCL[countCL] = inputSampleL + (feedbackCL * regen);
			dram->aDL[countDL] = inputSampleL + (feedbackDL * regen);
			
			dram->aDR[countDR] = inputSampleR + (feedbackDR * regen);
			dram->aHR[countHR] = inputSampleR + (feedbackHR * regen);
			dram->aLR[countLR] = inputSampleR + (feedbackLR * regen);
			dram->aPR[countPR] = inputSampleR + (feedbackPR * regen);
			
			countAL++; if (countAL < 0 || countAL > shortA) countAL = 0;
			countBL++; if (countBL < 0 || countBL > shortB) countBL = 0;
			countCL++; if (countCL < 0 || countCL > shortC) countCL = 0;
			countDL++; if (countDL < 0 || countDL > shortD) countDL = 0;
			
			countDR++; if (countDR < 0 || countDR > shortD) countDR = 0;
			countHR++; if (countHR < 0 || countHR > shortH) countHR = 0;
			countLR++; if (countLR < 0 || countLR > shortL) countLR = 0;
			countPR++; if (countPR < 0 || countPR > shortP) countPR = 0;
			
			float outAL = dram->aAL[countAL-((countAL > shortA)?shortA+1:0)];
			float outBL = dram->aBL[countBL-((countBL > shortB)?shortB+1:0)];
			float outCL = dram->aCL[countCL-((countCL > shortC)?shortC+1:0)];
			float outDL = dram->aDL[countDL-((countDL > shortD)?shortD+1:0)];
			
			float outDR = dram->aDR[countDR-((countDR > shortD)?shortD+1:0)];
			float outHR = dram->aHR[countHR-((countHR > shortH)?shortH+1:0)];
			float outLR = dram->aLR[countLR-((countLR > shortL)?shortL+1:0)];
			float outPR = dram->aPR[countPR-((countPR > shortP)?shortP+1:0)];
						
			if (stages > 0.792f) {
				outSample = (outBL + prevMulchAL)*0.5f;
				prevMulchAL = outBL; outBL = outSample;
				outSample = (outHR + prevMulchAR)*0.5f;
				prevMulchAR = outHR; outHR = outSample;
			} else {prevMulchAL = outBL; prevMulchAR = outHR;}
			if (stages > 0.990f) {
				outSample = (outCL + prevMulchEL)*0.5f;
				prevMulchEL = outCL; outCL = outSample;
				outSample = (outLR + prevMulchER)*0.5f;
				prevMulchER = outLR; outLR = outSample;
			} else {prevMulchEL = outCL; prevMulchER = outLR;}
			
			dram->aEL[countEL] = outAL - (outBL + outCL + outDL);
			dram->aFL[countFL] = outBL - (outAL + outCL + outDL);
			dram->aGL[countGL] = outCL - (outAL + outBL + outDL);
			dram->aHL[countHL] = outDL - (outAL + outBL + outCL);
			
			dram->aCR[countCR] = outDR - (outHR + outLR + outPR);
			dram->aGR[countGR] = outHR - (outDR + outLR + outPR);
			dram->aKR[countKR] = outLR - (outDR + outHR + outPR);
			dram->aOR[countOR] = outPR - (outDR + outHR + outLR);
			
			countEL++; if (countEL < 0 || countEL > shortE) countEL = 0;
			countFL++; if (countFL < 0 || countFL > shortF) countFL = 0;
			countGL++; if (countGL < 0 || countGL > shortG) countGL = 0;
			countHL++; if (countHL < 0 || countHL > shortH) countHL = 0;
			
			countCR++; if (countCR < 0 || countCR > shortC) countCR = 0;
			countGR++; if (countGR < 0 || countGR > shortG) countGR = 0;
			countKR++; if (countKR < 0 || countKR > shortK) countKR = 0;
			countOR++; if (countOR < 0 || countOR > shortO) countOR = 0;
			
			float outEL = dram->aEL[countEL-((countEL > shortE)?shortE+1:0)];
			float outFL = dram->aFL[countFL-((countFL > shortF)?shortF+1:0)];
			float outGL = dram->aGL[countGL-((countGL > shortG)?shortG+1:0)];
			float outHL = dram->aHL[countHL-((countHL > shortH)?shortH+1:0)];
			
			float outCR = dram->aCR[countCR-((countCR > shortC)?shortC+1:0)];
			float outGR = dram->aGR[countGR-((countGR > shortG)?shortG+1:0)];
			float outKR = dram->aKR[countKR-((countKR > shortK)?shortK+1:0)];
			float outOR = dram->aOR[countOR-((countOR > shortO)?shortO+1:0)];
						
			if (stages > 0.594f) {
				outSample = (outFL + prevMulchBL)*0.5f;
				prevMulchBL = outFL; outFL = outSample;
				outSample = (outGR + prevMulchBR)*0.5f;
				prevMulchBR = outGR; outGR = outSample;
			} else {prevMulchBL = outFL; prevMulchBR = outGR;}
						
			dram->aIL[countIL] = outEL - (outFL + outGL + outHL);
			dram->aJL[countJL] = outFL - (outEL + outGL + outHL);
			dram->aKL[countKL] = outGL - (outEL + outFL + outHL);
			dram->aLL[countLL] = outHL - (outEL + outFL + outGL);
			
			dram->aBR[countBR] = outCR - (outGR + outKR + outOR);
			dram->aFR[countFR] = outGR - (outCR + outKR + outOR);
			dram->aJR[countJR] = outKR - (outCR + outGR + outOR);
			dram->aNR[countNR] = outOR - (outCR + outGR + outKR);
			
			countIL++; if (countIL < 0 || countIL > shortI) countIL = 0;
			countJL++; if (countJL < 0 || countJL > shortJ) countJL = 0;
			countKL++; if (countKL < 0 || countKL > shortK) countKL = 0;
			countLL++; if (countLL < 0 || countLL > shortL) countLL = 0;
			
			countBR++; if (countBR < 0 || countBR > shortB) countBR = 0;
			countFR++; if (countFR < 0 || countFR > shortF) countFR = 0;
			countJR++; if (countJR < 0 || countJR > shortJ) countJR = 0;
			countNR++; if (countNR < 0 || countNR > shortN) countNR = 0;
			
			float outIL = dram->aIL[countIL-((countIL > shortI)?shortI+1:0)];
			float outJL = dram->aJL[countJL-((countJL > shortJ)?shortJ+1:0)];
			float outKL = dram->aKL[countKL-((countKL > shortK)?shortK+1:0)];
			float outLL = dram->aLL[countLL-((countLL > shortL)?shortL+1:0)];
			
			float outBR = dram->aBR[countBR-((countBR > shortB)?shortB+1:0)];
			float outFR = dram->aFR[countFR-((countFR > shortF)?shortF+1:0)];
			float outJR = dram->aJR[countJR-((countJR > shortJ)?shortJ+1:0)];
			float outNR = dram->aNR[countNR-((countNR > shortN)?shortN+1:0)];
						
			if (stages > 0.396f) {
				outSample = (outJL + prevMulchCL)*0.5f;
				prevMulchCL = outJL; outJL = outSample;
				outSample = (outFR + prevMulchCR)*0.5f;
				prevMulchCR = outFR; outFR = outSample;
			} else {prevMulchCL = outJL; prevMulchCR = outFR;}
						
			dram->aML[countML] = outIL - (outJL + outKL + outLL);
			dram->aNL[countNL] = outJL - (outIL + outKL + outLL);
			dram->aOL[countOL] = outKL - (outIL + outJL + outLL);
			dram->aPL[countPL] = outLL - (outIL + outJL + outKL);
			
			dram->aAR[countAR] = outBR - (outFR + outJR + outNR);
			dram->aER[countER] = outFR - (outBR + outJR + outNR);
			dram->aIR[countIR] = outJR - (outBR + outFR + outNR);
			dram->aMR[countMR] = outNR - (outBR + outFR + outJR);
			
			countML++; if (countML < 0 || countML > shortM) countML = 0;
			countNL++; if (countNL < 0 || countNL > shortN) countNL = 0;
			countOL++; if (countOL < 0 || countOL > shortO) countOL = 0;
			countPL++; if (countPL < 0 || countPL > shortP) countPL = 0;
			
			countAR++; if (countAR < 0 || countAR > shortA) countAR = 0;
			countER++; if (countER < 0 || countER > shortE) countER = 0;
			countIR++; if (countIR < 0 || countIR > shortI) countIR = 0;
			countMR++; if (countMR < 0 || countMR > shortM) countMR = 0;
			
			float outML = dram->aML[countML-((countML > shortM)?shortM+1:0)];
			float outNL = dram->aNL[countNL-((countNL > shortN)?shortN+1:0)];
			float outOL = dram->aOL[countOL-((countOL > shortO)?shortO+1:0)];
			float outPL = dram->aPL[countPL-((countPL > shortP)?shortP+1:0)];
			
			float outAR = dram->aAR[countAR-((countAR > shortA)?shortA+1:0)];
			float outER = dram->aER[countER-((countER > shortE)?shortE+1:0)];
			float outIR = dram->aIR[countIR-((countIR > shortI)?shortI+1:0)];
			float outMR = dram->aMR[countMR-((countMR > shortM)?shortM+1:0)];
						
			if (stages > 0.198f) {
				outSample = (outNL + prevMulchDL)*0.5f;
				prevMulchDL = outNL; outNL = outSample;
				outSample = (outER + prevMulchDR)*0.5f;
				prevMulchDR = outER; outER = outSample;
			} else {prevMulchDL = outNL; prevMulchDR = outER;}
			
			feedbackDR = outML - (outNL + outOL + outPL);
			feedbackAL = outAR - (outER + outIR + outMR);
			outSample = (feedbackDR + feedbackAL) * 0.5f;
			feedbackDR = feedbackAL = outSample;
			feedbackBL = outNL - (outML + outOL + outPL);
			feedbackHR = outER - (outAR + outIR + outMR);
			feedbackCL = outOL - (outML + outNL + outPL);
			feedbackLR = outIR - (outAR + outER + outMR);
			feedbackDL = outPL - (outML + outNL + outOL);
			feedbackPR = outMR - (outAR + outER + outIR);
			//which we need to feed back into the input again, a bit
			
			inputSampleL = (outML + outNL + outOL + outPL)/8.0f;
			inputSampleR = (outAR + outER + outIR + outMR)/8.0f;
			//and take the final combined sum of outputs, corrected for Householder gain
						
			if (stages > 0.924f) {
				outSample = (inputSampleL + prevOutAL)*0.5f;
				prevOutAL = inputSampleL; inputSampleL = outSample;
				outSample = (inputSampleR + prevOutAR)*0.5f;
				prevOutAR = inputSampleR; inputSampleR = outSample;
			} else {prevOutAL = inputSampleL; prevOutAR = inputSampleR;}
			if (stages > 0.726f) {
				outSample = (inputSampleL + prevOutBL)*0.5f;
				prevOutBL = inputSampleL; inputSampleL = outSample;
				outSample = (inputSampleR + prevOutBR)*0.5f;
				prevOutBR = inputSampleR; inputSampleR = outSample;
			} else {prevOutBL = inputSampleL; prevOutBR = inputSampleR;}
			if (stages > 0.528f) {
				outSample = (inputSampleL + prevOutCL)*0.5f;
				prevOutCL = inputSampleL; inputSampleL = outSample;
				outSample = (inputSampleR + prevOutCR)*0.5f;
				prevOutCR = inputSampleR; inputSampleR = outSample;
			} else {prevOutCL = inputSampleL; prevOutCR = inputSampleR;}
			if (stages > 0.330f) {
				outSample = (inputSampleL + prevOutDL)*0.5f;
				prevOutDL = inputSampleL; inputSampleL = outSample;
				outSample = (inputSampleR + prevOutDR)*0.5f;
				prevOutDR = inputSampleR; inputSampleR = outSample;
			} else {prevOutDL = inputSampleL; prevOutDR = inputSampleR;}
			if (stages > 0.132f) {
				outSample = (inputSampleL + prevOutEL)*0.5f;
				prevOutEL = inputSampleL; inputSampleL = outSample;
				outSample = (inputSampleR + prevOutER)*0.5f;
				prevOutER = inputSampleR; inputSampleR = outSample;
			} else {prevOutEL = inputSampleL; prevOutER = inputSampleR;}
			
			if (cycleEnd == 4) {
				lastRefL[0] = lastRefL[4]; //start from previous last
				lastRefL[2] = (lastRefL[0] + inputSampleL)/2; //half
				lastRefL[1] = (lastRefL[0] + lastRefL[2])/2; //one quarter
				lastRefL[3] = (lastRefL[2] + inputSampleL)/2; //three quarters
				lastRefL[4] = inputSampleL; //full
				lastRefR[0] = lastRefR[4]; //start from previous last
				lastRefR[2] = (lastRefR[0] + inputSampleR)/2; //half
				lastRefR[1] = (lastRefR[0] + lastRefR[2])/2; //one quarter
				lastRefR[3] = (lastRefR[2] + inputSampleR)/2; //three quarters
				lastRefR[4] = inputSampleR; //full
			}
			if (cycleEnd == 3) {
				lastRefL[0] = lastRefL[3]; //start from previous last
				lastRefL[2] = (lastRefL[0]+lastRefL[0]+inputSampleL)/3; //third
				lastRefL[1] = (lastRefL[0]+inputSampleL+inputSampleL)/3; //two thirds
				lastRefL[3] = inputSampleL; //full
				lastRefR[0] = lastRefR[3]; //start from previous last
				lastRefR[2] = (lastRefR[0]+lastRefR[0]+inputSampleR)/3; //third
				lastRefR[1] = (lastRefR[0]+inputSampleR+inputSampleR)/3; //two thirds
				lastRefR[3] = inputSampleR; //full
			}
			if (cycleEnd == 2) {
				lastRefL[0] = lastRefL[2]; //start from previous last
				lastRefL[1] = (lastRefL[0] + inputSampleL)/2; //half
				lastRefL[2] = inputSampleL; //full
				lastRefR[0] = lastRefR[2]; //start from previous last
				lastRefR[1] = (lastRefR[0] + inputSampleR)/2; //half
				lastRefR[2] = inputSampleR; //full
			}
			if (cycleEnd == 1) {
				lastRefL[0] = inputSampleL;
				lastRefR[0] = inputSampleR;
			}
			cycle = 0; //reset
			inputSampleL = lastRefL[cycle];
			inputSampleR = lastRefR[cycle];
		} else {
			inputSampleL = lastRefL[cycle];
			inputSampleR = lastRefR[cycle];
			//we are going through our references now
		}
		
		inputSampleL *= 0.5f; inputSampleR *= 0.5f;
		if (inputSampleL > 2.0f) inputSampleL = 2.0f;
		if (inputSampleL < -2.0f) inputSampleL = -2.0f;
		if (inputSampleR > 2.0f) inputSampleR = 2.0f;
		if (inputSampleR < -2.0f) inputSampleR = -2.0f;//clip BigFastArcSin harder
		if (inputSampleL > 0.0f) inputSampleL = (inputSampleL*2.0f)/(2.8274333882308f-inputSampleL);
		else inputSampleL = -(inputSampleL*-2.0f)/(2.8274333882308f+inputSampleL);
		if (inputSampleR > 0.0f) inputSampleR = (inputSampleR*2.0f)/(2.8274333882308f-inputSampleR);
		else inputSampleR = -(inputSampleR*-2.0f)/(2.8274333882308f+inputSampleR);
		//BigFastArcSin output stage
		
		if (stages > 0.924f) {
			outSample = (inputSampleL + finalOutAL)*0.5f;
			finalOutAL = inputSampleL; inputSampleL = outSample;
			outSample = (inputSampleR + finalOutAR)*0.5f;
			finalOutAR = inputSampleR; inputSampleR = outSample;
		} else {finalOutAL = inputSampleL; finalOutAR = inputSampleR;}
		if (stages > 0.726f) {
			outSample = (inputSampleL + finalOutBL)*0.5f;
			finalOutBL = inputSampleL; inputSampleL = outSample;
			outSample = (inputSampleR + finalOutBR)*0.5f;
			finalOutBR = inputSampleR; inputSampleR = outSample;
		} else {finalOutBL = inputSampleL; finalOutBR = inputSampleR;}
		if (stages > 0.528f) {
			outSample = (inputSampleL + finalOutCL)*0.5f;
			finalOutCL = inputSampleL; inputSampleL = outSample;
			outSample = (inputSampleR + finalOutCR)*0.5f;
			finalOutCR = inputSampleR; inputSampleR = outSample;
		} else {finalOutCL = inputSampleL; finalOutCR = inputSampleR;}
		if (stages > 0.330f) {
			outSample = (inputSampleL + finalOutDL)*0.5f;
			finalOutDL = inputSampleL; inputSampleL = outSample;
			outSample = (inputSampleR + finalOutDR)*0.5f;
			finalOutDR = inputSampleR; inputSampleR = outSample;
		} else {finalOutDL = inputSampleL; finalOutDR = inputSampleR;}
		if (stages > 0.132f) {
			outSample = (inputSampleL + finalOutEL)*0.5f;
			finalOutEL = inputSampleL; inputSampleL = outSample;
			outSample = (inputSampleR + finalOutER)*0.5f;
			finalOutER = inputSampleR; inputSampleR = outSample;
		} else {finalOutEL = inputSampleL; finalOutER = inputSampleR;}
		
				
		inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f-wet));
		inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0f-wet));
		//Galactic2 does a proper crossfade so you can perform with it more actively
		if (inputSampleL > 2.0f) inputSampleL = 2.0f;
		if (inputSampleL < -2.0f) inputSampleL = -2.0f;
		if (inputSampleR > 2.0f) inputSampleR = 2.0f;
		if (inputSampleR < -2.0f) inputSampleR = -2.0f;//catch meltdowns
		
		
		
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
	for(int count = 0; count < shortA+2; count++) {dram->aAL[count] = 0.0; dram->aAR[count] = 0.0;}
	for(int count = 0; count < shortB+2; count++) {dram->aBL[count] = 0.0; dram->aBR[count] = 0.0;}
	for(int count = 0; count < shortC+2; count++) {dram->aCL[count] = 0.0; dram->aCR[count] = 0.0;}
	for(int count = 0; count < shortD+2; count++) {dram->aDL[count] = 0.0; dram->aDR[count] = 0.0;}
	for(int count = 0; count < shortE+2; count++) {dram->aEL[count] = 0.0; dram->aER[count] = 0.0;}
	for(int count = 0; count < shortF+2; count++) {dram->aFL[count] = 0.0; dram->aFR[count] = 0.0;}
	for(int count = 0; count < shortG+2; count++) {dram->aGL[count] = 0.0; dram->aGR[count] = 0.0;}
	for(int count = 0; count < shortH+2; count++) {dram->aHL[count] = 0.0; dram->aHR[count] = 0.0;}
	for(int count = 0; count < shortI+2; count++) {dram->aIL[count] = 0.0; dram->aIR[count] = 0.0;}
	for(int count = 0; count < shortJ+2; count++) {dram->aJL[count] = 0.0; dram->aJR[count] = 0.0;}
	for(int count = 0; count < shortK+2; count++) {dram->aKL[count] = 0.0; dram->aKR[count] = 0.0;}
	for(int count = 0; count < shortL+2; count++) {dram->aLL[count] = 0.0; dram->aLR[count] = 0.0;}
	for(int count = 0; count < shortM+2; count++) {dram->aML[count] = 0.0; dram->aMR[count] = 0.0;}
	for(int count = 0; count < shortN+2; count++) {dram->aNL[count] = 0.0; dram->aNR[count] = 0.0;}
	for(int count = 0; count < shortO+2; count++) {dram->aOL[count] = 0.0; dram->aOR[count] = 0.0;}
	for(int count = 0; count < shortP+2; count++) {dram->aPL[count] = 0.0; dram->aPR[count] = 0.0;}
		
	
	feedbackA = feedbackB = feedbackC = feedbackD = 1.0;
	
	iirA = iirB = iirC = iirD = iirE = iirF = iirG = iirH = 0.0;
	
	feedbackAL = 0.0;
	feedbackBL = 0.0;
	feedbackCL = 0.0;
	feedbackDL = 0.0;
	
	previousAL = 0.0;
	previousBL = 0.0;
	previousCL = 0.0;
	previousDL = 0.0;
	previousEL = 0.0;
	
	feedbackDR = 0.0;
	feedbackHR = 0.0;
	feedbackLR = 0.0;
	feedbackPR = 0.0;
	
	previousAR = 0.0;
	previousBR = 0.0;
	previousCR = 0.0;
	previousDR = 0.0;
	previousER = 0.0;
	
	prevMulchAL = 0.0;
	prevMulchAR = 0.0;
	prevMulchBL = 0.0;
	prevMulchBR = 0.0;
	prevMulchCL = 0.0;
	prevMulchCR = 0.0;
	prevMulchDL = 0.0;
	prevMulchDR = 0.0;
	prevMulchEL = 0.0;
	prevMulchER = 0.0;
	
	prevOutAL = 0.0;
	prevOutAR = 0.0;
	prevOutBL = 0.0;
	prevOutBR = 0.0;
	prevOutCL = 0.0;
	prevOutCR = 0.0;
	prevOutDL = 0.0;
	prevOutDR = 0.0;
	prevOutEL = 0.0;
	prevOutER = 0.0;
	
	finalOutAL = 0.0;
	finalOutAR = 0.0;
	finalOutBL = 0.0;
	finalOutBR = 0.0;
	finalOutCL = 0.0;
	finalOutCR = 0.0;
	finalOutDL = 0.0;
	finalOutDR = 0.0;
	finalOutEL = 0.0;
	finalOutER = 0.0;
	
	prevInAL = 0.0;
	prevInAR = 0.0;
	prevInBL = 0.0;
	prevInBR = 0.0;
	prevInCL = 0.0;
	prevInCR = 0.0;
	prevInDL = 0.0;
	prevInDR = 0.0;
	prevInEL = 0.0;
	prevInER = 0.0;
	
	for(int count = 0; count < 6; count++) {lastRefL[count] = 0.0; lastRefR[count] = 0.0;}
	
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
	
	countZ = 1;
	
	cycle = 0;
	
	inTrimA = 1.0; inTrimB = 1.0;
	wetA = 0.25; wetB = 0.25;
		
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
