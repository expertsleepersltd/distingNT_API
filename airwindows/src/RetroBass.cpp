#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "RetroBass"
#define AIRWINDOWS_DESCRIPTION "A special kind of highpass."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','R','e','s' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
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
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float rbSampleL;
		enum {
			bip_drbLA, bip_drbLB, bip_drbLC, bip_drbLD, bip_prbLA, bip_prbLB, bip_prbLC, bip_prbLD,
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
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	float rbAmount = powf(GetParameter( kParam_A )*0.5f,2.0f)/overallscale;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;

		rbSampleL = (rbSampleL * (1.0f-rbAmount))+(inputSampleL*rbAmount);
		if (fabs(rbSampleL)>1.18e-37f) {
			float bip_delta = inputSampleL; //delta can be just local and re-used
			inputSampleL *= rbSampleL*0.96f;
			inputSampleL = fma(((inputSampleL*inputSampleL) * -0.166666666666666f),(inputSampleL*inputSampleL),inputSampleL);
			inputSampleL /= rbSampleL*0.96f;
			dram->bip[bip_drbLA] = bip_delta - inputSampleL; // these are derivatives: raw clip is position
			dram->bip[bip_drbLB] = dram->bip[bip_prbLA]-dram->bip[bip_drbLA]; dram->bip[bip_prbLA] = dram->bip[bip_drbLA];//velocity
			dram->bip[bip_drbLC] = dram->bip[bip_prbLB]-dram->bip[bip_drbLB]; dram->bip[bip_prbLB] = dram->bip[bip_drbLB];//acceleration
			dram->bip[bip_drbLD] = dram->bip[bip_prbLC]-dram->bip[bip_drbLC]; dram->bip[bip_prbLC] = dram->bip[bip_drbLC];//jerk
			float bip_drbE = dram->bip[bip_prbLD]-dram->bip[bip_drbLD]; dram->bip[bip_prbLD] = dram->bip[bip_drbLD];//snap
			inputSampleL *= (1.0f+(fabs(dram->bip[bip_drbLC])*0.0618f)+(fabs(dram->bip[bip_drbLD])*-0.05982f)+(fabs(bip_drbE)*0.0206f));
		}
		inputSampleL -= rbSampleL*0.92f;
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	rbSampleL = 0.0;
	for (int x = 0; x < bip_total; x++) dram->bip[x] = 0.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
