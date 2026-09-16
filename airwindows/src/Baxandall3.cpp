#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Baxandall3"
#define AIRWINDOWS_DESCRIPTION "For new tone colors."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','a','3' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Input", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Treble", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	
	enum {
		bax_freq,
		bax_reso,
		bax_a0,
		bax_a1,
		bax_a2,
		bax_b1,
		bax_b2,
		bax_sLA1,
		bax_sLA2,
		bax_sRA1,
		bax_sRA2,
		bax_sLB1,
		bax_sLB2,
		bax_sRB1,
		bax_sRB2,
		bax_total
	}; //baxed frequency biquad filter for ultrasonics, stereo
	bool flip;	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float baxH[bax_total];
	float baxL[bax_total];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	float inputGain = powf(GetParameter( kParam_A )*2.0f,2.0f);
	float trebleGain = powf(GetParameter( kParam_B )*2.0f,2.0f);
	float trebleFreq = ((2000.0f*trebleGain)+200.0f)/GetSampleRate();
	if (trebleFreq > 0.45f) trebleFreq = 0.45f;
	dram->baxH[bax_freq] = trebleFreq;
    dram->baxH[bax_reso] = 0.57735026919f; //bessel second order
	float K = tan(M_PI * dram->baxH[bax_freq]); //lowpass
	float norm = 1.0f / (1.0f + K / dram->baxH[bax_reso] + K * K);
	dram->baxH[bax_a0] = K * K * norm;
	dram->baxH[bax_a1] = 2.0f * dram->baxH[bax_a0];
	dram->baxH[bax_a2] = dram->baxH[bax_a0];
	dram->baxH[bax_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->baxH[bax_b2] = (1.0f - K / dram->baxH[bax_reso] + K * K) * norm;
	//end bax highpass
	float bassGain = powf(GetParameter( kParam_C )*2.0f,2.0f);
	float bassFreq = powf((1.0f-GetParameter( kParam_C ))*2.0f,2.0f);
	bassFreq = ((2000.0f*bassFreq)+200.0f)/GetSampleRate();
	if (bassFreq > 0.45f) bassFreq = 0.45f;
	dram->baxL[bax_freq] = bassFreq;
    dram->baxL[bax_reso] = 0.57735026919f; //bessel second order
	K = tan(M_PI * dram->baxL[bax_freq]); //lowpass
	norm = 1.0f / (1.0f + K / dram->baxL[bax_reso] + K * K);
	dram->baxL[bax_a0] = K * K * norm;
	dram->baxL[bax_a1] = 2.0f * dram->baxL[bax_a0];
	dram->baxL[bax_a2] = dram->baxL[bax_a0];
	dram->baxL[bax_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->baxL[bax_b2] = (1.0f - K / dram->baxL[bax_reso] + K * K) * norm;
	//end bax lowpass
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		inputSampleL = sin(fmax(fmin(inputSampleL*inputGain,M_PI_2),-M_PI_2));
		inputSampleR = sin(fmax(fmin(inputSampleR*inputGain,M_PI_2),-M_PI_2));
		//encode Console5: good cleanness
		float trebleSampleL;
		float trebleSampleR;
		float bassSampleL;
		float bassSampleR;
		if (flip) {
			trebleSampleL = (inputSampleL * dram->baxH[bax_a0]) + dram->baxH[bax_sLA1];
			dram->baxH[bax_sLA1] = (inputSampleL * dram->baxH[bax_a1]) - (trebleSampleL * dram->baxH[bax_b1]) + dram->baxH[bax_sLA2];
			dram->baxH[bax_sLA2] = (inputSampleL * dram->baxH[bax_a2]) - (trebleSampleL * dram->baxH[bax_b2]);
			trebleSampleL = inputSampleL - trebleSampleL;
			trebleSampleR = (inputSampleR * dram->baxH[bax_a0]) + dram->baxH[bax_sRA1];
			dram->baxH[bax_sRA1] = (inputSampleR * dram->baxH[bax_a1]) - (trebleSampleR * dram->baxH[bax_b1]) + dram->baxH[bax_sRA2];
			dram->baxH[bax_sRA2] = (inputSampleR * dram->baxH[bax_a2]) - (trebleSampleR * dram->baxH[bax_b2]);
			trebleSampleR = inputSampleR - trebleSampleR;
			bassSampleL = (inputSampleL * dram->baxL[bax_a0]) + dram->baxL[bax_sLA1];
			dram->baxL[bax_sLA1] = (inputSampleL * dram->baxL[bax_a1]) - (bassSampleL * dram->baxL[bax_b1]) + dram->baxL[bax_sLA2];
			dram->baxL[bax_sLA2] = (inputSampleL * dram->baxL[bax_a2]) - (bassSampleL * dram->baxL[bax_b2]);
			bassSampleR = (inputSampleR * dram->baxL[bax_a0]) + dram->baxL[bax_sRA1];
			dram->baxL[bax_sRA1] = (inputSampleR * dram->baxL[bax_a1]) - (bassSampleR * dram->baxL[bax_b1]) + dram->baxL[bax_sRA2];
			dram->baxL[bax_sRA2] = (inputSampleR * dram->baxL[bax_a2]) - (bassSampleR * dram->baxL[bax_b2]);
		} else {
			trebleSampleL = (inputSampleL * dram->baxH[bax_a0]) + dram->baxH[bax_sLB1];
			dram->baxH[bax_sLB1] = (inputSampleL * dram->baxH[bax_a1]) - (trebleSampleL * dram->baxH[bax_b1]) + dram->baxH[bax_sLB2];
			dram->baxH[bax_sLB2] = (inputSampleL * dram->baxH[bax_a2]) - (trebleSampleL * dram->baxH[bax_b2]);
			trebleSampleL = inputSampleL - trebleSampleL;
			trebleSampleR = (inputSampleR * dram->baxH[bax_a0]) + dram->baxH[bax_sRB1];
			dram->baxH[bax_sRB1] = (inputSampleR * dram->baxH[bax_a1]) - (trebleSampleR * dram->baxH[bax_b1]) + dram->baxH[bax_sRB2];
			dram->baxH[bax_sRB2] = (inputSampleR * dram->baxH[bax_a2]) - (trebleSampleR * dram->baxH[bax_b2]);
			trebleSampleR = inputSampleR - trebleSampleR;
			bassSampleL = (inputSampleL * dram->baxL[bax_a0]) + dram->baxL[bax_sLB1];
			dram->baxL[bax_sLB1] = (inputSampleL * dram->baxL[bax_a1]) - (bassSampleL * dram->baxL[bax_b1]) + dram->baxL[bax_sLB2];
			dram->baxL[bax_sLB2] = (inputSampleL * dram->baxL[bax_a2]) - (bassSampleL * dram->baxL[bax_b2]);
			bassSampleR = (inputSampleR * dram->baxL[bax_a0]) + dram->baxL[bax_sRB1];
			dram->baxL[bax_sRB1] = (inputSampleR * dram->baxL[bax_a1]) - (bassSampleR * dram->baxL[bax_b1]) + dram->baxL[bax_sRB2];
			dram->baxL[bax_sRB2] = (inputSampleR * dram->baxL[bax_a2]) - (bassSampleR * dram->baxL[bax_b2]);
		}
		flip = !flip;
		
		trebleSampleL *= trebleGain;
		trebleSampleR *= trebleGain;
		bassSampleL *= bassGain;
		bassSampleR *= bassGain;
		inputSampleL = bassSampleL + trebleSampleL; //interleaved biquad
		inputSampleR = bassSampleR + trebleSampleR; //interleaved biquad
						   
		inputSampleL = asin(fmax(fmin(inputSampleL,0.99999f),-0.99999f));
		inputSampleR = asin(fmax(fmin(inputSampleR,0.99999f),-0.99999f));
		//amplitude aspect
		
		
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		//direct stereo out
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{	
	for (int x = 0; x < bax_total; x++) {dram->baxH[x] = 0.0;dram->baxL[x] = 0.0;}
	flip = false;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
