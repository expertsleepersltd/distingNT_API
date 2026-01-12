#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Verbity"
#define AIRWINDOWS_DESCRIPTION "A dual-mono reverb, which uses feedforward reverb topology."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','V','e','t' )
#define AIRWINDOWS_TAGS kNT_tagEffect | kNT_tagReverb
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Bigness", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Longness", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Darkness", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Wetness", .min = 0, .max = 1000, .def = 250, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 		
		Float32 iirA;
		Float32 iirB;
		
		
		
		
		Float32 feedbackA;
		Float32 feedbackB;
		Float32 feedbackC;
		Float32 feedbackD;
		Float32 previousA;
		Float32 previousB;
		Float32 previousC;
		Float32 previousD;
		
		Float32 lastRef[7];
		Float32 thunder;		
		
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
		int cycle;
		
		uint32_t fpd;
	
	struct _dram {
			Float32 aI[6480];
		Float32 aJ[3660];
		Float32 aK[1720];
		Float32 aL[680];
		Float32 aA[9700];
		Float32 aB[6000];
		Float32 aC[2320];
		Float32 aD[940];
		Float32 aE[15220];
		Float32 aF[8460];
		Float32 aG[4540];
		Float32 aH[3200];
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
	
	Float32 size = (GetParameter( kParam_One )*1.77f)+0.1f;
	Float32 regen = 0.0625f+(GetParameter( kParam_Two )*0.03125f); //0.09375f max;
	Float32 lowpass = (1.0f-powf(GetParameter( kParam_Three ),2.0f))/sqrt(overallscale);
	Float32 interpolate = powf(GetParameter( kParam_Three ),2.0f)*0.618033988749894848204586f; //has IIRlike qualities
	Float32 thunderAmount = (0.3f-(GetParameter( kParam_Two )*0.22f))*GetParameter( kParam_Three )*0.1f;
	Float32 wet = GetParameter( kParam_Four )*2.0f;
	Float32 dry = 2.0f - wet;
	if (wet > 1.0f) wet = 1.0f;
	if (wet < 0.0f) wet = 0.0f;
	if (dry > 1.0f) dry = 1.0f;
	if (dry < 0.0f) dry = 0.0f;
	//this reverb makes 50% full dry AND full wet, not crossfaded.
	//that's so it can be on submixes without cutting back dry channel when adjusted:
	//unless you go super heavy, you are only adjusting the added verb loudness.
	
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
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		if (fabs(iirA)<1.18e-37f) iirA = 0.0f;
		iirA = (iirA*(1.0f-lowpass))+(inputSample*lowpass); inputSample = iirA;
		//initial filter
		
		cycle++;
		if (cycle == cycleEnd) { //hit the end point and we do a reverb sample
			feedbackA = (feedbackA*(1.0f-interpolate))+(previousA*interpolate); previousA = feedbackA;
			feedbackB = (feedbackB*(1.0f-interpolate))+(previousB*interpolate); previousB = feedbackB;
			feedbackC = (feedbackC*(1.0f-interpolate))+(previousC*interpolate); previousC = feedbackC;
			feedbackD = (feedbackD*(1.0f-interpolate))+(previousD*interpolate); previousD = feedbackD;
			
			thunder = (thunder*0.99f)-(feedbackA*thunderAmount);

			dram->aI[countI] = inputSample + ((feedbackA+thunder) * regen);
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
		
		if (fabs(iirB)<1.18e-37f) iirB = 0.0f;
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
	
	for(int count = 0; count < 6479; count++) {dram->aI[count] = 0.0;}
	for(int count = 0; count < 3659; count++) {dram->aJ[count] = 0.0;}
	for(int count = 0; count < 1719; count++) {dram->aK[count] = 0.0;}
	for(int count = 0; count < 679; count++) {dram->aL[count] = 0.0;}
	
	for(int count = 0; count < 9699; count++) {dram->aA[count] = 0.0;}
	for(int count = 0; count < 5999; count++) {dram->aB[count] = 0.0;}
	for(int count = 0; count < 2319; count++) {dram->aC[count] = 0.0;}
	for(int count = 0; count < 939; count++) {dram->aD[count] = 0.0;}
	
	for(int count = 0; count < 15219; count++) {dram->aE[count] = 0.0;}
	for(int count = 0; count < 8459; count++) {dram->aF[count] = 0.0;}
	for(int count = 0; count < 4539; count++) {dram->aG[count] = 0.0;}
	for(int count = 0; count < 3199; count++) {dram->aH[count] = 0.0;}
	
	feedbackA = 0.0;
	feedbackB = 0.0;
	feedbackC = 0.0;
	feedbackD = 0.0;
	previousA = 0.0;
	previousB = 0.0;
	previousC = 0.0;
	previousD = 0.0;
	
	for(int count = 0; count < 6; count++) {lastRef[count] = 0.0;}
	
	thunder = 0;
	
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
	
	cycle = 0;
		
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
