#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pressure6Mono"
#define AIRWINDOWS_DESCRIPTION "Pressure6Mono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','r','k' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Compres", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Ratio", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float muComp;
		float muSpd;
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

	float threshold = 1.0f-powf(GetParameter( kParam_A )*0.9f,3.0f);
	float adjSpd = ((GetParameter( kParam_A )*92.0f)+92.0f)*overallscale;
	float wet = GetParameter( kParam_B )*0.9f;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		inputSample *= (muComp/threshold);
		if (fabs(inputSample) > threshold)
		{
			muComp *= muSpd;
			if (threshold/fabs(inputSample) < threshold) muComp += threshold*fabs(inputSample);
			else muComp -= threshold/fabs(inputSample);
			muComp /= muSpd;
		} else {
			muComp *= (muSpd*muSpd);
			muComp += ((1.1f+threshold)-fabs(inputSample));
			muComp /= (muSpd*muSpd);
		}
		muComp = fmax(fmin(muComp,1.0f),threshold);
		inputSample *= (muComp*muComp);
		muSpd = fmax(fmin(((muSpd*(muSpd-1.0f))+(fabs(inputSample*adjSpd)))/muSpd,adjSpd*2.0f),adjSpd);
		
		inputSample = (drySample*(1.0f-(wet*1.1f)))+(sin(fmax(fmin(inputSample*wet,M_PI_2),-M_PI_2))*wet);
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	muComp = 1.0;
	muSpd = 100.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
