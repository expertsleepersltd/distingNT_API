#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Dubly"
#define AIRWINDOWS_DESCRIPTION "A retro vibe and atmosphere maker."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','u','g' )
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
{ .name = "Dubly", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		
		float iirSampleA;
		float iirSampleB;
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
	
	float dublyAmount = powf(GetParameter( kParam_One ),3)*0.16f;
	float outlyAmount = powf(GetParameter( kParam_One ),3)*0.160618f;
	float gain = outlyAmount+1.0f;
	float iirAmount = 0.4275f/overallscale;
	float wet = GetParameter( kParam_Two );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		iirSampleA = (iirSampleA * (1.0f - iirAmount)) + (inputSample * iirAmount);
		float doubly = inputSample - iirSampleA;
		if (doubly > 1.0f) doubly = 1.0f; if (doubly < -1.0f) doubly = -1.0f;
		if (doubly > 0) doubly = log(1.0f+(255*fabs(doubly)))/2.40823996531f;
		if (doubly < 0) doubly = -log(1.0f+(255*fabs(doubly)))/2.40823996531f;
		inputSample += doubly*dublyAmount;
		//end Dubly encode
		
		inputSample *= gain;
		inputSample = sin(inputSample);
		inputSample /= gain;
		
		iirSampleB = (iirSampleB * (1.0f - iirAmount)) + (inputSample * iirAmount);
		doubly = inputSample - iirSampleB;
		if (doubly > 1.0f) doubly = 1.0f; if (doubly < -1.0f) doubly = -1.0f;
		if (doubly > 0) doubly = log(1.0f+(255*fabs(doubly)))/2.40823996531f;
		if (doubly < 0) doubly = -log(1.0f+(255*fabs(doubly)))/2.40823996531f;
		inputSample -= doubly*outlyAmount;
		//end Dubly decode
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirSampleA = 0.0; iirSampleB = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
