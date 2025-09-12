#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SmoothEQ2"
#define AIRWINDOWS_DESCRIPTION "A wildly intuitive vibe EQ with clarity and intensity."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','m','q' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
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
{ .name = "High", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HMid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LMid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HighF", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HMidF", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LMidF", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "BassF", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
		float highIIR;
		
		float midIIR;
		
		float lowIIR;
				
		uint32_t fpd;
	
	struct _dram {
			float highA[biq_total];
		float highB[biq_total];
		float highC[biq_total];
		float midA[biq_total];
		float midB[biq_total];
		float midC[biq_total];
		float lowA[biq_total];
		float lowB[biq_total];
		float lowC[biq_total];
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
	float highmidGain = (GetParameter( kParam_B )-0.5f)*2.0f;
	highmidGain = 1.0f+(highmidGain*fabs(highmidGain)*fabs(highmidGain));
	float lowmidGain = (GetParameter( kParam_C )-0.5f)*2.0f;
	lowmidGain = 1.0f+(lowmidGain*fabs(lowmidGain)*fabs(lowmidGain));
	float bassGain = (GetParameter( kParam_D )-0.5f)*2.0f;
	bassGain = 1.0f+(bassGain*fabs(bassGain)*fabs(bassGain));
	
	float trebleRef = GetParameter( kParam_E )-0.5f;
	float highmidRef = GetParameter( kParam_F )-0.5f;
	float lowmidRef = GetParameter( kParam_G )-0.5f;
	float bassRef = GetParameter( kParam_H )-0.5f;
	float highF = 0.75f + ((trebleRef+trebleRef+trebleRef+highmidRef)*0.125f);
	float bassF = 0.25f + ((lowmidRef+bassRef+bassRef+bassRef)*0.125f);
	float midF = (highF*0.5f) + (bassF*0.5f) + ((highmidRef+lowmidRef)*0.125f);
	
	float highQ = fmax(fmin(1.0f+(highmidRef-trebleRef),4.0f),0.125f);
	float midQ = fmax(fmin(1.0f+(lowmidRef-highmidRef),4.0f),0.125f);
	float lowQ = fmax(fmin(1.0f+(bassRef-lowmidRef),4.0f),0.125f);
	
	dram->highA[biq_freq] = ((powf(highF,3)*20000.0f)/GetSampleRate());
	dram->highC[biq_freq] = dram->highB[biq_freq] = dram->highA[biq_freq] = fmax(fmin(dram->highA[biq_freq],0.4999f),0.00025f);
	float highFreq = powf(highF,3)*20000.0f;
	float omega = 2.0f*M_PI*(highFreq/GetSampleRate());
	float K = 2.0f-cos(omega);
	float highCoef = -sqrt((K*K)-1.0f)+K;
	dram->highA[biq_reso] = 2.24697960f * highQ;
	dram->highB[biq_reso] = 0.80193774f * highQ;
	dram->highC[biq_reso] = 0.55495813f * highQ;
	
	dram->midA[biq_freq] = ((powf(midF,3)*20000.0f)/GetSampleRate());
	dram->midC[biq_freq] = dram->midB[biq_freq] = dram->midA[biq_freq] = fmax(fmin(dram->midA[biq_freq],0.4999f),0.00025f);	
	float midFreq = powf(midF,3)*20000.0f;
	omega = 2.0f*M_PI*(midFreq/GetSampleRate());
	K = 2.0f-cos(omega);
	float midCoef = -sqrt((K*K)-1.0f)+K;
	dram->midA[biq_reso] = 2.24697960f * midQ;
	dram->midB[biq_reso] = 0.80193774f * midQ;
	dram->midC[biq_reso] = 0.55495813f * midQ;
	
	dram->lowA[biq_freq] = ((powf(bassF,3)*20000.0f)/GetSampleRate());
	dram->lowC[biq_freq] = dram->lowB[biq_freq] = dram->lowA[biq_freq] = fmax(fmin(dram->lowA[biq_freq],0.4999f),0.00025f);
	float lowFreq = powf(bassF,3)*20000.0f;
	omega = 2.0f*M_PI*(lowFreq/GetSampleRate());
	K = 2.0f-cos(omega);
	float lowCoef = -sqrt((K*K)-1.0f)+K;
	dram->lowA[biq_reso] = 2.24697960f * lowQ;
	dram->lowB[biq_reso] = 0.80193774f * lowQ;
	dram->lowC[biq_reso] = 0.55495813f * lowQ;
	
	K = tan(M_PI * dram->highA[biq_freq]);
	float norm = 1.0f / (1.0f + K / dram->highA[biq_reso] + K * K);
	dram->highA[biq_a0] = K * K * norm;
	dram->highA[biq_a1] = 2.0f * dram->highA[biq_a0];
	dram->highA[biq_a2] = dram->highA[biq_a0];
	dram->highA[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->highA[biq_b2] = (1.0f - K / dram->highA[biq_reso] + K * K) * norm;
	K = tan(M_PI * dram->highB[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->highB[biq_reso] + K * K);
	dram->highB[biq_a0] = K * K * norm;
	dram->highB[biq_a1] = 2.0f * dram->highB[biq_a0];
	dram->highB[biq_a2] = dram->highB[biq_a0];
	dram->highB[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->highB[biq_b2] = (1.0f - K / dram->highB[biq_reso] + K * K) * norm;
	K = tan(M_PI * dram->highC[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->highC[biq_reso] + K * K);
	dram->highC[biq_a0] = K * K * norm;
	dram->highC[biq_a1] = 2.0f * dram->highC[biq_a0];
	dram->highC[biq_a2] = dram->highC[biq_a0];
	dram->highC[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->highC[biq_b2] = (1.0f - K / dram->highC[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->midA[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->midA[biq_reso] + K * K);
	dram->midA[biq_a0] = K * K * norm;
	dram->midA[biq_a1] = 2.0f * dram->midA[biq_a0];
	dram->midA[biq_a2] = dram->midA[biq_a0];
	dram->midA[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->midA[biq_b2] = (1.0f - K / dram->midA[biq_reso] + K * K) * norm;
	K = tan(M_PI * dram->midB[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->midB[biq_reso] + K * K);
	dram->midB[biq_a0] = K * K * norm;
	dram->midB[biq_a1] = 2.0f * dram->midB[biq_a0];
	dram->midB[biq_a2] = dram->midB[biq_a0];
	dram->midB[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->midB[biq_b2] = (1.0f - K / dram->midB[biq_reso] + K * K) * norm;
	K = tan(M_PI * dram->midC[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->midC[biq_reso] + K * K);
	dram->midC[biq_a0] = K * K * norm;
	dram->midC[biq_a1] = 2.0f * dram->midC[biq_a0];
	dram->midC[biq_a2] = dram->midC[biq_a0];
	dram->midC[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->midC[biq_b2] = (1.0f - K / dram->midC[biq_reso] + K * K) * norm;
	
	K = tan(M_PI * dram->lowA[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->lowA[biq_reso] + K * K);
	dram->lowA[biq_a0] = K * K * norm;
	dram->lowA[biq_a1] = 2.0f * dram->lowA[biq_a0];
	dram->lowA[biq_a2] = dram->lowA[biq_a0];
	dram->lowA[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->lowA[biq_b2] = (1.0f - K / dram->lowA[biq_reso] + K * K) * norm;
	K = tan(M_PI * dram->lowB[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->lowB[biq_reso] + K * K);
	dram->lowB[biq_a0] = K * K * norm;
	dram->lowB[biq_a1] = 2.0f * dram->lowB[biq_a0];
	dram->lowB[biq_a2] = dram->lowB[biq_a0];
	dram->lowB[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->lowB[biq_b2] = (1.0f - K / dram->lowB[biq_reso] + K * K) * norm;
	K = tan(M_PI * dram->lowC[biq_freq]);
	norm = 1.0f / (1.0f + K / dram->lowC[biq_reso] + K * K);
	dram->lowC[biq_a0] = K * K * norm;
	dram->lowC[biq_a1] = 2.0f * dram->lowC[biq_a0];
	dram->lowC[biq_a2] = dram->lowC[biq_a0];
	dram->lowC[biq_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->lowC[biq_b2] = (1.0f - K / dram->lowC[biq_reso] + K * K) * norm;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		
		float trebleL = inputSampleL;		
		float outSample = (trebleL * dram->highA[biq_a0]) + dram->highA[biq_sL1];
		dram->highA[biq_sL1] = (trebleL * dram->highA[biq_a1]) - (outSample * dram->highA[biq_b1]) + dram->highA[biq_sL2];
		dram->highA[biq_sL2] = (trebleL * dram->highA[biq_a2]) - (outSample * dram->highA[biq_b2]);
		float highmidL = outSample; trebleL -= highmidL;
		
		outSample = (highmidL * dram->midA[biq_a0]) + dram->midA[biq_sL1];
		dram->midA[biq_sL1] = (highmidL * dram->midA[biq_a1]) - (outSample * dram->midA[biq_b1]) + dram->midA[biq_sL2];
		dram->midA[biq_sL2] = (highmidL * dram->midA[biq_a2]) - (outSample * dram->midA[biq_b2]);
		float lowmidL = outSample; highmidL -= lowmidL;
		
		outSample = (lowmidL * dram->lowA[biq_a0]) + dram->lowA[biq_sL1];
		dram->lowA[biq_sL1] = (lowmidL * dram->lowA[biq_a1]) - (outSample * dram->lowA[biq_b1]) + dram->lowA[biq_sL2];
		dram->lowA[biq_sL2] = (lowmidL * dram->lowA[biq_a2]) - (outSample * dram->lowA[biq_b2]);
		float bassL = outSample; lowmidL -= bassL;
		
		trebleL = (bassL*bassGain) + (lowmidL*lowmidGain) + (highmidL*highmidGain) + (trebleL*trebleGain);
		//first stage of three crossovers
		
		outSample = (trebleL * dram->highB[biq_a0]) + dram->highB[biq_sL1];
		dram->highB[biq_sL1] = (trebleL * dram->highB[biq_a1]) - (outSample * dram->highB[biq_b1]) + dram->highB[biq_sL2];
		dram->highB[biq_sL2] = (trebleL * dram->highB[biq_a2]) - (outSample * dram->highB[biq_b2]);
		highmidL = outSample; trebleL -= highmidL;
		
		outSample = (highmidL * dram->midB[biq_a0]) + dram->midB[biq_sL1];
		dram->midB[biq_sL1] = (highmidL * dram->midB[biq_a1]) - (outSample * dram->midB[biq_b1]) + dram->midB[biq_sL2];
		dram->midB[biq_sL2] = (highmidL * dram->midB[biq_a2]) - (outSample * dram->midB[biq_b2]);
		lowmidL = outSample; highmidL -= lowmidL;
		
		outSample = (lowmidL * dram->lowB[biq_a0]) + dram->lowB[biq_sL1];
		dram->lowB[biq_sL1] = (lowmidL * dram->lowB[biq_a1]) - (outSample * dram->lowB[biq_b1]) + dram->lowB[biq_sL2];
		dram->lowB[biq_sL2] = (lowmidL * dram->lowB[biq_a2]) - (outSample * dram->lowB[biq_b2]);
		bassL = outSample; lowmidL -= bassL;
		
		trebleL = (bassL*bassGain) + (lowmidL*lowmidGain) + (highmidL*highmidGain) + (trebleL*trebleGain);
		//second stage of three crossovers
		
		outSample = (trebleL * dram->highC[biq_a0]) + dram->highC[biq_sL1];
		dram->highC[biq_sL1] = (trebleL * dram->highC[biq_a1]) - (outSample * dram->highC[biq_b1]) + dram->highC[biq_sL2];
		dram->highC[biq_sL2] = (trebleL * dram->highC[biq_a2]) - (outSample * dram->highC[biq_b2]);
		highmidL = outSample; trebleL -= highmidL;
		
		outSample = (highmidL * dram->midC[biq_a0]) + dram->midC[biq_sL1];
		dram->midC[biq_sL1] = (highmidL * dram->midC[biq_a1]) - (outSample * dram->midC[biq_b1]) + dram->midC[biq_sL2];
		dram->midC[biq_sL2] = (highmidL * dram->midC[biq_a2]) - (outSample * dram->midC[biq_b2]);
		lowmidL = outSample; highmidL -= lowmidL;
		
		outSample = (lowmidL * dram->lowC[biq_a0]) + dram->lowC[biq_sL1];
		dram->lowC[biq_sL1] = (lowmidL * dram->lowC[biq_a1]) - (outSample * dram->lowC[biq_b1]) + dram->lowC[biq_sL2];
		dram->lowC[biq_sL2] = (lowmidL * dram->lowC[biq_a2]) - (outSample * dram->lowC[biq_b2]);
		bassL = outSample; lowmidL -= bassL;
		
		trebleL = (bassL*bassGain) + (lowmidL*lowmidGain) + (highmidL*highmidGain) + (trebleL*trebleGain);
		//third stage of three crossovers
		
		highIIR = (highIIR*highCoef) + (trebleL*(1.0f-highCoef));
		highmidL = highIIR; trebleL -= highmidL;
		
		midIIR = (midIIR*midCoef) + (highmidL*(1.0f-midCoef));
		lowmidL = midIIR; highmidL -= lowmidL;
		
		lowIIR = (lowIIR*lowCoef) + (lowmidL*(1.0f-lowCoef));
		bassL = lowIIR; lowmidL -= bassL;
		
		inputSampleL = (bassL*bassGain) + (lowmidL*lowmidGain) + (highmidL*highmidGain) + (trebleL*trebleGain);		
		//fourth stage of three crossovers is the exponential filters
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < biq_total; x++) {
		dram->highA[x] = 0.0;
		dram->highB[x] = 0.0;
		dram->highC[x] = 0.0;
		dram->midA[x] = 0.0;
		dram->midB[x] = 0.0;
		dram->midC[x] = 0.0;
		dram->lowA[x] = 0.0;
		dram->lowB[x] = 0.0;
		dram->lowC[x] = 0.0;
	}
	highIIR = 0.0;
	midIIR = 0.0;
	lowIIR = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
