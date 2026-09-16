#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "YNotNotch"
#define AIRWINDOWS_DESCRIPTION "Soft and smooth to nasty, edgy texture-varying filtering, no control smoothing."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','Y','N','h' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Gain", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Reson8", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "ResEdge", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		
		enum {
			biq_freq,
			biq_reso,
			biq_a0,
			biq_a1,
			biq_a2,
			biq_b1,
			biq_b2,
			biq_sL1,
			biq_sL2,
			biq_sR1,
			biq_sR2,
			biq_total
		}; //coefficient interpolating biquad filter, stereo
		
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
			float biquad[biq_total];
		float fixA[fix_total];
		float fixB[fix_total];
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
	
	float inTrim = GetParameter( kParam_One )*10.0f;
	
	dram->biquad[biq_freq] = powf(GetParameter( kParam_Two ),3)*20000.0f;
	if (dram->biquad[biq_freq] < 15.0f) dram->biquad[biq_freq] = 15.0f;
	dram->biquad[biq_freq] /= GetSampleRate();
    dram->biquad[biq_reso] = (powf(GetParameter( kParam_Three ),2)*15.0f)+0.0001f;
	float K = tan(M_PI * dram->biquad[biq_freq]);
	float norm = 1.0f / (1.0f + K / dram->biquad[biq_reso] + K * K);
	dram->biquad[biq_a0] = (1.0f + K * K) * norm;
	dram->biquad[biq_a1] = 2.0f * (K * K - 1) * norm;
	dram->biquad[biq_a2] = dram->biquad[biq_a0];
	dram->biquad[biq_b1] = dram->biquad[biq_a1];
	dram->biquad[biq_b2] = (1.0f - K / dram->biquad[biq_reso] + K * K) * norm;
	//for the coefficient-interpolated biquad filter
	
	float powFactor = powf(GetParameter( kParam_Four )+0.9f,4);
	
	//1.0f == target neutral
	
	float outTrim = GetParameter( kParam_Five );
	
	float wet = GetParameter( kParam_Six );
	
	dram->fixA[fix_freq] = dram->fixB[fix_freq] = 20000.0f / GetSampleRate();
    dram->fixA[fix_reso] = dram->fixB[fix_reso] = 0.7071f; //butterworth Q
	
	K = tan(M_PI * dram->fixA[fix_freq]);
	norm = 1.0f / (1.0f + K / dram->fixA[fix_reso] + K * K);
	dram->fixA[fix_a0] = dram->fixB[fix_a0] = K * K * norm;
	dram->fixA[fix_a1] = dram->fixB[fix_a1] = 2.0f * dram->fixA[fix_a0];
	dram->fixA[fix_a2] = dram->fixB[fix_a2] = dram->fixA[fix_a0];
	dram->fixA[fix_b1] = dram->fixB[fix_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->fixA[fix_b2] = dram->fixB[fix_b2] = (1.0f - K / dram->fixA[fix_reso] + K * K) * norm;
	//for the fixed-position biquad filter
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = *sourceP;
		
		inputSample *= inTrim;
		
		float temp = (inputSample * dram->fixA[fix_a0]) + dram->fixA[fix_sL1];
		dram->fixA[fix_sL1] = (inputSample * dram->fixA[fix_a1]) - (temp * dram->fixA[fix_b1]) + dram->fixA[fix_sL2];
		dram->fixA[fix_sL2] = (inputSample * dram->fixA[fix_a2]) - (temp * dram->fixA[fix_b2]);
		inputSample = temp; //fixed biquad filtering ultrasonics
		
		//encode/decode courtesy of torridgristle under the MIT license
		if (inputSample > 1.0f) inputSample = 1.0f;
		else if (inputSample > 0.0f) inputSample = 1.0f - powf(1.0f-inputSample,powFactor);
		if (inputSample < -1.0f) inputSample = -1.0f;
		else if (inputSample < 0.0f) inputSample = -1.0f + powf(1.0f+inputSample,powFactor);
		
		temp = (inputSample * dram->biquad[biq_a0]) + dram->biquad[biq_sL1];
		dram->biquad[biq_sL1] =  (inputSample * dram->biquad[biq_a1]) - (temp * dram->biquad[biq_b1]) + dram->biquad[biq_sL2];
		dram->biquad[biq_sL2] = (inputSample * dram->biquad[biq_a2]) - (temp * dram->biquad[biq_b2]);
		inputSample = temp; //coefficient interpolating biquad filter
		
		//encode/decode courtesy of torridgristle under the MIT license
		if (inputSample > 1.0f) inputSample = 1.0f;
		else if (inputSample > 0.0f) inputSample = 1.0f - powf(1.0f-inputSample,(1.0f/powFactor));
		if (inputSample < -1.0f) inputSample = -1.0f;
		else if (inputSample < 0.0f) inputSample = -1.0f + powf(1.0f+inputSample,(1.0f/powFactor));
		
		inputSample *= outTrim;
		
		temp = (inputSample * dram->fixB[fix_a0]) + dram->fixB[fix_sL1];
		dram->fixB[fix_sL1] = (inputSample * dram->fixB[fix_a1]) - (temp * dram->fixB[fix_b1]) + dram->fixB[fix_sL2];
		dram->fixB[fix_sL2] = (inputSample * dram->fixB[fix_a2]) - (temp * dram->fixB[fix_b2]);
		inputSample = temp; //fixed biquad filtering ultrasonics
		
		if (wet < 1.0f) {
			inputSample = (inputSample*wet) + (drySample*(1.0f-wet));
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < biq_total; x++) {dram->biquad[x] = 0.0;}
	for (int x = 0; x < fix_total; x++) {dram->fixA[x] = 0.0; dram->fixB[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
