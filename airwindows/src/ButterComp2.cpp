#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ButterComp2"
#define AIRWINDOWS_DESCRIPTION "Improved ButterComp with an output control and sound upgrades."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','u','2' )
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
{ .name = "Compress", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 2000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float controlApos;
		float controlAneg;
		float controlBpos;
		float controlBneg;
		float targetpos;
		float targetneg;
		float lastOutput;
		bool flip;
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

	Float32 inputgain = powf(10.0f,(GetParameter( kParam_One )*14.0f)/20.0f);
	Float32 compfactor = 0.012f * (GetParameter( kParam_One ) / 135.0f);
	Float32 output = GetParameter( kParam_Two );
	Float32 wet = GetParameter( kParam_Three );
	//removed unnecessary dry variable
	Float32 outputgain = inputgain;
	outputgain -= 1.0f;
	outputgain /= 1.5f;
	outputgain += 1.0f;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		inputSample *= inputgain;
		
		float divisor = compfactor / (1.0f+fabs(lastOutput));
		//this is slowing compressor recovery while output waveforms were high
		divisor /= overallscale;
		float remainder = divisor;
		divisor = 1.0f - divisor;
		//recalculate divisor every sample
		
		float inputpos = inputSample + 1.0f;
		if (inputpos < 0.0f) inputpos = 0.0f;
		float outputpos = inputpos / 2.0f;
		if (outputpos > 1.0f) outputpos = 1.0f;		
		inputpos *= inputpos;
		targetpos *= divisor;
		targetpos += (inputpos * remainder);
		float calcpos = powf((1.0f/targetpos),2);
		
		float inputneg = (-inputSample) + 1.0f;
		if (inputneg < 0.0f) inputneg = 0.0f;
		float outputneg = inputneg / 2.0f;
		if (outputneg > 1.0f) outputneg = 1.0f;		
		inputneg *= inputneg;
		targetneg *= divisor;
		targetneg += (inputneg * remainder);
		float calcneg = powf((1.0f/targetneg),2);
		//now we have mirrored targets for comp
		//outputpos and outputneg go from 0 to 1
		
		if (inputSample > 0)
		{ //working on pos
			if (flip)
			{
				controlApos *= divisor;
				controlApos += (calcpos*remainder);
				
			}
			else
			{
				controlBpos *= divisor;
				controlBpos += (calcpos*remainder);
			}
		}
		else
		{ //working on neg
			if (flip)
			{
				controlAneg *= divisor;
				controlAneg += (calcneg*remainder);
			}
			else
			{
				controlBneg *= divisor;
				controlBneg += (calcneg*remainder);
			}
		}
		//this causes each of the four to update only when active and in the correct 'flip'
		
		float totalmultiplier;
		if (flip)
		{totalmultiplier = (controlApos * outputpos) + (controlAneg * outputneg);}
		else
		{totalmultiplier = (controlBpos * outputpos) + (controlBneg * outputneg);}
		//this combines the sides according to flip, blending relative to the input value
		
		inputSample *= totalmultiplier;
		inputSample /= outputgain;
		
		if (output != 1.0f) {
			inputSample *= output;
		}
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider
		
		lastOutput = inputSample;
		//we will make this factor respond to use of dry/wet
		
		flip = !flip;

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	controlApos = 1.0;
	controlAneg = 1.0;
	controlBpos = 1.0;
	controlBneg = 1.0;
	targetpos = 1.0;
	targetneg = 1.0;
	flip = false;
	lastOutput = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
