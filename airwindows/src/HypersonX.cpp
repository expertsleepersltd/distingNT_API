#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "HypersonX"
#define AIRWINDOWS_DESCRIPTION "Like UltrasonX but with seven stages instead of five."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','y','X' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
static const int kA = 0;
static const int kB = 1;
static const int kC = 2;
static const int kD = 3;
static const int kE = 4;
static const int kF = 5;
static const int kG = 6;
static const int kDefaultValue_ParamOne = kD;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, };
static char const * const enumStrings0[] = { "Reso A", "Reso B", "Reso C", "Reso D", "Reso E", "Reso F", "Reso G", };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Q", .min = 0, .max = 6, .def = 3, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
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
			fix_freq,
			fix_reso,
			fix_a0,
			fix_a1,
			fix_a2,
			fix_b1,
			fix_b2,
			fix_sL1,
			fix_sL2,
			fix_sR1,
			fix_sR2,
			fix_total
		}; //fixed frequency biquad filter for ultrasonics, stereo
		uint32_t fpd;
	
	struct _dram {
			float fixA[fix_total];
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
	
	float cutoff = 25000.0f / GetSampleRate();
	if (cutoff > 0.49f) cutoff = 0.49f; //don't crash if run at 44.1k
	
	dram->fixA[fix_freq] = cutoff;
	
	switch ((int) GetParameter( kParam_One ))
	{
		case kA:
			dram->fixA[fix_reso] = 4.46570214f;
			break;
		case kB:
			dram->fixA[fix_reso] = 1.51387132f;;
			break;
		case kC:
			dram->fixA[fix_reso] = 0.93979296f;
			break;
		case kD:
			dram->fixA[fix_reso] = 0.70710678f; //butterworth Q
			break;
		case kE:
			dram->fixA[fix_reso] = 0.59051105f;
			break;
		case kF:
			dram->fixA[fix_reso] = 0.52972649f;
			break;
		case kG:
			dram->fixA[fix_reso] = 0.50316379f;
			break;
	}	
	
	float K = tan(M_PI * dram->fixA[fix_freq]); //lowpass
	float norm = 1.0f / (1.0f + K / dram->fixA[fix_reso] + K * K);
	dram->fixA[fix_a0] = K * K * norm;
	dram->fixA[fix_a1] = 2.0f * dram->fixA[fix_a0];
	dram->fixA[fix_a2] = dram->fixA[fix_a0];
	dram->fixA[fix_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->fixA[fix_b2] = (1.0f - K / dram->fixA[fix_reso] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		float outSample = (inputSample * dram->fixA[fix_a0]) + dram->fixA[fix_sL1];
		dram->fixA[fix_sL1] = (inputSample * dram->fixA[fix_a1]) - (outSample * dram->fixA[fix_b1]) + dram->fixA[fix_sL2];
		dram->fixA[fix_sL2] = (inputSample * dram->fixA[fix_a2]) - (outSample * dram->fixA[fix_b2]);
		inputSample = outSample; //fixed biquad filtering ultrasonics
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < fix_total; x++) dram->fixA[x] = 0.0;
	dram->fixA[fix_reso] = 0.7071; //butterworth Q
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
