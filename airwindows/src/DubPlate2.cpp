#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DubPlate2"
#define AIRWINDOWS_DESCRIPTION "Like an ITB mastering house for electronic music."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','u','2' )
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
	float iirA;
	float iirB; //first stage is the flipping one, for lowest slope. It is always engaged, and is the highest one
	float iirC; //we introduce the second pole at the same frequency, to become a pseudo-Capacitor behavior
	float iirD; //then there's a Mid highpass.
	float lastSinewAL;
	float lastSinewAR;
	float lastSinewBL;
	float lastSinewBR;
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
	
	float iirSide = 0.01862f / overallscale;
	float iirMid = 0.01102f / overallscale;
	float threshSinewA = 0.1442f / overallscale;
	float threshSinewB = 0.0274f / overallscale;	
	
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
		
		trebleSampleL *= trebleGain;
		trebleSampleR *= trebleGain;
		bassSampleL *= bassGain;
		bassSampleR *= bassGain;
		inputSampleL = bassSampleL + trebleSampleL; //interleaved biquad
		inputSampleR = bassSampleR + trebleSampleR; //interleaved biquad
		
		inputSampleL = asin(fmax(fmin(inputSampleL,0.99999f),-0.99999f));
		inputSampleR = asin(fmax(fmin(inputSampleR,0.99999f),-0.99999f));
		//amplitude aspect
		
		float mid = inputSampleL + inputSampleR;
		float side = inputSampleL - inputSampleR;
		//assign mid and side.Between these sections, you can do mid/side processing
		float temp = side;
		float correction;
		
		if (flip) {
			iirA = (iirA * (1.0f - iirSide)) + (temp * iirSide); temp -= iirA; correction = iirA;
		} else {
			iirB = (iirB * (1.0f - iirSide)) + (temp * iirSide); temp -= iirB; correction = iirB;
		}
		iirC = (iirC * (1.0f - iirSide)) + (temp * iirSide); temp -= iirC; correction += (iirC * 0.162f);

		side -= sin(correction);
		
		iirD = (iirD * (1.0f - iirMid)) + (mid * iirMid);
		mid -= sin(iirD);
		//gonna cut those lows a hair
		
		inputSampleL = (mid+side)/2.0f;
		inputSampleR = (mid-side)/2.0f;
		//unassign mid and side
		
		temp = inputSampleL;
		float sinew = threshSinewA * cos(lastSinewAL*lastSinewAL);
		if (inputSampleL - lastSinewAL > sinew) temp = lastSinewAL + sinew;
		if (-(inputSampleL - lastSinewAL) > sinew) temp = lastSinewAL - sinew;
		lastSinewAL = temp;
		if (lastSinewAL > 1.0f) lastSinewAL = 1.0f;
		if (lastSinewAL < -1.0f) lastSinewAL = -1.0f;
		inputSampleL = (inputSampleL * 0.5f)+(lastSinewAL * 0.5f);
		sinew = threshSinewB * cos(lastSinewBL*lastSinewBL);
		if (inputSampleL - lastSinewBL > sinew) temp = lastSinewBL + sinew;
		if (-(inputSampleL - lastSinewBL) > sinew) temp = lastSinewBL - sinew;
		lastSinewBL = temp;
		if (lastSinewBL > 1.0f) lastSinewBL = 1.0f;
		if (lastSinewBL < -1.0f) lastSinewBL = -1.0f;
		inputSampleL = (inputSampleL * 0.5f)+(lastSinewBL * 0.5f);
		
		temp = inputSampleR;
		sinew = threshSinewA * cos(lastSinewAR*lastSinewAR);
		if (inputSampleR - lastSinewAR > sinew) temp = lastSinewAR + sinew;
		if (-(inputSampleR - lastSinewAR) > sinew) temp = lastSinewAR - sinew;
		lastSinewAR = temp;
		if (lastSinewAR > 1.0f) lastSinewAR = 1.0f;
		if (lastSinewAR < -1.0f) lastSinewAR = -1.0f;
		inputSampleR = (inputSampleR * 0.5f)+(lastSinewAR * 0.5f);
		sinew = threshSinewB * cos(lastSinewBR*lastSinewBR);
		if (inputSampleR - lastSinewBR > sinew) temp = lastSinewBR + sinew;
		if (-(inputSampleR - lastSinewBR) > sinew) temp = lastSinewBR - sinew;
		lastSinewBR = temp;
		if (lastSinewBR > 1.0f) lastSinewBR = 1.0f;
		if (lastSinewBR < -1.0f) lastSinewBR = -1.0f;
		inputSampleR = (inputSampleR * 0.5f)+(lastSinewBR * 0.5f);
		//run Sinew to stop excess slews, two layers to make it more audible
		
		flip = !flip; //both for Baxandall3 and DubPlate highpass

		
		
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
	iirA = 0.0;
	iirB = 0.0;
	iirC = 0.0;
	iirD = 0.0;
	lastSinewAL = 0.0;
	lastSinewAR = 0.0;
	lastSinewBL = 0.0;
	lastSinewBR = 0.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
