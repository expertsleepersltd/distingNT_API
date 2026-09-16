#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Weight"
#define AIRWINDOWS_DESCRIPTION "A very accurate sub-bass boost based on Holt."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','W','e','t' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
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
{ .name = "Frequency", .min = 2000, .max = 12000, .def = 6000, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Weight", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		float previousSample[9];
		float previousTrend[9];
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
	
	Float32 targetFreq = (GetParameter( kParam_One )-20.0f)/100.0f;
	//gives us a 0-1 value like the VST will be. For the VST, start with 0-1 and
	//have the plugin display the number as 20-120.
	
	targetFreq = ((targetFreq+0.53f)*0.2f)/sqrt(overallscale);
	//must use square root of what the real scale would be, to get correct output
	Float32 alpha = powf(targetFreq,4);
	Float32 out = GetParameter( kParam_Two );
	Float32 resControl = (out*0.05f)+0.2f;
	Float32 beta = (alpha * powf(resControl,2));	
	alpha += ((1.0f-beta)*powf(targetFreq,3)); //correct for droop in frequency
		
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		//begin Weight
		float trend;
		float forecast;
		for (int i = 0; i < 8; i++) {
			trend = (beta * (inputSample - previousSample[i]) + ((0.999f-beta) * previousTrend[i]));
			forecast = previousSample[i] + previousTrend[i];
			inputSample = (alpha * inputSample) + ((0.999f-alpha) * forecast);
			previousSample[i] = inputSample; previousTrend[i] = trend;
		}
		//inputSample is now the bass boost to be added
		
		inputSample *= out;
		inputSample += drySample;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	
	for (int i = 0; i < 8; i++) {
		previousSample[i] = 0.0;
		previousTrend[i] = 0.0;
	}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
