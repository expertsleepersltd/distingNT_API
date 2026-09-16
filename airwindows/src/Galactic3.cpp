#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Galactic3"
#define AIRWINDOWS_DESCRIPTION "Exactly Galactic again, but with better undersampling."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','a','3' )
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
{ .name = "Replace", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Brightness", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Detune", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Derez", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bigness", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	float iirAL;
	float iirBL;
	
	
	
	
	float vibML, vibMR, depthM, oldfpd;
	
	float feedbackAL;
	float feedbackBL;
	float feedbackCL;
	float feedbackDL;
	
	float iirAR;
	float iirBR;
	
	
	
	
	float feedbackAR;
	float feedbackBR;
	float feedbackCR;
	float feedbackDR;
		
	int countA, delayA;
	int countB, delayB;
	int countC, delayC;
	int countD, delayD;
	int countE, delayE;
	int countF, delayF;
	int countG, delayG;
	int countH, delayH;
	int countI, delayI;
	int countJ, delayJ;
	int countK, delayK;
	int countL, delayL;
	int countM, delayM; //all these ints are shared across channels, not duplicated
	
	float vibM;
	
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
		float aIL[6480];
	float aJL[3660];
	float aKL[1720];
	float aLL[680];
	float aAL[9700];
	float aBL[6000];
	float aCL[2320];
	float aDL[940];
	float aEL[15220];
	float aFL[8460];
	float aGL[4540];
	float aHL[3200];
	float aML[3111];
	float aMR[3111];
	float aIR[6480];
	float aJR[3660];
	float aKR[1720];
	float aLR[680];
	float aAR[9700];
	float aBR[6000];
	float aCR[2320];
	float aDR[940];
	float aER[15220];
	float aFR[8460];
	float aGR[4540];
	float aHR[3200];
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
	
	float regen = 0.0625f+((1.0f-GetParameter( kParam_A ))*0.0625f);
	float attenuate = (1.0f - (regen / 0.125f))*1.333f;
	float lowpass = powf(1.00001f-(1.0f-GetParameter( kParam_B )),2.0f)/sqrt(overallscale);
	float drift = powf(GetParameter( kParam_C ),3)*0.001f;
	float derez = GetParameter( kParam_D )/overallscale;
	if (derez < 0.0005f) derez = 0.0005f; if (derez > 1.0f) derez = 1.0f;
	derez = 1.0f / ((int)(1.0f/derez));
	//this hard-locks derez to exact subdivisions of 1.0f
	float size = (GetParameter( kParam_E )*1.77f)+0.1f;
	float wet = 1.0f-(powf(1.0f-GetParameter( kParam_F ),3));
	
	delayI = 3407.0f*size;
	delayJ = 1823.0f*size;
	delayK = 859.0f*size;
	delayL = 331.0f*size;
	delayA = 4801.0f*size;
	delayB = 2909.0f*size;
	delayC = 1153.0f*size;
	delayD = 461.0f*size;
	delayE = 7607.0f*size;
	delayF = 4217.0f*size;
	delayG = 2269.0f*size;
	delayH = 1597.0f*size;
	delayM = 256;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		vibM += (oldfpd*drift);
		if (vibM > (3.141592653589793238f*2.0f)) {
			vibM = 0.0f;
			oldfpd = 0.4294967295f+(fpdL*0.0000000000618f);
		}
		
		dram->aML[countM] = inputSampleL * attenuate;
		dram->aMR[countM] = inputSampleR * attenuate;
		countM++; if (countM < 0 || countM > delayM) countM = 0;
		
		float offsetML = (sin(vibM)+1.0f)*127;
		float offsetMR = (sin(vibM+(3.141592653589793238f/2.0f))+1.0f)*127;
		int workingML = countM + offsetML;
		int workingMR = countM + offsetMR;
		float interpolML = (dram->aML[workingML-((workingML > delayM)?delayM+1:0)] * (1-(offsetML-floor(offsetML))));
		interpolML += (dram->aML[workingML+1-((workingML+1 > delayM)?delayM+1:0)] * ((offsetML-floor(offsetML))) );
		float interpolMR = (dram->aMR[workingMR-((workingMR > delayM)?delayM+1:0)] * (1-(offsetMR-floor(offsetMR))));
		interpolMR += (dram->aMR[workingMR+1-((workingMR+1 > delayM)?delayM+1:0)] * ((offsetMR-floor(offsetMR))) );
		inputSampleL = interpolML;
		inputSampleR = interpolMR;
		//predelay that applies vibrato
		//want vibrato speed AND depth like in MatrixVerb
		
		iirAL = (iirAL*(1.0f-lowpass))+(inputSampleL*lowpass); inputSampleL = iirAL;
		iirAR = (iirAR*(1.0f-lowpass))+(inputSampleR*lowpass); inputSampleR = iirAR;
		//initial filter
		
		dram->bez[bez_cycle] += derez;
		dram->bez[bez_SampL] += ((inputSampleL+dram->bez[bez_InL]) * derez);
		dram->bez[bez_SampR] += ((inputSampleR+dram->bez[bez_InR]) * derez);
		dram->bez[bez_InL] = inputSampleL; dram->bez[bez_InR] = inputSampleR;
		if (dram->bez[bez_cycle] > 1.0f) { //hit the end point and we do a reverb sample
			dram->bez[bez_cycle] = 0.0f;
			
			dram->aIL[countI] = (dram->bez[bez_SampL]+dram->bez[bez_UnInL]) + (feedbackAR * regen);
			dram->aJL[countJ] = (dram->bez[bez_SampL]+dram->bez[bez_UnInL]) + (feedbackBR * regen);
			dram->aKL[countK] = (dram->bez[bez_SampL]+dram->bez[bez_UnInL]) + (feedbackCR * regen);
			dram->aLL[countL] = (dram->bez[bez_SampL]+dram->bez[bez_UnInL]) + (feedbackDR * regen);
			dram->bez[bez_UnInL] = dram->bez[bez_SampL];
			
			dram->aIR[countI] = (dram->bez[bez_SampR]+dram->bez[bez_UnInR]) + (feedbackAL * regen);
			dram->aJR[countJ] = (dram->bez[bez_SampR]+dram->bez[bez_UnInR]) + (feedbackBL * regen);
			dram->aKR[countK] = (dram->bez[bez_SampR]+dram->bez[bez_UnInR]) + (feedbackCL * regen);
			dram->aLR[countL] = (dram->bez[bez_SampR]+dram->bez[bez_UnInR]) + (feedbackDL * regen);
			dram->bez[bez_UnInR] = dram->bez[bez_SampR];
			
			countI++; if (countI < 0 || countI > delayI) countI = 0;
			countJ++; if (countJ < 0 || countJ > delayJ) countJ = 0;
			countK++; if (countK < 0 || countK > delayK) countK = 0;
			countL++; if (countL < 0 || countL > delayL) countL = 0;
			
			float outIL = dram->aIL[countI-((countI > delayI)?delayI+1:0)];
			float outJL = dram->aJL[countJ-((countJ > delayJ)?delayJ+1:0)];
			float outKL = dram->aKL[countK-((countK > delayK)?delayK+1:0)];
			float outLL = dram->aLL[countL-((countL > delayL)?delayL+1:0)];
			float outIR = dram->aIR[countI-((countI > delayI)?delayI+1:0)];
			float outJR = dram->aJR[countJ-((countJ > delayJ)?delayJ+1:0)];
			float outKR = dram->aKR[countK-((countK > delayK)?delayK+1:0)];
			float outLR = dram->aLR[countL-((countL > delayL)?delayL+1:0)];
			//first block: now we have four outputs
			
			dram->aAL[countA] = (outIL - (outJL + outKL + outLL));
			dram->aBL[countB] = (outJL - (outIL + outKL + outLL));
			dram->aCL[countC] = (outKL - (outIL + outJL + outLL));
			dram->aDL[countD] = (outLL - (outIL + outJL + outKL));
			dram->aAR[countA] = (outIR - (outJR + outKR + outLR));
			dram->aBR[countB] = (outJR - (outIR + outKR + outLR));
			dram->aCR[countC] = (outKR - (outIR + outJR + outLR));
			dram->aDR[countD] = (outLR - (outIR + outJR + outKR));
			
			countA++; if (countA < 0 || countA > delayA) countA = 0;
			countB++; if (countB < 0 || countB > delayB) countB = 0;
			countC++; if (countC < 0 || countC > delayC) countC = 0;
			countD++; if (countD < 0 || countD > delayD) countD = 0;
			
			float outAL = dram->aAL[countA-((countA > delayA)?delayA+1:0)];
			float outBL = dram->aBL[countB-((countB > delayB)?delayB+1:0)];
			float outCL = dram->aCL[countC-((countC > delayC)?delayC+1:0)];
			float outDL = dram->aDL[countD-((countD > delayD)?delayD+1:0)];
			float outAR = dram->aAR[countA-((countA > delayA)?delayA+1:0)];
			float outBR = dram->aBR[countB-((countB > delayB)?delayB+1:0)];
			float outCR = dram->aCR[countC-((countC > delayC)?delayC+1:0)];
			float outDR = dram->aDR[countD-((countD > delayD)?delayD+1:0)];
			//second block: four more outputs
			
			dram->aEL[countE] = (outAL - (outBL + outCL + outDL));
			dram->aFL[countF] = (outBL - (outAL + outCL + outDL));
			dram->aGL[countG] = (outCL - (outAL + outBL + outDL));
			dram->aHL[countH] = (outDL - (outAL + outBL + outCL));
			dram->aER[countE] = (outAR - (outBR + outCR + outDR));
			dram->aFR[countF] = (outBR - (outAR + outCR + outDR));
			dram->aGR[countG] = (outCR - (outAR + outBR + outDR));
			dram->aHR[countH] = (outDR - (outAR + outBR + outCR));
			
			countE++; if (countE < 0 || countE > delayE) countE = 0;
			countF++; if (countF < 0 || countF > delayF) countF = 0;
			countG++; if (countG < 0 || countG > delayG) countG = 0;
			countH++; if (countH < 0 || countH > delayH) countH = 0;
			
			float outEL = dram->aEL[countE-((countE > delayE)?delayE+1:0)];
			float outFL = dram->aFL[countF-((countF > delayF)?delayF+1:0)];
			float outGL = dram->aGL[countG-((countG > delayG)?delayG+1:0)];
			float outHL = dram->aHL[countH-((countH > delayH)?delayH+1:0)];
			float outER = dram->aER[countE-((countE > delayE)?delayE+1:0)];
			float outFR = dram->aFR[countF-((countF > delayF)?delayF+1:0)];
			float outGR = dram->aGR[countG-((countG > delayG)?delayG+1:0)];
			float outHR = dram->aHR[countH-((countH > delayH)?delayH+1:0)];
			//third block: final outputs
			
			feedbackAL = (outEL - (outFL + outGL + outHL));
			feedbackBL = (outFL - (outEL + outGL + outHL));
			feedbackCL = (outGL - (outEL + outFL + outHL));
			feedbackDL = (outHL - (outEL + outFL + outGL));
			feedbackAR = (outER - (outFR + outGR + outHR));
			feedbackBR = (outFR - (outER + outGR + outHR));
			feedbackCR = (outGR - (outER + outFR + outHR));
			feedbackDR = (outHR - (outER + outFR + outGR));
			//which we need to feed back into the input again, a bit
			
			inputSampleL = (outEL + outFL + outGL + outHL)/8.0f;
			inputSampleR = (outER + outFR + outGR + outHR)/8.0f;
			//and take the final combined sum of outputs
			
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
		
		iirBL = (iirBL*(1.0f-lowpass))+(inputSampleL*lowpass); inputSampleL = iirBL;
		iirBR = (iirBR*(1.0f-lowpass))+(inputSampleR*lowpass); inputSampleR = iirBR;
		//end filter
		
		if (wet < 1.0f) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f-wet));
			inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0f-wet));
		}
		
		
		
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
	iirAL = 0.0; iirAR = 0.0;
	iirBL = 0.0; iirBR = 0.0;
	
	for(int count = 0; count < 6479; count++) {dram->aIL[count] = 0.0;dram->aIR[count] = 0.0;}
	for(int count = 0; count < 3659; count++) {dram->aJL[count] = 0.0;dram->aJR[count] = 0.0;}
	for(int count = 0; count < 1719; count++) {dram->aKL[count] = 0.0;dram->aKR[count] = 0.0;}
	for(int count = 0; count < 679; count++) {dram->aLL[count] = 0.0;dram->aLR[count] = 0.0;}
	
	for(int count = 0; count < 9699; count++) {dram->aAL[count] = 0.0;dram->aAR[count] = 0.0;}
	for(int count = 0; count < 5999; count++) {dram->aBL[count] = 0.0;dram->aBR[count] = 0.0;}
	for(int count = 0; count < 2319; count++) {dram->aCL[count] = 0.0;dram->aCR[count] = 0.0;}
	for(int count = 0; count < 939; count++) {dram->aDL[count] = 0.0;dram->aDR[count] = 0.0;}
	
	for(int count = 0; count < 15219; count++) {dram->aEL[count] = 0.0;dram->aER[count] = 0.0;}
	for(int count = 0; count < 8459; count++) {dram->aFL[count] = 0.0;dram->aFR[count] = 0.0;}
	for(int count = 0; count < 4539; count++) {dram->aGL[count] = 0.0;dram->aGR[count] = 0.0;}
	for(int count = 0; count < 3199; count++) {dram->aHL[count] = 0.0;dram->aHR[count] = 0.0;}
	
	for(int count = 0; count < 3110; count++) {dram->aML[count] = dram->aMR[count] = 0.0;}	
	
	feedbackAL = 0.0; feedbackAR = 0.0;
	feedbackBL = 0.0; feedbackBR = 0.0;
	feedbackCL = 0.0; feedbackCR = 0.0;
	feedbackDL = 0.0; feedbackDR = 0.0;
		
	countI = 1;
	countJ = 1;
	countK = 1;
	countL = 1;
	
	countA = 1;
	countB = 1;
	countC = 1;
	countD = 1;	
	
	countE = 1;
	countF = 1;
	countG = 1;
	countH = 1;
	countM = 1;
	//the predelay
	
	vibM = 3.0;
	
	oldfpd = 429496.7295;
	
	for (int x = 0; x < bez_total; x++) dram->bez[x] = 0.0;
	dram->bez[bez_cycle] = 1.0;

	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
