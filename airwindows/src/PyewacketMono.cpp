#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PyewacketMono"
#define AIRWINDOWS_DESCRIPTION "PyewacketMono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','y','o' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
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
{ .name = "Input Gain", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Gain", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		uint32_t fpd;
		Float32 chase;
		Float32 lastrectifier;
	
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
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	if (overallscale < 0.1f) overallscale = 1.0f;
	//insanity check
	float fpOld = 0.618033988749894848204586f; //golden ratio!
	float fpNew = 1.0f - fpOld;
	float inputSample;
	float drySample;
	Float32 bridgerectifier;
	Float32 temprectifier;
	Float32 inputSense;
	
	Float32 inputGain = powf(10.0f,(GetParameter( kParam_One ))/20.0f);
	Float32 attack = ((GetParameter( kParam_Two )+0.5f)*0.006f)/overallscale;
	Float32 decay = ((GetParameter( kParam_Two )+0.01f)*0.0004f)/overallscale;
	Float32 outputGain = powf(10.0f,(GetParameter( kParam_Three ))/20.0f);
	Float32 wet;
	Float32 maxblur;
	Float32 blurdry;
	Float32 out;
	Float32 dry;
	
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		
		
		if (inputGain != 1.0f) {
			inputSample *= inputGain;
		}
		drySample = inputSample;
		inputSense = fabs(inputSample);
		
		if (chase < inputSense) chase += attack;
		if (chase > 1.0f) chase = 1.0f;
		if (chase > inputSense) chase -= decay;
		if (chase < 0.0f) chase = 0.0f;
		//chase will be between 0 and ? (if input is super hot)
		out = wet = chase;
		if (wet > 1.0f) wet = 1.0f;
		maxblur = wet * fpNew;
		blurdry = 1.0f - maxblur;
		//scaled back so that blur remains balance of both
		if (out > fpOld) out = fpOld - (out - fpOld);
		if (out < 0.0f) out = 0.0f;
		dry = 1.0f - wet;

		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		temprectifier = 1-cos(bridgerectifier);
		bridgerectifier = ((lastrectifier*maxblur) + (temprectifier*blurdry));
		lastrectifier = temprectifier;
		//starved version is also blurred by one sample
		if (inputSample > 0) inputSample = (inputSample*dry)+(bridgerectifier*out);
		else inputSample = (inputSample*dry)-(bridgerectifier*out);
		
		if (outputGain != 1.0f) {
			inputSample *= outputGain;
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	chase = 1.0;
	lastrectifier = 0.0;
}
};
