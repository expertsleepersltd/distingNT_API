#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Dubly3"
#define AIRWINDOWS_DESCRIPTION "Refines and transforms the Dubly sound."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','u','5' )
#define AIRWINDOWS_TAGS kNT_tagEffect
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
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
{ .name = "Input", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tilt", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Shape", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float iirEnc;
		float iirDec;
		float compEnc;
		float compDec;
		float avgEnc;
		float avgDec;

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
	
	float inputGain = powf(GetParameter( kParam_A )*2.0f,2.0f);
	float dublyAmount = GetParameter( kParam_B )*2.0f;
	float outlyAmount = (1.0f-GetParameter( kParam_B ))*-2.0f;
	if (outlyAmount < -1.0f) outlyAmount = -1.0f;
	float iirEncFreq = (1.0f-GetParameter( kParam_C ))/overallscale;
	float iirDecFreq = GetParameter( kParam_C )/overallscale;
	float outputGain = GetParameter( kParam_D )*2.0f;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		if (inputGain != 1.0f) {
			inputSample *= inputGain;
		}
		
		//Dubly encode
		iirEnc = (iirEnc * (1.0f - iirEncFreq)) + (inputSample * iirEncFreq);
		float highPart = ((inputSample-iirEnc)*2.848f);
		highPart += avgEnc; avgEnc = (inputSample-iirEnc)*1.152f;
		if (highPart > 1.0f) highPart = 1.0f; if (highPart < -1.0f) highPart = -1.0f;
		float dubly = fabs(highPart);
		if (dubly > 0.0f) {
			float adjust = log(1.0f+(255.0f*dubly))/2.40823996531f;
			if (adjust > 0.0f) dubly /= adjust;
			compEnc = (compEnc*(1.0f-iirEncFreq))+(dubly*iirEncFreq);
			inputSample += ((highPart*compEnc)*dublyAmount);
		} //end Dubly encode
		
		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		
		//Dubly decode
		iirDec = (iirDec * (1.0f - iirDecFreq)) + (inputSample * iirDecFreq);
		highPart = ((inputSample-iirDec)*2.628f);
		highPart += avgDec; avgDec = (inputSample-iirDec)*1.372f;
		if (highPart > 1.0f) highPart = 1.0f; if (highPart < -1.0f) highPart = -1.0f;
		dubly = fabs(highPart);
		if (dubly > 0.0f) {
			float adjust = log(1.0f+(255.0f*dubly))/2.40823996531f;
			if (adjust > 0.0f) dubly /= adjust;
			compDec = (compDec*(1.0f-iirDecFreq))+(dubly*iirDecFreq);
			inputSample += ((highPart*compDec)*outlyAmount);
		} //end Dubly decode
		
		if (outputGain != 1.0f) {
			inputSample *= outputGain;
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirEnc = 0.0; iirDec = 0.0;
	compEnc = 1.0; compDec = 1.0;
	avgEnc = 0.0; avgDec = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
