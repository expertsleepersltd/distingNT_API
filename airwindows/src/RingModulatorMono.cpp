#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "RingModulatorMono"
#define AIRWINDOWS_DESCRIPTION "RingModulatorMono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','R','i','p' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
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
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Soar", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		float sinePos;
		float incA;
		float incB;
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

	incA = incB; incB = powf(GetParameter( kParam_A ),5)/overallscale;
	float soar = 0.3f-(GetParameter( kParam_B )*0.3f);
	float wet = powf(GetParameter( kParam_C ),2);
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		float temp = (float)nSampleFrames/inFramesToProcess;
		float inc = (incA*temp)+(incB*(1.0f-temp));

		sinePos += inc;
		if (sinePos > 6.283185307179586f) sinePos -= 6.283185307179586f;
		float sinResult = sin(sinePos);
		float out = 0.0f;
		float snM = fabs(sinResult)+(soar*soar);
		float inM = fabs(inputSample);
		if (inM < snM) {
			inM = fabs(sinResult);
			snM = fabs(inputSample)+(soar*soar);
		}
		
		if (inputSample > 0.0f && sinResult > 0.0f) out = fmax((sqrt((fabs(inM)/snM))*snM)-soar,0.0f);
		if (inputSample < 0.0f && sinResult > 0.0f) out = fmin((-sqrt((fabs(inM)/snM))*snM)+soar,0.0f);
		if (inputSample > 0.0f && sinResult < 0.0f) out = fmin((-sqrt((fabs(inM)/snM))*snM)+soar,0.0f);
		if (inputSample < 0.0f && sinResult < 0.0f) out = fmax((sqrt((fabs(inM)/snM))*snM)-soar,0.0f);
		inputSample = out;

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
	sinePos = 0.0;
	incA = 0.0; incB = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
