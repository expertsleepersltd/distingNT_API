#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleHChannel"
#define AIRWINDOWS_DESCRIPTION "The Airwindows console for hip-hop and music reinvention."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','3' )
enum {

	kParam_TRM =0,
	kParam_MOR =1,
	kParam_HIG =2,
	kParam_MID =3,
	kParam_LOW =4,
	kParam_CRS =5,
	kParam_TRF =6,
	kParam_TRG =7,
	kParam_TRB =8,
	kParam_HMF =9,
	kParam_HMG =10,
	kParam_HMB =11,
	kParam_LMF =12,
	kParam_LMG =13,
	kParam_LMB =14,
	kParam_BSF =15,
	kParam_BSG =16,
	kParam_BSB =17,
	kParam_THR =18,
	kParam_ATK =19,
	kParam_RLS =20,
	kParam_GAT =21,
	kParam_LOP =22,
	kParam_HIP =23,
	kParam_PAN =24,
	kParam_FAD =25,
	//Add your parameters here...
	kNumberOfParameters=26
};
static const int kDefaultValue_ParamTRM = 1;
const int dscBuf = 256;
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, kParam12, kParam13, kParam14, kParam15, kParam16, kParam17, kParam18, kParam19, kParam20, kParam21, kParam22, kParam23, kParam24, kParam25, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Trim", .min = 0, .max = 4000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "More", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "High", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Low", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "CrossFd", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tr Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Treble", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "TrCrush", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HM Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HighMid", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HMCrush", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LM Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LowMid", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LMCrush", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bs Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "BsCrush", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Thresh", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Gate", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Lowpass", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Hipass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Pan", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fader", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, kParam12, kParam13, kParam14, kParam15, kParam16, kParam17, kParam18, kParam19, kParam20, kParam21, kParam22, kParam23, kParam24, kParam25, };
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
		biqs_freq, biqs_reso, biqs_level,
		biqs_temp, biqs_bit,
		biqs_a0, biqs_a1, biqs_b1, biqs_b2,
		biqs_c0, biqs_c1, biqs_d1, biqs_d2,
		biqs_aL1, biqs_aL2, biqs_aR1, biqs_aR2,
		biqs_cL1, biqs_cL2, biqs_cR1, biqs_cR2,
		biqs_outL, biqs_outR, biqs_total
	};
	//HipCrush with four bands
	
	enum {
		bez_A,
		bez_B,
		bez_C,
		bez_Ctrl,
		bez_cycle,
		bez_total
	}; //the new undersampling. bez signifies the bezier curve reconstruction
	float bezMax;
	float bezMin;
	float bezGate;
	//Dynamics3
	
	bool hBypass;
	float iirLPositionL[15];
	float iirLAngleL[15];
	float iirLPositionR[15];
	float iirLAngleR[15];
	bool lBypass;
	float lFreqA;
	float lFreqB; //the lowpass
	float hFreqA;
	float hFreqB; //the highpass
	//Cabs2
	
	float dBaPosL;
	int dBaXL;
	float dBaPosR;
	int dBaXR;
	//Discontapeity
	
	float avg8L[9];
	float avg8R[9];
	float avg4L[5];
	float avg4R[5];
	float avg2L[3];
	float avg2R[3];
	float post8L[9];
	float post8R[9];
	float post4L[5];
	float post4R[5];
	float post2L[3];
	float post2R[3];
	float lastDarkL;
	float lastDarkR;
	int avgPos;
	//preTapeHack
	
	float panA;
	float panB;
	float inTrimA;
	float inTrimB;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float highFast[biq_total];
	float lowFast[biq_total];
	float high[biqs_total];
	float hmid[biqs_total];
	float lmid[biqs_total];	
	float bass[biqs_total];	
	float bezComp[bez_total];
	float iirHPositionL[23];
	float iirHAngleL[23];
	float iirHPositionR[23];
	float iirHAngleR[23];
	float dBaL[dscBuf+5];
	float dBaR[dscBuf+5];
	float avg32L[33];
	float avg32R[33];
	float avg16L[17];
	float avg16R[17];
	float post32L[33];
	float post32R[33];
	float post16L[17];
	float post16R[17];
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
	
	float moreTapeHack = (GetParameter( kParam_MOR )*2.0f)+1.0f;
	bool tapehackOff = (GetParameter( kParam_MOR ) == 0.0f);
	switch ((int)GetParameter( kParam_TRM )){
		case 0: moreTapeHack *= 0.5f; break;
		case 1: break;
		case 2: moreTapeHack *= 2.0f; break;
		case 3: moreTapeHack *= 4.0f; break;
		case 4: moreTapeHack *= 8.0f; break;
	}
	float moreDiscontinuity = fmax(powf(GetParameter( kParam_MOR )*0.42f,3.0f)*overallscale,0.00001f);
	//Discontapeity	
	
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
	
	float crossFade = GetParameter( kParam_CRS );
	bool hipcrushOff = (crossFade == 0.0f);
	if (!hipcrushOff) {
		dram->high[biqs_freq] = (((powf(GetParameter( kParam_TRF ),2.0f)*16000.0f)+1000.0f)/GetSampleRate());
		if (dram->high[biqs_freq] < 0.0001f) dram->high[biqs_freq] = 0.0001f;
		dram->high[biqs_bit] = (GetParameter( kParam_TRB )*2.0f)-1.0f;
		dram->high[biqs_level] = (1.0f-powf(1.0f-GetParameter( kParam_TRG ),2.0f))*1.618033988749894848204586f;
		dram->high[biqs_reso] = powf(GetParameter( kParam_TRG )+0.618033988749894848204586f,2.0f);
		biqK = tan(M_PI * dram->high[biqs_freq]);
		norm = 1.0f / (1.0f + biqK / (dram->high[biqs_reso]*0.618033988749894848204586f) + biqK * biqK);
		dram->high[biqs_a0] = biqK / (dram->high[biqs_reso]*0.618033988749894848204586f) * norm;
		dram->high[biqs_b1] = 2.0f * (biqK * biqK - 1.0f) * norm;
		dram->high[biqs_b2] = (1.0f - biqK / (dram->high[biqs_reso]*0.618033988749894848204586f) + biqK * biqK) * norm;
		norm = 1.0f / (1.0f + biqK / (dram->high[biqs_reso]*1.618033988749894848204586f) + biqK * biqK);
		dram->high[biqs_c0] = biqK / (dram->high[biqs_reso]*1.618033988749894848204586f) * norm;
		dram->high[biqs_d1] = 2.0f * (biqK * biqK - 1.0f) * norm;
		dram->high[biqs_d2] = (1.0f - biqK / (dram->high[biqs_reso]*1.618033988749894848204586f) + biqK * biqK) * norm;
		//high
		
		dram->hmid[biqs_freq] = (((powf(GetParameter( kParam_HMF ),3.0f)*7000.0f)+300.0f)/GetSampleRate());
		if (dram->hmid[biqs_freq] < 0.0001f) dram->hmid[biqs_freq] = 0.0001f;
		dram->hmid[biqs_bit] = (GetParameter( kParam_HMB )*2.0f)-1.0f;
		dram->hmid[biqs_level] = (1.0f-powf(1.0f-GetParameter( kParam_HMG ),2.0f))*1.618033988749894848204586f;
		dram->hmid[biqs_reso] = powf(GetParameter( kParam_HMG )+0.618033988749894848204586f,2.0f);
		biqK = tan(M_PI * dram->hmid[biqs_freq]);
		norm = 1.0f / (1.0f + biqK / (dram->hmid[biqs_reso]*0.618033988749894848204586f) + biqK * biqK);
		dram->hmid[biqs_a0] = biqK / (dram->hmid[biqs_reso]*0.618033988749894848204586f) * norm;
		dram->hmid[biqs_b1] = 2.0f * (biqK * biqK - 1.0f) * norm;
		dram->hmid[biqs_b2] = (1.0f - biqK / (dram->hmid[biqs_reso]*0.618033988749894848204586f) + biqK * biqK) * norm;
		norm = 1.0f / (1.0f + biqK / (dram->hmid[biqs_reso]*1.618033988749894848204586f) + biqK * biqK);
		dram->hmid[biqs_c0] = biqK / (dram->hmid[biqs_reso]*1.618033988749894848204586f) * norm;
		dram->hmid[biqs_d1] = 2.0f * (biqK * biqK - 1.0f) * norm;
		dram->hmid[biqs_d2] = (1.0f - biqK / (dram->hmid[biqs_reso]*1.618033988749894848204586f) + biqK * biqK) * norm;
		//hmid
		
		dram->lmid[biqs_freq] = (((powf(GetParameter( kParam_LMF ),3.0f)*3000.0f)+40.0f)/GetSampleRate());
		if (dram->lmid[biqs_freq] < 0.00001f) dram->lmid[biqs_freq] = 0.00001f;
		dram->lmid[biqs_bit] = (GetParameter( kParam_LMB )*2.0f)-1.0f;
		dram->lmid[biqs_level] = (1.0f-powf(1.0f-GetParameter( kParam_LMG ),2.0f))*1.618033988749894848204586f;
		dram->lmid[biqs_reso] = powf(GetParameter( kParam_LMG )+0.618033988749894848204586f,2.0f);
		biqK = tan(M_PI * dram->lmid[biqs_freq]);
		norm = 1.0f / (1.0f + biqK / (dram->lmid[biqs_reso]*0.618033988749894848204586f) + biqK * biqK);
		dram->lmid[biqs_a0] = biqK / (dram->lmid[biqs_reso]*0.618033988749894848204586f) * norm;
		dram->lmid[biqs_b1] = 2.0f * (biqK * biqK - 1.0f) * norm;
		dram->lmid[biqs_b2] = (1.0f - biqK / (dram->lmid[biqs_reso]*0.618033988749894848204586f) + biqK * biqK) * norm;
		norm = 1.0f / (1.0f + biqK / (dram->lmid[biqs_reso]*1.618033988749894848204586f) + biqK * biqK);
		dram->lmid[biqs_c0] = biqK / (dram->lmid[biqs_reso]*1.618033988749894848204586f) * norm;
		dram->lmid[biqs_d1] = 2.0f * (biqK * biqK - 1.0f) * norm;
		dram->lmid[biqs_d2] = (1.0f - biqK / (dram->lmid[biqs_reso]*1.618033988749894848204586f) + biqK * biqK) * norm;
		//lmid
		
		dram->bass[biqs_freq] = (((powf(GetParameter( kParam_BSF ),4.0f)*1000.0f)+20.0f)/GetSampleRate());
		if (dram->bass[biqs_freq] < 0.00001f) dram->bass[biqs_freq] = 0.00001f;
		dram->bass[biqs_bit] = (GetParameter( kParam_BSB )*2.0f)-1.0f;
		dram->bass[biqs_level] = (1.0f-powf(1.0f-GetParameter( kParam_BSG ),2.0f))*1.618033988749894848204586f;
		dram->bass[biqs_reso] = powf(GetParameter( kParam_BSG )+0.618033988749894848204586f,2.0f);
		biqK = tan(M_PI * dram->bass[biqs_freq]);
		norm = 1.0f / (1.0f + biqK / (dram->bass[biqs_reso]*0.618033988749894848204586f) + biqK * biqK);
		dram->bass[biqs_a0] = biqK / (dram->bass[biqs_reso]*0.618033988749894848204586f) * norm;
		dram->bass[biqs_b1] = 2.0f * (biqK * biqK - 1.0f) * norm;
		dram->bass[biqs_b2] = (1.0f - biqK / (dram->bass[biqs_reso]*0.618033988749894848204586f) + biqK * biqK) * norm;
		norm = 1.0f / (1.0f + biqK / (dram->bass[biqs_reso]*1.618033988749894848204586f) + biqK * biqK);
		dram->bass[biqs_c0] = biqK / (dram->bass[biqs_reso]*1.618033988749894848204586f) * norm;
		dram->bass[biqs_d1] = 2.0f * (biqK * biqK - 1.0f) * norm;
		dram->bass[biqs_d2] = (1.0f - biqK / (dram->bass[biqs_reso]*1.618033988749894848204586f) + biqK * biqK) * norm;
		//bass
	}
	//HipCrush with four bands
	
	float bezThresh = powf(1.0f-GetParameter( kParam_THR ), 4.0f) * 8.0f;
	float bezRez = powf(1.0f-GetParameter( kParam_ATK ), 4.0f) / overallscale; 
	float sloRez = powf(1.0f-GetParameter( kParam_RLS ), 4.0f) / overallscale;
	float gate = powf(GetParameter( kParam_GAT ),4.0f);
	bezRez = fmin(fmax(bezRez,0.0001f),1.0f);
	sloRez = fmin(fmax(sloRez,0.0001f),1.0f);
	//Dynamics3
	
	lFreqA = lFreqB; lFreqB = powf(fmax(GetParameter( kParam_LOP ),0.002f),overallscale); //the lowpass
	hFreqA = hFreqB; hFreqB = powf(GetParameter( kParam_HIP ),overallscale+2.0f); //the highpass
	//Cabs2
	
	panA = panB; panB = GetParameter( kParam_PAN )*1.57079633f;
	inTrimA = inTrimB; inTrimB = GetParameter( kParam_FAD )*2.0f;
	//Console
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		inputSampleL *= moreTapeHack;
		inputSampleR *= moreTapeHack;
		//trim control gets to work even when MORE is off
		
		if (!tapehackOff) {
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
			} //only update avgPos after the post-distortion filter stage
			float avgSlewL = fmin(fabs(lastDarkL-inputSampleL)*0.12f*overallscale,1.0f);
			avgSlewL = 1.0f-(1.0f-avgSlewL*1.0f-avgSlewL);
			inputSampleL = (inputSampleL*(1.0f-avgSlewL)) + (darkSampleL*avgSlewL);
			lastDarkL = darkSampleL;
			float avgSlewR = fmin(fabs(lastDarkR-inputSampleR)*0.12f*overallscale,1.0f);
			avgSlewR = 1.0f-(1.0f-avgSlewR*1.0f-avgSlewR);
			inputSampleR = (inputSampleR*(1.0f-avgSlewR)) + (darkSampleR*avgSlewR);
			lastDarkR = darkSampleR;
			
			//begin Discontinuity section
			inputSampleL *= moreDiscontinuity;
			dram->dBaL[dBaXL] = inputSampleL; dBaPosL *= 0.5f; dBaPosL += fabs((inputSampleL*((inputSampleL*0.25f)-0.5f))*0.5f);
			dBaPosL = fmin(dBaPosL,1.0f);
			int dBdly = floor(dBaPosL*dscBuf);
			float dBi = (dBaPosL*dscBuf)-dBdly;
			inputSampleL = dram->dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
			dBdly++; inputSampleL += dram->dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*dBi;
			dBaXL++; if (dBaXL < 0 || dBaXL >= dscBuf) dBaXL = 0;
			inputSampleL /= moreDiscontinuity;
			//end Discontinuity section, begin TapeHack section
			inputSampleL = fmax(fmin(inputSampleL,2.305929007734908f),-2.305929007734908f);
			float addtwo = inputSampleL * inputSampleL;
			float empower = inputSampleL * addtwo; // inputSampleL to the third power
			inputSampleL -= (empower / 6.0f);
			empower *= addtwo; // to the fifth power
			inputSampleL += (empower / 69.0f);
			empower *= addtwo; //seventh
			inputSampleL -= (empower / 2530.08f);
			empower *= addtwo; //ninth
			inputSampleL += (empower / 224985.6f);
			empower *= addtwo; //eleventh
			inputSampleL -= (empower / 9979200.0f);
			//this is a degenerate form of a Taylor Series to approximate sin()
			//end TapeHack section
			//begin Discontinuity section
			inputSampleR *= moreDiscontinuity;
			dram->dBaR[dBaXR] = inputSampleR; dBaPosR *= 0.5f; dBaPosR += fabs((inputSampleR*((inputSampleR*0.25f)-0.5f))*0.5f);
			dBaPosR = fmin(dBaPosR,1.0f);
			dBdly = floor(dBaPosR*dscBuf);
			dBi = (dBaPosR*dscBuf)-dBdly;
			inputSampleR = dram->dBaR[dBaXR-dBdly +((dBaXR-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
			dBdly++; inputSampleR += dram->dBaR[dBaXR-dBdly +((dBaXR-dBdly < 0)?dscBuf:0)]*dBi;
			dBaXR++; if (dBaXR < 0 || dBaXR >= dscBuf) dBaXR = 0;
			inputSampleR /= moreDiscontinuity;
			//end Discontinuity section, begin TapeHack section
			inputSampleR = fmax(fmin(inputSampleR,2.305929007734908f),-2.305929007734908f);
			addtwo = inputSampleR * inputSampleR;
			empower = inputSampleR * addtwo; // inputSampleR to the third power
			inputSampleR -= (empower / 6.0f);
			empower *= addtwo; // to the fifth power
			inputSampleR += (empower / 69.0f);
			empower *= addtwo; //seventh
			inputSampleR -= (empower / 2530.08f);
			empower *= addtwo; //ninth
			inputSampleR += (empower / 224985.6f);
			empower *= addtwo; //eleventh
			inputSampleR -= (empower / 9979200.0f);
			//this is a degenerate form of a Taylor Series to approximate sin()
			//end TapeHack section
			//Discontapeity
			
			darkSampleL = inputSampleL;
			darkSampleR = inputSampleR;
			if (avgPos > 31) avgPos = 0;
			if (spacing > 31) {
				dram->post32L[avgPos] = darkSampleL; dram->post32R[avgPos] = darkSampleR;
				darkSampleL = 0.0f; darkSampleR = 0.0f;
				for (int x = 0; x < 32; x++) {darkSampleL += dram->post32L[x]; darkSampleR += dram->post32R[x];}
				darkSampleL /= 32.0f; darkSampleR /= 32.0f;
			} if (spacing > 15) {
				dram->post16L[avgPos%16] = darkSampleL; dram->post16R[avgPos%16] = darkSampleR;
				darkSampleL = 0.0f; darkSampleR = 0.0f;
				for (int x = 0; x < 16; x++) {darkSampleL += dram->post16L[x]; darkSampleR += dram->post16R[x];}
				darkSampleL /= 16.0f; darkSampleR /= 16.0f;
			} if (spacing > 7) {
				post8L[avgPos%8] = darkSampleL; post8R[avgPos%8] = darkSampleR;
				darkSampleL = 0.0f; darkSampleR = 0.0f;
				for (int x = 0; x < 8; x++) {darkSampleL += post8L[x]; darkSampleR += post8R[x];}
				darkSampleL /= 8.0f; darkSampleR /= 8.0f;
			} if (spacing > 3) {
				post4L[avgPos%4] = darkSampleL; post4R[avgPos%4] = darkSampleR;
				darkSampleL = 0.0f; darkSampleR = 0.0f;
				for (int x = 0; x < 4; x++) {darkSampleL += post4L[x]; darkSampleR += post4R[x];}
				darkSampleL /= 4.0f; darkSampleR /= 4.0f;
			} if (spacing > 1) {
				post2L[avgPos%2] = darkSampleL; post2R[avgPos%2] = darkSampleR;
				darkSampleL = 0.0f; darkSampleR = 0.0f;
				for (int x = 0; x < 2; x++) {darkSampleL += post2L[x]; darkSampleR += post2R[x];}
				darkSampleL /= 2.0f; darkSampleR /= 2.0f; 
			} avgPos++;
			inputSampleL = (inputSampleL*(1.0f-avgSlewL)) + (darkSampleL*avgSlewL);
			inputSampleR = (inputSampleR*(1.0f-avgSlewR)) + (darkSampleR*avgSlewR);
			//use the previously calculated depth of the filter			
		}
		
		float smoothEQL = inputSampleL;
		float smoothEQR = inputSampleR;
		
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
			smoothEQL = (bassFastL*bassGain) + (midFastL*midGain) + (trebleFastL*trebleGain);		
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
			smoothEQR = (bassFastR*bassGain) + (midFastR*midGain) + (trebleFastR*trebleGain);		
			//second stage of two crossovers is the exponential filters
			//this produces a slightly steeper Butterworth filter very cheaply
		}
		//SmoothEQ3
		
		float parametricL = 0.0f;
		float parametricR = 0.0f;
		
		if (!hipcrushOff) {
			//begin Stacked Biquad With Reversed Neutron Flow L
			dram->high[biqs_outL] = inputSampleL * fabs(dram->high[biqs_level]);
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
			dram->hmid[biqs_outL] = inputSampleL * fabs(dram->hmid[biqs_level]);
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
			dram->lmid[biqs_outL] = inputSampleL * fabs(dram->lmid[biqs_level]);
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
			
			//begin Stacked Biquad With Reversed Neutron Flow L
			dram->bass[biqs_outL] = inputSampleL * fabs(dram->bass[biqs_level]);
			dram->bass[biqs_temp] = (dram->bass[biqs_outL] * dram->bass[biqs_a0]) + dram->bass[biqs_aL1];
			dram->bass[biqs_aL1] = dram->bass[biqs_aL2] - (dram->bass[biqs_temp]*dram->bass[biqs_b1]);
			dram->bass[biqs_aL2] = (dram->bass[biqs_outL] * -dram->bass[biqs_a0]) - (dram->bass[biqs_temp]*dram->bass[biqs_b2]);
			dram->bass[biqs_outL] = dram->bass[biqs_temp];
			if (dram->bass[biqs_bit] != 0.0f) {
				float bitFactor = dram->bass[biqs_bit];
				bool crushGate = (bitFactor < 0.0f);
				bitFactor = powf(2.0f,fmin(fmax((1.0f-fabs(bitFactor))*16.0f,0.5f),16.0f));
				dram->bass[biqs_outL] *= bitFactor;		
				dram->bass[biqs_outL] = floor(dram->bass[biqs_outL]+(crushGate?0.5f/bitFactor:0.0f));
				dram->bass[biqs_outL] /= bitFactor;
			}
			dram->bass[biqs_temp] = (dram->bass[biqs_outL] * dram->bass[biqs_c0]) + dram->bass[biqs_cL1];
			dram->bass[biqs_cL1] = dram->bass[biqs_cL2] - (dram->bass[biqs_temp]*dram->bass[biqs_d1]);
			dram->bass[biqs_cL2] = (dram->bass[biqs_outL] * -dram->bass[biqs_c0]) - (dram->bass[biqs_temp]*dram->bass[biqs_d2]);
			dram->bass[biqs_outL] = dram->bass[biqs_temp];
			dram->bass[biqs_outL] *= dram->bass[biqs_level];
			parametricL = dram->high[biqs_outL] + dram->hmid[biqs_outL] + dram->lmid[biqs_outL] + dram->bass[biqs_outL];
			//end Stacked Biquad With Reversed Neutron Flow L
			
			//begin Stacked Biquad With Reversed Neutron Flow R
			dram->high[biqs_outR] = inputSampleR * fabs(dram->high[biqs_level]);
			dram->high[biqs_temp] = (dram->high[biqs_outR] * dram->high[biqs_a0]) + dram->high[biqs_aR1];
			dram->high[biqs_aR1] = dram->high[biqs_aR2] - (dram->high[biqs_temp]*dram->high[biqs_b1]);
			dram->high[biqs_aR2] = (dram->high[biqs_outR] * -dram->high[biqs_a0]) - (dram->high[biqs_temp]*dram->high[biqs_b2]);
			dram->high[biqs_outR] = dram->high[biqs_temp];
			if (dram->high[biqs_bit] != 0.0f) {
				float bitFactor = dram->high[biqs_bit];
				bool crushGate = (bitFactor < 0.0f);
				bitFactor = powf(2.0f,fmin(fmax((1.0f-fabs(bitFactor))*16.0f,0.5f),16.0f));
				dram->high[biqs_outR] *= bitFactor;		
				dram->high[biqs_outR] = floor(dram->high[biqs_outR]+(crushGate?0.5f/bitFactor:0.0f));
				dram->high[biqs_outR] /= bitFactor;
			}
			dram->high[biqs_temp] = (dram->high[biqs_outR] * dram->high[biqs_c0]) + dram->high[biqs_cR1];
			dram->high[biqs_cR1] = dram->high[biqs_cR2] - (dram->high[biqs_temp]*dram->high[biqs_d1]);
			dram->high[biqs_cR2] = (dram->high[biqs_outR] * -dram->high[biqs_c0]) - (dram->high[biqs_temp]*dram->high[biqs_d2]);
			dram->high[biqs_outR] = dram->high[biqs_temp];
			dram->high[biqs_outR] *= dram->high[biqs_level];
			//end Stacked Biquad With Reversed Neutron Flow R
			
			//begin Stacked Biquad With Reversed Neutron Flow R
			dram->hmid[biqs_outR] = inputSampleR * fabs(dram->hmid[biqs_level]);
			dram->hmid[biqs_temp] = (dram->hmid[biqs_outR] * dram->hmid[biqs_a0]) + dram->hmid[biqs_aR1];
			dram->hmid[biqs_aR1] = dram->hmid[biqs_aR2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_b1]);
			dram->hmid[biqs_aR2] = (dram->hmid[biqs_outR] * -dram->hmid[biqs_a0]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_b2]);
			dram->hmid[biqs_outR] = dram->hmid[biqs_temp];
			if (dram->hmid[biqs_bit] != 0.0f) {
				float bitFactor = dram->hmid[biqs_bit];
				bool crushGate = (bitFactor < 0.0f);
				bitFactor = powf(2.0f,fmin(fmax((1.0f-fabs(bitFactor))*16.0f,0.5f),16.0f));
				dram->hmid[biqs_outR] *= bitFactor;		
				dram->hmid[biqs_outR] = floor(dram->hmid[biqs_outR]+(crushGate?0.5f/bitFactor:0.0f));
				dram->hmid[biqs_outR] /= bitFactor;
			}
			dram->hmid[biqs_temp] = (dram->hmid[biqs_outR] * dram->hmid[biqs_c0]) + dram->hmid[biqs_cR1];
			dram->hmid[biqs_cR1] = dram->hmid[biqs_cR2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_d1]);
			dram->hmid[biqs_cR2] = (dram->hmid[biqs_outR] * -dram->hmid[biqs_c0]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_d2]);
			dram->hmid[biqs_outR] = dram->hmid[biqs_temp];
			dram->hmid[biqs_outR] *= dram->hmid[biqs_level];
			//end Stacked Biquad With Reversed Neutron Flow R
			
			//begin Stacked Biquad With Reversed Neutron Flow R
			dram->lmid[biqs_outR] = inputSampleR * fabs(dram->lmid[biqs_level]);
			dram->lmid[biqs_temp] = (dram->lmid[biqs_outR] * dram->lmid[biqs_a0]) + dram->lmid[biqs_aR1];
			dram->lmid[biqs_aR1] = dram->lmid[biqs_aR2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_b1]);
			dram->lmid[biqs_aR2] = (dram->lmid[biqs_outR] * -dram->lmid[biqs_a0]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_b2]);
			dram->lmid[biqs_outR] = dram->lmid[biqs_temp];
			if (dram->lmid[biqs_bit] != 0.0f) {
				float bitFactor = dram->lmid[biqs_bit];
				bool crushGate = (bitFactor < 0.0f);
				bitFactor = powf(2.0f,fmin(fmax((1.0f-fabs(bitFactor))*16.0f,0.5f),16.0f));
				dram->lmid[biqs_outR] *= bitFactor;		
				dram->lmid[biqs_outR] = floor(dram->lmid[biqs_outR]+(crushGate?0.5f/bitFactor:0.0f));
				dram->lmid[biqs_outR] /= bitFactor;
			}
			dram->lmid[biqs_temp] = (dram->lmid[biqs_outR] * dram->lmid[biqs_c0]) + dram->lmid[biqs_cR1];
			dram->lmid[biqs_cR1] = dram->lmid[biqs_cR2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_d1]);
			dram->lmid[biqs_cR2] = (dram->lmid[biqs_outR] * -dram->lmid[biqs_c0]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_d2]);
			dram->lmid[biqs_outR] = dram->lmid[biqs_temp];
			dram->lmid[biqs_outR] *= dram->lmid[biqs_level];
			//end Stacked Biquad With Reversed Neutron Flow R
			
			//begin Stacked Biquad With Reversed Neutron Flow R
			dram->bass[biqs_outR] = inputSampleR * fabs(dram->bass[biqs_level]);
			dram->bass[biqs_temp] = (dram->bass[biqs_outR] * dram->bass[biqs_a0]) + dram->bass[biqs_aR1];
			dram->bass[biqs_aR1] = dram->bass[biqs_aR2] - (dram->bass[biqs_temp]*dram->bass[biqs_b1]);
			dram->bass[biqs_aR2] = (dram->bass[biqs_outR] * -dram->bass[biqs_a0]) - (dram->bass[biqs_temp]*dram->bass[biqs_b2]);
			dram->bass[biqs_outR] = dram->bass[biqs_temp];
			if (dram->bass[biqs_bit] != 0.0f) {
				float bitFactor = dram->bass[biqs_bit];
				bool crushGate = (bitFactor < 0.0f);
				bitFactor = powf(2.0f,fmin(fmax((1.0f-fabs(bitFactor))*16.0f,0.5f),16.0f));
				dram->bass[biqs_outR] *= bitFactor;		
				dram->bass[biqs_outR] = floor(dram->bass[biqs_outR]+(crushGate?0.5f/bitFactor:0.0f));
				dram->bass[biqs_outR] /= bitFactor;
			}
			dram->bass[biqs_temp] = (dram->bass[biqs_outR] * dram->bass[biqs_c0]) + dram->bass[biqs_cR1];
			dram->bass[biqs_cR1] = dram->bass[biqs_cR2] - (dram->bass[biqs_temp]*dram->bass[biqs_d1]);
			dram->bass[biqs_cR2] = (dram->bass[biqs_outR] * -dram->bass[biqs_c0]) - (dram->bass[biqs_temp]*dram->bass[biqs_d2]);
			dram->bass[biqs_outR] = dram->bass[biqs_temp];
			dram->bass[biqs_outR] *= dram->bass[biqs_level];
			parametricR = dram->high[biqs_outR] + dram->hmid[biqs_outR] + dram->lmid[biqs_outR] + dram->bass[biqs_outR];
			//end Stacked Biquad With Reversed Neutron Flow R
		}
		//end HipCrush as four band
		
		if (fmax(fabs(inputSampleL),fabs(inputSampleR)) > gate) bezGate = overallscale/fmin(bezRez,sloRez);
		else bezGate = fmax(0.000001f, bezGate-fmin(bezRez,sloRez));
		
		if (bezThresh > 0.0f) {
			inputSampleL *= (bezThresh+1.0f);
			inputSampleR *= (bezThresh+1.0f);
			smoothEQL *= (bezThresh+1.0f);
			smoothEQR *= (bezThresh+1.0f);
			parametricL *= (bezThresh+1.0f);
			parametricR *= (bezThresh+1.0f);
		} //makeup gain		
		
		float ctrl = fmax(fabs(inputSampleL),fabs(inputSampleR));
		bezMax = fmax(bezMax,ctrl);
		bezMin = fmax(bezMin-sloRez,ctrl);
		dram->bezComp[bez_cycle] += bezRez;
		dram->bezComp[bez_Ctrl] += (bezMin * bezRez);
		
		if (dram->bezComp[bez_cycle] > 1.0f) {
			if (bezGate < 1.0f) dram->bezComp[bez_Ctrl] /= bezGate;
			dram->bezComp[bez_cycle] -= 1.0f;
			dram->bezComp[bez_C] = dram->bezComp[bez_B];
			dram->bezComp[bez_B] = dram->bezComp[bez_A];
			dram->bezComp[bez_A] = dram->bezComp[bez_Ctrl];
			dram->bezComp[bez_Ctrl] = 0.0f;
			bezMax = 0.0f;
		}
		float CB = (dram->bezComp[bez_C]*(1.0f-dram->bezComp[bez_cycle]))+(dram->bezComp[bez_B]*dram->bezComp[bez_cycle]);
		float BA = (dram->bezComp[bez_B]*(1.0f-dram->bezComp[bez_cycle]))+(dram->bezComp[bez_A]*dram->bezComp[bez_cycle]);
		float CBA = (dram->bezComp[bez_B]+(CB*(1.0f-dram->bezComp[bez_cycle]))+(BA*dram->bezComp[bez_cycle]))*0.5f;
		//switch over to the EQed or HipCrushed sound and compress
		inputSampleL = (smoothEQL * (1.0f-crossFade)) + (parametricL * crossFade);
		inputSampleR = (smoothEQR * (1.0f-crossFade)) + (parametricR * crossFade);
		
		if (bezThresh > 0.0f) {
			inputSampleL *= 1.0f-(fmin(CBA*bezThresh,1.0f));
			inputSampleR *= 1.0f-(fmin(CBA*bezThresh,1.0f));
		}
		//Dynamics3, but with crossfade over EQ or HipCrush
		
		const float temp = (float)nSampleFrames/inFramesToProcess;
		const float hFreq = (hFreqA*temp)+(hFreqB*(1.0f-temp));
		if (hFreq > 0.0f) {
			float lowSampleL = inputSampleL;
			float lowSampleR = inputSampleR;
			for(int count = 0; count < 21; count++) {
				dram->iirHAngleL[count] = (dram->iirHAngleL[count]*(1.0f-hFreq))+((lowSampleL-dram->iirHPositionL[count])*hFreq);
				lowSampleL = ((dram->iirHPositionL[count]+(dram->iirHAngleL[count]*hFreq))*(1.0f-hFreq))+(lowSampleL*hFreq);
				dram->iirHPositionL[count] = ((dram->iirHPositionL[count]+(dram->iirHAngleL[count]*hFreq))*(1.0f-hFreq))+(lowSampleL*hFreq);
				inputSampleL -= (lowSampleL * (1.0f/21.0f));//left
				dram->iirHAngleR[count] = (dram->iirHAngleR[count]*(1.0f-hFreq))+((lowSampleR-dram->iirHPositionR[count])*hFreq);
				lowSampleR = ((dram->iirHPositionR[count]+(dram->iirHAngleR[count]*hFreq))*(1.0f-hFreq))+(lowSampleR*hFreq);
				dram->iirHPositionR[count] = ((dram->iirHPositionR[count]+(dram->iirHAngleR[count]*hFreq))*(1.0f-hFreq))+(lowSampleR*hFreq);
				inputSampleR -= (lowSampleR * (1.0f/21.0f));//right
			} //the highpass
			hBypass = false;
		} else {
			if (!hBypass) {
				hBypass = true;
				for(int count = 0; count < 22; count++) {
					dram->iirHPositionL[count] = 0.0f;
					dram->iirHAngleL[count] = 0.0f;
					dram->iirHPositionR[count] = 0.0f;
					dram->iirHAngleR[count] = 0.0f;
				}
			} //blank out highpass if just switched off
		}
		const float lFreq = (lFreqA*temp)+(lFreqB*(1.0f-temp));
		if (lFreq < 1.0f) {
			for(int count = 0; count < 13; count++) {
				iirLAngleL[count] = (iirLAngleL[count]*(1.0f-lFreq))+((inputSampleL-iirLPositionL[count])*lFreq);
				inputSampleL = ((iirLPositionL[count]+(iirLAngleL[count]*lFreq))*(1.0f-lFreq))+(inputSampleL*lFreq);
				iirLPositionL[count] = ((iirLPositionL[count]+(iirLAngleL[count]*lFreq))*(1.0f-lFreq))+(inputSampleL*lFreq);//left
				iirLAngleR[count] = (iirLAngleR[count]*(1.0f-lFreq))+((inputSampleR-iirLPositionR[count])*lFreq);
				inputSampleR = ((iirLPositionR[count]+(iirLAngleR[count]*lFreq))*(1.0f-lFreq))+(inputSampleR*lFreq);
				iirLPositionR[count] = ((iirLPositionR[count]+(iirLAngleR[count]*lFreq))*(1.0f-lFreq))+(inputSampleR*lFreq);//right
			} //the lowpass
			lBypass = false;
		} else {
			if (!lBypass) {
				lBypass = true;
				for(int count = 0; count < 14; count++) {
					iirLPositionL[count] = 0.0f;
					iirLAngleL[count] = 0.0f;
					iirLPositionR[count] = 0.0f;
					iirLAngleR[count] = 0.0f;
				}
			} //blank out lowpass if just switched off
		}		
		//Cabs2
		
		float gainR = (panA*temp)+(panB*(1.0f-temp));
		float gainL = 1.57079633f-gainR;
		gainR = sin(gainR); gainL = sin(gainL);
		
		float gain = (inTrimA*temp)+(inTrimB*(1.0f-temp));
		if (gain > 1.0f) gain *= gain;
		if (gain < 1.0f) gain = 1.0f-powf(1.0f-gain,2);
		
		inputSampleL = inputSampleL * gainL * gain;
		inputSampleR = inputSampleR * gainR * gain;
		//applies pan section, and smoothed fader gain
		
		if (inputSampleL > 1.0f) inputSampleL = 1.0f;
		else if (inputSampleL > 0.0f) inputSampleL = -expm1((log1p(-inputSampleL) * 1.618033988749895f));
		if (inputSampleL < -1.0f) inputSampleL = -1.0f;
		else if (inputSampleL < 0.0f) inputSampleL = expm1((log1p(inputSampleL) * 1.618033988749895f));
		
		if (inputSampleR > 1.0f) inputSampleR = 1.0f;
		else if (inputSampleR > 0.0f) inputSampleR = -expm1((log1p(-inputSampleR) * 1.618033988749895f));
		if (inputSampleR < -1.0f) inputSampleR = -1.0f;
		else if (inputSampleR < 0.0f) inputSampleR = expm1((log1p(inputSampleR) * 1.618033988749895f));
		
		
		
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
	
	for (int x = 0; x < biqs_total; x++) {
		dram->high[x] = 0.0;
		dram->hmid[x] = 0.0;
		dram->lmid[x] = 0.0;
		dram->bass[x] = 0.0;
	}
	//HipCrush with four bands
	
	for (int x = 0; x < bez_total; x++) dram->bezComp[x] = 0.0;
	dram->bezComp[bez_cycle] = 1.0; bezMax = 0.0; bezMin = 0.0;
	bezGate = 2.0;
	//Dynamics3
	
	for(int count = 0; count < 22; count++) {
		dram->iirHPositionL[count] = 0.0;
		dram->iirHAngleL[count] = 0.0;
		dram->iirHPositionR[count] = 0.0;
		dram->iirHAngleR[count] = 0.0;
	}
	hBypass = false;
	
	for(int count = 0; count < 14; count++) {
		iirLPositionL[count] = 0.0;
		iirLAngleL[count] = 0.0;
		iirLPositionR[count] = 0.0;
		iirLAngleR[count] = 0.0;
	}
	lBypass = false;	
	//Cabs2
	
	for(int count = 0; count < dscBuf+2; count++) {
		dram->dBaL[count] = 0.0;
		dram->dBaR[count] = 0.0;
	}
	dBaPosL = 0.0;
	dBaPosR = 0.0;
	dBaXL = 1;
	dBaXR = 1;
	//Discontapeity
	
	for (int x = 0; x < 33; x++) {dram->avg32L[x] = 0.0; dram->post32L[x] = 0.0; dram->avg32R[x] = 0.0; dram->post32R[x] = 0.0;}
	for (int x = 0; x < 17; x++) {dram->avg16L[x] = 0.0; dram->post16L[x] = 0.0; dram->avg16R[x] = 0.0; dram->post16R[x] = 0.0;}
	for (int x = 0; x < 9; x++) {avg8L[x] = 0.0; post8L[x] = 0.0; avg8R[x] = 0.0; post8R[x] = 0.0;}
	for (int x = 0; x < 5; x++) {avg4L[x] = 0.0; post4L[x] = 0.0; avg4R[x] = 0.0; post4R[x] = 0.0;}
	for (int x = 0; x < 3; x++) {avg2L[x] = 0.0; post2L[x] = 0.0; avg2R[x] = 0.0; post2R[x] = 0.0;}
	avgPos = 0;
	lastDarkL = 0.0; lastDarkL = 0.0;
	//preTapeHack
	
	lFreqA = 1.0; lFreqB = 1.0;
	hFreqA = 0.0; hFreqB = 0.0;
	panA = 0.5; panB = 0.5;
	inTrimA = 0.5; inTrimB = 0.5;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
