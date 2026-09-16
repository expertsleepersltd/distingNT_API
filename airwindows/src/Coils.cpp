#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Coils"
#define AIRWINDOWS_DESCRIPTION "Models the types of distortion you'll find in transformers."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','s' )
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
{ .name = "Saturate", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Core DC", .min = -1000, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float figure[9];
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
	
	//[0] is frequency: 0.000001f to 0.499999f is near-zero to near-Nyquist
	//[1] is resonance, 0.7071f is Butterworth. Also can't be zero
	Float32 boost = 1.0f-powf(GetParameter( kParam_One ),2);
	if (boost < 0.001f) boost = 0.001f; //there's a divide, we can't have this be zero
	figure[0] = 600.0f/GetSampleRate(); //fixed frequency, 600hz
	figure[1] = 0.023f; //resonance
	Float32 offset = GetParameter( kParam_Two );
	Float32 sinOffset = sin(offset); //we can cache this, it's expensive
	Float32 wet = GetParameter( kParam_Three );
	
	
	float K = tan(M_PI * figure[0]);
	float norm = 1.0f / (1.0f + K / figure[1] + K * K);
	figure[2] = K / figure[1] * norm;
	figure[4] = -figure[2];
	figure[5] = 2.0f * (K * K - 1.0f) * norm;
	figure[6] = (1.0f - K / figure[1] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		//float tempSample = (inputSample * figure[2]) + figure[7];
		//figure[7] = -(tempSample * figure[5]) + figure[8];
		//figure[8] = (inputSample * figure[4]) - (tempSample * figure[6]);
		//inputSample = tempSample + sin(drySample-tempSample);
		//or
		//inputSample = tempSample + ((sin(((drySample-tempSample)/boost)+offset)-sinOffset)*boost);
		//
		//given a bandlimited inputSample, freq 600hz and Q of 0.023f, this restores a lot of
		//the full frequencies but distorts like a real transformer. Purest case, and since
		//we are not using a high Q we can remove the extra sin/asin on the biquad.
		
		float tempSample = (inputSample * figure[2]) + figure[7];
		figure[7] = -(tempSample * figure[5]) + figure[8];
		figure[8] = (inputSample * figure[4]) - (tempSample * figure[6]);
		inputSample = tempSample + ((sin(((drySample-tempSample)/boost)+offset)-sinOffset)*boost);
		//given a bandlimited inputSample, freq 600hz and Q of 0.023f, this restores a lot of
		//the full frequencies but distorts like a real transformer. Since
		//we are not using a high Q we can remove the extra sin/asin on the biquad.
		
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
	for (int x = 0; x < 9; x++) {figure[x] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
