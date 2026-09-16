#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "HighImpact"
#define AIRWINDOWS_DESCRIPTION "Distorted grit and punch without fatness. This is also another little window into Airwindows history, as this is the point where I began putting out the for-pay plugins for free through Patreon. The post is presented in its entirety as a picture of where I was at on April 9, 2017. By now I have put out more than 300 plugins and I'm still going, but this was when I'd done 47 and all the most sought-after ones were still being held back as goals… here is the original post."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','i','t' )
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
{ .name = "Impact", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Level", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 lastSample;
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
	Float32 density = GetParameter( kParam_One )*5.0f;
	Float32 out = density / 5.0f;
	Float32 sustain = 1.0f - (1.0f/(1.0f + (density*GetParameter( kParam_One ))));
	Float32 bridgerectifier;
	Float32 count;
	Float32 inputSample;
	Float32 drySample;
	Float32 output = GetParameter( kParam_Two );
	Float32 wet = GetParameter( kParam_Three );
	Float32 dry = 1.0f-wet;	
	Float32 clamp;
	Float32 threshold = (1.25f - out);
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;
	
		
		count = density;
		while (count > 1.0f)
			{
				bridgerectifier = fabs(inputSample)*1.57079633f;
				if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
				//max value for sine function
				bridgerectifier = sin(bridgerectifier);
				if (inputSample > 0.0f) inputSample = bridgerectifier;
				else inputSample = -bridgerectifier;
				count = count - 1.0f;
			}
		//we have now accounted for any really high density settings.
		while (out > 1.0f) out = out - 1.0f;
		bridgerectifier = fabs(inputSample)*1.57079633f;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		if (density > 0) bridgerectifier = sin(bridgerectifier);
		else bridgerectifier = 1-cos(bridgerectifier);
		//produce either boosted or starved version
		if (inputSample > 0) inputSample = (inputSample*(1-out))+(bridgerectifier*out);
		else inputSample = (inputSample*(1-out))-(bridgerectifier*out);
		//blend according to density control
		//done first density. Next, sustain-reducer
		bridgerectifier = fabs(inputSample)*1.57079633f;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		bridgerectifier = 1-cos(bridgerectifier);
		if (inputSample > 0) inputSample = (inputSample*(1-sustain))+(bridgerectifier*sustain);
		else inputSample = (inputSample*(1-sustain))-(bridgerectifier*sustain);
		//done sustain removing, converted to Slew inputs
		clamp = inputSample - lastSample;
		if (clamp > threshold)
			inputSample = lastSample + threshold;
		if (-clamp > threshold)
			inputSample = lastSample - threshold;
		lastSample = inputSample;

		if (output < 1.0f) inputSample *= output;
		if (wet < 1.0f) inputSample = (drySample * dry)+(inputSample*wet);
		//nice little output stage template: if we have another scale of floating point
		//number, we really don't want to meaninglessly multiply that by 1.0f.
		
		
		
		*destP = inputSample;
		sourceP += inNumChannels;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
