#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "kGuitarHall"
#define AIRWINDOWS_DESCRIPTION "Accentuates loud midrange skronk!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','k','G','l' )
#define AIRWINDOWS_TAGS kNT_tagEffect | kNT_tagReverb
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
const int dscBuf = 90;
const int predelay = 15000; const int vlfpredelay = 11000;
const int delayA = 543; const int delayB = 402; const int delayC = 793; const int delayD = 223; const int delayE = 849; const int delayF = 886; const int delayG = 480; const int delayH = 658; const int delayI = 389; const int delayJ = 90; const int delayK = 745; const int delayL = 31; const int delayM = 62; const int delayN = 717; const int delayO = 874; const int delayP = 24; const int delayQ = 846; const int delayR = 270; const int delayS = 911; const int delayT = 641; const int delayU = 11; const int delayV = 800; const int delayW = 840; const int delayX = 267; const int delayY = 870; //8 to 99 ms, 347 seat club  
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Top dB", .min = 7000, .max = 14000, .def = 10000, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Regen", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DeRez", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Predelay", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Wetness", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	float gainOutL;
 	float gainOutR;
	
	
	
	
	
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
	int countVLF;		
	
	enum {
		prevSampL1,
		prevSlewL1,
		prevSampR1,
		prevSlewR1,
		pear_total
	}; //fixed frequency pear filter for ultrasonics, stereo
	
	
	float vibratoL;
	float vibratoR;
	float vibAL;
	float vibAR;
	float vibBL;
	float vibBR;
	
	float subAL;
	float subAR;
	float subBL;
	float subBR;
	float subCL;
	float subCR;
		
	float dBaPosL;
	int dBaXL;
	
	float dBaPosR;
	int dBaXR;
	
	float dBbPosL;
	int dBbXL;
	
	float dBbPosR;
	int dBbXR;
	
	float dBcPosL;
	int dBcXL;	
	
	float dBcPosR;
	int dBcXR;	
	
	float dBdPosL;
	int dBdXL;	
	
	float dBdPosR;
	int dBdXR;	
	
	float dBePosL;
	int dBeXL;	
	
	float dBePosR;
	int dBeXR;	
	
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
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
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
	float aVLFL[vlfpredelay+5];
	float aVLFR[vlfpredelay+5];
	float pearA[pear_total]; //probably worth just using a number here
	float pearB[pear_total]; //probably worth just using a number here
	float pearC[pear_total]; //probably worth just using a number here
	float pearD[pear_total]; //probably worth just using a number here
	float pearE[pear_total]; //probably worth just using a number here
	float pearF[pear_total]; //probably worth just using a number here
	float dBaL[dscBuf+5];
	float dBaR[dscBuf+5];
	float dBbL[dscBuf+5];
	float dBbR[dscBuf+5];
	float dBcL[dscBuf+5];
	float dBcR[dscBuf+5];
	float dBdL[dscBuf+5];
	float dBdR[dscBuf+5];
	float dBeL[dscBuf+5];
	float dBeR[dscBuf+5];
	float bez[bez_total];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	float refdB = GetParameter( kParam_A );
	float topdB = 0.000000075f * powf(10.0f,refdB/20.0f) * overallscale * 0.000025f;
	//the final 0.0001f scales it to the internal reverb path
	
	float regen = (1.0f-powf(1.0f-GetParameter( kParam_B ),3.0f))*0.00029f;
	float derez = GetParameter( kParam_C )/overallscale;
	if (derez < 0.0005f) derez = 0.0005f; if (derez > 1.0f) derez = 1.0f;
	derez = 1.0f / ((int)(1.0f/derez));
	//this hard-locks derez to exact subdivisions of 1.0f
	
	float pear = 0.22f/derez; if (pear > 1.0f) pear = 1.0f; //inside derez filter
	float pearScaled = 0.19f/overallscale; //outside derez filter
	
	int adjPredelay = predelay*GetParameter( kParam_D )*derez;	
	int adjSubDelay = vlfpredelay*derez;
	float wet = GetParameter( kParam_E )*2.0f;
	float dry = 2.0f - wet;
	if (wet > 1.0f) wet = 1.0f; else wet *= wet;
	if (wet < 0.0f) wet = 0.0f;
	if (dry > 1.0f) dry = 1.0f;
	if (dry < 0.0f) dry = 0.0f;
	//this reverb makes 50% full dry AND full wet, not crossfaded.
	//that's so it can be on submixes without cutting back dry channel when adjusted:
	//unless you go super heavy, you are only adjusting the added verb loudness.
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		dram->bez[bez_cycle] += derez;
		dram->bez[bez_SampL] += ((inputSampleL+dram->bez[bez_InL]) * derez);
		dram->bez[bez_SampR] += ((inputSampleR+dram->bez[bez_InR]) * derez);
		dram->bez[bez_InL] = inputSampleL; dram->bez[bez_InR] = inputSampleR;
		if (dram->bez[bez_cycle] > 1.0f) { //hit the end point and we do a reverb sample
			dram->bez[bez_cycle] = 0.0f;
			
			//predelay
			dram->aZL[countZ] = dram->bez[bez_SampL];
			dram->aZR[countZ] = dram->bez[bez_SampR];
			countZ++; if (countZ < 0 || countZ > adjPredelay) countZ = 0;
			float avgSampL = dram->aZL[countZ-((countZ > adjPredelay)?adjPredelay+1:0)];
			float avgSampR = dram->aZR[countZ-((countZ > adjPredelay)?adjPredelay+1:0)];
			//end predelay
			
			//begin SubTight section
			float outSampleL = avgSampL * 0.0019f;
			float outSampleR = avgSampR * 0.0019f;
			float scale = 0.5f+fabs(outSampleL*0.5f);
			outSampleL = (subAL+(sin(subAL-outSampleL)*scale));
			subAL = outSampleL*scale;
			scale = 0.5f+fabs(outSampleR*0.5f);
			outSampleR = (subAR+(sin(subAR-outSampleR)*scale));
			subAR = outSampleR*scale;
			scale = 0.5f+fabs(outSampleL*0.5f);
			outSampleL = (subBL+(sin(subBL-outSampleL)*scale));
			subBL = outSampleL*scale;
			scale = 0.5f+fabs(outSampleR*0.5f);
			outSampleR = (subBR+(sin(subBR-outSampleR)*scale));
			subBR = outSampleR*scale;
			scale = 0.5f+fabs(outSampleL*0.5f);
			outSampleL = (subCL+(sin(subCL-outSampleL)*scale));
			subCL = outSampleL*scale;
			scale = 0.5f+fabs(outSampleR*0.5f);
			outSampleR = (subCR+(sin(subCR-outSampleR)*scale));
			subCR = outSampleR*scale;
			outSampleL = -outSampleL; outSampleR = -outSampleR;
			if (outSampleL > 0.25f) outSampleL = 0.25f; if (outSampleL < -0.25f) outSampleL = -0.25f;
			if (outSampleR > 0.25f) outSampleR = 0.25f; if (outSampleR < -0.25f) outSampleR = -0.25f;
			outSampleL *= 16.0f;
			outSampleR *= 16.0f;
			avgSampL -= outSampleL;
			avgSampR -= outSampleR;
			//end SubTight section
			
			//VLF predelay
			dram->aVLFL[countVLF] = outSampleL;
			dram->aVLFR[countVLF] = outSampleR;
			countVLF++; if (countVLF < 0 || countVLF > adjSubDelay) countVLF = 0;
			outSampleL = dram->aVLFL[countVLF-((countVLF > adjSubDelay)?adjSubDelay+1:0)] * 2.0f;
			outSampleR = dram->aVLFR[countVLF-((countVLF > adjSubDelay)?adjSubDelay+1:0)] * 2.0f;
			//end VLF predelay
			
			avgSampL += outSampleL;
			avgSampR += outSampleR;
			//having re-added our VLF delayed channel we can now re-use outSample
			
			dram->aAL[countAL] = avgSampL + (feedbackAL * regen);
			dram->aBL[countBL] = avgSampL + (feedbackBL * regen);
			dram->aCL[countCL] = avgSampL + (feedbackCL * regen);
			dram->aDL[countDL] = avgSampL + (feedbackDL * regen);
			dram->aEL[countEL] = avgSampL + (feedbackEL * regen);
			
			dram->aER[countER] = avgSampR + (feedbackER * regen);
			dram->aJR[countJR] = avgSampR + (feedbackJR * regen);
			dram->aOR[countOR] = avgSampR + (feedbackOR * regen);
			dram->aTR[countTR] = avgSampR + (feedbackTR * regen);
			dram->aYR[countYR] = avgSampR + (feedbackYR * regen);
			
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
			
			dram->aVL[countVL] = ((outQL*3.0f) - ((outPL + outRL + outSL + outTL)*2.0f));
			dram->aWL[countWL] = ((outRL*3.0f) - ((outPL + outQL + outSL + outTL)*2.0f));
			dram->aXL[countXL] = ((outSL*3.0f) - ((outPL + outQL + outRL + outTL)*2.0f));
			dram->aYL[countYL] = ((outTL*3.0f) - ((outPL + outQL + outRL + outSL)*2.0f));
			
			dram->aAR[countAR] = ((outBR*3.0f) - ((outGR + outLR + outQR + outVR)*2.0f));
			dram->aFR[countFR] = ((outGR*3.0f) - ((outBR + outLR + outQR + outVR)*2.0f));
			dram->aKR[countKR] = ((outLR*3.0f) - ((outBR + outGR + outQR + outVR)*2.0f));
			dram->aPR[countPR] = ((outQR*3.0f) - ((outBR + outGR + outLR + outVR)*2.0f));
			
			float outUL = ((outPL*3.0f) - ((outQL + outRL + outSL + outTL)*2.0f)) - (dram->aUL[(countUL+1)-((countUL+1 > delayU)?delayU+1:0)]*0.618033988749894848204586f);
			float outUR = ((outVR*3.0f) - ((outBR + outGR + outLR + outQR)*2.0f)) - (dram->aUR[(countUR+1)-((countUR+1 > delayU)?delayU+1:0)]*0.618033988749894848204586f);
			dram->aUL[countUL] = outUL; outUL *= 0.618033988749894848204586f;
			dram->aUR[countUR] = outUR; outUR *= 0.618033988749894848204586f;
			countUL++; if (countUL < 0 || countUL > delayU) countUL = 0;
			countUR++; if (countUR < 0 || countUR > delayU) countUR = 0;
			outUL += dram->aUL[countUL-((countUL > delayU)?delayU+1:0)];
			outUR += dram->aUR[countUR-((countUR > delayU)?delayU+1:0)];
			//the 11-length delay slot becomes a sole allpass
			
			vibBL = vibAL; vibAL = outUL;
			vibBR = vibAR; vibAR = outUR; //tiny two sample delay chains
			vibratoL += fpdL * 0.5e-13f; if (vibratoL > M_PI*2.0f) vibratoL -= M_PI*2.0f;
			vibratoR += fpdR * 0.5e-13f; if (vibratoR > M_PI*2.0f) vibratoR -= M_PI*2.0f;
			float quadL = sin(vibratoL)+1.0f;
			float quadR = sin(vibratoR)+1.0f;
			//quadrature delay points play back from a position in delay chains
			if (quadL < 1.0f) outUL = (outUL*(1.0f-quadL))+(vibAL*quadL);
			else outUL = (vibAL*(1.0f-(quadL-1.0f)))+(vibBL*(quadL-1.0f));
			if (quadR < 1.0f) outUR = (outUR*(1.0f-quadR))+(vibAR*quadR);
			else outUR = (vibAR*(1.0f-(quadR-1.0f)))+(vibBR*(quadR-1.0f));
			//also, pitch drift this allpass slot for very subtle motion
			
			countVL++; if (countVL < 0 || countVL > delayV) countVL = 0;
			countWL++; if (countWL < 0 || countWL > delayW) countWL = 0;
			countXL++; if (countXL < 0 || countXL > delayX) countXL = 0;
			countYL++; if (countYL < 0 || countYL > delayY) countYL = 0;
			
			countAR++; if (countAR < 0 || countAR > delayA) countAR = 0;
			countFR++; if (countFR < 0 || countFR > delayF) countFR = 0;
			countKR++; if (countKR < 0 || countKR > delayK) countKR = 0;
			countPR++; if (countPR < 0 || countPR > delayP) countPR = 0;
			
			float outVL = dram->aVL[countVL-((countVL > delayV)?delayV+1:0)];
			float outWL = dram->aWL[countWL-((countWL > delayW)?delayW+1:0)];
			float outXL = dram->aXL[countXL-((countXL > delayX)?delayX+1:0)];
			float outYL = dram->aYL[countYL-((countYL > delayY)?delayY+1:0)];
			
			float outAR = dram->aAR[countAR-((countAR > delayA)?delayA+1:0)];
			float outFR = dram->aFR[countFR-((countFR > delayF)?delayF+1:0)];
			float outKR = dram->aKR[countKR-((countKR > delayK)?delayK+1:0)];
			float outPR = dram->aPR[countPR-((countPR > delayP)?delayP+1:0)];
			
			//-------- five
			
			feedbackAL = ((outAR*3.0f) - ((outFR + outKR + outPR + outUR)*2.0f));
			feedbackER = ((outUL*3.0f) - ((outVL + outWL + outXL + outYL)*2.0f));
			float slew = ((feedbackAL - dram->pearA[0]) + dram->pearA[1])*pear*0.5f;
			dram->pearA[0] = feedbackAL = (pear * feedbackAL) + ((1.0f-pear) * (dram->pearA[0] + dram->pearA[1]));
			dram->pearA[1] = slew;
			slew = ((feedbackER - dram->pearA[2]) + dram->pearA[3])*pear*0.5f;
			dram->pearA[2] = feedbackER = (pear * feedbackER) + ((1.0f-pear) * (dram->pearA[2] + dram->pearA[3]));
			dram->pearA[3] = slew;
			//Air Discontinuity A begin
			feedbackAL *= topdB;
			if (feedbackAL < -0.222f) feedbackAL = -0.222f; if (feedbackAL > 0.222f) feedbackAL = 0.222f;
			dram->dBaL[dBaXL] = feedbackAL; dBaPosL *= 0.5f; dBaPosL += fabs((feedbackAL*((feedbackAL*0.25f)-0.5f))*0.5f);
			int dBdly = floor(dBaPosL*dscBuf);
			float dBi = (dBaPosL*dscBuf)-dBdly;
			feedbackAL = dram->dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
			dBdly++; feedbackAL += dram->dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*dBi;
			dBaXL++; if (dBaXL < 0 || dBaXL >= dscBuf) dBaXL = 0;
			feedbackAL /= topdB;
			feedbackER *= topdB;
			if (feedbackER < -0.222f) feedbackER = -0.222f; if (feedbackER > 0.222f) feedbackER = 0.222f;
			dram->dBaR[dBaXR] = feedbackER; dBaPosR *= 0.5f; dBaPosR += fabs((feedbackER*((feedbackER*0.25f)-0.5f))*0.5f);
			dBdly = floor(dBaPosR*dscBuf);
			dBi = (dBaPosR*dscBuf)-dBdly;
			feedbackER = dram->dBaR[dBaXR-dBdly +((dBaXR-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
			dBdly++; feedbackER += dram->dBaR[dBaXR-dBdly +((dBaXR-dBdly < 0)?dscBuf:0)]*dBi;
			dBaXR++; if (dBaXR < 0 || dBaXR >= dscBuf) dBaXR = 0;
			feedbackER /= topdB;
			//Air Discontinuity A end			
			
			feedbackBL = ((outVL*3.0f) - ((outUL + outWL + outXL + outYL)*2.0f));
			feedbackJR = ((outFR*3.0f) - ((outAR + outKR + outPR + outUR)*2.0f));
			slew = ((feedbackBL - dram->pearB[0]) + dram->pearB[1])*pear*0.5f;
			dram->pearB[0] = feedbackBL = (pear * feedbackBL) + ((1.0f-pear) * (dram->pearB[0] + dram->pearB[1]));
			dram->pearB[1] = slew;
			slew = ((feedbackJR - dram->pearB[2]) + dram->pearB[3])*pear*0.5f;
			dram->pearB[2] = feedbackJR = (pear * feedbackJR) + ((1.0f-pear) * (dram->pearB[2] + dram->pearB[3]));
			dram->pearB[3] = slew;
			//Air Discontinuity B begin
			feedbackBL *= topdB;
			if (feedbackBL < -0.222f) feedbackBL = -0.222f; if (feedbackBL > 0.222f) feedbackBL = 0.222f;
			dram->dBbL[dBbXL] = feedbackBL;  dBbPosL *= 0.5f; dBbPosL += fabs((feedbackBL*((feedbackBL*0.25f)-0.5f))*0.5f);
			dBdly = floor(dBbPosL*dscBuf); dBi = (dBbPosL*dscBuf)-dBdly;
			feedbackBL = dram->dBbL[dBbXL-dBdly +((dBbXL-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
			dBdly++; feedbackBL += dram->dBbL[dBbXL-dBdly +((dBbXL-dBdly < 0)?dscBuf:0)]*dBi;
			dBbXL++; if (dBbXL < 0 || dBbXL >= dscBuf) dBbXL = 0;
			feedbackBL /= topdB;
			feedbackJR *= topdB;
			if (feedbackJR < -0.222f) feedbackJR = -0.222f; if (feedbackJR > 0.222f) feedbackJR = 0.222f;
			dram->dBbR[dBbXR] = feedbackJR;  dBbPosR *= 0.5f; dBbPosR += fabs((feedbackJR*((feedbackJR*0.25f)-0.5f))*0.5f);
			dBdly = floor(dBbPosR*dscBuf); dBi = (dBbPosR*dscBuf)-dBdly;
			feedbackJR = dram->dBbR[dBbXR-dBdly +((dBbXR-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
			dBdly++; feedbackJR += dram->dBbR[dBbXR-dBdly +((dBbXR-dBdly < 0)?dscBuf:0)]*dBi;
			dBbXR++; if (dBbXR < 0 || dBbXR >= dscBuf) dBbXR = 0;
			feedbackJR /= topdB;
			//Air Discontinuity B end			
			
			feedbackCL = ((outWL*3.0f) - ((outUL + outVL + outXL + outYL)*2.0f));
			feedbackOR = ((outKR*3.0f) - ((outAR + outFR + outPR + outUR)*2.0f));
			slew = ((feedbackCL - dram->pearC[0]) + dram->pearC[1])*pear*0.5f;
			dram->pearC[0] = feedbackCL = (pear * feedbackCL) + ((1.0f-pear) * (dram->pearC[0] + dram->pearC[1]));
			dram->pearC[1] = slew;
			slew = ((feedbackOR - dram->pearC[2]) + dram->pearC[3])*pear*0.5f;
			dram->pearC[2] = feedbackOR = (pear * feedbackOR) + ((1.0f-pear) * (dram->pearC[2] + dram->pearC[3]));
			dram->pearC[3] = slew;
			//Air Discontinuity C begin
			feedbackCL *= topdB;
			if (feedbackCL < -0.222f) feedbackCL = -0.222f; if (feedbackCL > 0.222f) feedbackCL = 0.222f;
			dram->dBcL[dBcXL] = feedbackCL;  dBcPosL *= 0.5f; dBcPosL += fabs((feedbackCL*((feedbackCL*0.25f)-0.5f))*0.5f);
			dBdly = floor(dBcPosL*dscBuf); dBi = (dBcPosL*dscBuf)-dBdly;
			feedbackCL = dram->dBcL[dBcXL-dBdly +((dBcXL-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
			dBdly++; feedbackCL += dram->dBcL[dBcXL-dBdly +((dBcXL-dBdly < 0)?dscBuf:0)]*dBi;
			dBcXL++; if (dBcXL < 0 || dBcXL >= dscBuf) dBcXL = 0;
			feedbackCL /= topdB;
			feedbackOR *= topdB;
			if (feedbackOR < -0.222f) feedbackOR = -0.222f; if (feedbackOR > 0.222f) feedbackOR = 0.222f;
			dram->dBcR[dBcXR] = feedbackOR;  dBcPosR *= 0.5f; dBcPosR += fabs((feedbackOR*((feedbackOR*0.25f)-0.5f))*0.5f);
			dBdly = floor(dBcPosR*dscBuf); dBi = (dBcPosR*dscBuf)-dBdly;
			feedbackOR = dram->dBcR[dBcXR-dBdly +((dBcXR-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
			dBdly++; feedbackOR += dram->dBcR[dBcXR-dBdly +((dBcXR-dBdly < 0)?dscBuf:0)]*dBi;
			dBcXR++; if (dBcXR < 0 || dBcXR >= dscBuf) dBcXR = 0;
			feedbackOR /= topdB;
			//Air Discontinuity C end			
			
			feedbackDL = ((outXL*3.0f) - ((outUL + outVL + outWL + outYL)*2.0f));
			feedbackTR = ((outPR*3.0f) - ((outAR + outFR + outKR + outUR)*2.0f));
			slew = ((feedbackDL - dram->pearD[0]) + dram->pearD[1])*pear*0.5f;
			dram->pearD[0] = feedbackDL = (pear * feedbackDL) + ((1.0f-pear) * (dram->pearD[0] + dram->pearD[1]));
			dram->pearD[1] = slew;
			slew = ((feedbackTR - dram->pearD[2]) + dram->pearD[3])*pear*0.5f;
			dram->pearD[2] = feedbackTR = (pear * feedbackTR) + ((1.0f-pear) * (dram->pearD[2] + dram->pearD[3]));
			dram->pearD[3] = slew;
			//Air Discontinuity D begin
			feedbackDL *= topdB;
			if (feedbackDL < -0.222f) feedbackDL = -0.222f; if (feedbackDL > 0.222f) feedbackDL = 0.222f;
			dram->dBdL[dBdXL] = feedbackDL;  dBdPosL *= 0.5f; dBdPosL += fabs((feedbackDL*((feedbackDL*0.25f)-0.5f))*0.5f);
			dBdly = floor(dBdPosL*dscBuf); dBi = (dBdPosL*dscBuf)-dBdly;
			feedbackDL = dram->dBdL[dBdXL-dBdly +((dBdXL-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
			dBdly++; feedbackDL += dram->dBdL[dBdXL-dBdly +((dBdXL-dBdly < 0)?dscBuf:0)]*dBi;
			dBdXL++; if (dBdXL < 0 || dBdXL >= dscBuf) dBdXL = 0;
			feedbackDL /= topdB;
			feedbackTR *= topdB;
			if (feedbackTR < -0.222f) feedbackTR = -0.222f; if (feedbackTR > 0.222f) feedbackTR = 0.222f;
			dram->dBdR[dBdXR] = feedbackTR;  dBdPosR *= 0.5f; dBdPosR += fabs((feedbackTR*((feedbackTR*0.25f)-0.5f))*0.5f);
			dBdly = floor(dBdPosR*dscBuf); dBi = (dBdPosR*dscBuf)-dBdly;
			feedbackTR = dram->dBdR[dBdXR-dBdly +((dBdXR-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
			dBdly++; feedbackTR += dram->dBdR[dBdXR-dBdly +((dBdXR-dBdly < 0)?dscBuf:0)]*dBi;
			dBdXR++; if (dBdXR < 0 || dBdXR >= dscBuf) dBdXR = 0;
			feedbackTR /= topdB;
			//Air Discontinuity D end
			
			feedbackEL = ((outYL*3.0f) - ((outUL + outVL + outWL + outXL)*2.0f));
			feedbackYR = ((outUR*3.0f) - ((outAR + outFR + outKR + outPR)*2.0f));
			slew = ((feedbackEL - dram->pearE[0]) + dram->pearE[1])*pear*0.5f;
			dram->pearE[0] = feedbackEL = (pear * feedbackEL) + ((1.0f-pear) * (dram->pearE[0] + dram->pearE[1]));
			dram->pearE[1] = slew;
			slew = ((feedbackYR - dram->pearE[2]) + dram->pearE[3])*pear*0.5f;
			dram->pearE[2] = feedbackYR = (pear * feedbackYR) + ((1.0f-pear) * (dram->pearE[2] + dram->pearE[3]));
			dram->pearE[3] = slew;
			//Air Discontinuity E begin
			feedbackEL *= topdB;
			if (feedbackEL < -0.222f) feedbackEL = -0.222f; if (feedbackEL > 0.222f) feedbackEL = 0.222f;
			dram->dBeL[dBeXL] = feedbackEL;  dBePosL *= 0.5f; dBePosL += fabs((feedbackEL*((feedbackEL*0.25f)-0.5f))*0.5f);
			dBdly = floor(dBePosL*dscBuf); dBi = (dBePosL*dscBuf)-dBdly;
			feedbackEL = dram->dBeL[dBeXL-dBdly +((dBeXL-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
			dBdly++; feedbackEL += dram->dBeL[dBeXL-dBdly +((dBeXL-dBdly < 0)?dscBuf:0)]*dBi;
			dBeXL++; if (dBeXL < 0 || dBeXL >= dscBuf) dBeXL = 0;
			feedbackEL /= topdB;
			feedbackYR *= topdB;
			if (feedbackYR < -0.222f) feedbackYR = -0.222f; if (feedbackYR > 0.222f) feedbackYR = 0.222f;
			dram->dBeR[dBeXR] = feedbackYR;  dBePosR *= 0.5f; dBePosR += fabs((feedbackYR*((feedbackYR*0.25f)-0.5f))*0.5f);
			dBdly = floor(dBePosR*dscBuf); dBi = (dBePosR*dscBuf)-dBdly;
			feedbackYR = dram->dBeR[dBeXR-dBdly +((dBeXR-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
			dBdly++; feedbackYR += dram->dBeR[dBeXR-dBdly +((dBeXR-dBdly < 0)?dscBuf:0)]*dBi;
			dBeXR++; if (dBeXR < 0 || dBeXR >= dscBuf) dBeXR = 0;
			feedbackYR /= topdB;
			//Air Discontinuity E end			
			//which we need to feed back into the input again, a bit
			
			inputSampleL = (outUL + outVL + outWL + outXL + outYL)*0.0008f;
			inputSampleR = (outAR + outFR + outKR + outPR + outUR)*0.0008f;
			//and take the final combined sum of outputs, corrected for Householder gain
						
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
		
		if (inputSampleL > 1.0f) inputSampleL = 1.0f;
		if (inputSampleL < -1.0f) inputSampleL = -1.0f;
		if (inputSampleR > 1.0f) inputSampleR = 1.0f;
		if (inputSampleR < -1.0f) inputSampleR = -1.0f;
		
		float slew = ((inputSampleL - dram->pearF[0]) + dram->pearF[1])*pearScaled*0.5f;
		dram->pearF[0] = inputSampleL = (pearScaled * inputSampleL) + ((1.0f-pearScaled) * (dram->pearF[0] + dram->pearF[1]));
		dram->pearF[1] = slew;
		slew = ((inputSampleR - dram->pearF[2]) + dram->pearF[3])*pearScaled*0.5f;
		dram->pearF[2] = inputSampleR = (pearScaled * inputSampleR) + ((1.0f-pearScaled) * (dram->pearF[2] + dram->pearF[3]));
		dram->pearF[3] = slew;
		
		if (wet < 1.0f) {inputSampleL *= wet; inputSampleR *= wet;}
		if (dry < 1.0f) {drySampleL *= dry; drySampleR *= dry;}
		inputSampleL += drySampleL; inputSampleR += drySampleR;
		//this is our submix verb dry/wet: 0.5f is BOTH at FULL VOLUME
		//purpose is that, if you're adding verb, you're not altering other balances
		
		
		
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
	gainOutL = gainOutR = 1.0;
	
	for(int count = 0; count < delayA+2; count++) {dram->aAL[count] = 0.0; dram->aAR[count] = 0.0;}
	for(int count = 0; count < delayB+2; count++) {dram->aBL[count] = 0.0; dram->aBR[count] = 0.0;}
	for(int count = 0; count < delayC+2; count++) {dram->aCL[count] = 0.0; dram->aCR[count] = 0.0;}
	for(int count = 0; count < delayD+2; count++) {dram->aDL[count] = 0.0; dram->aDR[count] = 0.0;}
	for(int count = 0; count < delayE+2; count++) {dram->aEL[count] = 0.0; dram->aER[count] = 0.0;}
	for(int count = 0; count < delayF+2; count++) {dram->aFL[count] = 0.0; dram->aFR[count] = 0.0;}
	for(int count = 0; count < delayG+2; count++) {dram->aGL[count] = 0.0; dram->aGR[count] = 0.0;}
	for(int count = 0; count < delayH+2; count++) {dram->aHL[count] = 0.0; dram->aHR[count] = 0.0;}
	for(int count = 0; count < delayI+2; count++) {dram->aIL[count] = 0.0; dram->aIR[count] = 0.0;}
	for(int count = 0; count < delayJ+2; count++) {dram->aJL[count] = 0.0; dram->aJR[count] = 0.0;}
	for(int count = 0; count < delayK+2; count++) {dram->aKL[count] = 0.0; dram->aKR[count] = 0.0;}
	for(int count = 0; count < delayL+2; count++) {dram->aLL[count] = 0.0; dram->aLR[count] = 0.0;}
	for(int count = 0; count < delayM+2; count++) {dram->aML[count] = 0.0; dram->aMR[count] = 0.0;}
	for(int count = 0; count < delayN+2; count++) {dram->aNL[count] = 0.0; dram->aNR[count] = 0.0;}
	for(int count = 0; count < delayO+2; count++) {dram->aOL[count] = 0.0; dram->aOR[count] = 0.0;}
	for(int count = 0; count < delayP+2; count++) {dram->aPL[count] = 0.0; dram->aPR[count] = 0.0;}
	for(int count = 0; count < delayQ+2; count++) {dram->aQL[count] = 0.0; dram->aQR[count] = 0.0;}
	for(int count = 0; count < delayR+2; count++) {dram->aRL[count] = 0.0; dram->aRR[count] = 0.0;}
	for(int count = 0; count < delayS+2; count++) {dram->aSL[count] = 0.0; dram->aSR[count] = 0.0;}
	for(int count = 0; count < delayT+2; count++) {dram->aTL[count] = 0.0; dram->aTR[count] = 0.0;}
	for(int count = 0; count < delayU+2; count++) {dram->aUL[count] = 0.0; dram->aUR[count] = 0.0;}
	for(int count = 0; count < delayV+2; count++) {dram->aVL[count] = 0.0; dram->aVR[count] = 0.0;}
	for(int count = 0; count < delayW+2; count++) {dram->aWL[count] = 0.0; dram->aWR[count] = 0.0;}
	for(int count = 0; count < delayX+2; count++) {dram->aXL[count] = 0.0; dram->aXR[count] = 0.0;}
	for(int count = 0; count < delayY+2; count++) {dram->aYL[count] = 0.0; dram->aYR[count] = 0.0;}
	
	for(int count = 0; count < predelay+2; count++) {dram->aZL[count] = 0.0; dram->aZR[count] = 0.0;}
	for(int count = 0; count < vlfpredelay+2; count++) {dram->aVLFL[count] = 0.0; dram->aVLFR[count] = 0.0;}
	
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
	countVLF = 1;
	
	for (int x = 0; x < pear_total; x++) {dram->pearA[x] = 0.0; dram->pearB[x] = 0.0; dram->pearC[x] = 0.0; dram->pearD[x] = 0.0; dram->pearE[x] = 0.0; dram->pearF[x] = 0.0;}
	//from PearEQ
	
	vibratoL = vibAL = vibAR = vibBL = vibBR = 0.0;
	vibratoR = M_PI_4;
	
	subAL = subAR = subBL = subBR = subCL = subCR = 0.0;
	//from SubTight
	
	for(int count = 0; count < dscBuf+2; count++) {
		dram->dBaL[count] = 0.0;
		dram->dBaR[count] = 0.0;
		dram->dBbL[count] = 0.0;
		dram->dBbR[count] = 0.0;
		dram->dBcL[count] = 0.0;
		dram->dBcR[count] = 0.0;
		dram->dBdL[count] = 0.0;
		dram->dBdR[count] = 0.0;
		dram->dBeL[count] = 0.0;
		dram->dBeR[count] = 0.0;
	}
	dBaPosL = 0.0;
	dBbPosL = 0.0;
	dBcPosL = 0.0;
	dBdPosL = 0.0;
	dBePosL = 0.0;
	dBaPosR = 0.0;
	dBbPosR = 0.0;
	dBcPosR = 0.0;
	dBdPosR = 0.0;
	dBePosR = 0.0;
	dBaXL = 1;
	dBbXL = 1;
	dBcXL = 1;	
	dBdXL = 1;	
	dBeXL = 1;	
	dBaXR = 1;
	dBbXR = 1;
	dBcXR = 1;	
	dBdXR = 1;	
	dBeXR = 1;	
	
	for (int x = 0; x < bez_total; x++) dram->bez[x] = 0.0;
	dram->bez[bez_cycle] = 1.0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
