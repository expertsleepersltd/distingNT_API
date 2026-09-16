#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Chamber"
#define AIRWINDOWS_DESCRIPTION "A feedforward reverb based on the golden ratio."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','h','r' )
#define AIRWINDOWS_TAGS kNT_tagEffect | kNT_tagReverb
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Bigness", .min = 0, .max = 1000, .def = 350, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Longness", .min = 0, .max = 1000, .def = 350, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Liteness", .min = 0, .max = 1000, .def = 350, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Darkness", .min = 0, .max = 1000, .def = 350, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Wetness", .min = 0, .max = 1000, .def = 350, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 iirA;
		Float32 iirB;
		Float32 iirC;
		
		
		Float32 feedbackA;
		Float32 feedbackB;
		Float32 feedbackC;
		Float32 feedbackD;
		Float32 previousA;
		Float32 previousB;
		Float32 previousC;
		Float32 previousD;
		
		float lastRef[10];
		int cycle;
		
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
		
		uint32_t fpd;
	
	struct _dram {
			Float32 aE[20000];
		Float32 aF[12361];
		Float32 aG[7640];
		Float32 aH[4722];
		Float32 aA[2916];
		Float32 aB[1804];
		Float32 aC[1115];
		Float32 aD[689];
		Float32 aI[426];
		Float32 aJ[264];
		Float32 aK[163];
		Float32 aL[101];
	};
	_dram* dram;
};
_kernel kernels[1];

#include "../include/template2.h"
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 4) cycleEnd = 4;
	//this is going to be 2 for 88.1f or 96k, 3 for silly people, 4 for 176 or 192k
	if (cycle > cycleEnd-1) cycle = cycleEnd-1; //sanity check
	
	Float32 size = (powf(GetParameter( kParam_One ),2)*0.9f)+0.1f;
	Float32 regen = (1.0f-(powf(1.0f-GetParameter( kParam_Two ),6)))*0.123f;
	Float32 highpass = (powf(GetParameter( kParam_Three ),2.0f))/sqrt(overallscale);
	Float32 lowpass = (1.0f-powf(GetParameter( kParam_Four ),2.0f))/sqrt(overallscale);
	Float32 interpolate = size*0.381966011250105f;
	Float32 wet = GetParameter( kParam_Five )*2.0f;
	Float32 dry = 2.0f - wet;
	if (wet > 1.0f) wet = 1.0f;
	if (wet < 0.0f) wet = 0.0f;
	if (dry > 1.0f) dry = 1.0f;
	if (dry < 0.0f) dry = 0.0f;
	//this reverb makes 50% full dry AND full wet, not crossfaded.
	//that's so it can be on submixes without cutting back dry channel when adjusted:
	//unless you go super heavy, you are only adjusting the added verb loudness.
	
	delayE = 19900*size;
	delayF = delayE*0.618033988749894848204586f; 
	delayG = delayF*0.618033988749894848204586f;
	delayH = delayG*0.618033988749894848204586f;
	delayA = delayH*0.618033988749894848204586f;
	delayB = delayA*0.618033988749894848204586f;
	delayC = delayB*0.618033988749894848204586f;
	delayD = delayC*0.618033988749894848204586f;
	delayI = delayD*0.618033988749894848204586f;
	delayJ = delayI*0.618033988749894848204586f;
	delayK = delayJ*0.618033988749894848204586f;
	delayL = delayK*0.618033988749894848204586f;
	//initially designed around the Fibonnaci series, Chamber uses
	//delay coefficients that are all related to the Golden Ratio,
	//Turns out that as you continue to sustain them, it turns from a
	//chunky slapback effect into a smoother reverb tail that can
	//sustain infinitely.	
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		iirC = (iirC*(1.0f-highpass))+(inputSample*highpass); inputSample -= iirC;
		//initial highpass
		
		iirA = (iirA*(1.0f-lowpass))+(inputSample*lowpass); inputSample = iirA;
		//initial filter
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do a reverb sample
			
			feedbackA = (feedbackA*(1.0f-interpolate))+(previousA*interpolate); previousA = feedbackA;
			feedbackB = (feedbackB*(1.0f-interpolate))+(previousB*interpolate); previousB = feedbackB;
			feedbackC = (feedbackC*(1.0f-interpolate))+(previousC*interpolate); previousC = feedbackC;
			feedbackD = (feedbackD*(1.0f-interpolate))+(previousD*interpolate); previousD = feedbackD;
			
			dram->aI[countI] = inputSample + (feedbackA * regen);
			dram->aJ[countJ] = inputSample + (feedbackB * regen);
			dram->aK[countK] = inputSample + (feedbackC * regen);
			dram->aL[countL] = inputSample + (feedbackD * regen);
			
			countI++; if (countI < 0 || countI > delayI) countI = 0;
			countJ++; if (countJ < 0 || countJ > delayJ) countJ = 0;
			countK++; if (countK < 0 || countK > delayK) countK = 0;
			countL++; if (countL < 0 || countL > delayL) countL = 0;
			
			Float32 outI = dram->aI[countI-((countI > delayI)?delayI+1:0)];
			Float32 outJ = dram->aJ[countJ-((countJ > delayJ)?delayJ+1:0)];
			Float32 outK = dram->aK[countK-((countK > delayK)?delayK+1:0)];
			Float32 outL = dram->aL[countL-((countL > delayL)?delayL+1:0)];
			//first block: now we have four outputs
			
			dram->aA[countA] = (outI - (outJ + outK + outL));
			dram->aB[countB] = (outJ - (outI + outK + outL));
			dram->aC[countC] = (outK - (outI + outJ + outL));
			dram->aD[countD] = (outL - (outI + outJ + outK));
			
			countA++; if (countA < 0 || countA > delayA) countA = 0;
			countB++; if (countB < 0 || countB > delayB) countB = 0;
			countC++; if (countC < 0 || countC > delayC) countC = 0;
			countD++; if (countD < 0 || countD > delayD) countD = 0;
			
			Float32 outA = dram->aA[countA-((countA > delayA)?delayA+1:0)];
			Float32 outB = dram->aB[countB-((countB > delayB)?delayB+1:0)];
			Float32 outC = dram->aC[countC-((countC > delayC)?delayC+1:0)];
			Float32 outD = dram->aD[countD-((countD > delayD)?delayD+1:0)];
			//second block: four more outputs
			
			dram->aE[countE] = (outA - (outB + outC + outD));
			dram->aF[countF] = (outB - (outA + outC + outD));
			dram->aG[countG] = (outC - (outA + outB + outD));
			dram->aH[countH] = (outD - (outA + outB + outC));
			
			countE++; if (countE < 0 || countE > delayE) countE = 0;
			countF++; if (countF < 0 || countF > delayF) countF = 0;
			countG++; if (countG < 0 || countG > delayG) countG = 0;
			countH++; if (countH < 0 || countH > delayH) countH = 0;
			
			Float32 outE = dram->aE[countE-((countE > delayE)?delayE+1:0)];
			Float32 outF = dram->aF[countF-((countF > delayF)?delayF+1:0)];
			Float32 outG = dram->aG[countG-((countG > delayG)?delayG+1:0)];
			Float32 outH = dram->aH[countH-((countH > delayH)?delayH+1:0)];
			//third block: final outputs
			
			feedbackA = (outE - (outF + outG + outH));
			feedbackB = (outF - (outE + outG + outH));
			feedbackC = (outG - (outE + outF + outH));
			feedbackD = (outH - (outE + outF + outG));
			//which we need to feed back into the input again, a bit
			
			inputSample = (outE + outF + outG + outH)/8.0f;
			//and take the final combined sum of outputs
			if (cycleEnd == 4) {
				lastRef[0] = lastRef[4]; //start from previous last
				lastRef[2] = (lastRef[0] + inputSample)/2; //half
				lastRef[1] = (lastRef[0] + lastRef[2])/2; //one quarter
				lastRef[3] = (lastRef[2] + inputSample)/2; //three quarters
				lastRef[4] = inputSample; //full
			}
			if (cycleEnd == 3) {
				lastRef[0] = lastRef[3]; //start from previous last
				lastRef[2] = (lastRef[0]+lastRef[0]+inputSample)/3; //third
				lastRef[1] = (lastRef[0]+inputSample+inputSample)/3; //two thirds
				lastRef[3] = inputSample; //full
			}
			if (cycleEnd == 2) {
				lastRef[0] = lastRef[2]; //start from previous last
				lastRef[1] = (lastRef[0] + inputSample)/2; //half
				lastRef[2] = inputSample; //full
			}
			if (cycleEnd == 1) lastRef[0] = inputSample;
			cycle = 0; //reset
			inputSample = lastRef[cycle];
		} else {
			inputSample = lastRef[cycle];
			//we are going through our references now
		}
		switch (cycleEnd) //multi-pole average using lastRef[] variables
		{
			case 4:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[7])*0.5f;
				lastRef[7] = lastRef[8]; //continue, do not break
			case 3:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[6])*0.5f;
				lastRef[6] = lastRef[8]; //continue, do not break
			case 2:
				lastRef[8] = inputSample; inputSample = (inputSample+lastRef[5])*0.5f;
				lastRef[5] = lastRef[8]; //continue, do not break
			case 1:
				break; //no further averaging
		}
		
		
		iirB = (iirB*(1.0f-lowpass))+(inputSample*lowpass); inputSample = iirB;
		//end filter
		
		if (wet < 1.0f) inputSample *= wet;
		if (dry < 1.0f) drySample *= dry;
		inputSample += drySample;
		//this is our submix verb dry/wet: 0.5f is BOTH at FULL VOLUME
		//purpose is that, if you're adding verb, you're not altering other balances
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirA = 0.0;
	iirB = 0.0;
	iirC = 0.0;
	
	for(int count = 0; count < 19999; count++) {dram->aE[count] = 0.0;}
	for(int count = 0; count < 12360; count++) {dram->aF[count] = 0.0;}
	for(int count = 0; count < 7639; count++) {dram->aG[count] = 0.0;}
	for(int count = 0; count < 4721; count++) {dram->aH[count] = 0.0;}
	for(int count = 0; count < 2915; count++) {dram->aA[count] = 0.0;}
	for(int count = 0; count < 1803; count++) {dram->aB[count] = 0.0;}
	for(int count = 0; count < 1114; count++) {dram->aC[count] = 0.0;}
	for(int count = 0; count < 688; count++) {dram->aD[count] = 0.0;}
	for(int count = 0; count < 425; count++) {dram->aI[count] = 0.0;}
	for(int count = 0; count < 263; count++) {dram->aJ[count] = 0.0;}
	for(int count = 0; count < 162; count++) {dram->aK[count] = 0.0;}
	for(int count = 0; count < 100; count++) {dram->aL[count] = 0.0;}
	
	feedbackA = 0.0;
	feedbackB = 0.0;
	feedbackC = 0.0;
	feedbackD = 0.0;
	previousA = 0.0;
	previousB = 0.0;
	previousC = 0.0;
	previousD = 0.0;
	
	for(int count = 0; count < 9; count++) {lastRef[count] = 0.0;}
	cycle = 0;
	
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
	
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
