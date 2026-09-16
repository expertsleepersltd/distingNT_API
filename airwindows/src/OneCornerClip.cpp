#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "OneCornerClip"
#define AIRWINDOWS_DESCRIPTION "An ultimate full-bandwidth clipper."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','O','n','p' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Input Gain", .min = -12000, .max = 24000, .def = 0, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Pos Threshold", .min = -24000, .max = 0, .def = -300, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Neg Threshold", .min = -24000, .max = 0, .def = -300, .unit = kNT_unitDb, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Voicing", .min = 0, .max = 1000, .def = 618, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;

		Float32 lastSample;
		Float32 limitPos;
		Float32 limitNeg;
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
	
	Float32 inputGain = powf(10.0f,(GetParameter( kParam_One ))/20.0f);
	Float32 posThreshold = powf(10.0f,(GetParameter( kParam_Two ))/20.0f);
	Float32 posTarget = posThreshold;
	Float32 negThreshold = -powf(10.0f,(GetParameter( kParam_Three ))/20.0f);
	Float32 negTarget = negThreshold;
	Float32 voicing = GetParameter( kParam_Four );
	if (voicing == 0.618f) voicing = 0.618033988749894848204586f;
	//special case: we will do a perfect golden ratio as the default 0.618f
	//just 'cos magic universality sauce (seriously, it seems a sweetspot)
	if (overallscale > 0.0f) voicing /= overallscale;
	//translate to desired sample rate, 44.1K is the base
	if (voicing < 0.0f) voicing = 0.0f;
	if (voicing > 1.0f) voicing = 1.0f;
	//some insanity checking
	Float32 inverseHardness = 1.0f - voicing;
	bool clipEngage = false;
	Float32 wet = GetParameter( kParam_Five );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;		

		if (inputGain != 1.0f)
			{
				inputSample *= inputGain;
				clipEngage = true;
				//if we are altering gain we will always process
			}
		else
			{
				clipEngage = false;
				//if we are not touching gain, we will bypass unless
				//a clip is actively being softened.
			}

		
		if (inputSample > posTarget)
			{
				inputSample = (lastSample * voicing) + (posThreshold * inverseHardness);
				posTarget = inputSample;
				clipEngage = true;
			}
		else
			{
				posTarget = posThreshold;
			}

		
		
		if (inputSample < negTarget)
			{
				inputSample = (lastSample * voicing) + (negThreshold * inverseHardness);
				negTarget = inputSample;
				clipEngage = true;
			}
		else {
			negTarget = negThreshold;
		}

		lastSample = inputSample;
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}		
		
		
		
		if (clipEngage == false)
		{
			inputSample = *sourceP;
		}
		//fall back to raw passthrough if at all possible
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSample = 0.0;
	limitPos = 0.0;
	limitNeg = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
