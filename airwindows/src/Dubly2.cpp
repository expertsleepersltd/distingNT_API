#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Dubly2"
#define AIRWINDOWS_DESCRIPTION "A key part of seventies sonics!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','u','4' )
#define AIRWINDOWS_TAGS kNT_tagEffect
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	kParam_G =6,
	//Add your parameters here...
	kNumberOfParameters=7
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "EncAmnt", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "EncFreq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "TapeDrv", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DecAmnt", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "DecFreq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Out Pad", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float iirEnc;
		float iirDec;

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

	float dublyAmount = powf(GetParameter( kParam_A ),3)*0.25f;
	float iirEncFreq = GetParameter( kParam_B )/overallscale;
	float tapeDrv = (GetParameter( kParam_C )*2.0f)+1.0f;
	
	float outlyAmount = powf(GetParameter( kParam_D ),3)*0.25f;
	float iirDecFreq = GetParameter( kParam_E )/overallscale;
	float outPad = (GetParameter( kParam_F )*2.0f)+1.0f;
	if (tapeDrv > 1.0f) outPad += 0.005f;
	
	float wet = GetParameter( kParam_G );

	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		
		iirEnc = (iirEnc * (1.0f - iirEncFreq)) + (inputSample * iirEncFreq);
		float doubly = inputSample - iirEnc;
		if (doubly > 1.0f) doubly = 1.0f; if (doubly < -1.0f) doubly = -1.0f;
		if (doubly > 0) doubly = log(1.0f+(255*fabs(doubly)))/2.40823996531f;
		if (doubly < 0) doubly = -log(1.0f+(255*fabs(doubly)))/2.40823996531f;
		inputSample += doubly*dublyAmount;
		//end Dubly encode
		
		if (tapeDrv > 1.0f) inputSample *= tapeDrv;
		if (inputSample > 1.57079633f) inputSample = 1.57079633f;
		if (inputSample < -1.57079633f) inputSample = -1.57079633f;
		inputSample = sin(inputSample);
		if (outPad > 1.0f) inputSample /= outPad;
		
		iirDec = (iirDec * (1.0f - iirDecFreq)) + (inputSample * iirDecFreq);
		doubly = inputSample - iirDec;
		if (doubly > 1.0f) doubly = 1.0f; if (doubly < -1.0f) doubly = -1.0f;
		if (doubly > 0) doubly = log(1.0f+(255*fabs(doubly)))/2.40823996531f;
		if (doubly < 0) doubly = -log(1.0f+(255*fabs(doubly)))/2.40823996531f;
		inputSample -= doubly*outlyAmount;
		//end Dubly decode
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirEnc = 0.0; iirDec = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
