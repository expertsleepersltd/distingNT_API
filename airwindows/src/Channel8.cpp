#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Channel8"
#define AIRWINDOWS_DESCRIPTION "Channel7 with updates from Capacitor2 and Slew3. More and better of what Channel is."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','h','8' )
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
{ .name = "Drive", .min = 0, .max = 20000, .def = 0, .unit = kNT_unitPercent, .scaling = kNT_scaling100, .enumStrings = NULL },
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
		float lastSampleA;
		float lastSampleB;
		float lastSampleC;
	
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
	Float32 density = GetParameter( kParam_Two )/100.0f; //0-2
	Float32 phattity = density - 1.0f;
	if (density > 1.0f) density = 1.0f; //max out at full wet for Spiral aspect
	if (phattity < 0.0f) phattity = 0.0f; //
	Float32 nonLin = 5.0f-density; //number is smaller for more intense, larger for more subtle
	Float32 output = GetParameter( kParam_Three );
	Float32 iirAmount = 0.005832f;
	Float32 threshold = 0.33362176f;	
	switch (console)
	{
		case 1: iirAmount = 0.005832f; threshold = 0.33362176f; break; //Neve
		case 2: iirAmount = 0.004096f; threshold = 0.59969536f; break; //API
		case 3: iirAmount = 0.004913f; threshold = 0.84934656f; break; //SSL
	}
	iirAmount /= overallscale; //we've learned not to try and adjust threshold for sample rate
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		Float32 dielectricScale = fabs(2.0f-((inputSample+nonLin)/nonLin));
		
		if (flip)
		{
			iirSampleA = (iirSampleA * (1.0f - (iirAmount * dielectricScale))) + (inputSample * iirAmount * dielectricScale);
			inputSample = inputSample - iirSampleA;
		}
		else
		{
			iirSampleB = (iirSampleB * (1.0f - (iirAmount * dielectricScale))) + (inputSample * iirAmount * dielectricScale);
			inputSample = inputSample - iirSampleB;
		}
		//highpass section including capacitor modeling nonlinearity
		float drySample = inputSample;
		
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		
		float phatSample = sin(inputSample * 1.57079633f);
		inputSample *= 1.2533141373155f;
		//clip to 1.2533141373155f to reach maximum output, or 1.57079633f for pure sine 'phat' version
		float distSample = sin(inputSample * fabs(inputSample)) / ((fabs(inputSample) == 0.0f) ?1:fabs(inputSample));
		
		inputSample = distSample; //purest form is full Spiral
		if (density < 1.0f) inputSample = (drySample*(1-density))+(distSample*density); //fade Spiral aspect
		if (phattity > 0.0f) inputSample = (inputSample*(1-phattity))+(phatSample*phattity); //apply original Density on top
		
		Float32 clamp = (lastSampleB - lastSampleC) * 0.381966011250105f;
		clamp -= (lastSampleA - lastSampleB) * 0.6180339887498948482045f;
		clamp += inputSample - lastSampleA; //regular slew clamping added

		lastSampleC = lastSampleB;
		lastSampleB = lastSampleA;
		lastSampleA = inputSample; //now our output relates off lastSampleB
		
		if (clamp > threshold)
			inputSample = lastSampleB + threshold;
		if (-clamp > threshold)
			inputSample = lastSampleB - threshold;
		//slew section		
		lastSampleA = (lastSampleA*0.381966011250105f)+(inputSample*0.6180339887498948482045f); //split the difference between raw and smoothed for buffer
				
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
	iirSampleA = iirSampleB = 0.0;
	flip = false;
	lastSampleA = lastSampleB = lastSampleC = 0.0;
}
};
