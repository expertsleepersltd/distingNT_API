#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DeRez3"
#define AIRWINDOWS_DESCRIPTION "Reinvents retro digital tones."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','3' )
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
{ .name = "Rate", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Rez", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		
		float rezA;
		float rezB;
		float bitA;
		float bitB;
		float wetA;
		float wetB;
		
		enum {
			bez_AL,
			bez_BL,
			bez_CL,	
			bez_InL,
			bez_UnInL,
			bez_SampL,
			bez_cycle,
			bez_total
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		
		uint32_t fpd;
	
	struct _dram {
			float bez[bez_total];
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

	rezA = rezB;
	rezB = powf(GetParameter( kParam_A ),3.0f)/overallscale;
	bitA = bitB;
	bitB = (GetParameter( kParam_B)*15.0f)+1.0f;
	wetA = wetB;
	wetB = GetParameter( kParam_C )*2.0f;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		float drySampleL = inputSampleL;
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		float rez = (rezA*temp)+(rezB*(1.0f-temp));
		float bit = (bitA*temp)+(bitB*(1.0f-temp));
		float wet = (wetA*temp)+(wetB*(1.0f-temp));
		if (rez < 0.0005f) rez = 0.0005f;
		float bitFactor = powf(2.0f,bit);
		float dry = 2.0f - wet;
		if (wet > 1.0f) wet = 1.0f;
		if (wet < 0.0f) wet = 0.0f;
		if (dry > 1.0f) dry = 1.0f;
		if (dry < 0.0f) dry = 0.0f;
		//this bitcrush makes 50% full dry AND full wet, not crossfaded.
		//that's so it can be on tracks without cutting back dry channel when adjusted
		
		inputSampleL *= bitFactor;		
		inputSampleL = floor(inputSampleL+(0.5f/bitFactor));
		inputSampleL /= bitFactor;
		
		dram->bez[bez_cycle] += rez;
		dram->bez[bez_SampL] += (inputSampleL * rez);
		if (dram->bez[bez_cycle] > 1.0f) {
			dram->bez[bez_cycle] -= 1.0f;
			dram->bez[bez_CL] = dram->bez[bez_BL];
			dram->bez[bez_BL] = dram->bez[bez_AL];
			dram->bez[bez_AL] = inputSampleL;
			dram->bez[bez_SampL] = 0.0f;
		}
		float CBL = (dram->bez[bez_CL]*(1.0f-dram->bez[bez_cycle]))+(dram->bez[bez_BL]*dram->bez[bez_cycle]);
		float BAL = (dram->bez[bez_BL]*(1.0f-dram->bez[bez_cycle]))+(dram->bez[bez_AL]*dram->bez[bez_cycle]);
		float CBAL = (dram->bez[bez_BL]+(CBL*(1.0f-dram->bez[bez_cycle]))+(BAL*dram->bez[bez_cycle]))*0.5f;
		
		inputSampleL = (wet*CBAL)+(dry*drySampleL);
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	rezA = 1.0; rezB = 1.0;
	bitA = 1.0; bitB = 1.0;
	wetA = 1.0; wetB = 1.0;
	for (int x = 0; x < bez_total; x++) dram->bez[x] = 0.0;
	dram->bez[bez_cycle] = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
