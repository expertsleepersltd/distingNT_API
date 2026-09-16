#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Capacitor"
#define AIRWINDOWS_DESCRIPTION "A lowpass/highpass filter of a new type."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','a','r' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
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
{ .name = "Lowpass", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Highpass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

		
		Float32 iirHighpassA;
		Float32 iirHighpassB;
		Float32 iirHighpassC;
		Float32 iirHighpassD;
		Float32 iirHighpassE;
		Float32 iirHighpassF;
		Float32 iirLowpassA;
		Float32 iirLowpassB;
		Float32 iirLowpassC;
		Float32 iirLowpassD;
		Float32 iirLowpassE;
		Float32 iirLowpassF;
		int count;
		
		Float32 lowpassChase;
		Float32 highpassChase;
		Float32 wetChase;
		
		Float32 lowpassAmount;
		Float32 highpassAmount;
		Float32 wet;
		
		Float32 lastLowpass;
		Float32 lastHighpass;
		Float32 lastWet;
		
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
	
	lowpassChase = powf(GetParameter( kParam_One ),2);
	highpassChase = powf(GetParameter( kParam_Two ),2);
	wetChase = GetParameter( kParam_Three );
	//should not scale with sample rate, because values reaching 1 are important
	//to its ability to bypass when set to max
	Float32 lowpassSpeed = 300 / (fabs( lastLowpass - lowpassChase)+1.0f);
	Float32 highpassSpeed = 300 / (fabs( lastHighpass - highpassChase)+1.0f);
	Float32 wetSpeed = 300 / (fabs( lastWet - wetChase)+1.0f);
	lastLowpass = lowpassChase;
	lastHighpass = highpassChase;
	lastWet = wetChase;
	
	Float32 invLowpass;
	Float32 invHighpass;
	Float32 dry;

	float inputSample;
	Float32 drySample;
	
	while (nSampleFrames-- > 0) {
		inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		drySample = inputSample;

		lowpassAmount = (((lowpassAmount*lowpassSpeed)+lowpassChase)/(lowpassSpeed + 1.0f)); invLowpass = 1.0f - lowpassAmount;
		highpassAmount = (((highpassAmount*highpassSpeed)+highpassChase)/(highpassSpeed + 1.0f)); invHighpass = 1.0f - highpassAmount;
		wet = (((wet*wetSpeed)+wetChase)/(wetSpeed+1.0f)); dry = 1.0f - wet;
		
		count++; if (count > 5) count = 0; switch (count)
		{
			case 0:
				iirHighpassA = (iirHighpassA * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassA;
				iirLowpassA = (iirLowpassA * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassA;
				iirHighpassB = (iirHighpassB * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassB;
				iirLowpassB = (iirLowpassB * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassB;
				iirHighpassD = (iirHighpassD * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassD;
				iirLowpassD = (iirLowpassD * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassD;
				break;
			case 1:
				iirHighpassA = (iirHighpassA * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassA;
				iirLowpassA = (iirLowpassA * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassA;
				iirHighpassC = (iirHighpassC * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassC;
				iirLowpassC = (iirLowpassC * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassC;
				iirHighpassE = (iirHighpassE * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassE;
				iirLowpassE = (iirLowpassE * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassE;
				break;
			case 2:
				iirHighpassA = (iirHighpassA * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassA;
				iirLowpassA = (iirLowpassA * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassA;
				iirHighpassB = (iirHighpassB * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassB;
				iirLowpassB = (iirLowpassB * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassB;
				iirHighpassF = (iirHighpassF * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassF;
				iirLowpassF = (iirLowpassF * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassF;
				break;
			case 3:
				iirHighpassA = (iirHighpassA * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassA;
				iirLowpassA = (iirLowpassA * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassA;
				iirHighpassC = (iirHighpassC * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassC;
				iirLowpassC = (iirLowpassC * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassC;
				iirHighpassD = (iirHighpassD * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassD;
				iirLowpassD = (iirLowpassD * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassD;
				break;
			case 4:
				iirHighpassA = (iirHighpassA * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassA;
				iirLowpassA = (iirLowpassA * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassA;
				iirHighpassB = (iirHighpassB * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassB;
				iirLowpassB = (iirLowpassB * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassB;
				iirHighpassE = (iirHighpassE * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassE;
				iirLowpassE = (iirLowpassE * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassE;
				break;
			case 5:
				iirHighpassA = (iirHighpassA * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassA;
				iirLowpassA = (iirLowpassA * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassA;
				iirHighpassC = (iirHighpassC * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassC;
				iirLowpassC = (iirLowpassC * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassC;
				iirHighpassF = (iirHighpassF * invHighpass) + (inputSample * highpassAmount); inputSample -= iirHighpassF;
				iirLowpassF = (iirLowpassF * invLowpass) + (inputSample * lowpassAmount); inputSample = iirLowpassF;
				break;
		}
		//Highpass Filter chunk. This is three poles of IIR highpass, with a 'gearbox' that progressively
		//steepens the filter after minimizing artifacts.
		
		inputSample = (drySample * dry) + (inputSample * wet);

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirHighpassA = 0.0;
	iirHighpassB = 0.0;
	iirHighpassC = 0.0;
	iirHighpassD = 0.0;
	iirHighpassE = 0.0;
	iirHighpassF = 0.0;
	iirLowpassA = 0.0;
	iirLowpassB = 0.0;
	iirLowpassC = 0.0;
	iirLowpassD = 0.0;
	iirLowpassE = 0.0;
	iirLowpassF = 0.0;
	count = 0;
	lowpassChase = 0.0;
	highpassChase = 0.0;
	wetChase = 0.0;
	lowpassAmount = 1.0;
	highpassAmount = 0.0;
	wet = 1.0;
	lastLowpass = 1000.0;
	lastHighpass = 1000.0;
	lastWet = 1000.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
