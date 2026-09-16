#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Gatelope"
#define AIRWINDOWS_DESCRIPTION "A special gate that applies filters."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','G','a','f' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Threshold", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Treble Sustain", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass Sustain", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack Speed", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		Float32 iirLowpassA;
		Float32 iirLowpassB;
		Float32 iirHighpassA;
		Float32 iirHighpassB;
		Float32 treblefreq;
		Float32 bassfreq;
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
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	//speed settings around release
	Float32 threshold = powf(GetParameter( kParam_One ),2);
	//gain settings around threshold
	Float32 trebledecay = powf(1.0f-GetParameter( kParam_Two ),2)/4196.0f;
	Float32 bassdecay =  powf(1.0f-GetParameter( kParam_Three ),2)/8192.0f;
	Float32 slowAttack = (powf(GetParameter( kParam_Four ),3)*3)+0.003f;
	Float32 wet = GetParameter( kParam_Five );
	slowAttack /= overallscale;
	trebledecay /= overallscale;
	bassdecay /= overallscale;
	trebledecay += 1.0f;
	bassdecay += 1.0f;
	Float32 attackSpeed;

	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		attackSpeed = slowAttack - (fabs(inputSample)*slowAttack*0.5f);
		
		if (attackSpeed < 0.0f) attackSpeed = 0.0f;
		//softening onset click depending on how hard we're getting it
		
		if (flip)
		{
			if (fabs(inputSample) > threshold)
			{
				treblefreq += attackSpeed;
				if (treblefreq > 2.0f) treblefreq = 2.0f;
				bassfreq -= attackSpeed;
				bassfreq -= attackSpeed;
				if (bassfreq < 0.0f) bassfreq = 0.0f;
				iirLowpassA = inputSample;
				iirLowpassB = inputSample;
				iirHighpassA = 0.0f;
				iirHighpassB = 0.0f;
			}
			else
			{
				treblefreq -= bassfreq;
				treblefreq /= trebledecay;
				treblefreq += bassfreq;
				bassfreq -= treblefreq;
				bassfreq /= bassdecay;
				bassfreq += treblefreq;
			}
			
			if (treblefreq >= 1.0f) iirLowpassA = inputSample;
			else iirLowpassA = (iirLowpassA * (1.0f - treblefreq)) + (inputSample * treblefreq);
			
			if (bassfreq > 1.0f) bassfreq = 1.0f;
			
			if (bassfreq > 0.0f) iirHighpassA = (iirHighpassA * (1.0f - bassfreq)) + (inputSample * bassfreq);
			else iirHighpassA = 0.0f;
			
			if (treblefreq > bassfreq) inputSample = (iirLowpassA - iirHighpassA);
			else inputSample = 0.0f;
		}
		else
		{
			if (fabs(inputSample) > threshold)
			{
				treblefreq += attackSpeed;
				if (treblefreq > 2.0f) treblefreq = 2.0f;
				bassfreq -= attackSpeed;
				bassfreq -= attackSpeed;
				if (bassfreq < 0.0f) bassfreq = 0.0f;
				iirLowpassA = inputSample;
				iirLowpassB = inputSample;
				iirHighpassA = 0.0f;
				iirHighpassB = 0.0f;
			}
			else
			{
				treblefreq -= bassfreq;
				treblefreq /= trebledecay;
				treblefreq += bassfreq;
				bassfreq -= treblefreq;
				bassfreq /= bassdecay;
				bassfreq += treblefreq;
			}
			
			if (treblefreq >= 1.0f) iirLowpassB = inputSample;
			else iirLowpassB = (iirLowpassB * (1.0f - treblefreq)) + (inputSample * treblefreq);
			
			if (bassfreq > 1.0f) bassfreq = 1.0f;

			if (bassfreq > 0.0f) iirHighpassB = (iirHighpassB * (1.0f - bassfreq)) + (inputSample * bassfreq);
			else iirHighpassB = 0.0f;
			
			if (treblefreq > bassfreq) inputSample = (iirLowpassB - iirHighpassB);
			else inputSample = 0.0f;
		}
		//done full gated envelope filtered effect
		inputSample  = ((1-wet)*drySample)+(wet*inputSample);
		//we're going to set up a dry/wet control instead of a min. threshold
		
		flip = !flip;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirLowpassA = 0.0;
	iirLowpassB = 0.0;
	iirHighpassA = 0.0;
	iirHighpassB = 0.0;
	treblefreq = 1.0;
	bassfreq = 0.0;
	flip = false;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
