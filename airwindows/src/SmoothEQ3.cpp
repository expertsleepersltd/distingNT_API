#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SmoothEQ3"
#define AIRWINDOWS_DESCRIPTION "SmoothEQ3"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','m','r' )
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
{ .name = "High", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
		};
		float highFastIIR;
		float lowFastIIR;
		
		uint32_t fpd;
	
	struct _dram {
			float highFast[biq_total];
		float lowFast[biq_total];
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
	
	float trebleGain = (GetParameter( kParam_A )-0.5f)*2.0f;
	trebleGain = 1.0f+(trebleGain*fabs(trebleGain)*fabs(trebleGain));
	float midGain = (GetParameter( kParam_B )-0.5f)*2.0f;
	midGain = 1.0f+(midGain*fabs(midGain)*fabs(midGain));
	float bassGain = (GetParameter( kParam_C )-0.5f)*2.0f;
	bassGain = 1.0f+(bassGain*fabs(bassGain)*fabs(bassGain));
	//separate from filtering stage, this is amplitude, centered on 1.0f unity gain
	
	//SmoothEQ3 is how to get 3rd order steepness at very low CPU.
	//because sample rate varies, you could also vary the crossovers
	//you can't vary Q because math is simplified to take advantage of
	//how the accurate Q value for this filter is always exactly 1.0f.
	dram->highFast[biq_freq] = (4000.0f/GetSampleRate());
	float omega = 2.0f*M_PI*(4000.0f/GetSampleRate()); //mid-high crossover freq
	float K = 2.0f - cos(omega);
	float highCoef = -sqrt(K*K - 1.0f) + K;
	dram->lowFast[biq_freq] = (200.0f/GetSampleRate());
	omega = 2.0f*M_PI*(200.0f/GetSampleRate()); //low-mid crossover freq
	K = 2.0f - cos(omega);
	float lowCoef = -sqrt(K*K - 1.0f) + K;
	//exponential IIR filter as part of an accurate 3rd order Butterworth filter 
	K = tan(M_PI * dram->highFast[biq_freq]);
	float norm = 1.0f / (1.0f + K + K*K);
	dram->highFast[biq_a0] = K * K * norm;
	dram->highFast[biq_a1] = 2.0f * dram->highFast[biq_a0];
	dram->highFast[biq_a2] = dram->highFast[biq_a0];
	dram->highFast[biq_b1] = 2.0f * (K*K - 1.0f) * norm;
	dram->highFast[biq_b2] = (1.0f - K + K*K) * norm;
	K = tan(M_PI * dram->lowFast[biq_freq]);
	norm = 1.0f / (1.0f + K + K*K);
	dram->lowFast[biq_a0] = K * K * norm;
	dram->lowFast[biq_a1] = 2.0f * dram->lowFast[biq_a0];
	dram->lowFast[biq_a2] = dram->lowFast[biq_a0];
	dram->lowFast[biq_b1] = 2.0f * (K*K - 1.0f) * norm;
	dram->lowFast[biq_b2] = (1.0f - K + K*K) * norm;
	//custom biquad setup with Q = 1.0f gets to omit some divides
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		
		float trebleFastL = inputSampleL;		
		float outSample = (trebleFastL * dram->highFast[biq_a0]) + dram->highFast[biq_sL1];
		dram->highFast[biq_sL1] = (trebleFastL * dram->highFast[biq_a1]) - (outSample * dram->highFast[biq_b1]) + dram->highFast[biq_sL2];
		dram->highFast[biq_sL2] = (trebleFastL * dram->highFast[biq_a2]) - (outSample * dram->highFast[biq_b2]);
		float midFastL = outSample; trebleFastL -= midFastL;
		outSample = (midFastL * dram->lowFast[biq_a0]) + dram->lowFast[biq_sL1];
		dram->lowFast[biq_sL1] = (midFastL * dram->lowFast[biq_a1]) - (outSample * dram->lowFast[biq_b1]) + dram->lowFast[biq_sL2];
		dram->lowFast[biq_sL2] = (midFastL * dram->lowFast[biq_a2]) - (outSample * dram->lowFast[biq_b2]);
		float bassFastL = outSample; midFastL -= bassFastL;
		trebleFastL = (bassFastL*bassGain) + (midFastL*midGain) + (trebleFastL*trebleGain);
		//first stage of two crossovers is biquad of exactly 1.0f Q
		highFastIIR = (highFastIIR*highCoef) + (trebleFastL*(1.0f-highCoef));
		midFastL = highFastIIR; trebleFastL -= midFastL;
		lowFastIIR = (lowFastIIR*lowCoef) + (midFastL*(1.0f-lowCoef));
		bassFastL = lowFastIIR; midFastL -= bassFastL;
		inputSampleL = (bassFastL*bassGain) + (midFastL*midGain) + (trebleFastL*trebleGain);		
		//second stage of two crossovers is the exponential filters
		//this produces a slightly steeper Butterworth filter very cheaply
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < biq_total; x++) {
		dram->highFast[x] = 0.0;
		dram->lowFast[x] = 0.0;
	}
	highFastIIR = 0.0;
	lowFastIIR = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
