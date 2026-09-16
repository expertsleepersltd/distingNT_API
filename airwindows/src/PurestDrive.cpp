#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PurestDrive"
#define AIRWINDOWS_DESCRIPTION "The magic saturation plugin of subtlety and French House tone."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','u','f' )
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
{ .name = "Drive", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float previousSample;
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
	Float32 intensity = GetParameter( kParam_One );
	float inputSample;
	float drySample;
	Float32 apply;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;
		
		inputSample = sin(inputSample);
		//basic distortion factor
		
		apply = (fabs(previousSample + inputSample) / 2.0f) * intensity;
		//saturate less if previous sample was undistorted and low level, or if it was
		//inverse polarity. Lets through highs and brightness more.
		
		inputSample = (drySample * (1.0f - apply)) + (inputSample * apply);		
		//dry-wet control for intensity also has FM modulation to clean up highs
		
		previousSample = sin(drySample);
		//apply the sine while storing previous sample
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
		//increment both of our counters within our input and output buffers.
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	previousSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	
}
};
