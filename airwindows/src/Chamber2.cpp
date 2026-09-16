#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Chamber2"
#define AIRWINDOWS_DESCRIPTION "A feedforward reverb, a blur delay, and a glitch topping!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','h','2' )
#define AIRWINDOWS_TAGS kNT_tagEffect | kNT_tagReverb
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
{ .name = "Delay", .min = 0, .max = 1000, .def = 340, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Regen", .min = 0, .max = 1000, .def = 310, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Thick", .min = 0, .max = 1000, .def = 280, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Wet", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
    
	
	float feedbackAL;
	float feedbackBL;
	float feedbackCL;
	float feedbackDL;
	float previousAL;
	float previousBL;
	float previousCL;
	float previousDL;
	
	float lastRefL[10];
	
	
	float feedbackAR;
	float feedbackBR;
	float feedbackCR;
	float feedbackDR;
	float previousAR;
	float previousBR;
	float previousCR;
	float previousDR;
	
	float lastRefR[10];
	
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
	int countM, delayM;
	int cycle; //all these ints are shared across channels, not duplicated	
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float aEL[10000];
	float aFL[10000];
	float aGL[10000];
	float aHL[10000];
	float aAL[10000];
	float aBL[10000];
	float aCL[10000];
	float aDL[10000];
	float aIL[10000];
	float aJL[10000];
	float aKL[10000];
	float aLL[10000];
	float aML[10000];
	float aER[10000];
	float aFR[10000];
	float aGR[10000];
	float aHR[10000];
	float aAR[10000];
	float aBR[10000];
	float aCR[10000];
	float aDR[10000];
	float aIR[10000];
	float aJR[10000];
	float aKR[10000];
	float aLR[10000];
	float aMR[10000];
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
	
	Float32 size = (GetParameter( kParam_One )*0.9f)+0.1f;
	Float32 regen = (1.0f-(powf(1.0f-GetParameter( kParam_Two ),2)))*0.123f;
	Float32 echoScale = 1.0f-GetParameter( kParam_Three );
	Float32 echo = 0.618033988749894848204586f+((1.0f-0.618033988749894848204586f)*echoScale);
	Float32 interpolate = (1.0f-echo)*0.381966011250105f;
	//this now goes from Chamber, to all the reverb delays being exactly the same
	//much larger usage of RAM due to the larger reverb delays everywhere, but
	//ability to go to an unusual variation on blurred delay.
	Float32 wet = GetParameter( kParam_Four )*2.0f;
	Float32 dry = 2.0f - wet;
	if (wet > 1.0f) wet = 1.0f;
	if (wet < 0.0f) wet = 0.0f;
	if (dry > 1.0f) dry = 1.0f;
	if (dry < 0.0f) dry = 0.0f;
	//this reverb makes 50% full dry AND full wet, not crossfaded.
	//that's so it can be on submixes without cutting back dry channel when adjusted:
	//unless you go super heavy, you are only adjusting the added verb loudness.
	
	delayM = sqrt(9900*size);
	delayE = 9900*size;
	delayF = delayE*echo; 
	delayG = delayF*echo;
	delayH = delayG*echo;
	delayA = delayH*echo;
	delayB = delayA*echo;
	delayC = delayB*echo;
	delayD = delayC*echo;
	delayI = delayD*echo;
	delayJ = delayI*echo;
	delayK = delayJ*echo;
	delayL = delayK*echo;
	//initially designed around the Fibonnaci series, Chamber uses
	//delay coefficients that are all related to the Golden Ratio,
	//Turns out that as you continue to sustain them, it turns from a
	//chunky slapback effect into a smoother reverb tail that can
	//sustain infinitely.	
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do a reverb sample
			
			dram->aML[countM] = inputSampleL;
			dram->aMR[countM] = inputSampleR;
			countM++; if (countM < 0 || countM > delayM) countM = 0;
			inputSampleL = dram->aML[countM-((countM > delayM)?delayM+1:0)];
			inputSampleR = dram->aMR[countM-((countM > delayM)?delayM+1:0)];
			//predelay to make the first echo still be an echo even when blurred
			
			feedbackAL = (feedbackAL*(1.0f-interpolate))+(previousAL*interpolate); previousAL = feedbackAL;
			feedbackBL = (feedbackBL*(1.0f-interpolate))+(previousBL*interpolate); previousBL = feedbackBL;
			feedbackCL = (feedbackCL*(1.0f-interpolate))+(previousCL*interpolate); previousCL = feedbackCL;
			feedbackDL = (feedbackDL*(1.0f-interpolate))+(previousDL*interpolate); previousDL = feedbackDL;
			feedbackAR = (feedbackAR*(1.0f-interpolate))+(previousAR*interpolate); previousAR = feedbackAR;
			feedbackBR = (feedbackBR*(1.0f-interpolate))+(previousBR*interpolate); previousBR = feedbackBR;
			feedbackCR = (feedbackCR*(1.0f-interpolate))+(previousCR*interpolate); previousCR = feedbackCR;
			feedbackDR = (feedbackDR*(1.0f-interpolate))+(previousDR*interpolate); previousDR = feedbackDR;
			
			dram->aIL[countI] = inputSampleL + (feedbackAL * regen);
			dram->aJL[countJ] = inputSampleL + (feedbackBL * regen);
			dram->aKL[countK] = inputSampleL + (feedbackCL * regen);
			dram->aLL[countL] = inputSampleL + (feedbackDL * regen);
			dram->aIR[countI] = inputSampleR + (feedbackAR * regen);
			dram->aJR[countJ] = inputSampleR + (feedbackBR * regen);
			dram->aKR[countK] = inputSampleR + (feedbackCR * regen);
			dram->aLR[countL] = inputSampleR + (feedbackDR * regen);
			
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
			
			feedbackAR = (outEL - (outFL + outGL + outHL));
			feedbackBL = (outFL - (outEL + outGL + outHL));
			feedbackCR = (outGL - (outEL + outFL + outHL));
			feedbackDL = (outHL - (outEL + outFL + outGL));
			feedbackAL = (outER - (outFR + outGR + outHR));
			feedbackBR = (outFR - (outER + outGR + outHR));
			feedbackCL = (outGR - (outER + outFR + outHR));
			feedbackDR = (outHR - (outER + outFR + outGR));
			//which we need to feed back into the input again, a bit
			
			inputSampleL = (outEL + outFL + outGL + outHL)/8.0f;
			inputSampleR = (outER + outFR + outGR + outHR)/8.0f;
			//and take the final combined sum of outputs
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
		
		switch (cycleEnd) //multi-pole average using lastRef[] variables
		{
			case 4:
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[7])*0.5f;
				lastRefL[7] = lastRefL[8]; //continue, do not break
				lastRefR[8] = inputSampleR; inputSampleR = (inputSampleR+lastRefR[7])*0.5f;
				lastRefR[7] = lastRefR[8]; //continue, do not break
			case 3:
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[6])*0.5f;
				lastRefL[6] = lastRefL[8]; //continue, do not break
				lastRefR[8] = inputSampleR; inputSampleR = (inputSampleR+lastRefR[6])*0.5f;
				lastRefR[6] = lastRefR[8]; //continue, do not break
			case 2:
				lastRefL[8] = inputSampleL; inputSampleL = (inputSampleL+lastRefL[5])*0.5f;
				lastRefL[5] = lastRefL[8]; //continue, do not break
				lastRefR[8] = inputSampleR; inputSampleR = (inputSampleR+lastRefR[5])*0.5f;
				lastRefR[5] = lastRefR[8]; //continue, do not break
			case 1:
				break; //no further averaging
		}
		
		if (wet < 1.0f) {inputSampleL *= wet; inputSampleR *= wet;}
		if (dry < 1.0f) {drySampleL *= dry; drySampleR *= dry;}
		inputSampleL += drySampleL;
		inputSampleR += drySampleR;
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
	for(int count = 0; count < 9999; count++) {dram->aEL[count] = 0.0;dram->aER[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {dram->aFL[count] = 0.0;dram->aFR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {dram->aGL[count] = 0.0;dram->aGR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {dram->aHL[count] = 0.0;dram->aHR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {dram->aAL[count] = 0.0;dram->aAR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {dram->aBL[count] = 0.0;dram->aBR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {dram->aCL[count] = 0.0;dram->aCR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {dram->aDL[count] = 0.0;dram->aDR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {dram->aIL[count] = 0.0;dram->aIR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {dram->aJL[count] = 0.0;dram->aJR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {dram->aKL[count] = 0.0;dram->aKR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {dram->aLL[count] = 0.0;dram->aLR[count] = 0.0;}
	for(int count = 0; count < 9999; count++) {dram->aML[count] = 0.0;dram->aMR[count] = 0.0;}
	
	feedbackAL = 0.0; feedbackAR = 0.0;
	feedbackBL = 0.0; feedbackBR = 0.0;
	feedbackCL = 0.0; feedbackCR = 0.0;
	feedbackDL = 0.0; feedbackDR = 0.0;
	previousAL = 0.0; previousAR = 0.0;
	previousBL = 0.0; previousBR = 0.0;
	previousCL = 0.0; previousCR = 0.0;
	previousDL = 0.0; previousDR = 0.0;
	
	for(int count = 0; count < 9; count++) {lastRefL[count] = 0.0;lastRefR[count] = 0.0;}
	
	countI = 1;
	countJ = 1;
	countK = 1;
	countL = 1;
	countM = 1;
	
	countA = 1;
	countB = 1;
	countC = 1;
	countD = 1;	
	
	countE = 1;
	countF = 1;
	countG = 1;
	countH = 1;
	cycle = 0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
