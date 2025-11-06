#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SlewSonic"
#define AIRWINDOWS_DESCRIPTION "Combines SlewOnly with ultrasonic filtering to solo brightness."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','l','j' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Mute", .min = 5000, .max = 25000, .def = 15000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bright", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float biquadA[11];
		float lastSampleA;
		float biquadB[11];
		float lastSampleB;
		float biquadC[11];
		float lastSampleC;
		float biquadD[11];
		uint32_t fpd;
	
	struct _dram {
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
	
	float trim = 2.302585092994045684017991f; //natural logarithm of 10
	float freq = (GetParameter( kParam_One )*1000.0f) / GetSampleRate();
	if (freq > 0.499f) freq = 0.499f;
	biquadD[0] = biquadC[0] = biquadB[0] = biquadA[0] = freq;
	biquadA[1] = 2.24697960f;
	biquadB[1] = 0.80193774f;
	biquadC[1] = 0.55495813f;
	biquadD[1] = 0.5f;
	
	float K = tan(M_PI * biquadA[0]); //lowpass
	float norm = 1.0f / (1.0f + K / biquadA[1] + K * K);
	biquadA[2] = K * K * norm;
	biquadA[3] = 2.0f * biquadA[2];
	biquadA[4] = biquadA[2];
	biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadA[6] = (1.0f - K / biquadA[1] + K * K) * norm;
	
	K = tan(M_PI * biquadA[0]);
	norm = 1.0f / (1.0f + K / biquadB[1] + K * K);
	biquadB[2] = K * K * norm;
	biquadB[3] = 2.0f * biquadB[2];
	biquadB[4] = biquadB[2];
	biquadB[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadB[6] = (1.0f - K / biquadB[1] + K * K) * norm;
	
	K = tan(M_PI * biquadC[0]);
	norm = 1.0f / (1.0f + K / biquadC[1] + K * K);
	biquadC[2] = K * K * norm;
	biquadC[3] = 2.0f * biquadC[2];
	biquadC[4] = biquadC[2];
	biquadC[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadC[6] = (1.0f - K / biquadC[1] + K * K) * norm;
	
	K = tan(M_PI * biquadD[0]);
	norm = 1.0f / (1.0f + K / biquadD[1] + K * K);
	biquadD[2] = K * K * norm;
	biquadD[3] = 2.0f * biquadD[2];
	biquadD[4] = biquadD[2];
	biquadD[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadD[6] = (1.0f - K / biquadD[1] + K * K) * norm;
	
	Float32 aWet = 0.0f;
	Float32 bWet = 0.0f;
	Float32 cWet = GetParameter( kParam_Two )*3.0f;
	//eight-stage wet/dry control using progressive stages that bypass when not engaged
	if (cWet < 1.0f) {aWet = cWet; cWet = 0.0f;}
	else if (cWet < 2.0f) {bWet = cWet - 1.0f; aWet = 1.0f; cWet = 0.0f;}
	else {cWet -= 2.0f; bWet = aWet = 1.0f;}
	//this is one way to make a little set of dry/wet stages that are successively added to the
	//output as the control is turned up. Each one independently goes from 0-1 and stays at 1
	//beyond that point: this is a way to progressively add a 'black box' sound processing
	//which lets you fall through to simpler processing at lower settings.
	
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		float dryStageA = 0.0f;
		float dryStageB = 0.0f;
		float dryFinalBiquad = 0.0f;
		float tempSample = 0.0f;
		float outputSample = 0.0f;
		
		if (aWet > 0.0f) {
			tempSample = biquadA[2]*inputSample+biquadA[3]*biquadA[7]+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
			biquadA[8] = biquadA[7]; biquadA[7] = inputSample; inputSample = tempSample; 
			biquadA[10] = biquadA[9]; biquadA[9] = inputSample; //DF1
			outputSample = (inputSample - lastSampleA)*trim;
			lastSampleA = inputSample; inputSample = outputSample;
			dryStageA = inputSample = (inputSample * aWet) + (drySample * (1.0f-aWet));
			//first stage always runs, dry/wet between raw signal and this
		}
		
		if (bWet > 0.0f) {
			tempSample = biquadB[2]*inputSample+biquadB[3]*biquadB[7]+biquadB[4]*biquadB[8]-biquadB[5]*biquadB[9]-biquadB[6]*biquadB[10];
			biquadB[8] = biquadB[7]; biquadB[7] = inputSample; inputSample = tempSample; 
			biquadB[10] = biquadB[9]; biquadB[9] = inputSample; //DF1
			outputSample = (inputSample - lastSampleB)*trim;
			lastSampleB = inputSample; inputSample = outputSample;
			dryStageB = inputSample = (inputSample * bWet) + (dryStageA * (1.0f-bWet));
			//second stage adds upon first stage, crossfade between them
		}
		
		if (cWet > 0.0f) {
			tempSample = biquadC[2]*inputSample+biquadC[3]*biquadC[7]+biquadC[4]*biquadC[8]-biquadC[5]*biquadC[9]-biquadC[6]*biquadC[10];
			biquadC[8] = biquadC[7]; biquadC[7] = inputSample; inputSample = tempSample; 
			biquadC[10] = biquadC[9]; biquadC[9] = inputSample; //DF1
			outputSample = (inputSample - lastSampleC)*trim;
			lastSampleC = inputSample; inputSample = outputSample;
			inputSample = (inputSample * cWet) + (dryStageB * (1.0f-cWet));
			//third stage adds upon second stage, crossfade between them
		}
		
		if (aWet > 0.0f) {
			dryFinalBiquad = inputSample;
			tempSample = biquadD[2]*inputSample+biquadD[3]*biquadD[7]+biquadD[4]*biquadD[8]-biquadD[5]*biquadD[9]-biquadD[6]*biquadD[10];
			biquadD[8] = biquadD[7]; biquadD[7] = inputSample; inputSample = tempSample; 
			biquadD[10] = biquadD[9]; biquadD[9] = inputSample; //DF1
			//final post-slew-only stage always runs, minimum of one stage on dry and two on single slew-only
			inputSample = (inputSample * aWet) + (dryFinalBiquad * (1.0f-aWet));
		}
		
		
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < 11; x++) {
		biquadA[x] = 0.0;
		biquadB[x] = 0.0;
		biquadC[x] = 0.0;
		biquadD[x] = 0.0;
	}
	lastSampleA = lastSampleB = lastSampleC = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
