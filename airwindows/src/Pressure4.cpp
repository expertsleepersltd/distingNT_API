#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pressure4"
#define AIRWINDOWS_DESCRIPTION "A compressor adjustable between vari-mu and ‘new york’ peak-retaining behaviors."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','r','4' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Pressure", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Speed", .min = 0, .max = 1000, .def = 200, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mewiness", .min = -1000, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Gain", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
		Float32 muVary;
		Float32 muAttack;
		Float32 muNewSpeed;
		Float32 muSpeedA;
		Float32 muSpeedB;
		Float32 muCoefficientA;
		Float32 muCoefficientB;
	uint32_t fpdL;
	uint32_t fpdR;
	bool flip;


	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {


	
	UInt32 nSampleFrames = inFramesToProcess;
//	const Float32 *sourceP = inSourceP;
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
//	Float32 *destP = inDestP;
	Float32 threshold = 1.0f - (GetParameter( kParam_One ) * 0.95f);
	Float32 muMakeupGain = 1.0f / threshold;
	//gain settings around threshold
	Float32 release = powf((1.28f-GetParameter( kParam_Two )),5)*32768.0f;
	release /= overallscale;
	Float32 fastest = sqrt(release);
	//speed settings around release
	float bridgerectifier;
	Float32 coefficient;
	Float32 inputSense;
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
	float inputSampleL;
	float inputSampleR;
	
	while (nSampleFrames-- > 0) {
		inputSampleL = *inputL;
		inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;

		inputSampleL = inputSampleL * muMakeupGain;
		inputSampleR = inputSampleR * muMakeupGain;
		
		inputSense = fabs(inputSampleL);
		if (fabs(inputSampleR) > inputSense)
			inputSense = fabs(inputSampleR);
		//we will take the greater of either channel and just use that, then apply the result
		//to both stereo channels.
		
		if (flip)
		{
			if (inputSense > threshold)
			{
				muVary = threshold / inputSense;
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
			muNewSpeed = muNewSpeed + fabs(inputSense*release)+fastest;
			muSpeedA = muNewSpeed / muSpeedA;
		}
		else
		{
			if (inputSense > threshold)
			{
				muVary = threshold / inputSense;
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
			muNewSpeed = muNewSpeed + fabs(inputSense*release)+fastest;
			muSpeedB = muNewSpeed / muSpeedB;
		}
		//got coefficients, adjusted speeds
		
		if (flip)
		{
			if (positivemu) coefficient = powf(muCoefficientA,2);
			else coefficient = sqrt(muCoefficientA);
			coefficient = (coefficient*mewiness)+(muCoefficientA*unmewiness);
			inputSampleL *= coefficient;
			inputSampleR *= coefficient;
		}
		else
		{
			if (positivemu) coefficient = powf(muCoefficientB,2);
			else coefficient = sqrt(muCoefficientB);
			coefficient = (coefficient*mewiness)+(muCoefficientB*unmewiness);
			inputSampleL *= coefficient;
			inputSampleR *= coefficient;
		}
		//applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
		//applied gain correction to control output level- tends to constrain sound rather than inflate it
		
		if (outputGain != 1.0f) {
			inputSampleL *= outputGain;
			inputSampleR *= outputGain;
		}		
		
		bridgerectifier = fabs(inputSampleL);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.0f;
		else bridgerectifier = sin(bridgerectifier);
		if (inputSampleL > 0){inputSampleL = bridgerectifier;}
		else {inputSampleL = -bridgerectifier;}
		//second stage of overdrive to prevent overs and allow bloody loud extremeness
		bridgerectifier = fabs(inputSampleR);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.0f;
		else bridgerectifier = sin(bridgerectifier);
		if (inputSampleR > 0){inputSampleR = bridgerectifier;}
		else {inputSampleR = -bridgerectifier;}
		//second stage of overdrive to prevent overs and allow bloody loud extremeness
		
		flip = !flip;
		
		
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		//don't know why we're getting a volume boost, cursed thing
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	muSpeedA = 10000;
	muSpeedB = 10000;
	muCoefficientA = 1;
	muCoefficientB = 1;
	muVary = 1;
	flip = false;	
	return noErr;
}

};
