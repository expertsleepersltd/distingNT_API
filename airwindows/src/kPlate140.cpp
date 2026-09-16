#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "kPlate140"
#define AIRWINDOWS_DESCRIPTION "A next-generation Airwindows plate reverb."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','k','P','0' )
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
const int predelay = 16000; const int vlfpredelay = 16000;
const int delayA = 466; const int delayB = 233; const int delayC = 747; const int delayD = 70; const int delayE = 776; const int delayF = 719; const int delayG = 130; const int delayH = 511; const int delayI = 148; const int delayJ = 84; const int delayK = 787; const int delayL = 124; const int delayM = 72; const int delayN = 667; const int delayO = 773; const int delayP = 29; const int delayQ = 710; const int delayR = 309; const int delayS = 773; const int delayT = 487; const int delayU = 11; const int delayV = 73; const int delayW = 756; const int delayX = 627; const int delayY = 328; //6 to 86 ms, 254 seat club  
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
{ .name = "Input Pad", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Regen", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DeRez", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Predelay", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Wetness", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	float iirAL;
	float iirBL;
	
	float gainIn;
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
	
	float feedblurAL;
	float feedblurBL;
	float feedblurCL;
	float feedblurDL;
	float feedblurEL;
	
	float feedblurER;
	float feedblurJR;
	float feedblurOR;
	float feedblurTR;
	float feedblurYR;
	
	float sbAL;
	float sbBL;
	float sbCL;
	float sbDL;
	float sbEL;
	
	float sbER;
	float sbJR;
	float sbOR;
	float sbTR;
	float sbYR;
	
	
	enum {
		prevSampL1,
		prevSlewL1,
		prevSampR1,
		prevSlewR1,
		prevSampL2,
		prevSlewL2,
		prevSampR2,
		prevSlewR2,
		prevSampL3,
		prevSlewL3,
		prevSampR3,
		prevSlewR3,
		prevSampL4,
		prevSlewL4,
		prevSampR4,
		prevSlewR4,
		prevSampL5,
		prevSlewL5,
		prevSampR5,
		prevSlewR5,
		prevSampL6,
		prevSlewL6,
		prevSampR6,
		prevSlewR6,
		prevSampL7,
		prevSlewL7,
		prevSampR7,
		prevSlewR7,
		prevSampL8,
		prevSlewL8,
		prevSampR8,
		prevSlewR8,
		prevSampL9,
		prevSlewL9,
		prevSampR9,
		prevSlewR9,
		prevSampL10,
		prevSlewL10,
		prevSampR10,
		prevSlewR10,
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
	
	float previousAL;
	float previousBL;
	float previousCL;
	float previousDL;
	float previousEL;
	
	float iirAR;
	float iirBR;
	
	float previousAR;
	float previousBR;
	float previousCR;
	float previousDR;
	float previousER;
	
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
	
	float prevMulchBL;
	float prevMulchBR;
	float prevMulchCL;
	float prevMulchCR;
	float prevMulchDL;
	float prevMulchDR;
	float prevMulchEL;
	float prevMulchER;
	
	float prevOutDL;
	float prevOutDR;
	float prevOutEL;
	float prevOutER;
	
	float prevInDL;
	float prevInDR;
	float prevInEL;
	float prevInER;
	
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
	
	float inputPad = GetParameter( kParam_A );
	float sbScale = powf(1.0f-GetParameter( kParam_B ),3)*-0.0000001f;
	float sbRebound = (powf(GetParameter( kParam_B ),2)*24.448f)+39.552f;
	float blur = (1.0f-GetParameter( kParam_B ))*0.5f;
	float regen = 1.0f-powf(1.0f-GetParameter(kParam_B),2);
	regen = (regen*0.0001f)+0.000195f;
	float derez = GetParameter( kParam_C )/overallscale;
	if (derez < 0.0005f) derez = 0.0005f; if (derez > 1.0f) derez = 1.0f;
	derez = 1.0f / ((int)(1.0f/derez));
	//this hard-locks derez to exact subdivisions of 1.0f
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
		
		if (inputPad < 1.0f) {
			inputSampleL *= inputPad;
			inputSampleR *= inputPad;
		}
		
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
			dram->bez[bez_SampL] = dram->aZL[countZ-((countZ > adjPredelay)?adjPredelay+1:0)];
			dram->bez[bez_SampR] = dram->aZR[countZ-((countZ > adjPredelay)?adjPredelay+1:0)];
			//end predelay
			
			float avgSampL = (dram->bez[bez_SampL]+dram->bez[bez_UnInL]) * 0.125f;
			float avgSampR = (dram->bez[bez_SampR]+dram->bez[bez_UnInR]) * 0.125f;
			dram->bez[bez_UnInL] = dram->bez[bez_SampL];
			dram->bez[bez_UnInR] = dram->bez[bez_SampR];

			//begin SubTight section
			float outSampleL = avgSampL * 0.00187f;
			float outSampleR = avgSampR * 0.00187f;
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
			dram->aUL[countUL] = outUL; outUL *= 0.618033988749894848204586f;
			countUL++; if (countUL < 0 || countUL > delayU) countUL = 0;
			outUL += dram->aUL[countUL-((countUL > delayU)?delayU+1:0)];
			//a delay slot becomes an allpass
			vibBL = vibAL; vibAL = outUL; //tiny two sample delay chains
			vibratoL += fpdL * 0.5e-13f; if (vibratoL > M_PI*2.0f) vibratoL -= M_PI*2.0f;
			float quadL = sin(vibratoL)+1.0f;
			if (quadL < 1.0f) outUL = (outUL*(1.0f-quadL))+(vibAL*quadL);
			else outUL = (vibAL*(1.0f-(quadL-1.0f)))+(vibBL*(quadL-1.0f));
			//also, pitch drift this allpass slot for very subtle motion
			
			float outUR = ((outVR*3.0f) - ((outBR + outGR + outLR + outQR)*2.0f)) - (dram->aUR[(countUR+1)-((countUR+1 > delayU)?delayU+1:0)]*0.618033988749894848204586f);
			dram->aUR[countUR] = outUR; outUR *= 0.618033988749894848204586f;
			countUR++; if (countUR < 0 || countUR > delayU) countUR = 0;
			outUR += dram->aUR[countUR-((countUR > delayU)?delayU+1:0)];
			//a delay slot becomes an allpass
			vibBR = vibAR; vibAR = outUR; //tiny two sample delay chains
			vibratoR += fpdR * 0.5e-13f; if (vibratoR > M_PI*2.0f) vibratoR -= M_PI*2.0f;
			float quadR = sin(vibratoR)+1.0f;
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
			
			feedbackER = ((outUL*3.0f) - ((outVL + outWL + outXL + outYL)*2.0f));
			feedbackAL = ((outAR*3.0f) - ((outFR + outKR + outPR + outUR)*2.0f));
			feedbackJR = ((outVL*3.0f) - ((outUL + outWL + outXL + outYL)*2.0f));
			feedbackBL = ((outFR*3.0f) - ((outAR + outKR + outPR + outUR)*2.0f));
			feedbackOR = ((outWL*3.0f) - ((outUL + outVL + outXL + outYL)*2.0f));
			feedbackCL = ((outKR*3.0f) - ((outAR + outFR + outPR + outUR)*2.0f));
			feedbackTR = ((outXL*3.0f) - ((outUL + outVL + outWL + outYL)*2.0f));
			feedbackDL = ((outPR*3.0f) - ((outAR + outFR + outKR + outUR)*2.0f));
			feedbackYR = ((outYL*3.0f) - ((outUL + outVL + outWL + outXL)*2.0f));
			feedbackEL = ((outUR*3.0f) - ((outAR + outFR + outKR + outPR)*2.0f));
			//which we need to feed back into the input again, a bit
			
			if (fabs(feedbackER) < 2000.0f) {
				feedbackER += (2.0f * feedbackER * feedbackER) * sbER;
				sbER += ((feedbackER - sin(feedbackER))*sbScale);
			}
			sbER = sin(sbER*0.015625f)*sbRebound;
			if (fabs(feedbackAL) < 2000.0f) {
				feedbackAL += (2.0f * feedbackAL * feedbackAL) * sbAL;
				sbAL += ((feedbackAL - sin(feedbackAL))*sbScale);
			}
			sbAL = sin(sbAL*0.015625f)*sbRebound;
			if (fabs(feedbackJR) < 2000.0f) {
				feedbackJR += (2.0f * feedbackJR * feedbackJR) * sbJR;
				sbJR += ((feedbackJR - sin(feedbackJR))*sbScale);
			}
			sbJR = sin(sbJR*0.015625f)*sbRebound;
			if (fabs(feedbackBL) < 2000.0f) {
				feedbackBL += (2.0f * feedbackBL * feedbackBL) * sbBL;
				sbBL += ((feedbackBL - sin(feedbackBL))*sbScale);
			}
			sbBL = sin(sbBL*0.015625f)*sbRebound;
			if (fabs(feedbackOR) < 2000.0f) {
				feedbackOR += (2.0f * feedbackOR * feedbackOR) * sbOR;
				sbOR += ((feedbackOR - sin(feedbackOR))*sbScale);
			}
			sbOR = sin(sbOR*0.015625f)*sbRebound;
			if (fabs(feedbackCL) < 2000.0f) {
				feedbackCL += (2.0f * feedbackCL * feedbackCL) * sbCL;
				sbCL += ((feedbackCL - sin(feedbackCL))*sbScale);
			}
			sbCL = sin(sbCL*0.015625f)*sbRebound;
			if (fabs(feedbackTR) < 2000.0f) {
				feedbackTR += (2.0f * feedbackTR * feedbackTR) * sbTR;
				sbTR += ((feedbackTR - sin(feedbackTR))*sbScale);
			}
			sbTR = sin(sbTR*0.015625f)*sbRebound;
			if (fabs(feedbackDL) < 2000.0f) {
				feedbackDL += (2.0f * feedbackDL * feedbackDL) * sbDL;
				sbDL += ((feedbackDL - sin(feedbackDL))*sbScale);
			}
			sbDL = sin(sbDL*0.015625f)*sbRebound;
			if (fabs(feedbackYR) < 2000.0f) {
				feedbackYR += (2.0f * feedbackYR * feedbackYR) * sbYR;
				sbYR += ((feedbackYR - sin(feedbackYR))*sbScale);
			}
			sbYR = sin(sbYR*0.015625f)*sbRebound;
			if (fabs(feedbackEL) < 2000.0f) {
				feedbackEL += (2.0f * feedbackEL * feedbackEL) * sbEL;
				sbEL += ((feedbackEL - sin(feedbackEL))*sbScale);
			}
			sbEL = sin(sbEL*0.015625f)*sbRebound;
			
			float temp;
			temp = ((feedbackER*(1.0f-blur)) + (feedblurER*blur)); feedblurER = feedbackER; feedbackER = temp;
			temp = ((feedbackAL*(1.0f-blur)) + (feedblurAL*blur)); feedblurAL = feedbackAL; feedbackAL = temp;
			temp = ((feedbackJR*(1.0f-blur)) + (feedblurJR*blur)); feedblurJR = feedbackJR; feedbackJR = temp;
			temp = ((feedbackBL*(1.0f-blur)) + (feedblurBL*blur)); feedblurBL = feedbackBL; feedbackBL = temp;
			temp = ((feedbackOR*(1.0f-blur)) + (feedblurOR*blur)); feedblurOR = feedbackOR; feedbackOR = temp;
			temp = ((feedbackCL*(1.0f-blur)) + (feedblurCL*blur)); feedblurCL = feedbackCL; feedbackCL = temp;
			temp = ((feedbackTR*(1.0f-blur)) + (feedblurTR*blur)); feedblurTR = feedbackTR; feedbackTR = temp;
			temp = ((feedbackDL*(1.0f-blur)) + (feedblurDL*blur)); feedblurDL = feedbackDL; feedbackDL = temp;
			temp = ((feedbackYR*(1.0f-blur)) + (feedblurYR*blur)); feedblurYR = feedbackYR; feedbackYR = temp;
			temp = ((feedbackEL*(1.0f-blur)) + (feedblurEL*blur)); feedblurEL = feedbackEL; feedbackEL = temp;
			
			inputSampleL = (outUL + outVL + outWL + outXL + outYL)*0.0016f;
			inputSampleR = (outAR + outFR + outKR + outPR + outUR)*0.0016f;
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
	gainIn = gainOutL = gainOutR = 1.0;
	
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
	
	previousAL = 0.0;
	previousBL = 0.0;
	previousCL = 0.0;
	previousDL = 0.0;
	previousEL = 0.0;
	
	feedbackER = 0.0;
	feedbackJR = 0.0;
	feedbackOR = 0.0;
	feedbackTR = 0.0;
	feedbackYR = 0.0;
	
	previousAR = 0.0;
	previousBR = 0.0;
	previousCR = 0.0;
	previousDR = 0.0;
	previousER = 0.0;
	
	feedblurAL = 0.0;
	feedblurBL = 0.0;
	feedblurCL = 0.0;
	feedblurDL = 0.0;
	feedblurEL = 0.0;
	
	feedblurER = 0.0;
	feedblurJR = 0.0;
	feedblurOR = 0.0;
	feedblurTR = 0.0;
	feedblurYR = 0.0;
	
	sbAL = 0.0;
	sbBL = 0.0;
	sbCL = 0.0;
	sbDL = 0.0;
	sbEL = 0.0;
	
	sbER = 0.0;
	sbJR = 0.0;
	sbOR = 0.0;
	sbTR = 0.0;
	sbYR = 0.0;
			
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
	
	for (int x = 0; x < bez_total; x++) dram->bez[x] = 0.0;
	dram->bez[bez_cycle] = 1.0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
