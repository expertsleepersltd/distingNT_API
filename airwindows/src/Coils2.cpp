#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Coils2"
#define AIRWINDOWS_DESCRIPTION "A transformer overdrive emulator."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','2' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kNumberOfParameters=3
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Saturation", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Cheapness", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float biquadA[9];
		float biquadB[9];
		float hysteresis;
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
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	Float32 distScaling = powf(1.0f-GetParameter( kParam_One ),2);
	if (distScaling < 0.0001f) distScaling = 0.0001f;
	biquadA[0] = 600.0f/GetSampleRate();
	biquadA[1] = 0.01f+(powf(GetParameter( kParam_Two ),2)*0.5f);
	float iirAmount = biquadA[1]/overallscale;
	float K = tan(M_PI * biquadA[0]);
	float norm = 1.0f / (1.0f + K / biquadA[1] + K * K);
	biquadA[2] = K / biquadA[1] * norm;
	biquadA[4] = -biquadA[2];
	biquadA[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadA[6] = (1.0f - K / biquadA[1] + K * K) * norm;
	biquadB[0] = (21890.0f-(biquadA[1]*890.0f))/GetSampleRate();
	biquadB[1] = 0.89f;
	K = tan(M_PI * biquadB[0]);
	norm = 1.0f / (1.0f + K / biquadB[1] + K * K);
	biquadB[2] = K * K * norm;
	biquadB[3] = 2.0f * biquadB[2];
	biquadB[4] = biquadB[2];
	biquadB[5] = 2.0f * (K * K - 1.0f) * norm;
	biquadB[6] = (1.0f - K / biquadB[1] + K * K) * norm;
	Float32 wet = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		if (biquadA[0] < 0.49999f) {
			float tempSample = (inputSample * biquadA[2]) + biquadA[7];
			biquadA[7] = -(tempSample * biquadA[5]) + biquadA[8];
			biquadA[8] = (inputSample * biquadA[4]) - (tempSample * biquadA[6]);
			inputSample = tempSample; //create bandpass of clean tone
		}
		float diffSample = (drySample-inputSample)/distScaling; //mids notched out		
		if (biquadB[0] < 0.49999f) {
			float tempSample = (diffSample * biquadB[2]) + biquadB[7];
			biquadB[7] = (diffSample * biquadB[3]) - (tempSample * biquadB[5]) + biquadB[8];
			biquadB[8] = (diffSample * biquadB[4]) - (tempSample * biquadB[6]);
			diffSample = tempSample; //lowpass filter the notch content before distorting
		}
		hysteresis = (hysteresis * (1.0f-iirAmount)) + (diffSample * iirAmount);
		if (fabs(hysteresis)<1.18e-37f) hysteresis = 0.0f; else diffSample -= hysteresis;
		if (diffSample > 1.571f) diffSample = 1.571f; else if (diffSample < -1.571f) diffSample = -1.571f;
		if (hysteresis > 1.571f) hysteresis = 1.571f; else if (hysteresis < -1.571f) hysteresis = -1.571f;
		inputSample += (sin(diffSample)-sin(hysteresis))*distScaling; //apply transformer distortions
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < 9; x++) {biquadA[x] = 0.0; biquadB[x] = 0.0;}
	hysteresis = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
