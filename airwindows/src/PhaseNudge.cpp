#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PhaseNudge"
#define AIRWINDOWS_DESCRIPTION "A phase rotator/allpass filter."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','h','e' )
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
{ .name = "Phase Nudge", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		int one, maxdelay;
		uint32_t fpd;
	
	struct _dram {
			Float32 d[1503];
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
	
	int allpasstemp;
	Float32 outallpass = 0.618033988749894848204586f; //golden ratio!
	//if you see 0.6180f it's not a wild stretch to wonder whether you are working with a constant
	int maxdelayTarget = (int)(powf(GetParameter( kParam_One ),3)*1501.0f);
	Float32 wet = GetParameter( kParam_Two );
	//removed unnecessary dry variable
	Float32 bridgerectifier;
	
	float inputSample;
	float drySample;
	
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;
		
		inputSample /= 4.0f;
		bridgerectifier = fabs(inputSample);
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0) inputSample = bridgerectifier;
		else inputSample = -bridgerectifier;

		if (fabs(maxdelay - maxdelayTarget) > 1500) maxdelay = maxdelayTarget;
		if (maxdelay < maxdelayTarget)
		{maxdelay++; dram->d[maxdelay] = (dram->d[0]+dram->d[maxdelay-1]) / 2.0f;}
		
		if (maxdelay > maxdelayTarget)
		{maxdelay--; dram->d[maxdelay] = (dram->d[0]+dram->d[maxdelay]) / 2.0f;}
		
		allpasstemp = one - 1;
		if (allpasstemp < 0 || allpasstemp > maxdelay) {allpasstemp = maxdelay;}
		inputSample -= dram->d[allpasstemp]*outallpass;
		dram->d[one] = inputSample;
		inputSample *= outallpass;
		one--; if (one < 0 || one > maxdelay) {one = maxdelay;}
		inputSample += (dram->d[one]);
		
		bridgerectifier = fabs(inputSample);
		bridgerectifier = 1.0f-cos(bridgerectifier);
		if (inputSample > 0) inputSample -= bridgerectifier;
		else inputSample += bridgerectifier;
		inputSample *= 4.0f;
		if (wet < 1.0f) inputSample = (drySample * (1.0f-wet))+(inputSample * wet);
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for(int count = 0; count < 1502; count++) {dram->d[count] = 0.0;}
	one = 1; maxdelay = 9001;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	//coded to snap directly to the value on instantiation if the disparity is great enough
}
};
