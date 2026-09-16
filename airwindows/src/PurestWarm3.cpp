#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PurestWarm3"
#define AIRWINDOWS_DESCRIPTION "A subtle tone shaper and warmth adder."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','u','3' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Asym", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 		
		enum {
			bip_dvA, bip_dvB, bip_dvC, bip_dvD, bip_pvA, bip_pvB, bip_pvC, bip_pvD,
			bip_total //each distortion section can have one of these, it stacks well
		}; //not remotely elliptic BLEP antialiasing, instead it is derivative BIP :D

		uint32_t fpd;
	
	struct _dram {
			float bip[bip_total];
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
	float asym = (GetParameter( kParam_A )-0.5f)*8.0f;

	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;

		if (asym != 0.0f) {
			float bip_delta = inputSample; //delta can be just local and re-used
			
			inputSample *= asym;
			inputSample = fma(((inputSample*inputSample) * -0.166666666666666f),(inputSample*inputSample),inputSample);
			inputSample /= asym;
			
			dram->bip[bip_dvA] = bip_delta - inputSample; // these are derivatives: raw clip is position
			dram->bip[bip_dvB] = dram->bip[bip_pvA]-dram->bip[bip_dvA]; dram->bip[bip_pvA] = dram->bip[bip_dvA];//velocity
			dram->bip[bip_dvC] = dram->bip[bip_pvB]-dram->bip[bip_dvB]; dram->bip[bip_pvB] = dram->bip[bip_dvB];//acceleration
			dram->bip[bip_dvD] = dram->bip[bip_pvC]-dram->bip[bip_dvC]; dram->bip[bip_pvC] = dram->bip[bip_dvC];//jerk
			float bip_dvE = dram->bip[bip_pvD]-dram->bip[bip_dvD]; dram->bip[bip_pvD] = dram->bip[bip_dvD];//snap
			inputSample *= (1.0f+(fabs(dram->bip[bip_dvC])*0.0618f)+(fabs(dram->bip[bip_dvD])*-0.05982f)+(fabs(bip_dvE)*0.0206f));
		}
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < bip_total; x++) dram->bip[x] = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
