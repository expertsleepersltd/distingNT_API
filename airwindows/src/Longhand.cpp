#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Longhand"
#define AIRWINDOWS_DESCRIPTION "An experiment in softclipping."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','L','o','d' )
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
{ .name = "Input", .min = 0, .max = 1000, .def = 445, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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

	float inputGain = powf(GetParameter( kParam_A ),2.0f)*7.72f;//normalized input does not wavefold too far
	float outputGain = GetParameter( kParam_B );		
	float wet = GetParameter( kParam_C );

	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		inputSampleL = fmin(fmax(inputSampleL,-1.0f),1.0f);
		float drySampleL = inputSampleL;
		
		if (inputGain != 1.0f) {
			inputSampleL *= inputGain;
			drySampleL *= inputGain;
		}
		
		float X = inputSampleL; X *= X; //float for even
		float temp = inputSampleL * X; //the initial multiplies
		inputSampleL -= temp*0.16666666666666666666666666666666666f; temp *= X;
		inputSampleL += temp*0.00833333333333333333333333333333333f; temp *= X;
		inputSampleL -= temp*0.00019841269841269841269841269841269f; temp *= X;
		inputSampleL += temp*0.00000275573192239858906525573192239f; temp *= X;
		inputSampleL -= temp*0.00000002505210838544171877521083854f; temp *= X;
		inputSampleL += temp*0.00000000016059043836821614599392377f; temp *= X;
		inputSampleL -= temp*0.00000000000076471637318198164759011f; temp *= X;
		inputSampleL += temp*0.00000000000000281145725434552076319f; temp *= X;
		inputSampleL -= temp*0.00000000000000000822063524662432971f; temp *= X;
		inputSampleL += temp*0.00000000000000000001957294106339126f;
		//retain mantissa of a float increasing power function
		//float probably doesn't handle more than 36 digits or so
		
		if (outputGain != 1.0f) {
			inputSampleL *= outputGain;
		}
		
		if (wet !=1.0f) {
			inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f-wet));
		}

		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
