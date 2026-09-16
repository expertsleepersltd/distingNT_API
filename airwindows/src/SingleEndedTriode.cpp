#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SingleEndedTriode"
#define AIRWINDOWS_DESCRIPTION "Unusual analog modeling effects."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','i','f' )
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
{ .name = "Single-Ended Triode", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Class AB Distortion", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Class B Distortion", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 postsine;
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
	Float32 intensity = powf(GetParameter( kParam_One ),2)*8.0f;
	Float32 triode = intensity;
	intensity +=0.001f;
	Float32 softcrossover = powf(GetParameter( kParam_Two ),3)/8.0f;
	Float32 hardcrossover = powf(GetParameter( kParam_Three ),7)/8.0f;
	Float32 wet = GetParameter( kParam_Four );
	//removed unnecessary dry variable
	
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		Float32 drySample = inputSample;
		
		if (triode > 0.0f)
			{
				inputSample *= intensity;
				inputSample -= 0.5f;
				float bridgerectifier = fabs(inputSample);
				if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
				bridgerectifier = sin(bridgerectifier);
				if (inputSample > 0) inputSample = bridgerectifier;
				else inputSample = -bridgerectifier;
				inputSample += postsine;
				inputSample /= intensity;
			}
		
		if (softcrossover > 0.0f)
			{
				float bridgerectifier = fabs(inputSample);
				if (bridgerectifier > 0.0f) bridgerectifier -= (softcrossover*(bridgerectifier+sqrt(bridgerectifier)));
				if (bridgerectifier < 0.0f) bridgerectifier = 0;
				if (inputSample > 0.0f) inputSample = bridgerectifier;
				else inputSample = -bridgerectifier;				
			}
		
		
		if (hardcrossover > 0.0f)
			{
				float bridgerectifier = fabs(inputSample);
				bridgerectifier -= hardcrossover;
				if (bridgerectifier < 0.0f) bridgerectifier = 0.0f;
				if (inputSample > 0.0f) inputSample = bridgerectifier;
				else inputSample = -bridgerectifier;				
			}
		
		if (wet < 1.0f) {
			inputSample = (drySample * (1.0f-wet))+(inputSample*wet);
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	postsine = sin(0.5);
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
