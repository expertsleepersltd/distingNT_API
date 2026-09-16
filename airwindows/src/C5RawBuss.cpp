#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "C5RawBuss"
#define AIRWINDOWS_DESCRIPTION "The original Console5 algorithm, with optional very gentle DC suppression."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','5','s' )
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
{ .name = "Centering Force", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 lastFXBuss;
		Float32 lastSampleBuss;
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

	float centering = GetParameter( kParam_One ) * 0.5f;
	centering = 1.0f - powf(centering,5);
	//we can set our centering force from zero to rather high, but
	//there's a really intense taper on it forcing it to mostly be almost 1.0f.
	//If it's literally 1.0f, we don't even apply it, and you get the original
	//Xmas Morning bugged-out Console5, which is the default setting for Raw Console5.
	Float32 difference;

	float inputSample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;

		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		inputSample = asin(inputSample);
		//amplitude aspect
		
		difference = lastSampleBuss - inputSample;
		lastSampleBuss = inputSample;
		//derive slew part off direct sample measurement + from last time
		if (difference > 1.57079633f) difference = 1.57079633f;
		if (difference < -1.57079633f) difference = -1.57079633f;
		inputSample = lastFXBuss + sin(difference);
		lastFXBuss = inputSample;
		if (centering < 1.0f) lastFXBuss *= centering;
		//if we're using the crude centering force, it's applied here
		if (lastFXBuss > 1.0f) lastFXBuss = 1.0f;
		if (lastFXBuss < -1.0f) lastFXBuss = -1.0f;
		//build new signal off what was present in output last time
		//slew aspect
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastFXBuss = 0.0;
	lastSampleBuss = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
