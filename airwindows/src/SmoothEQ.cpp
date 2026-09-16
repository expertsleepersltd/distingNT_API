#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SmoothEQ"
#define AIRWINDOWS_DESCRIPTION "A sharp, accurate, transparent three-band filter."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','m','Q' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "High", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Low", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "XoverH", .min = 0, .max = 1000, .def = 600, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "XoverL", .min = 0, .max = 1000, .def = 400, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		enum {
			biq_freq,
			biq_reso,
			biq_a0,
			biq_a1,
			biq_a2,
			biq_b1,
			biq_b2,
			biq_sL1,
			biq_sL2,
			biq_sR1,
			biq_sR2,
			biq_total
		}; //coefficient interpolating bessel filter, stereo
		
		uint32_t fpd;
	
	struct _dram {
			float besselA[biq_total];
		float besselB[biq_total];
		float besselC[biq_total];
		float besselD[biq_total];
		float besselE[biq_total];
		float besselF[biq_total];
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
	
	float trebleGain = GetParameter( kParam_A )*2.0f;
	float midGain = GetParameter( kParam_B )*2.0f;
	float bassGain = GetParameter( kParam_C )*2.0f; //amount ends up being powf(gain,3)
	//simple three band to adjust
	
	dram->besselA[biq_freq] = powf(GetParameter( kParam_D ),2) * (0.25f/overallscale) * 1.9047076123f;
	if (dram->besselA[biq_freq] < 0.00025f) dram->besselA[biq_freq] = 0.00025f;
	if (dram->besselA[biq_freq] > 0.4999f) dram->besselA[biq_freq] = 0.4999f;
	dram->besselA[biq_reso] = 1.02331395383f;
	dram->besselB[biq_freq] = powf(GetParameter( kParam_D ),2) * (0.25f/overallscale) * 1.68916826762f;
	if (dram->besselB[biq_freq] < 0.00025f) dram->besselB[biq_freq] = 0.00025f;
	if (dram->besselB[biq_freq] > 0.4999f) dram->besselB[biq_freq] = 0.4999f;
	dram->besselB[biq_reso] = 0.611194546878f;
	dram->besselC[biq_freq] = powf(GetParameter( kParam_D ),2) * (0.25f/overallscale) * 1.60391912877f;
	if (dram->besselC[biq_freq] < 0.00025f) dram->besselC[biq_freq] = 0.00025f;
	if (dram->besselC[biq_freq] > 0.4999f) dram->besselC[biq_freq] = 0.4999f;
	dram->besselC[biq_reso] = 0.510317824749f;
	
	dram->besselD[biq_freq] = powf(GetParameter( kParam_E ),4) * (0.25f/overallscale) * 1.9047076123f;
	if (dram->besselD[biq_freq] < 0.00025f) dram->besselD[biq_freq] = 0.00025f;
	if (dram->besselD[biq_freq] > 0.4999f) dram->besselD[biq_freq] = 0.4999f;
	dram->besselD[biq_reso] = 1.02331395383f;
	dram->besselE[biq_freq] = powf(GetParameter( kParam_E ),4) * (0.25f/overallscale) * 1.68916826762f;
	if (dram->besselE[biq_freq] < 0.00025f) dram->besselE[biq_freq] = 0.00025f;
	if (dram->besselE[biq_freq] > 0.4999f) dram->besselE[biq_freq] = 0.4999f;
	dram->besselE[biq_reso] = 0.611194546878f;
	dram->besselF[biq_freq] = powf(GetParameter( kParam_E ),4) * (0.25f/overallscale) * 1.60391912877f;
	if (dram->besselF[biq_freq] < 0.00025f) dram->besselF[biq_freq] = 0.00025f;
	if (dram->besselF[biq_freq] > 0.4999f) dram->besselF[biq_freq] = 0.4999f;
	dram->besselF[biq_reso] = 0.510317824749f;
		
	float K = tan(M_PI * dram->besselA[biq_freq]);
	float norm = 1.0f / (1.0f + K / dram->besselA[biq_reso] + K * K);
	dram->besselA[biq_a0] = K * K * norm;
	dram->besselA[biq_a1] = 2.0f * dram->besselA[biq_a0];
	dram->besselA[biq_a2] = dram->besselA[biq_a0];
	dram->besselA[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->besselA[biq_b2] = (1.0f - K / dram->besselA[biq_reso] + K * K) * norm;
	K = tan(M_PI * dram->besselB[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->besselB[biq_reso] + K * K);
	dram->besselB[biq_a0] = K * K * norm;
	dram->besselB[biq_a1] = 2.0f * dram->besselB[biq_a0];
	dram->besselB[biq_a2] = dram->besselB[biq_a0];
	dram->besselB[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->besselB[biq_b2] = (1.0f - K / dram->besselB[biq_reso] + K * K) * norm;
	K = tan(M_PI * dram->besselC[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->besselC[biq_reso] + K * K);
	dram->besselC[biq_a0] = K * K * norm;
	dram->besselC[biq_a1] = 2.0f * dram->besselC[biq_a0];
	dram->besselC[biq_a2] = dram->besselC[biq_a0];
	dram->besselC[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->besselC[biq_b2] = (1.0f - K / dram->besselC[biq_reso] + K * K) * norm;
	K = tan(M_PI * dram->besselD[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->besselD[biq_reso] + K * K);
	dram->besselD[biq_a0] = K * K * norm;
	dram->besselD[biq_a1] = 2.0f * dram->besselD[biq_a0];
	dram->besselD[biq_a2] = dram->besselD[biq_a0];
	dram->besselD[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->besselD[biq_b2] = (1.0f - K / dram->besselD[biq_reso] + K * K) * norm;
	K = tan(M_PI * dram->besselE[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->besselE[biq_reso] + K * K);
	dram->besselE[biq_a0] = K * K * norm;
	dram->besselE[biq_a1] = 2.0f * dram->besselE[biq_a0];
	dram->besselE[biq_a2] = dram->besselE[biq_a0];
	dram->besselE[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->besselE[biq_b2] = (1.0f - K / dram->besselE[biq_reso] + K * K) * norm;
	K = tan(M_PI * dram->besselF[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->besselF[biq_reso] + K * K);
	dram->besselF[biq_a0] = K * K * norm;
	dram->besselF[biq_a1] = 2.0f * dram->besselF[biq_a0];
	dram->besselF[biq_a2] = dram->besselF[biq_a0];
	dram->besselF[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->besselF[biq_b2] = (1.0f - K / dram->besselF[biq_reso] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		
		float trebleL = inputSampleL;		
		float outSample = (trebleL * dram->besselA[biq_a0]) + dram->besselA[biq_sL1];
		dram->besselA[biq_sL1] = (trebleL * dram->besselA[biq_a1]) - (outSample * dram->besselA[biq_b1]) + dram->besselA[biq_sL2];
		dram->besselA[biq_sL2] = (trebleL * dram->besselA[biq_a2]) - (outSample * dram->besselA[biq_b2]);
		float midL = outSample; trebleL -= midL;
		outSample = (midL * dram->besselD[biq_a0]) + dram->besselD[biq_sL1];
		dram->besselD[biq_sL1] = (midL * dram->besselD[biq_a1]) - (outSample * dram->besselD[biq_b1]) + dram->besselD[biq_sL2];
		dram->besselD[biq_sL2] = (midL * dram->besselD[biq_a2]) - (outSample * dram->besselD[biq_b2]);
		float bassL = outSample; midL -= bassL;
		trebleL = (bassL*bassGain) + (midL*midGain) + (trebleL*trebleGain);
		outSample = (trebleL * dram->besselB[biq_a0]) + dram->besselB[biq_sL1];
		dram->besselB[biq_sL1] = (trebleL * dram->besselB[biq_a1]) - (outSample * dram->besselB[biq_b1]) + dram->besselB[biq_sL2];
		dram->besselB[biq_sL2] = (trebleL * dram->besselB[biq_a2]) - (outSample * dram->besselB[biq_b2]);
		midL = outSample; trebleL -= midL;
		outSample = (midL * dram->besselE[biq_a0]) + dram->besselE[biq_sL1];
		dram->besselE[biq_sL1] = (midL * dram->besselE[biq_a1]) - (outSample * dram->besselE[biq_b1]) + dram->besselE[biq_sL2];
		dram->besselE[biq_sL2] = (midL * dram->besselE[biq_a2]) - (outSample * dram->besselE[biq_b2]);
		bassL = outSample; midL -= bassL;
		trebleL = (bassL*bassGain) + (midL*midGain) + (trebleL*trebleGain);
		outSample = (trebleL * dram->besselC[biq_a0]) + dram->besselC[biq_sL1];
		dram->besselC[biq_sL1] = (trebleL * dram->besselC[biq_a1]) - (outSample * dram->besselC[biq_b1]) + dram->besselC[biq_sL2];
		dram->besselC[biq_sL2] = (trebleL * dram->besselC[biq_a2]) - (outSample * dram->besselC[biq_b2]);
		midL = outSample; trebleL -= midL;
		outSample = (midL * dram->besselF[biq_a0]) + dram->besselF[biq_sL1];
		dram->besselF[biq_sL1] = (midL * dram->besselF[biq_a1]) - (outSample * dram->besselF[biq_b1]) + dram->besselF[biq_sL2];
		dram->besselF[biq_sL2] = (midL * dram->besselF[biq_a2]) - (outSample * dram->besselF[biq_b2]);
		bassL = outSample; midL -= bassL;
		inputSampleL = (bassL*bassGain) + (midL*midGain) + (trebleL*trebleGain);
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < biq_total; x++) {
		dram->besselA[x] = 0.0;
		dram->besselB[x] = 0.0;
		dram->besselC[x] = 0.0;
		dram->besselD[x] = 0.0;
		dram->besselE[x] = 0.0;
		dram->besselF[x] = 0.0;
	}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
