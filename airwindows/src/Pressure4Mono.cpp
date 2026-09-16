#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pressure4Mono"
#define AIRWINDOWS_DESCRIPTION "Pressure4Mono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','r','o' )
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
{ .name = "Pressure", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 200, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mewiness", .min = -1000, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Gain", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 muVary;
		Float32 muAttack;
		Float32 muNewSpeed;
		Float32 muSpeedA;
		Float32 muSpeedB;
		Float32 muCoefficientA;
		Float32 muCoefficientB;
		uint32_t fpd;
		bool flip;
		
	
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
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	Float32 *destP = inDestP;
	Float32 threshold = 1.0f - (GetParameter( kParam_One ) * 0.95f);
	Float32 muMakeupGain = 1.0f / threshold;
	//gain settings around threshold
	Float32 release = powf((1.28f-GetParameter( kParam_Two )),5)*32768.0f;
	release /= overallscale;
	Float32 fastest = sqrt(release);
	//speed settings around release
	float bridgerectifier;
	Float32 coefficient;
	Float32 mewiness = GetParameter( kParam_Three );
	Float32 outputGain = GetParameter( kParam_Four );
	Float32 unmewiness;
	bool positivemu;
	if (mewiness >= 0)
	{
		positivemu = true;
		unmewiness = 1.0f-mewiness;
	}
	else
	{
		positivemu = false;
		mewiness = -mewiness;
		unmewiness = 1.0f-mewiness;
	}
	// µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
	float inputSample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		
		sourceP += inNumChannels;	// advance to next frame (e.g. if stereo, we're advancing 2 samples);
		inputSample = inputSample * muMakeupGain;
		
		if (flip)
		{
			if (fabs(inputSample) > threshold)
			{
				muVary = threshold / fabs(inputSample);
				muAttack = sqrt(fabs(muSpeedA));
				muCoefficientA = muCoefficientA * (muAttack-1.0f);
				if (muVary < threshold)
				{
					muCoefficientA = muCoefficientA + threshold;
				}
				else
				{
					muCoefficientA = muCoefficientA + muVary;
				}
				muCoefficientA = muCoefficientA / muAttack;
			}
			else
			{
				muCoefficientA = muCoefficientA * ((muSpeedA * muSpeedA)-1.0f);
				muCoefficientA = muCoefficientA + 1.0f;
				muCoefficientA = muCoefficientA / (muSpeedA * muSpeedA);
			}
			muNewSpeed = muSpeedA * (muSpeedA-1);
			muNewSpeed = muNewSpeed + fabs(inputSample*release)+fastest;
			muSpeedA = muNewSpeed / muSpeedA;
		}
		else
		{
			if (fabs(inputSample) > threshold)
			{
				muVary = threshold / fabs(inputSample);
				muAttack = sqrt(fabs(muSpeedB));
				muCoefficientB = muCoefficientB * (muAttack-1);
				if (muVary < threshold)
				{
					muCoefficientB = muCoefficientB + threshold;
				}
				else
				{
					muCoefficientB = muCoefficientB + muVary;
				}
				muCoefficientB = muCoefficientB / muAttack;
			}
			else
			{
				muCoefficientB = muCoefficientB * ((muSpeedB * muSpeedB)-1.0f);
				muCoefficientB = muCoefficientB + 1.0f;
				muCoefficientB = muCoefficientB / (muSpeedB * muSpeedB);
			}
			muNewSpeed = muSpeedB * (muSpeedB-1);
			muNewSpeed = muNewSpeed + fabs(inputSample*release)+fastest;
			muSpeedB = muNewSpeed / muSpeedB;
		}
		//got coefficients, adjusted speeds
		
		if (flip)
		{
			if (positivemu) coefficient = powf(muCoefficientA,2);
			else coefficient = sqrt(muCoefficientA);
			coefficient = (coefficient*mewiness)+(muCoefficientA*unmewiness);
			inputSample *= coefficient;
		}
		else
		{
			if (positivemu) coefficient = powf(muCoefficientB,2);
			else coefficient = sqrt(muCoefficientB);
			coefficient = (coefficient*mewiness)+(muCoefficientB*unmewiness);
			inputSample *= coefficient;
		}
		//applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
		//applied gain correction to control output level- tends to constrain sound rather than inflate it
		
		if (outputGain != 1.0f) {
			inputSample *= outputGain;
		}
		
		bridgerectifier = fabs(inputSample);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.0f;
		else bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0){inputSample = bridgerectifier;}
		else {inputSample = -bridgerectifier;}
		//second stage of overdrive to prevent overs and allow bloody loud extremeness
		flip = !flip;
		
		

		*destP = inputSample;
		destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
	muSpeedA = 10000;
	muSpeedB = 10000;
	muCoefficientA = 1;
	muCoefficientB = 1;
	muVary = 1;
	flip = false;
}
};
