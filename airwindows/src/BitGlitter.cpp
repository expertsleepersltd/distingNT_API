#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BitGlitter"
#define AIRWINDOWS_DESCRIPTION "An old-sampler style digital lo-fi plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','#' )
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
{ .name = "Input Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bit Glitter", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Trim", .min = -18000, .max = 18000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 ataLastSample;
		Float32 ataHalfwaySample;
		Float32 ataDrySample;
		Float32 lastSample;
		Float32 heldSampleA;
		Float32 heldSampleB;
		Float32 positionA;
		Float32 positionB;
		Float32 lastOutputSample;
	
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
	Float32 factor = GetParameter( kParam_Two )+1.0f;
	factor = powf(factor,7)+2.0f;
	int divvy = (int)(factor*overallscale);
	Float32 rateA = 1.0f / divvy;
	Float32 rezA = 0.0016666666666667f; //looks to be a fixed bitcrush
	Float32 rateB = 1.61803398875f / divvy;
	Float32 rezB = 0.0026666666666667f; //looks to be a fixed bitcrush
	Float32 offset;
	Float32 ingain = powf(10.0f,GetParameter( kParam_One )/14.0f); //add adjustment factor
	Float32 outgain = powf(10.0f,GetParameter( kParam_Three )/14.0f); //add adjustment factor
	Float32 wet = GetParameter( kParam_Four );

	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		float drySample = inputSample;
		//first, the distortion section
		inputSample *= ingain;
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		inputSample *= 1.2533141373155f;
		//clip to 1.2533141373155f to reach maximum output
		inputSample = sin(inputSample * fabs(inputSample)) / ((fabs(inputSample) == 0.0f) ?1:fabs(inputSample));
		
		ataDrySample = inputSample;
		ataHalfwaySample = (inputSample + ataLastSample ) / 2.0f;
		ataLastSample = inputSample;
		//setting up crude oversampling
		
		//begin raw sample
		positionA += rateA;
		float outputSample = heldSampleA;
		if (positionA > 1.0f)
		{
			positionA -= 1.0f;
			heldSampleA = (lastSample * positionA) + (inputSample * (1-positionA));
			outputSample = (outputSample * 0.5f) + (heldSampleA * 0.5f);
			//softens the edge of the derez
		}
		if (outputSample > 0)
		{
			offset = outputSample;
			while (offset > 0) {offset -= rezA;}
			outputSample -= offset;
			//it's below 0 so subtracting adds the remainder
		}
		if (outputSample < 0)
		{
			offset = outputSample;
			while (offset < 0) {offset += rezA;}
			outputSample -= offset;
			//it's above 0 so subtracting subtracts the remainder
		}
		outputSample *= (1.0f - rezA);
		if (fabs(outputSample) < rezA) outputSample = 0.0f;
		inputSample = outputSample;
		//end raw sample
		
		//begin interpolated sample
		positionB += rateB;
		outputSample = heldSampleB;
		if (positionB > 1.0f)
		{
			positionB -= 1.0f;
			heldSampleB = (lastSample * positionB) + (ataHalfwaySample * (1-positionB));
			outputSample = (outputSample * 0.5f) + (heldSampleB * 0.5f);
			//softens the edge of the derez
		}
		if (outputSample > 0)
		{
			offset = outputSample;
			while (offset > 0) {offset -= rezB;}
			outputSample -= offset;
			//it's below 0 so subtracting adds the remainder
		}
		if (outputSample < 0)
		{
			offset = outputSample;
			while (offset < 0) {offset += rezB;}
			outputSample -= offset;
			//it's above 0 so subtracting subtracts the remainder
		}
		outputSample *= (1.0f - rezB);
		if (fabs(outputSample) < rezB) outputSample = 0.0f;
		ataHalfwaySample = outputSample;
		//end interpolated sample
		
		inputSample += ataHalfwaySample;
		inputSample /= 2.0f;
		//plain old blend the two
		
		outputSample = (inputSample * (1.0f-(wet/2))) + (lastOutputSample*(wet/2));
		//darken to extent of wet in wet/dry, maximum 50%
		lastOutputSample = inputSample;
		outputSample *= outgain;
		
		if (wet < 1.0f) {
			outputSample = (drySample * (1.0f-wet)) + (outputSample * wet);
		}
		
		*destP = outputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	ataLastSample = 0.0;
	ataHalfwaySample = 0.0;
	lastSample = 0.0;
	heldSampleA = 0.0;
	positionA = 0.0;
	heldSampleB = 0.0;
	positionB = 0.0;
	lastOutputSample = 0.0;
}
};
