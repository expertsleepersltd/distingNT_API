#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "XHighpass"
#define AIRWINDOWS_DESCRIPTION "A distorted digital EQ, inspired by retro sampler DSP."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','X','H','s' )
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
{ .name = "Gain", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Nuke", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float biquad[11];
		float biquadA[11];
		float biquadB[11];
		float biquadC[11];
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
	
	Float32 gain = powf(GetParameter( kParam_One )+0.5f,4);
	biquadA[0] = (powf(GetParameter( kParam_Two ),5)*20000.0f)/GetSampleRate();
	if (biquadA[0] < 0.001f) biquadA[0] = 0.001f;
	Float32 clipThreshold = 1.0f-(GetParameter( kParam_Three )*sqrt(GetParameter( kParam_Two )));
	
	float K = tan(M_PI * biquadA[0]);
	float norm = 1.0f / (1.0f + K / 0.7071f + K * K);
	biquadA[2] = norm;
	biquadA[3] = -2.0f * biquadA[2];
	biquadA[4] = biquadA[2];
	biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadA[6] = (1.0f - K / 0.7071f + K * K) * norm;
	
	for (int x = 0; x < 7; x++) {biquad[x] = biquadD[x] = biquadC[x] = biquadB[x] = biquadA[x];}
	
	Float32 aWet = 1.0f;
	Float32 bWet = 1.0f;
	Float32 cWet = 1.0f;
	Float32 dWet = GetParameter( kParam_Three )*4.0f;
	Float32 wet = GetParameter( kParam_Four );
	//four-stage wet/dry control using progressive stages that bypass when not engaged
	if (dWet < 1.0f) {aWet = dWet; bWet = 0.0f; cWet = 0.0f; dWet = 0.0f;}
	else if (dWet < 2.0f) {bWet = dWet - 1.0f; cWet = 0.0f; dWet = 0.0f;}
	else if (dWet < 3.0f) {cWet = dWet - 2.0f; dWet = 0.0f;}
	else {dWet -= 3.0f;}
	//this is one way to make a little set of dry/wet stages that are successively added to the
	//output as the control is turned up. Each one independently goes from 0-1 and stays at 1
	//beyond that point: this is a way to progressively add a 'black box' sound processing
	//which lets you fall through to simpler processing at lower settings.
	float outSample = 0.0f;
		
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		if (gain != 1.0f) {
			inputSample *= gain;
		}
		
		float nukeLevel = inputSample;
		
		outSample = biquad[2]*inputSample+biquad[3]*biquad[7]+biquad[4]*biquad[8]-biquad[5]*biquad[9]-biquad[6]*biquad[10];
		biquad[8] = biquad[7]; biquad[7] = inputSample; biquad[10] = biquad[9];
		if (outSample > clipThreshold) outSample = clipThreshold;
		if (outSample < -clipThreshold) outSample = -clipThreshold;
		nukeLevel = inputSample = biquad[9] = outSample; //DF1
		
		if (aWet > 0.0f) {
			outSample = biquadA[2]*inputSample+biquadA[3]*biquadA[7]+biquadA[4]*biquadA[8]-biquadA[5]*biquadA[9]-biquadA[6]*biquadA[10];
			biquadA[8] = biquadA[7]; biquadA[7] = inputSample; biquadA[10] = biquadA[9];
			if (outSample > clipThreshold) outSample = clipThreshold;
			if (outSample < -clipThreshold) outSample = -clipThreshold;
			inputSample = biquadA[9] = outSample; //DF1
			nukeLevel = inputSample = (inputSample * aWet) + (nukeLevel * (1.0f-aWet));
		}
		if (bWet > 0.0f) {
			outSample = biquadB[2]*inputSample+biquadB[3]*biquadB[7]+biquadB[4]*biquadB[8]-biquadB[5]*biquadB[9]-biquadB[6]*biquadB[10];
			biquadB[8] = biquadB[7]; biquadB[7] = inputSample; biquadB[10] = biquadB[9]; 
			if (outSample > clipThreshold) outSample = clipThreshold;
			if (outSample < -clipThreshold) outSample = -clipThreshold;
			inputSample = biquadB[9] = outSample; //DF1
			nukeLevel = inputSample = (inputSample * bWet) + (nukeLevel * (1.0f-bWet));
		}
		if (cWet > 0.0f) {
			outSample = biquadC[2]*inputSample+biquadC[3]*biquadC[7]+biquadC[4]*biquadC[8]-biquadC[5]*biquadC[9]-biquadC[6]*biquadC[10];
			biquadC[8] = biquadC[7]; biquadC[7] = inputSample; biquadC[10] = biquadC[9]; 
			if (outSample > clipThreshold) outSample = clipThreshold;
			if (outSample < -clipThreshold) outSample = -clipThreshold;
			inputSample = biquadC[9] = outSample; //DF1
			nukeLevel = inputSample = (inputSample * cWet) + (nukeLevel * (1.0f-cWet));
		}
		if (dWet > 0.0f) {
			outSample = biquadD[2]*inputSample+biquadD[3]*biquadD[7]+biquadD[4]*biquadD[8]-biquadD[5]*biquadD[9]-biquadD[6]*biquadD[10];
			biquadD[8] = biquadD[7]; biquadD[7] = inputSample; biquadD[10] = biquadD[9]; 
			if (outSample > clipThreshold) outSample = clipThreshold;
			if (outSample < -clipThreshold) outSample = -clipThreshold;
			inputSample = biquadD[9] = outSample; //DF1
			nukeLevel = inputSample = (inputSample * dWet) + (nukeLevel * (1.0f-dWet));
		}
		
		if (wet < 1.0f) {
			inputSample = (drySample * (1.0f-wet))+(inputSample * wet);
		}		
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < 11; x++) {biquad[x] = 0.0; biquadA[x] = 0.0; biquadB[x] = 0.0; biquadC[x] = 0.0; biquadD[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
