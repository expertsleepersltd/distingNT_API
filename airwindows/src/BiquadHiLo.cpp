#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BiquadHiLo"
#define AIRWINDOWS_DESCRIPTION "The highpass and lowpass filter in ConsoleX."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','i','o' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_HIP =0,
	kParam_LOP =1,
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
{ .name = "Highpas", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Lowpass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		
		enum {
			hilp_freq, hilp_temp,
			hilp_a0, hilp_a1, hilp_b1, hilp_b2,
			hilp_c0, hilp_c1, hilp_d1, hilp_d2,
			hilp_e0, hilp_e1, hilp_f1, hilp_f2,
			hilp_aL1, hilp_aL2, hilp_aR1, hilp_aR2,
			hilp_cL1, hilp_cL2, hilp_cR1, hilp_cR2,
			hilp_eL1, hilp_eL2, hilp_eR1, hilp_eR2,
			hilp_total
		};
		
		uint32_t fpd;
	
	struct _dram {
			float highpass[hilp_total];
		float lowpass[hilp_total];
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
	
	dram->highpass[hilp_freq] = ((GetParameter( kParam_HIP )*330.0f)+20.0f)/GetSampleRate();
	bool highpassEngage = true; if (GetParameter( kParam_HIP ) == 0.0f) highpassEngage = false;
	
	dram->lowpass[hilp_freq] = ((powf(1.0f-GetParameter( kParam_LOP ),2)*17000.0f)+3000.0f)/GetSampleRate();
	bool lowpassEngage = true; if (GetParameter( kParam_LOP ) == 0.0f) lowpassEngage = false;
	
	float K = tan(M_PI * dram->highpass[hilp_freq]); //highpass
	float norm = 1.0f / (1.0f + K / 1.93185165f + K * K);
	dram->highpass[hilp_a0] = norm;
	dram->highpass[hilp_a1] = -2.0f * dram->highpass[hilp_a0];
	dram->highpass[hilp_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->highpass[hilp_b2] = (1.0f - K / 1.93185165f + K * K) * norm;
	norm = 1.0f / (1.0f + K / 0.70710678f + K * K);
	dram->highpass[hilp_c0] = norm;
	dram->highpass[hilp_c1] = -2.0f * dram->highpass[hilp_c0];
	dram->highpass[hilp_d1] = 2.0f * (K * K - 1.0f) * norm;
	dram->highpass[hilp_d2] = (1.0f - K / 0.70710678f + K * K) * norm;
	norm = 1.0f / (1.0f + K / 0.51763809f + K * K);
	dram->highpass[hilp_e0] = norm;
	dram->highpass[hilp_e1] = -2.0f * dram->highpass[hilp_e0];
	dram->highpass[hilp_f1] = 2.0f * (K * K - 1.0f) * norm;
	dram->highpass[hilp_f2] = (1.0f - K / 0.51763809f + K * K) * norm;
		
	K = tan(M_PI * dram->lowpass[hilp_freq]); //lowpass
	norm = 1.0f / (1.0f + K / 1.93185165f + K * K);
	dram->lowpass[hilp_a0] = K * K * norm;
	dram->lowpass[hilp_a1] = 2.0f * dram->lowpass[hilp_a0];
	dram->lowpass[hilp_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->lowpass[hilp_b2] = (1.0f - K / 1.93185165f + K * K) * norm;
	norm = 1.0f / (1.0f + K / 0.70710678f + K * K);
	dram->lowpass[hilp_c0] = K * K * norm;
	dram->lowpass[hilp_c1] = 2.0f * dram->lowpass[hilp_c0];
	dram->lowpass[hilp_d1] = 2.0f * (K * K - 1.0f) * norm;
	dram->lowpass[hilp_d2] = (1.0f - K / 0.70710678f + K * K) * norm;
	norm = 1.0f / (1.0f + K / 0.51763809f + K * K);
	dram->lowpass[hilp_e0] = K * K * norm;
	dram->lowpass[hilp_e1] = 2.0f * dram->lowpass[hilp_e0];
	dram->lowpass[hilp_f1] = 2.0f * (K * K - 1.0f) * norm;
	dram->lowpass[hilp_f2] = (1.0f - K / 0.51763809f + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		if (highpassEngage) { //begin Stacked Highpass
			dram->highpass[hilp_temp] = (inputSample*dram->highpass[hilp_a0])+dram->highpass[hilp_aL1];
			dram->highpass[hilp_aL1] = (inputSample*dram->highpass[hilp_a1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_b1])+dram->highpass[hilp_aL2];
			dram->highpass[hilp_aL2] = (inputSample*dram->highpass[hilp_a0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_b2]); inputSample = dram->highpass[hilp_temp];
			dram->highpass[hilp_temp] = (inputSample*dram->highpass[hilp_c0])+dram->highpass[hilp_cL1];
			dram->highpass[hilp_cL1] = (inputSample*dram->highpass[hilp_c1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_d1])+dram->highpass[hilp_cL2];
			dram->highpass[hilp_cL2] = (inputSample*dram->highpass[hilp_c0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_d2]); inputSample = dram->highpass[hilp_temp];
			dram->highpass[hilp_temp] = (inputSample*dram->highpass[hilp_e0])+dram->highpass[hilp_eL1];
			dram->highpass[hilp_eL1] = (inputSample*dram->highpass[hilp_e1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_f1])+dram->highpass[hilp_eL2];
			dram->highpass[hilp_eL2] = (inputSample*dram->highpass[hilp_e0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_f2]); inputSample = dram->highpass[hilp_temp];
		} else {
			dram->highpass[hilp_aL1] = dram->highpass[hilp_aL2] = 0.0f;
		} //end Stacked Highpass
		
		//rest of control strip goes here
		
		if (lowpassEngage) { //begin Stacked Lowpass
			dram->lowpass[hilp_temp] = (inputSample*dram->lowpass[hilp_a0])+dram->lowpass[hilp_aL1];
			dram->lowpass[hilp_aL1] = (inputSample*dram->lowpass[hilp_a1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_b1])+dram->lowpass[hilp_aL2];
			dram->lowpass[hilp_aL2] = (inputSample*dram->lowpass[hilp_a0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_b2]); inputSample = dram->lowpass[hilp_temp];
			dram->lowpass[hilp_temp] = (inputSample*dram->lowpass[hilp_c0])+dram->lowpass[hilp_cL1];
			dram->lowpass[hilp_cL1] = (inputSample*dram->lowpass[hilp_c1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_d1])+dram->lowpass[hilp_cL2];
			dram->lowpass[hilp_cL2] = (inputSample*dram->lowpass[hilp_c0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_d2]); inputSample = dram->lowpass[hilp_temp];
			dram->lowpass[hilp_temp] = (inputSample*dram->lowpass[hilp_e0])+dram->lowpass[hilp_eL1];
			dram->lowpass[hilp_eL1] = (inputSample*dram->lowpass[hilp_e1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_f1])+dram->lowpass[hilp_eL2];
			dram->lowpass[hilp_eL2] = (inputSample*dram->lowpass[hilp_e0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_f2]); inputSample = dram->lowpass[hilp_temp];
		} //end Stacked Lowpass
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < hilp_total; x++) {
		dram->highpass[x] = 0.0;
		dram->lowpass[x] = 0.0;
	}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
