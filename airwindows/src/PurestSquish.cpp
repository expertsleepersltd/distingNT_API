#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PurestSquish"
#define AIRWINDOWS_DESCRIPTION "An open-sounding compressor with bass bloom."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','u','&' )
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
{ .name = "Squish", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass Bloom", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 muVary;
		Float32 muAttack;
		Float32 muNewSpeed;
		Float32 muSpeedA;
		Float32 muSpeedB;
		Float32 muSpeedC;
		Float32 muSpeedD;
		Float32 muSpeedE;
		Float32 muCoefficientA;
		Float32 muCoefficientB;
		Float32 muCoefficientC;
		Float32 muCoefficientD;
		Float32 muCoefficientE;
		Float32 iirSampleA;
		Float32 iirSampleB;
		Float32 iirSampleC;
		Float32 iirSampleD;
		Float32 iirSampleE;
		Float32 lastCoefficientA;
		Float32 lastCoefficientB;
		Float32 lastCoefficientC;
		Float32 lastCoefficientD;
		Float32 mergedCoefficients;
		int count;
		bool fpFlip;
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
	
	float highpassSample;
	float highpassSampleB;
	Float32 threshold = 1.01f - (1.0f-powf(1.0f-(GetParameter( kParam_One )*0.5f),4));
	Float32 iirAmount = powf(GetParameter( kParam_Two ),4)/overallscale;
	Float32 output = GetParameter( kParam_Three );
	Float32 wet = GetParameter( kParam_Four );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;

		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		Float32 muMakeupGain = 1.0f / threshold;
		Float32 outMakeupGain = sqrt(muMakeupGain);
		muMakeupGain += outMakeupGain;
		muMakeupGain *= 0.5f;
		outMakeupGain *= 0.5f;
		//gain settings around threshold
		Float32 release = mergedCoefficients * 32768.0f;
		release /= overallscale;
		Float32 fastest = sqrt(release);
		//speed settings around release
		Float32 lastCorrection = mergedCoefficients;
		// µ µ µ µ µ µ µ µ µ µ µ µ is the kitten song o/~
		
		if (muMakeupGain != 1.0f) inputSample = inputSample * muMakeupGain;
		
		highpassSampleB = highpassSample = inputSample;
		
		if (count < 1 || count > 3) count = 1;
		switch (count)
		{
			case 1:
				iirSampleA = (iirSampleA * (1 - iirAmount)) + (highpassSample * iirAmount);
				highpassSample -= iirSampleA;
				if (fabs(highpassSample) > threshold)
				{
					muVary = threshold / fabs(highpassSample);
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
				muNewSpeed = muNewSpeed + fabs(highpassSample*release)+fastest;
				muSpeedA = muNewSpeed / muSpeedA;
				lastCoefficientA = powf(muCoefficientA,2);
				mergedCoefficients = lastCoefficientB;
				mergedCoefficients += lastCoefficientA;
				lastCoefficientA *= (1.0f - lastCorrection);
				lastCoefficientA += (muCoefficientA * lastCorrection);
				lastCoefficientB = lastCoefficientA;
				break;
			case 2:
				iirSampleB = (iirSampleB * (1 - iirAmount)) + (highpassSample * iirAmount);
				highpassSample -= iirSampleB;
				if (fabs(highpassSample) > threshold)
				{
					muVary = threshold / fabs(highpassSample);
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
				muNewSpeed = muNewSpeed + fabs(highpassSample*release)+fastest;
				muSpeedB = muNewSpeed / muSpeedB;
				lastCoefficientA = powf(muCoefficientB,2);
				mergedCoefficients = lastCoefficientB;
				mergedCoefficients += lastCoefficientA;
				lastCoefficientA *= (1.0f - lastCorrection);
				lastCoefficientA += (muCoefficientB * lastCorrection);
				lastCoefficientB = lastCoefficientA;
				break;
			case 3:
				iirSampleC = (iirSampleC * (1 - iirAmount)) + (highpassSample * iirAmount);
				highpassSample -= iirSampleC;
				if (fabs(highpassSample) > threshold)
				{
					muVary = threshold / fabs(highpassSample);
					muAttack = sqrt(fabs(muSpeedC));
					muCoefficientC = muCoefficientC * (muAttack-1);
					if (muVary < threshold)
					{
						muCoefficientC = muCoefficientC + threshold;
					}
					else
					{
						muCoefficientC = muCoefficientC + muVary;
					}
					muCoefficientC = muCoefficientC / muAttack;
				}
				else
				{
					muCoefficientC = muCoefficientC * ((muSpeedC * muSpeedC)-1.0f);
					muCoefficientC = muCoefficientC + 1.0f;
					muCoefficientC = muCoefficientC / (muSpeedC * muSpeedC);
				}
				muNewSpeed = muSpeedC * (muSpeedC-1);
				muNewSpeed = muNewSpeed + fabs(highpassSample*release)+fastest;
				muSpeedC = muNewSpeed / muSpeedC;
				lastCoefficientA = powf(muCoefficientC,2);
				mergedCoefficients = lastCoefficientB;
				mergedCoefficients += lastCoefficientA;
				lastCoefficientA *= (1.0f - lastCorrection);
				lastCoefficientA += (muCoefficientC * lastCorrection);
				lastCoefficientB = lastCoefficientA;
				break;
		}		
		count++;
		
		//applied compression with vari-vari-µ-µ-µ-µ-µ-µ-is-the-kitten-song o/~
		//applied gain correction to control output level- tends to constrain sound rather than inflate it
		
		if (fpFlip) {
			iirSampleD = (iirSampleD * (1 - iirAmount)) + (highpassSampleB * iirAmount);
			highpassSampleB -= iirSampleD;
			if (fabs(highpassSampleB) > threshold)
			{
				muVary = threshold / fabs(highpassSampleB);
				muAttack = sqrt(fabs(muSpeedD));
				muCoefficientD = muCoefficientD * (muAttack-1.0f);
				if (muVary < threshold)
				{
					muCoefficientD = muCoefficientD + threshold;
				}
				else
				{
					muCoefficientD = muCoefficientD + muVary;
				}
				muCoefficientD = muCoefficientD / muAttack;
			}
			else
			{
				muCoefficientD = muCoefficientD * ((muSpeedD * muSpeedD)-1.0f);
				muCoefficientD = muCoefficientD + 1.0f;
				muCoefficientD = muCoefficientD / (muSpeedD * muSpeedD);
			}
			muNewSpeed = muSpeedD * (muSpeedD-1);
			muNewSpeed = muNewSpeed + fabs(highpassSampleB*release)+fastest;
			muSpeedD = muNewSpeed / muSpeedD;
			lastCoefficientC = powf(muCoefficientE,2);
			mergedCoefficients += lastCoefficientD;
			mergedCoefficients += lastCoefficientC;
			lastCoefficientC *= (1.0f - lastCorrection);
			lastCoefficientC += (muCoefficientD * lastCorrection);
			lastCoefficientD = lastCoefficientC;
		} else {
			iirSampleE = (iirSampleE * (1 - iirAmount)) + (highpassSampleB * iirAmount);
			highpassSampleB -= iirSampleE;
			if (fabs(highpassSampleB) > threshold)
			{
				muVary = threshold / fabs(highpassSampleB);
				muAttack = sqrt(fabs(muSpeedE));
				muCoefficientE = muCoefficientE * (muAttack-1.0f);
				if (muVary < threshold)
				{
					muCoefficientE = muCoefficientE + threshold;
				}
				else
				{
					muCoefficientE = muCoefficientE + muVary;
				}
				muCoefficientE = muCoefficientE / muAttack;
			}
			else
			{
				muCoefficientE = muCoefficientE * ((muSpeedE * muSpeedE)-1.0f);
				muCoefficientE = muCoefficientE + 1.0f;
				muCoefficientE = muCoefficientE / (muSpeedE * muSpeedE);
			}
			muNewSpeed = muSpeedE * (muSpeedE-1);
			muNewSpeed = muNewSpeed + fabs(highpassSampleB*release)+fastest;
			muSpeedE = muNewSpeed / muSpeedE;
			lastCoefficientC = powf(muCoefficientE,2);
			mergedCoefficients += lastCoefficientD;
			mergedCoefficients += lastCoefficientC;
			lastCoefficientC *= (1.0f - lastCorrection);
			lastCoefficientC += (muCoefficientE * lastCorrection);
			lastCoefficientD = lastCoefficientC;
		}
		mergedCoefficients *= 0.25f;
		inputSample *= mergedCoefficients;
		
		if (outMakeupGain != 1.0f) inputSample = inputSample * outMakeupGain;
		
		fpFlip = !fpFlip;
		
		if (output < 1.0f) {
			inputSample *= output;
		}
		
		if (wet < 1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	muSpeedA = 10000;
	muSpeedB = 10000;
	muSpeedC = 10000;
	muSpeedD = 10000;
	muSpeedE = 10000;
	muCoefficientA = 1;
	muCoefficientB = 1;
	muCoefficientC = 1;
	muCoefficientD = 1;
	muCoefficientE = 1;
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	iirSampleE = 0.0;
	lastCoefficientA = 1;
	lastCoefficientB = 1;
	lastCoefficientC = 1;
	lastCoefficientD = 1;
	mergedCoefficients = 1;
	muVary = 1;
	count = 1;
	fpFlip = true;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
