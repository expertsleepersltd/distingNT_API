#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "HipCrush"
#define AIRWINDOWS_DESCRIPTION "The de-rez section for ConsoleH."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','i','p' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_TRF =0,
	kParam_TRG =1,
	kParam_TRB =2,
	kParam_HMF =3,
	kParam_HMG =4,
	kParam_HMB =5,
	kParam_LMF =6,
	kParam_LMG =7,
	kParam_LMB =8,
	kParam_DW = 9,
	//Add your parameters here...
	kNumberOfParameters=10
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Hi Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "High", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HiCrush", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "MidFreq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "MdCrush", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Lo Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Low", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LoCrush", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		
		enum { 
			biqs_freq, biqs_reso, biqs_level,
			biqs_temp, biqs_bit,
			biqs_a0, biqs_a1, biqs_b1, biqs_b2,
			biqs_c0, biqs_c1, biqs_d1, biqs_d2,
			biqs_aL1, biqs_aL2, biqs_aR1, biqs_aR2,
			biqs_cL1, biqs_cL2, biqs_cR1, biqs_cR2,
			biqs_outL, biqs_outR, biqs_total
		};
		
		uint32_t fpd;
	
	struct _dram {
			float high[biqs_total];
		float hmid[biqs_total];
		float lmid[biqs_total];
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
	
	dram->high[biqs_freq] = (((powf(GetParameter( kParam_TRF ),3)*16000.0f)+1000.0f)/GetSampleRate());
	if (dram->high[biqs_freq] < 0.0001f) dram->high[biqs_freq] = 0.0001f;
	dram->high[biqs_bit] = (GetParameter( kParam_TRB )*2.0f)-1.0f;
	dram->high[biqs_level] = (1.0f-powf(1.0f-GetParameter( kParam_TRG ),2.0f))*1.618033988749894848204586f;
	dram->high[biqs_reso] = powf(GetParameter( kParam_TRG )+0.618033988749894848204586f,2.0f);
	float K = tan(M_PI * dram->high[biqs_freq]);
	float norm = 1.0f / (1.0f + K / (dram->high[biqs_reso]*0.618033988749894848204586f) + K * K);
	dram->high[biqs_a0] = K / (dram->high[biqs_reso]*0.618033988749894848204586f) * norm;
	dram->high[biqs_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->high[biqs_b2] = (1.0f - K / (dram->high[biqs_reso]*0.618033988749894848204586f) + K * K) * norm;
	norm = 1.0f / (1.0f + K / (dram->high[biqs_reso]*1.618033988749894848204586f) + K * K);
	dram->high[biqs_c0] = K / (dram->high[biqs_reso]*1.618033988749894848204586f) * norm;
	dram->high[biqs_d1] = 2.0f * (K * K - 1.0f) * norm;
	dram->high[biqs_d2] = (1.0f - K / (dram->high[biqs_reso]*1.618033988749894848204586f) + K * K) * norm;
	//high
	
	dram->hmid[biqs_freq] = (((powf(GetParameter( kParam_HMF ),3)*7000.0f)+300.0f)/GetSampleRate());
	if (dram->hmid[biqs_freq] < 0.0001f) dram->hmid[biqs_freq] = 0.0001f;
	dram->hmid[biqs_bit] = (GetParameter( kParam_HMB )*2.0f)-1.0f;
	dram->hmid[biqs_level] = (1.0f-powf(1.0f-GetParameter( kParam_HMG ),2.0f))*1.618033988749894848204586f;
	dram->hmid[biqs_reso] = powf(GetParameter( kParam_HMG )+0.618033988749894848204586f,2.0f);
	K = tan(M_PI * dram->hmid[biqs_freq]);
	norm = 1.0f / (1.0f + K / (dram->hmid[biqs_reso]*0.618033988749894848204586f) + K * K);
	dram->hmid[biqs_a0] = K / (dram->hmid[biqs_reso]*0.618033988749894848204586f) * norm;
	dram->hmid[biqs_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hmid[biqs_b2] = (1.0f - K / (dram->hmid[biqs_reso]*0.618033988749894848204586f) + K * K) * norm;
	norm = 1.0f / (1.0f + K / (dram->hmid[biqs_reso]*1.618033988749894848204586f) + K * K);
	dram->hmid[biqs_c0] = K / (dram->hmid[biqs_reso]*1.618033988749894848204586f) * norm;
	dram->hmid[biqs_d1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hmid[biqs_d2] = (1.0f - K / (dram->hmid[biqs_reso]*1.618033988749894848204586f) + K * K) * norm;
	//hmid
	
	dram->lmid[biqs_freq] = (((powf(GetParameter( kParam_LMF ),3)*3000.0f)+20.0f)/GetSampleRate());
	if (dram->lmid[biqs_freq] < 0.00001f) dram->lmid[biqs_freq] = 0.00001f;
	dram->lmid[biqs_bit] = (GetParameter( kParam_LMB )*2.0f)-1.0f;
	dram->lmid[biqs_level] = (1.0f-powf(1.0f-GetParameter( kParam_LMG ),2.0f))*1.618033988749894848204586f;
	dram->lmid[biqs_reso] = powf(GetParameter( kParam_LMG )+0.618033988749894848204586f,2.0f);
	K = tan(M_PI * dram->lmid[biqs_freq]);
	norm = 1.0f / (1.0f + K / (dram->lmid[biqs_reso]*0.618033988749894848204586f) + K * K);
	dram->lmid[biqs_a0] = K / (dram->lmid[biqs_reso]*0.618033988749894848204586f) * norm;
	dram->lmid[biqs_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->lmid[biqs_b2] = (1.0f - K / (dram->lmid[biqs_reso]*0.618033988749894848204586f) + K * K) * norm;
	norm = 1.0f / (1.0f + K / (dram->lmid[biqs_reso]*1.618033988749894848204586f) + K * K);
	dram->lmid[biqs_c0] = K / (dram->lmid[biqs_reso]*1.618033988749894848204586f) * norm;
	dram->lmid[biqs_d1] = 2.0f * (K * K - 1.0f) * norm;
	dram->lmid[biqs_d2] = (1.0f - K / (dram->lmid[biqs_reso]*1.618033988749894848204586f) + K * K) * norm;
	//lmid
	
	float wet = GetParameter( kParam_DW );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		//begin Stacked Biquad With Reversed Neutron Flow L
		dram->high[biqs_outL] = inputSample * fabs(dram->high[biqs_level]);
		dram->high[biqs_temp] = (dram->high[biqs_outL] * dram->high[biqs_a0]) + dram->high[biqs_aL1];
		dram->high[biqs_aL1] = dram->high[biqs_aL2] - (dram->high[biqs_temp]*dram->high[biqs_b1]);
		dram->high[biqs_aL2] = (dram->high[biqs_outL] * -dram->high[biqs_a0]) - (dram->high[biqs_temp]*dram->high[biqs_b2]);
		dram->high[biqs_outL] = dram->high[biqs_temp];
		if (dram->high[biqs_bit] != 0.0f) {
			float bitFactor = dram->high[biqs_bit];
			bool crushGate = (bitFactor < 0.0f);
			bitFactor = powf(2.0f,fmin(fmax((1.0f-fabs(bitFactor))*16.0f,0.5f),16.0f));
			dram->high[biqs_outL] *= bitFactor;		
			dram->high[biqs_outL] = floor(dram->high[biqs_outL]+(crushGate?0.5f/bitFactor:0.0f));
			dram->high[biqs_outL] /= bitFactor;
		}
		dram->high[biqs_temp] = (dram->high[biqs_outL] * dram->high[biqs_c0]) + dram->high[biqs_cL1];
		dram->high[biqs_cL1] = dram->high[biqs_cL2] - (dram->high[biqs_temp]*dram->high[biqs_d1]);
		dram->high[biqs_cL2] = (dram->high[biqs_outL] * -dram->high[biqs_c0]) - (dram->high[biqs_temp]*dram->high[biqs_d2]);
		dram->high[biqs_outL] = dram->high[biqs_temp];
		dram->high[biqs_outL] *= dram->high[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		//begin Stacked Biquad With Reversed Neutron Flow L
		dram->hmid[biqs_outL] = inputSample * fabs(dram->hmid[biqs_level]);
		dram->hmid[biqs_temp] = (dram->hmid[biqs_outL] * dram->hmid[biqs_a0]) + dram->hmid[biqs_aL1];
		dram->hmid[biqs_aL1] = dram->hmid[biqs_aL2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_b1]);
		dram->hmid[biqs_aL2] = (dram->hmid[biqs_outL] * -dram->hmid[biqs_a0]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_b2]);
		dram->hmid[biqs_outL] = dram->hmid[biqs_temp];
		if (dram->hmid[biqs_bit] != 0.0f) {
			float bitFactor = dram->hmid[biqs_bit];
			bool crushGate = (bitFactor < 0.0f);
			bitFactor = powf(2.0f,fmin(fmax((1.0f-fabs(bitFactor))*16.0f,0.5f),16.0f));
			dram->hmid[biqs_outL] *= bitFactor;		
			dram->hmid[biqs_outL] = floor(dram->hmid[biqs_outL]+(crushGate?0.5f/bitFactor:0.0f));
			dram->hmid[biqs_outL] /= bitFactor;
		}
		dram->hmid[biqs_temp] = (dram->hmid[biqs_outL] * dram->hmid[biqs_c0]) + dram->hmid[biqs_cL1];
		dram->hmid[biqs_cL1] = dram->hmid[biqs_cL2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_d1]);
		dram->hmid[biqs_cL2] = (dram->hmid[biqs_outL] * -dram->hmid[biqs_c0]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_d2]);
		dram->hmid[biqs_outL] = dram->hmid[biqs_temp];
		dram->hmid[biqs_outL] *= dram->hmid[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		//begin Stacked Biquad With Reversed Neutron Flow L
		dram->lmid[biqs_outL] = inputSample * fabs(dram->lmid[biqs_level]);
		dram->lmid[biqs_temp] = (dram->lmid[biqs_outL] * dram->lmid[biqs_a0]) + dram->lmid[biqs_aL1];
		dram->lmid[biqs_aL1] = dram->lmid[biqs_aL2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_b1]);
		dram->lmid[biqs_aL2] = (dram->lmid[biqs_outL] * -dram->lmid[biqs_a0]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_b2]);
		dram->lmid[biqs_outL] = dram->lmid[biqs_temp];
		if (dram->lmid[biqs_bit] != 0.0f) {
			float bitFactor = dram->lmid[biqs_bit];
			bool crushGate = (bitFactor < 0.0f);
			bitFactor = powf(2.0f,fmin(fmax((1.0f-fabs(bitFactor))*16.0f,0.5f),16.0f));
			dram->lmid[biqs_outL] *= bitFactor;		
			dram->lmid[biqs_outL] = floor(dram->lmid[biqs_outL]+(crushGate?0.5f/bitFactor:0.0f));
			dram->lmid[biqs_outL] /= bitFactor;
		}
		dram->lmid[biqs_temp] = (dram->lmid[biqs_outL] * dram->lmid[biqs_c0]) + dram->lmid[biqs_cL1];
		dram->lmid[biqs_cL1] = dram->lmid[biqs_cL2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_d1]);
		dram->lmid[biqs_cL2] = (dram->lmid[biqs_outL] * -dram->lmid[biqs_c0]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_d2]);
		dram->lmid[biqs_outL] = dram->lmid[biqs_temp];
		dram->lmid[biqs_outL] *= dram->lmid[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		float parametric = dram->high[biqs_outL] + dram->hmid[biqs_outL] + dram->lmid[biqs_outL];
		inputSample = (drySample * (1.0f-wet)) + (parametric * wet);
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < biqs_total; x++) {
		dram->high[x] = 0.0;
		dram->hmid[x] = 0.0;
		dram->lmid[x] = 0.0;
	}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
