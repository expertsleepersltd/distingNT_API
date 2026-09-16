#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Console8ChannelOut"
#define AIRWINDOWS_DESCRIPTION "Moves to a channel/submix/buss topology and adds analog simulation."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','#' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Fader", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float inTrimA;
		float inTrimB;
		bool hsr;
		enum {
			fix_freq,
			fix_reso,
			fix_a0,
			fix_a1,
			fix_a2,
			fix_b1,
			fix_b2,
			fix_sL1,
			fix_sL2,
			fix_sR1,
			fix_sR2,
			fix_total
		}; //fixed frequency biquad filter for ultrasonics, stereo
		uint32_t fpd;
	
	struct _dram {
			float fix[fix_total];
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
	
	inTrimA = inTrimB; inTrimB = GetParameter( kParam_One )*2.0f;
	//0.5f is unity gain, and we can attenuate to silence or boost slightly over 12dB
	//into softclipping overdrive.
	if (GetSampleRate() > 49000.0f) hsr = true; else hsr = false;
	dram->fix[fix_freq] = 24000.0f / GetSampleRate();
	dram->fix[fix_reso] = 3.51333709f;
	float K = tan(M_PI * dram->fix[fix_freq]); //lowpass
	float norm = 1.0f / (1.0f + K / dram->fix[fix_reso] + K * K);
	dram->fix[fix_a0] = K * K * norm;
	dram->fix[fix_a1] = 2.0f * dram->fix[fix_a0];
	dram->fix[fix_a2] = dram->fix[fix_a0];
	dram->fix[fix_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->fix[fix_b2] = (1.0f - K / dram->fix[fix_reso] + K * K) * norm;
	//this is the fixed biquad distributed anti-aliasing filter
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		
		float position = (float)nSampleFrames/inFramesToProcess;
		float inTrim = (inTrimA*position)+(inTrimB*(1.0f-position));
		//input trim smoothed to cut out zipper noise
		inputSample *= inTrim;
		if (inputSample > 1.57079633f) inputSample = 1.57079633f; if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		//Console8 gain stage clips at exactly 1.0f post-sin()
		if (hsr){
			float outSample = (inputSample * dram->fix[fix_a0]) + dram->fix[fix_sL1];
			dram->fix[fix_sL1] = (inputSample * dram->fix[fix_a1]) - (outSample * dram->fix[fix_b1]) + dram->fix[fix_sL2];
			dram->fix[fix_sL2] = (inputSample * dram->fix[fix_a2]) - (outSample * dram->fix[fix_b2]);
			inputSample = outSample;
		} //fixed biquad filtering ultrasonics
		inputSample *= inTrim;
		if (inputSample > 1.57079633f) inputSample = 1.57079633f; if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		//Console8 gain stage clips at exactly 1.0f post-sin()
		
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	inTrimA = 0.5; inTrimB = 0.5;
	for (int x = 0; x < fix_total; x++) dram->fix[x] = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
