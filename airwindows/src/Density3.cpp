#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Density3"
#define AIRWINDOWS_DESCRIPTION "Density3"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','p' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
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
{ .name = "Density", .min = -1000, .max = 4000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Highpass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float iirSample;
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
	float density = GetParameter( kParam_A )+1.0f;
	float iirAmount = powf(GetParameter( kParam_B ),3)/overallscale;
	if (iirAmount == 0.0f) iirSample = 0.0f;
	float output = GetParameter( kParam_C );
	float wet = GetParameter( kParam_D );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		iirSample = (iirSample * (1.0f - iirAmount)) + (inputSample * iirAmount);
		inputSample -= iirSample;
		
		float altered = inputSample;
		if (density > 1.0f) {
			altered = fmax(fmin(inputSample*density*M_PI_2,M_PI_2),-M_PI_2);
			float X = altered*altered;
			float temp = altered*X;
			altered -= (temp / 6.0f); temp *= X;
			altered += (temp / 120.0f); temp *= X;
			altered -= (temp / 5040.0f); temp *= X;
			altered += (temp / 362880.0f); temp *= X;
			altered -= (temp / 39916800.0f);
		}
		if (density < 1.0f) {
			altered = fmax(fmin(inputSample,1.0f),-1.0f);
			float polarity = altered;
			float X = inputSample * altered;
			float temp = X;
			altered = (temp / 2.0f); temp *= X;
			altered -= (temp / 24.0f); temp *= X;
			altered += (temp / 720.0f); temp *= X;
			altered -= (temp / 40320.0f); temp *= X;
			altered += (temp / 3628800.0f);
			altered *= ((polarity<0.0f)?-1.0f:1.0f);
		}
		if (density > 2.0f) inputSample = altered;
		else inputSample = (inputSample*(1.0f-fabs(density-1.0f)))+(altered*fabs(density-1.0f));
		
		inputSample = (drySample*(1.0f-wet))+(inputSample*output*wet);
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
