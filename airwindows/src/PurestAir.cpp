#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PurestAir"
#define AIRWINDOWS_DESCRIPTION "A brightness experiment."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','u','s' )
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
{ .name = "Air Boost", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Air Limit", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float last1Sample;
		float halfwaySample;
		float halfDrySample;
		float halfDiffSample;
		float diffSample;
		float lastSample;
		Float32 s1;
		Float32 s2;
		Float32 s3;
		Float32 apply;
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

	Float32 applyTarget = GetParameter( kParam_One );
	Float32 threshold = powf((1-fabs(applyTarget)),3);
	if (applyTarget > 0) applyTarget *= 3;
	
	Float32 intensity = powf(GetParameter( kParam_Two ),2)*5.0f;
	Float32 wet = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		halfDrySample = halfwaySample = (inputSample + last1Sample) / 2.0f;
		last1Sample = inputSample;
		s3 = s2;
		s2 = s1;
		s1 = inputSample;
		Float32 m1 = (s1-s2)*((s1-s2)/1.3f);
		Float32 m2 = (s2-s3)*((s1-s2)/1.3f);
		Float32 sense = fabs((m1-m2)*((m1-m2)/1.3f))*intensity;
		//this will be 0 for smooth, high for SSS
		apply += applyTarget - sense;
		apply *= 0.5f;
		if (apply < -1.0f) apply = -1.0f;
		
		Float32 clamp = halfwaySample - halfDrySample;
		if (clamp > threshold) halfwaySample = lastSample + threshold;
		if (-clamp > threshold) halfwaySample = lastSample - threshold;
		lastSample = halfwaySample;
		
		clamp = inputSample - lastSample;
		if (clamp > threshold) inputSample = lastSample + threshold;
		if (-clamp > threshold) inputSample = lastSample - threshold;
		lastSample = inputSample;
		
		diffSample = *sourceP - inputSample;
		halfDiffSample = halfDrySample - halfwaySample;
		
		inputSample = *sourceP + ((diffSample + halfDiffSample)*apply);
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	last1Sample = 0.0;
	halfwaySample = halfDrySample = halfDiffSample = diffSample = 0.0;
	lastSample = 0.0;
	s1 = s2 = s3 = 0.0;
	apply = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
