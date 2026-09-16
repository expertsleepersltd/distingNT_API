#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ButterComp"
#define AIRWINDOWS_DESCRIPTION "My softest, smoothest compressor."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','u','p' )
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
{ .name = "Compress", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		Float32 controlpos;
		Float32 controlneg;
		Float32 targetpos;
		Float32 targetneg;
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
	Float32 overallscale = 2.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	Float32 inputpos;
	Float32 inputneg;
	Float32 calcpos;
	Float32 calcneg;
	Float32 outputpos;
	Float32 outputneg;
	float totalmultiplier;
	float inputSample;
	Float32 drySample;
	Float32 inputgain = powf(10.0f,(GetParameter( kParam_One )*14.0f)/20.0f);
	Float32 wet = GetParameter( kParam_Two );
	//removed unnecessary dry variable
	Float32 outputgain = inputgain;
	outputgain -= 1.0f;
	outputgain /= 1.5f;
	outputgain += 1.0f;
	Float32 divisor = 0.012f * (GetParameter( kParam_One ) / 135.0f);
	divisor /= overallscale;
	Float32 remainder = divisor;
	divisor = 1.0f - divisor;
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;
		
		inputSample *= inputgain;		
		inputpos = inputSample + 1.0f;
		if (inputpos < 0.0f) inputpos = 0.0f;
		outputpos = inputpos / 2.0f;
		if (outputpos > 1.0f) outputpos = 1.0f;		
		inputpos *= inputpos;
		targetpos *= divisor;
		targetpos += (inputpos * remainder);
		calcpos = powf((1.0f/targetpos),2);

		inputneg = (-inputSample) + 1.0f;
		if (inputneg < 0.0f) inputneg = 0.0f;
		outputneg = inputneg / 2.0f;
		if (outputneg > 1.0f) outputneg = 1.0f;		
		inputneg *= inputneg;
		targetneg *= divisor;
		targetneg += (inputneg * remainder);
		calcneg = powf((1.0f/targetneg),2);
		//now we have mirrored targets for comp
		//outputpos and outputneg go from 0 to 1

		//this is an example of a simple C bug one might get.
		//ButterComp2 has this routine the way it was intended, but in the original ButterComp
		//and the reissue (kept exactly the same sonically) there's an if statement that went:
		// if (fpFlip = true) {}
		//That's an assignment operator. So ButterComp Original never interleaved compressors
		//(though it still did the bi-polar thing it was meant to do)
		//and this is simplified code, leaving out the parts that never executed.
		if (inputSample > 0)
			{ //working on pos
				controlpos *= divisor;
				controlpos += (calcpos*remainder);
			}
		else
			{ //working on neg
				controlneg *= divisor;
				controlneg += (calcneg*remainder);
			}
		//this causes each of the four to update only when active and in the correct 'flip'

		totalmultiplier = (controlpos * outputpos) + (controlneg * outputneg);
		//this combines the sides according to flip, blending relative to the input value

		inputSample *= totalmultiplier;
		inputSample /= outputgain;
		if (wet < 1.0f) inputSample = (drySample * (1.0f-wet))+(inputSample*wet);

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	controlpos = 1.0;
	controlneg = 1.0;
	targetpos = 1.0;
	targetneg = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
