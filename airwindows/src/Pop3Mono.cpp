#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pop3Mono"
#define AIRWINDOWS_DESCRIPTION "Pop3Mono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','o','o' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	kParam_G =6,
	kParam_H =7,
	//Add your parameters here...
	kNumberOfParameters=8
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Thresld", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "C Ratio", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "C Atk", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "C Rls", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Thresld", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "G Ratio", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "G Sust", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "G Rls", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float popComp;
		float popGate;
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
	
	float compThresh = powf(GetParameter( kParam_A ),4);
	float compRatio = 1.0f-powf(1.0f-GetParameter( kParam_B ),2);
	float compAttack = 1.0f/(((powf(GetParameter( kParam_C ),3)*5000.0f)+500.0f)*overallscale);
	float compRelease = 1.0f/(((powf(GetParameter( kParam_D ),5)*50000.0f)+500.0f)*overallscale);
	
	float gateThresh = powf(GetParameter( kParam_E ),4);
	float gateRatio = 1.0f-powf(1.0f-GetParameter( kParam_F ),2);
	float gateSustain = M_PI_2 * powf(GetParameter( kParam_G )+1.0f,4.0f);
	float gateRelease = 1.0f/(((powf(GetParameter( kParam_H ),5)*500000.0f)+500.0f)*overallscale);
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		if (fabs(inputSample) > compThresh) { //compression 
			popComp -= (popComp * compAttack);
			popComp += ((compThresh / fabs(inputSample))*compAttack);
		} else popComp = (popComp*(1.0f-compRelease))+compRelease;		
		if (fabs(inputSample) > gateThresh) popGate = gateSustain;
		else popGate *= (1.0f-gateRelease);
		if (popGate < 0.0f) popGate = 0.0f;
		popComp = fmax(fmin(popComp,1.0f),0.0f);
		inputSample *= ((1.0f-compRatio)+(popComp*compRatio));
		if (popGate < M_PI_2) inputSample *= ((1.0f-gateRatio)+(sin(popGate)*gateRatio));
				
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	popComp = 1.0;
	popGate = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
