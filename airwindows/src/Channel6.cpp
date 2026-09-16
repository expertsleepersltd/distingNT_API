#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Channel6"
#define AIRWINDOWS_DESCRIPTION "Uses the Spiral algorithm."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','h','6' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One = 0,
	kParam_Two = 1,
	kParam_Three = 2,
	//Add your parameters here...
	kNumberOfParameters=3
};
static const int kNeve = 1;
static const int kAPI = 2;
static const int kSSL = 3;
static const int kDefaultValue_ParamOne = kNeve;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static char const * const enumStrings0[] = { "", "Neve", "API", "SSL", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Console Type", .min = 1, .max = 3, .def = 1, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
{ .name = "Drive", .min = 0, .max = 10000, .def = 0, .unit = kNT_unitPercent, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		uint32_t fpd;
		float iirSampleA;
		float iirSampleB;
		bool flip;
		float lastSample;
	
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
	int console = (int) GetParameter( kParam_One );
	Float32 density = powf(GetParameter( kParam_Two )/100.0f,2);
	Float32 output = GetParameter( kParam_Three );
	Float32 iirAmount = 0.005832f;
	Float32 threshold = 0.33362176f;	
	switch (console)
	{
		case 1: iirAmount = 0.005832f; threshold = 0.33362176f; break; //Neve
		case 2: iirAmount = 0.004096f; threshold = 0.59969536f; break; //API
		case 3: iirAmount = 0.004913f; threshold = 0.84934656f; break; //SSL
	}
	iirAmount /= overallscale;
	threshold /= overallscale; //now with 96K AND working selector!
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		if (flip)
		{
			iirSampleA = (iirSampleA * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample = inputSample - iirSampleA;
		}
		else
		{
			iirSampleB = (iirSampleB * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample = inputSample - iirSampleB;
		}
		//highpass section
		float drySample = inputSample;
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		inputSample *= 1.2533141373155f;
		//clip to 1.2533141373155f to reach maximum output
		
		float distSample = sin(inputSample * fabs(inputSample)) / ((fabs(inputSample) == 0.0f) ?1:fabs(inputSample));
		inputSample = (drySample*(1-density))+(distSample*density);
		//drive section
		
		Float32 clamp = inputSample - lastSample;
		if (clamp > threshold)
			inputSample = lastSample + threshold;
		if (-clamp > threshold)
			inputSample = lastSample - threshold;
		lastSample = inputSample;
		//slew section
		flip = !flip;
		
		if (output < 1.0f)
		{
			inputSample *= output;
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	flip = false;
	lastSample = 0.0;
}
};
