#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleHBuss"
#define AIRWINDOWS_DESCRIPTION "The Airwindows console for hip-hop and music reinvention."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','2' )
enum {

	kParam_HIG =0,
	kParam_MID =1,
	kParam_LOW =2,
	kParam_THR =3,
	kParam_LOP =4,
	kParam_HIP =5,
	kParam_PAN =6,
	kParam_FAD =7,
	//Add your parameters here...
	kNumberOfParameters=8
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "High", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Low", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Thresh", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Lowpass", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Hipass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Pan", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fader", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
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
	}; //coefficient interpolating filter, stereo
	float highFastLIIR;
	float highFastRIIR;
	float lowFastLIIR;
	float lowFastRIIR;
	//SmoothEQ3
		
	enum {
		bez_A,
		bez_B,
		bez_C,
		bez_Ctrl,
		bez_cycle,
		bez_total
	}; //the new undersampling. bez signifies the bezier curve reconstruction
	//Dynamics2 custom for buss
	
	enum {
		hilp_freq, hilp_temp,
		hilp_a0, hilp_aA0, hilp_aB0, hilp_a1, hilp_aA1, hilp_aB1, hilp_b1, hilp_bA1, hilp_bB1, hilp_b2, hilp_bA2, hilp_bB2,
		hilp_c0, hilp_cA0, hilp_cB0, hilp_c1, hilp_cA1, hilp_cB1, hilp_d1, hilp_dA1, hilp_dB1, hilp_d2, hilp_dA2, hilp_dB2,
		hilp_e0, hilp_eA0, hilp_eB0, hilp_e1, hilp_eA1, hilp_eB1, hilp_f1, hilp_fA1, hilp_fB1, hilp_f2, hilp_fA2, hilp_fB2,
		hilp_aL1, hilp_aL2, hilp_aR1, hilp_aR2,
		hilp_cL1, hilp_cL2, hilp_cR1, hilp_cR2,
		hilp_eL1, hilp_eL2, hilp_eR1, hilp_eR2,
		hilp_total
	};
	
	float panA;
	float panB;
	float inTrimA;
	float inTrimB;

	float avg8L[9];
	float avg8R[9];
	float avg4L[5];
	float avg4R[5];
	float avg2L[3];
	float avg2R[3];
	int avgPos;
	float lastSlewL;
	float lastSlewR;
	float lastSlewpleL;
	float lastSlewpleR;
	//preTapeHack
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float highFast[biq_total];
	float lowFast[biq_total];
	float bezCompF[bez_total];
	float bezCompS[bez_total];
	float highpass[hilp_total];
	float lowpass[hilp_total];	
	float avg32L[33];
	float avg32R[33];
	float avg16L[17];
	float avg16R[17];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	int spacing = floor(overallscale*2.0f);
	if (spacing < 2) spacing = 2; if (spacing > 32) spacing = 32;
	
	float trebleGain = (GetParameter( kParam_HIG )-0.5f)*2.0f;
	trebleGain = 1.0f+(trebleGain*fabs(trebleGain)*fabs(trebleGain));
	float midGain = (GetParameter( kParam_MID )-0.5f)*2.0f;
	midGain = 1.0f+(midGain*fabs(midGain)*fabs(midGain));
	float bassGain = (GetParameter( kParam_LOW )-0.5f)*2.0f;
	bassGain = 1.0f+(bassGain*fabs(bassGain)*fabs(bassGain));
	//separate from filtering stage, this is amplitude, centered on 1.0f unity gain
	float highCoef = 0.0f;
	float lowCoef = 0.0f;
	float omega = 0.0f;
	float biqK = 0.0f;
	float norm = 0.0f;
	
	bool eqOff = (trebleGain == 1.0f && midGain == 1.0f && bassGain == 1.0f);
	//we get to completely bypass EQ if we're truly not using it. The mechanics of it mean that
	//it cancels out to bit-identical anyhow, but we get to skip the calculation
	if (!eqOff) {
		//SmoothEQ3 is how to get 3rd order steepness at very low CPU.
		//because sample rate varies, you could also vary the crossovers
		//you can't vary Q because math is simplified to take advantage of
		//how the accurate Q value for this filter is always exactly 1.0f.
		dram->highFast[biq_freq] = (4000.0f/GetSampleRate());
		omega = 2.0f*M_PI*(4000.0f/GetSampleRate()); //mid-high crossover freq
		biqK = 2.0f - cos(omega);
		highCoef = -sqrt(biqK*biqK - 1.0f) + biqK;
		dram->lowFast[biq_freq] = (200.0f/GetSampleRate());
		omega = 2.0f*M_PI*(200.0f/GetSampleRate()); //low-mid crossover freq
		biqK = 2.0f - cos(omega);
		lowCoef = -sqrt(biqK*biqK - 1.0f) + biqK;
		//exponential IIR filter as part of an accurate 3rd order Butterworth filter 
		biqK = tan(M_PI * dram->highFast[biq_freq]);
		norm = 1.0f / (1.0f + biqK + biqK*biqK);
		dram->highFast[biq_a0] = biqK * biqK * norm;
		dram->highFast[biq_a1] = 2.0f * dram->highFast[biq_a0];
		dram->highFast[biq_a2] = dram->highFast[biq_a0];
		dram->highFast[biq_b1] = 2.0f * (biqK*biqK - 1.0f) * norm;
		dram->highFast[biq_b2] = (1.0f - biqK + biqK*biqK) * norm;
		biqK = tan(M_PI * dram->lowFast[biq_freq]);
		norm = 1.0f / (1.0f + biqK + biqK*biqK);
		dram->lowFast[biq_a0] = biqK * biqK * norm;
		dram->lowFast[biq_a1] = 2.0f * dram->lowFast[biq_a0];
		dram->lowFast[biq_a2] = dram->lowFast[biq_a0];
		dram->lowFast[biq_b1] = 2.0f * (biqK*biqK - 1.0f) * norm;
		dram->lowFast[biq_b2] = (1.0f - biqK + biqK*biqK) * norm;
		//custom biquad setup with Q = 1.0f gets to omit some divides
	}
	//SmoothEQ3
	
	float bezCThresh = powf(1.0f-GetParameter( kParam_THR ), 6.0f) * 8.0f;
	float bezRez = powf(1.0f-GetParameter( kParam_THR ), 12.360679774997898f) / overallscale;
	float sloRez = powf(1.0f-GetParameter( kParam_THR ),10.0f) / overallscale;
	sloRez = fmin(fmax(sloRez,0.00001f),1.0f);
	bezRez = fmin(fmax(bezRez,0.00001f),1.0f);
	//Dynamics2
	
	dram->highpass[hilp_freq] = ((powf(GetParameter( kParam_HIP ),3)*24000.0f)+10.0f)/GetSampleRate();
	if (dram->highpass[hilp_freq] > 0.495f) dram->highpass[hilp_freq] = 0.495f;
	bool highpassEngage = true; if (GetParameter( kParam_HIP ) == 0.0f) highpassEngage = false;
	
	dram->lowpass[hilp_freq] = ((powf(GetParameter( kParam_LOP ),3)*24000.0f)+10.0f)/GetSampleRate();
	if (dram->lowpass[hilp_freq] > 0.495f) dram->lowpass[hilp_freq] = 0.495f;
	bool lowpassEngage = true; if (GetParameter( kParam_LOP ) == 1.0f) lowpassEngage = false;
	
	dram->highpass[hilp_aA0] = dram->highpass[hilp_aB0];
	dram->highpass[hilp_aA1] = dram->highpass[hilp_aB1];
	dram->highpass[hilp_bA1] = dram->highpass[hilp_bB1];
	dram->highpass[hilp_bA2] = dram->highpass[hilp_bB2];
	dram->highpass[hilp_cA0] = dram->highpass[hilp_cB0];
	dram->highpass[hilp_cA1] = dram->highpass[hilp_cB1];
	dram->highpass[hilp_dA1] = dram->highpass[hilp_dB1];
	dram->highpass[hilp_dA2] = dram->highpass[hilp_dB2];
	dram->highpass[hilp_eA0] = dram->highpass[hilp_eB0];
	dram->highpass[hilp_eA1] = dram->highpass[hilp_eB1];
	dram->highpass[hilp_fA1] = dram->highpass[hilp_fB1];
	dram->highpass[hilp_fA2] = dram->highpass[hilp_fB2];
	dram->lowpass[hilp_aA0] = dram->lowpass[hilp_aB0];
	dram->lowpass[hilp_aA1] = dram->lowpass[hilp_aB1];
	dram->lowpass[hilp_bA1] = dram->lowpass[hilp_bB1];
	dram->lowpass[hilp_bA2] = dram->lowpass[hilp_bB2];
	dram->lowpass[hilp_cA0] = dram->lowpass[hilp_cB0];
	dram->lowpass[hilp_cA1] = dram->lowpass[hilp_cB1];
	dram->lowpass[hilp_dA1] = dram->lowpass[hilp_dB1];
	dram->lowpass[hilp_dA2] = dram->lowpass[hilp_dB2];
	dram->lowpass[hilp_eA0] = dram->lowpass[hilp_eB0];
	dram->lowpass[hilp_eA1] = dram->lowpass[hilp_eB1];
	dram->lowpass[hilp_fA1] = dram->lowpass[hilp_fB1];
	dram->lowpass[hilp_fA2] = dram->lowpass[hilp_fB2];
	//previous run through the buffer is still in the filter, so we move it
	//to the A section and now it's the new starting point.
	//On the buss, highpass and lowpass are isolators meant to be moved,
	//so they are interpolated where the channels are not
	
	biqK = tan(M_PI * dram->highpass[hilp_freq]); //highpass
	norm = 1.0f / (1.0f + biqK / 1.93185165f + biqK * biqK);
	dram->highpass[hilp_aB0] = norm;
	dram->highpass[hilp_aB1] = -2.0f * dram->highpass[hilp_aB0];
	dram->highpass[hilp_bB1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->highpass[hilp_bB2] = (1.0f - biqK / 1.93185165f + biqK * biqK) * norm;
	norm = 1.0f / (1.0f + biqK / 0.70710678f + biqK * biqK);
	dram->highpass[hilp_cB0] = norm;
	dram->highpass[hilp_cB1] = -2.0f * dram->highpass[hilp_cB0];
	dram->highpass[hilp_dB1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->highpass[hilp_dB2] = (1.0f - biqK / 0.70710678f + biqK * biqK) * norm;
	norm = 1.0f / (1.0f + biqK / 0.51763809f + biqK * biqK);
	dram->highpass[hilp_eB0] = norm;
	dram->highpass[hilp_eB1] = -2.0f * dram->highpass[hilp_eB0];
	dram->highpass[hilp_fB1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->highpass[hilp_fB2] = (1.0f - biqK / 0.51763809f + biqK * biqK) * norm;
	
	biqK = tan(M_PI * dram->lowpass[hilp_freq]); //lowpass
	norm = 1.0f / (1.0f + biqK / 1.93185165f + biqK * biqK);
	dram->lowpass[hilp_aB0] = biqK * biqK * norm;
	dram->lowpass[hilp_aB1] = 2.0f * dram->lowpass[hilp_aB0];
	dram->lowpass[hilp_bB1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->lowpass[hilp_bB2] = (1.0f - biqK / 1.93185165f + biqK * biqK) * norm;
	norm = 1.0f / (1.0f + biqK / 0.70710678f + biqK * biqK);
	dram->lowpass[hilp_cB0] = biqK * biqK * norm;
	dram->lowpass[hilp_cB1] = 2.0f * dram->lowpass[hilp_cB0];
	dram->lowpass[hilp_dB1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->lowpass[hilp_dB2] = (1.0f - biqK / 0.70710678f + biqK * biqK) * norm;
	norm = 1.0f / (1.0f + biqK / 0.51763809f + biqK * biqK);
	dram->lowpass[hilp_eB0] = biqK * biqK * norm;
	dram->lowpass[hilp_eB1] = 2.0f * dram->lowpass[hilp_eB0];
	dram->lowpass[hilp_fB1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->lowpass[hilp_fB2] = (1.0f - biqK / 0.51763809f + biqK * biqK) * norm;
	
	if (dram->highpass[hilp_aA0] == 0.0f) { // if we have just started, start directly with raw info
		dram->highpass[hilp_aA0] = dram->highpass[hilp_aB0];
		dram->highpass[hilp_aA1] = dram->highpass[hilp_aB1];
		dram->highpass[hilp_bA1] = dram->highpass[hilp_bB1];
		dram->highpass[hilp_bA2] = dram->highpass[hilp_bB2];
		dram->highpass[hilp_cA0] = dram->highpass[hilp_cB0];
		dram->highpass[hilp_cA1] = dram->highpass[hilp_cB1];
		dram->highpass[hilp_dA1] = dram->highpass[hilp_dB1];
		dram->highpass[hilp_dA2] = dram->highpass[hilp_dB2];
		dram->highpass[hilp_eA0] = dram->highpass[hilp_eB0];
		dram->highpass[hilp_eA1] = dram->highpass[hilp_eB1];
		dram->highpass[hilp_fA1] = dram->highpass[hilp_fB1];
		dram->highpass[hilp_fA2] = dram->highpass[hilp_fB2];
		dram->lowpass[hilp_aA0] = dram->lowpass[hilp_aB0];
		dram->lowpass[hilp_aA1] = dram->lowpass[hilp_aB1];
		dram->lowpass[hilp_bA1] = dram->lowpass[hilp_bB1];
		dram->lowpass[hilp_bA2] = dram->lowpass[hilp_bB2];
		dram->lowpass[hilp_cA0] = dram->lowpass[hilp_cB0];
		dram->lowpass[hilp_cA1] = dram->lowpass[hilp_cB1];
		dram->lowpass[hilp_dA1] = dram->lowpass[hilp_dB1];
		dram->lowpass[hilp_dA2] = dram->lowpass[hilp_dB2];
		dram->lowpass[hilp_eA0] = dram->lowpass[hilp_eB0];
		dram->lowpass[hilp_eA1] = dram->lowpass[hilp_eB1];
		dram->lowpass[hilp_fA1] = dram->lowpass[hilp_fB1];
		dram->lowpass[hilp_fA2] = dram->lowpass[hilp_fB2];
	}
	
	panA = panB; panB = GetParameter( kParam_PAN )*1.57079633f;
	inTrimA = inTrimB; inTrimB = GetParameter( kParam_FAD )*2.0f;
	//Console
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		const float temp = (float)nSampleFrames/inFramesToProcess;
		dram->highpass[hilp_a0] = (dram->highpass[hilp_aA0]*temp)+(dram->highpass[hilp_aB0]*(1.0f-temp));
		dram->highpass[hilp_a1] = (dram->highpass[hilp_aA1]*temp)+(dram->highpass[hilp_aB1]*(1.0f-temp));
		dram->highpass[hilp_b1] = (dram->highpass[hilp_bA1]*temp)+(dram->highpass[hilp_bB1]*(1.0f-temp));
		dram->highpass[hilp_b2] = (dram->highpass[hilp_bA2]*temp)+(dram->highpass[hilp_bB2]*(1.0f-temp));
		dram->highpass[hilp_c0] = (dram->highpass[hilp_cA0]*temp)+(dram->highpass[hilp_cB0]*(1.0f-temp));
		dram->highpass[hilp_c1] = (dram->highpass[hilp_cA1]*temp)+(dram->highpass[hilp_cB1]*(1.0f-temp));
		dram->highpass[hilp_d1] = (dram->highpass[hilp_dA1]*temp)+(dram->highpass[hilp_dB1]*(1.0f-temp));
		dram->highpass[hilp_d2] = (dram->highpass[hilp_dA2]*temp)+(dram->highpass[hilp_dB2]*(1.0f-temp));
		dram->highpass[hilp_e0] = (dram->highpass[hilp_eA0]*temp)+(dram->highpass[hilp_eB0]*(1.0f-temp));
		dram->highpass[hilp_e1] = (dram->highpass[hilp_eA1]*temp)+(dram->highpass[hilp_eB1]*(1.0f-temp));
		dram->highpass[hilp_f1] = (dram->highpass[hilp_fA1]*temp)+(dram->highpass[hilp_fB1]*(1.0f-temp));
		dram->highpass[hilp_f2] = (dram->highpass[hilp_fA2]*temp)+(dram->highpass[hilp_fB2]*(1.0f-temp));
		dram->lowpass[hilp_a0] = (dram->lowpass[hilp_aA0]*temp)+(dram->lowpass[hilp_aB0]*(1.0f-temp));
		dram->lowpass[hilp_a1] = (dram->lowpass[hilp_aA1]*temp)+(dram->lowpass[hilp_aB1]*(1.0f-temp));
		dram->lowpass[hilp_b1] = (dram->lowpass[hilp_bA1]*temp)+(dram->lowpass[hilp_bB1]*(1.0f-temp));
		dram->lowpass[hilp_b2] = (dram->lowpass[hilp_bA2]*temp)+(dram->lowpass[hilp_bB2]*(1.0f-temp));
		dram->lowpass[hilp_c0] = (dram->lowpass[hilp_cA0]*temp)+(dram->lowpass[hilp_cB0]*(1.0f-temp));
		dram->lowpass[hilp_c1] = (dram->lowpass[hilp_cA1]*temp)+(dram->lowpass[hilp_cB1]*(1.0f-temp));
		dram->lowpass[hilp_d1] = (dram->lowpass[hilp_dA1]*temp)+(dram->lowpass[hilp_dB1]*(1.0f-temp));
		dram->lowpass[hilp_d2] = (dram->lowpass[hilp_dA2]*temp)+(dram->lowpass[hilp_dB2]*(1.0f-temp));
		dram->lowpass[hilp_e0] = (dram->lowpass[hilp_eA0]*temp)+(dram->lowpass[hilp_eB0]*(1.0f-temp));
		dram->lowpass[hilp_e1] = (dram->lowpass[hilp_eA1]*temp)+(dram->lowpass[hilp_eB1]*(1.0f-temp));
		dram->lowpass[hilp_f1] = (dram->lowpass[hilp_fA1]*temp)+(dram->lowpass[hilp_fB1]*(1.0f-temp));
		dram->lowpass[hilp_f2] = (dram->lowpass[hilp_fA2]*temp)+(dram->lowpass[hilp_fB2]*(1.0f-temp));
		
		
		if (highpassEngage) { //distributed Highpass
			dram->highpass[hilp_temp] = (inputSampleL*dram->highpass[hilp_a0])+dram->highpass[hilp_aL1];
			dram->highpass[hilp_aL1] = (inputSampleL*dram->highpass[hilp_a1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_b1])+dram->highpass[hilp_aL2];
			dram->highpass[hilp_aL2] = (inputSampleL*dram->highpass[hilp_a0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_b2]); inputSampleL = dram->highpass[hilp_temp];
			dram->highpass[hilp_temp] = (inputSampleR*dram->highpass[hilp_a0])+dram->highpass[hilp_aR1];
			dram->highpass[hilp_aR1] = (inputSampleR*dram->highpass[hilp_a1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_b1])+dram->highpass[hilp_aR2];
			dram->highpass[hilp_aR2] = (inputSampleR*dram->highpass[hilp_a0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_b2]); inputSampleR = dram->highpass[hilp_temp];
		} else dram->highpass[hilp_aR1] = dram->highpass[hilp_aR2] = dram->highpass[hilp_aL1] = dram->highpass[hilp_aL2] = 0.0f;
		if (lowpassEngage) { //distributed Lowpass
			dram->lowpass[hilp_temp] = (inputSampleL*dram->lowpass[hilp_a0])+dram->lowpass[hilp_aL1];
			dram->lowpass[hilp_aL1] = (inputSampleL*dram->lowpass[hilp_a1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_b1])+dram->lowpass[hilp_aL2];
			dram->lowpass[hilp_aL2] = (inputSampleL*dram->lowpass[hilp_a0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_b2]); inputSampleL = dram->lowpass[hilp_temp];
			dram->lowpass[hilp_temp] = (inputSampleR*dram->lowpass[hilp_a0])+dram->lowpass[hilp_aR1];
			dram->lowpass[hilp_aR1] = (inputSampleR*dram->lowpass[hilp_a1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_b1])+dram->lowpass[hilp_aR2];
			dram->lowpass[hilp_aR2] = (inputSampleR*dram->lowpass[hilp_a0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_b2]); inputSampleR = dram->lowpass[hilp_temp];
		} else dram->lowpass[hilp_aR1] = dram->lowpass[hilp_aR2] = dram->lowpass[hilp_aL1] = dram->lowpass[hilp_aL2] = 0.0f;
		//first Highpass/Lowpass blocks aliasing before the nonlinearity of ConsoleXBuss and Parametric		
		
		if (inputSampleL > 1.0f) inputSampleL = 1.0f;
		else if (inputSampleL > 0.0f) inputSampleL = -expm1((log1p(-inputSampleL) * 0.6180339887498949f));
		if (inputSampleL < -1.0f) inputSampleL = -1.0f;
		else if (inputSampleL < 0.0f) inputSampleL = expm1((log1p(inputSampleL) * 0.6180339887498949f));
		
		if (inputSampleR > 1.0f) inputSampleR = 1.0f;
		else if (inputSampleR > 0.0f) inputSampleR = -expm1((log1p(-inputSampleR) * 0.6180339887498949f));
		if (inputSampleR < -1.0f) inputSampleR = -1.0f;
		else if (inputSampleR < 0.0f) inputSampleR = expm1((log1p(inputSampleR) * 0.6180339887498949f));
		
		if (highpassEngage) { //distributed Highpass
			dram->highpass[hilp_temp] = (inputSampleL*dram->highpass[hilp_c0])+dram->highpass[hilp_cL1];
			dram->highpass[hilp_cL1] = (inputSampleL*dram->highpass[hilp_c1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_d1])+dram->highpass[hilp_cL2];
			dram->highpass[hilp_cL2] = (inputSampleL*dram->highpass[hilp_c0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_d2]); inputSampleL = dram->highpass[hilp_temp];
			dram->highpass[hilp_temp] = (inputSampleR*dram->highpass[hilp_c0])+dram->highpass[hilp_cR1];
			dram->highpass[hilp_cR1] = (inputSampleR*dram->highpass[hilp_c1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_d1])+dram->highpass[hilp_cR2];
			dram->highpass[hilp_cR2] = (inputSampleR*dram->highpass[hilp_c0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_d2]); inputSampleR = dram->highpass[hilp_temp];
		} else dram->highpass[hilp_cR1] = dram->highpass[hilp_cR2] = dram->highpass[hilp_cL1] = dram->highpass[hilp_cL2] = 0.0f;
		if (lowpassEngage) { //distributed Lowpass
			dram->lowpass[hilp_temp] = (inputSampleL*dram->lowpass[hilp_c0])+dram->lowpass[hilp_cL1];
			dram->lowpass[hilp_cL1] = (inputSampleL*dram->lowpass[hilp_c1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_d1])+dram->lowpass[hilp_cL2];
			dram->lowpass[hilp_cL2] = (inputSampleL*dram->lowpass[hilp_c0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_d2]); inputSampleL = dram->lowpass[hilp_temp];
			dram->lowpass[hilp_temp] = (inputSampleR*dram->lowpass[hilp_c0])+dram->lowpass[hilp_cR1];
			dram->lowpass[hilp_cR1] = (inputSampleR*dram->lowpass[hilp_c1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_d1])+dram->lowpass[hilp_cR2];
			dram->lowpass[hilp_cR2] = (inputSampleR*dram->lowpass[hilp_c0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_d2]); inputSampleR = dram->lowpass[hilp_temp];
		} else dram->lowpass[hilp_cR1] = dram->lowpass[hilp_cR2] = dram->lowpass[hilp_cL1] = dram->lowpass[hilp_cL2] = 0.0f;
		//another stage of Highpass/Lowpass before bringing in the parametric bands		
		
		if (!eqOff) {
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
			highFastLIIR = (highFastLIIR*highCoef) + (trebleFastL*(1.0f-highCoef));
			midFastL = highFastLIIR; trebleFastL -= midFastL;
			lowFastLIIR = (lowFastLIIR*lowCoef) + (midFastL*(1.0f-lowCoef));
			bassFastL = lowFastLIIR; midFastL -= bassFastL;
			inputSampleL = (bassFastL*bassGain) + (midFastL*midGain) + (trebleFastL*trebleGain);		
			//second stage of two crossovers is the exponential filters
			//this produces a slightly steeper Butterworth filter very cheaply
			float trebleFastR = inputSampleR;		
			outSample = (trebleFastR * dram->highFast[biq_a0]) + dram->highFast[biq_sR1];
			dram->highFast[biq_sR1] = (trebleFastR * dram->highFast[biq_a1]) - (outSample * dram->highFast[biq_b1]) + dram->highFast[biq_sR2];
			dram->highFast[biq_sR2] = (trebleFastR * dram->highFast[biq_a2]) - (outSample * dram->highFast[biq_b2]);
			float midFastR = outSample; trebleFastR -= midFastR;
			outSample = (midFastR * dram->lowFast[biq_a0]) + dram->lowFast[biq_sR1];
			dram->lowFast[biq_sR1] = (midFastR * dram->lowFast[biq_a1]) - (outSample * dram->lowFast[biq_b1]) + dram->lowFast[biq_sR2];
			dram->lowFast[biq_sR2] = (midFastR * dram->lowFast[biq_a2]) - (outSample * dram->lowFast[biq_b2]);
			float bassFastR = outSample; midFastR -= bassFastR;
			trebleFastR = (bassFastR*bassGain) + (midFastR*midGain) + (trebleFastR*trebleGain);
			//first stage of two crossovers is biquad of exactly 1.0f Q
			highFastRIIR = (highFastRIIR*highCoef) + (trebleFastR*(1.0f-highCoef));
			midFastR = highFastRIIR; trebleFastR -= midFastR;
			lowFastRIIR = (lowFastRIIR*lowCoef) + (midFastR*(1.0f-lowCoef));
			bassFastR = lowFastRIIR; midFastR -= bassFastR;
			inputSampleR = (bassFastR*bassGain) + (midFastR*midGain) + (trebleFastR*trebleGain);		
			//second stage of two crossovers is the exponential filters
			//this produces a slightly steeper Butterworth filter very cheaply
		}
		//SmoothEQ3
		
		if (bezCThresh > 0.0f) {
			inputSampleL *= ((bezCThresh*0.5f)+1.0f);
			inputSampleR *= ((bezCThresh*0.5f)+1.0f);
			dram->bezCompF[bez_cycle] += bezRez;
			dram->bezCompF[bez_Ctrl] += (fmax(fabs(inputSampleL),fabs(inputSampleR)) * bezRez);
			if (dram->bezCompF[bez_cycle] > 1.0f) {
				dram->bezCompF[bez_cycle] -= 1.0f;
				dram->bezCompF[bez_C] = dram->bezCompF[bez_B];
				dram->bezCompF[bez_B] = dram->bezCompF[bez_A];
				dram->bezCompF[bez_A] = dram->bezCompF[bez_Ctrl];
				dram->bezCompF[bez_Ctrl] = 0.0f;
			}
			dram->bezCompS[bez_cycle] += sloRez;
			dram->bezCompS[bez_Ctrl] += (fmax(fabs(inputSampleL),fabs(inputSampleR)) * sloRez);
			if (dram->bezCompS[bez_cycle] > 1.0f) {
				dram->bezCompS[bez_cycle] -= 1.0f;
				dram->bezCompS[bez_C] = dram->bezCompS[bez_B];
				dram->bezCompS[bez_B] = dram->bezCompS[bez_A];
				dram->bezCompS[bez_A] = dram->bezCompS[bez_Ctrl];
				dram->bezCompS[bez_Ctrl] = 0.0f;
			}
			float CBF = (dram->bezCompF[bez_C]*(1.0f-dram->bezCompF[bez_cycle]))+(dram->bezCompF[bez_B]*dram->bezCompF[bez_cycle]);
			float BAF = (dram->bezCompF[bez_B]*(1.0f-dram->bezCompF[bez_cycle]))+(dram->bezCompF[bez_A]*dram->bezCompF[bez_cycle]);
			float CBAF = (dram->bezCompF[bez_B]+(CBF*(1.0f-dram->bezCompF[bez_cycle]))+(BAF*dram->bezCompF[bez_cycle]))*0.5f;
			float CBS = (dram->bezCompS[bez_C]*(1.0f-dram->bezCompS[bez_cycle]))+(dram->bezCompS[bez_B]*dram->bezCompS[bez_cycle]);
			float BAS = (dram->bezCompS[bez_B]*(1.0f-dram->bezCompS[bez_cycle]))+(dram->bezCompS[bez_A]*dram->bezCompS[bez_cycle]);
			float CBAS = (dram->bezCompS[bez_B]+(CBS*(1.0f-dram->bezCompS[bez_cycle]))+(BAS*dram->bezCompS[bez_cycle]))*0.5f;
			float CBAMax = fmax(CBAS,CBAF); if (CBAMax > 0.0f) CBAMax = 1.0f/CBAMax;
			float CBAFade = ((CBAS*-CBAMax)+(CBAF*CBAMax)+1.0f)*0.5f;
			inputSampleL *= 1.0f-(fmin(((CBAS*(1.0f-CBAFade))+(CBAF*CBAFade))*bezCThresh,1.0f));
			inputSampleR *= 1.0f-(fmin(((CBAS*(1.0f-CBAFade))+(CBAF*CBAFade))*bezCThresh,1.0f));
		} else {dram->bezCompF[bez_Ctrl] = 0.0f; dram->bezCompS[bez_Ctrl] = 0.0f;}
		//Dynamics2 custom version for buss
		
		if (highpassEngage) { //distributed Highpass
			dram->highpass[hilp_temp] = (inputSampleL*dram->highpass[hilp_e0])+dram->highpass[hilp_eL1];
			dram->highpass[hilp_eL1] = (inputSampleL*dram->highpass[hilp_e1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_f1])+dram->highpass[hilp_eL2];
			dram->highpass[hilp_eL2] = (inputSampleL*dram->highpass[hilp_e0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_f2]); inputSampleL = dram->highpass[hilp_temp];
			dram->highpass[hilp_temp] = (inputSampleR*dram->highpass[hilp_e0])+dram->highpass[hilp_eR1];
			dram->highpass[hilp_eR1] = (inputSampleR*dram->highpass[hilp_e1])-(dram->highpass[hilp_temp]*dram->highpass[hilp_f1])+dram->highpass[hilp_eR2];
			dram->highpass[hilp_eR2] = (inputSampleR*dram->highpass[hilp_e0])-(dram->highpass[hilp_temp]*dram->highpass[hilp_f2]); inputSampleR = dram->highpass[hilp_temp];
		} else dram->highpass[hilp_eR1] = dram->highpass[hilp_eR2] = dram->highpass[hilp_eL1] = dram->highpass[hilp_eL2] = 0.0f;
		if (lowpassEngage) { //distributed Lowpass
			dram->lowpass[hilp_temp] = (inputSampleL*dram->lowpass[hilp_e0])+dram->lowpass[hilp_eL1];
			dram->lowpass[hilp_eL1] = (inputSampleL*dram->lowpass[hilp_e1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_f1])+dram->lowpass[hilp_eL2];
			dram->lowpass[hilp_eL2] = (inputSampleL*dram->lowpass[hilp_e0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_f2]); inputSampleL = dram->lowpass[hilp_temp];
			dram->lowpass[hilp_temp] = (inputSampleR*dram->lowpass[hilp_e0])+dram->lowpass[hilp_eR1];
			dram->lowpass[hilp_eR1] = (inputSampleR*dram->lowpass[hilp_e1])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_f1])+dram->lowpass[hilp_eR2];
			dram->lowpass[hilp_eR2] = (inputSampleR*dram->lowpass[hilp_e0])-(dram->lowpass[hilp_temp]*dram->lowpass[hilp_f2]); inputSampleR = dram->lowpass[hilp_temp];
		} else dram->lowpass[hilp_eR1] = dram->lowpass[hilp_eR2] = dram->lowpass[hilp_eL1] = dram->lowpass[hilp_eL2] = 0.0f;		
		//final Highpass/Lowpass continues to address aliasing
		//final stacked biquad section is the softest Q for smoothness
		
		float gainR = (panA*temp)+(panB*(1.0f-temp));
		float gainL = 1.57079633f-gainR;
		gainR = sin(gainR); gainL = sin(gainL);
		float gain = (inTrimA*temp)+(inTrimB*(1.0f-temp));
		if (gain > 1.0f) gain *= gain;
		if (gain < 1.0f) gain = 1.0f-powf(1.0f-gain,2);
		gain *= 2.0f;
		
		inputSampleL = inputSampleL * gainL * gain;
		inputSampleR = inputSampleR * gainR * gain;
		//applies pan section, and smoothed fader gain
		
		float darkSampleL = inputSampleL;
		float darkSampleR = inputSampleR;
		if (avgPos > 31) avgPos = 0;
		if (spacing > 31) {
			dram->avg32L[avgPos] = darkSampleL; dram->avg32R[avgPos] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 32; x++) {darkSampleL += dram->avg32L[x]; darkSampleR += dram->avg32R[x];}
			darkSampleL /= 32.0f; darkSampleR /= 32.0f;
		} if (spacing > 15) {
			dram->avg16L[avgPos%16] = darkSampleL; dram->avg16R[avgPos%16] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 16; x++) {darkSampleL += dram->avg16L[x]; darkSampleR += dram->avg16R[x];}
			darkSampleL /= 16.0f; darkSampleR /= 16.0f;
		} if (spacing > 7) {
			avg8L[avgPos%8] = darkSampleL; avg8R[avgPos%8] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 8; x++) {darkSampleL += avg8L[x]; darkSampleR += avg8R[x];}
			darkSampleL /= 8.0f; darkSampleR /= 8.0f;
		} if (spacing > 3) {
			avg4L[avgPos%4] = darkSampleL; avg4R[avgPos%4] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 4; x++) {darkSampleL += avg4L[x]; darkSampleR += avg4R[x];}
			darkSampleL /= 4.0f; darkSampleR /= 4.0f;
		} if (spacing > 1) {
			avg2L[avgPos%2] = darkSampleL; avg2R[avgPos%2] = darkSampleR;
			darkSampleL = 0.0f; darkSampleR = 0.0f;
			for (int x = 0; x < 2; x++) {darkSampleL += avg2L[x]; darkSampleR += avg2R[x];}
			darkSampleL /= 2.0f; darkSampleR /= 2.0f; 
		} avgPos++;
		lastSlewL += fabs(lastSlewpleL-inputSampleL); lastSlewpleL = inputSampleL;
		float avgSlewL = fmin(lastSlewL*lastSlewL*(0.0635f-(overallscale*0.0018436f)),1.0f);
		lastSlewL = fmax(lastSlewL*0.78f,2.39996322972865332223f);
		lastSlewR += fabs(lastSlewpleR-inputSampleR); lastSlewpleR = inputSampleR;
		float avgSlewR = fmin(lastSlewR*lastSlewR*(0.0635f-(overallscale*0.0018436f)),1.0f);
		lastSlewR = fmax(lastSlewR*0.78f,2.39996322972865332223f); //look up Golden Angle, it's cool
		inputSampleL = (inputSampleL*(1.0f-avgSlewL)) + (darkSampleL*avgSlewL);
		inputSampleR = (inputSampleR*(1.0f-avgSlewR)) + (darkSampleR*avgSlewR);
		
		inputSampleL = fmin(fmax(inputSampleL,-2.032610446872596f),2.032610446872596f);
		float X = inputSampleL * inputSampleL;
		float sat = inputSampleL * X;
		inputSampleL -= (sat*0.125f); sat *= X;
		inputSampleL += (sat*0.0078125f); sat *= X;
		inputSampleL -= (sat*0.000244140625f); sat *= X;
		inputSampleL += (sat*0.000003814697265625f); sat *= X;
		inputSampleL -= (sat*0.0000000298023223876953125f); sat *= X;
		//purestsaturation: sine, except all the corrections
		//retain mantissa of a float increasing power function
		
		inputSampleR = fmin(fmax(inputSampleR,-2.032610446872596f),2.032610446872596f);
		X = inputSampleR * inputSampleR;
		sat = inputSampleR * X;
		inputSampleR -= (sat*0.125f); sat *= X;
		inputSampleR += (sat*0.0078125f); sat *= X;
		inputSampleR -= (sat*0.000244140625f); sat *= X;
		inputSampleR += (sat*0.000003814697265625f); sat *= X;
		inputSampleR -= (sat*0.0000000298023223876953125f); sat *= X;
		//purestsaturation: sine, except all the corrections
		//retain mantissa of a float increasing power function
		
		//we are leaving it as a clip that will go over 0dB.
		//it is a softclip so it will give you a more forgiving experience,
		//but you are meant to not drive the softclip for just level.
		
		
		
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
	for (int x = 0; x < biq_total; x++) {
		dram->highFast[x] = 0.0;
		dram->lowFast[x] = 0.0;
	}
	highFastLIIR = 0.0;
	highFastRIIR = 0.0;
	lowFastLIIR = 0.0;
	lowFastRIIR = 0.0;
	//SmoothEQ3
	
	for (int x = 0; x < bez_total; x++) {dram->bezCompF[x] = 0.0;dram->bezCompS[x] = 0.0;}
	dram->bezCompF[bez_cycle] = 1.0;
	dram->bezCompS[bez_cycle] = 1.0;
	//Dynamics2
	
	for (int x = 0; x < hilp_total; x++) {
		dram->highpass[x] = 0.0;
		dram->lowpass[x] = 0.0;
	}	
	
	for (int x = 0; x < 33; x++) {dram->avg32L[x] = 0.0; dram->avg32R[x] = 0.0;}
	for (int x = 0; x < 17; x++) {dram->avg16L[x] = 0.0; dram->avg16R[x] = 0.0;}
	for (int x = 0; x < 9; x++) {avg8L[x] = 0.0; avg8R[x] = 0.0;}
	for (int x = 0; x < 5; x++) {avg4L[x] = 0.0; avg4R[x] = 0.0;}
	for (int x = 0; x < 3; x++) {avg2L[x] = 0.0; avg2R[x] = 0.0;}
	avgPos = 0;
	lastSlewL = 0.0; lastSlewR = 0.0;
	lastSlewpleL = 0.0; lastSlewpleR = 0.0;
	//preTapeHack
	
	panA = 0.5; panB = 0.5;
	inTrimA = 0.5; inTrimB = 0.5;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
