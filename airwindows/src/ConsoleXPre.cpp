#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleXPre"
#define AIRWINDOWS_DESCRIPTION "Just the tone shaping from ConsoleX."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','@' )
enum {

	kParam_HIP = 0,
	kParam_LOP = 1,
	kParam_AIR = 2,
	kParam_FIR = 3,
	kParam_STO = 4,
	kParam_RNG = 5,
	kParam_FCT = 6,
	kParam_SCT = 7,
	kParam_FCR = 8,
	kParam_SCR = 9,
	kParam_FCA = 10,
	kParam_SCA = 11,
	kParam_FCL = 12,
	kParam_SCL = 13,
	kParam_FGT = 14,
	kParam_SGT = 15,
	kParam_FGR = 16,
	kParam_SGR = 17,
	kParam_FGS = 18,
	kParam_SGS = 19,
	kParam_FGL = 20,
	kParam_SGL = 21,	
	kParam_TRF = 22,
	kParam_TRG = 23,
	kParam_TRR = 24,
	kParam_HMF = 25,
	kParam_HMG = 26,
	kParam_HMR = 27,
	kParam_LMF = 28,
	kParam_LMG = 29,
	kParam_LMR = 30,
	kParam_BSF = 31,
	kParam_BSG = 32,
	kParam_BSR = 33,
	kParam_DSC = 34,
	kParam_PAN = 35,
	kParam_FAD = 36,
	//Add your parameters here...
	kNumberOfParameters=37
};
const int dscBuf = 90;
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, kParam12, kParam13, kParam14, kParam15, kParam16, kParam17, kParam18, kParam19, kParam20, kParam21, kParam22, kParam23, kParam24, kParam25, kParam26, kParam27, kParam28, kParam29, kParam30, kParam31, kParam32, kParam33, kParam34, kParam35, kParam36, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Highpas", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Lowpass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Air", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fire", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Stone", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Range", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FC Thrs", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SC Thrs", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FC Rati", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SC Rati", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FC Atk", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SC Atk", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FC Rls", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SC Rls", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FG Thrs", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SG Thrs", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FG Rati", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SG Rati", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FG Sust", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SG Sust", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "FG Rls", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "SG Rls", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tr Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Treble", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tr Reso", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HM Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HighMid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HM Reso", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LM Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LowMid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LM Reso", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bs Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bs Reso", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Top dB", .min = 7000, .max = 14000, .def = 10000, .unit = kNT_unitNone, .scaling = kNT_scaling100, .enumStrings = NULL },
{ .name = "Pan", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fader", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, kParam12, kParam13, kParam14, kParam15, kParam16, kParam17, kParam18, kParam19, kParam20, kParam21, kParam22, kParam23, kParam24, kParam25, kParam26, kParam27, kParam28, kParam29, kParam30, kParam31, kParam32, kParam33, kParam34, kParam35, kParam36, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"

	
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
	
	enum {
		pvAL1, pvSL1, accSL1, acc2SL1,
		pvAL2, pvSL2, accSL2, acc2SL2,
		pvAL3, pvSL3, accSL3,
		pvAL4, pvSL4,
		gndavgL, outAL, gainAL,
		pvAR1, pvSR1, accSR1, acc2SR1,
		pvAR2, pvSR2, accSR2, acc2SR2,
		pvAR3, pvSR3, accSR3,
		pvAR4, pvSR4,
		gndavgR, outAR, gainAR,
		air_total
	};
	
	enum {
		prevSampL1, prevSlewL1, accSlewL1,
		prevSampL2, prevSlewL2, accSlewL2,
		prevSampL3, prevSlewL3, accSlewL3,
		kalGainL, kalOutL,
		prevSampR1, prevSlewR1, accSlewR1,
		prevSampR2, prevSlewR2, accSlewR2,
		prevSampR3, prevSlewR3, accSlewR3,
		kalGainR, kalOutR,
		kal_total
	};
	float fireCompL;
	float fireCompR;
	float fireGate;
	float stoneCompL;
	float stoneCompR;
	float stoneGate;
	float airGainA;
	float airGainB;
	float fireGainA;
	float fireGainB;
	float stoneGainA;
	float stoneGainB;
	
	enum { 
		biqs_freq, biqs_reso, biqs_level,
		biqs_nonlin, biqs_temp, biqs_dis,
		biqs_a0, biqs_a1, biqs_b1, biqs_b2,
		biqs_c0, biqs_c1, biqs_d1, biqs_d2,
		biqs_e0, biqs_e1, biqs_f1, biqs_f2,
		biqs_aL1, biqs_aL2, biqs_aR1, biqs_aR2,
		biqs_cL1, biqs_cL2, biqs_cR1, biqs_cR2,
		biqs_eL1, biqs_eL2, biqs_eR1, biqs_eR2,
		biqs_outL, biqs_outR, biqs_total
	};
	
	float dBaPosL;
	float dBaPosR;
	int dBaXL;
	int dBaXR;
	
	float panA;
	float panB;
	float inTrimA;
	float inTrimB;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float highpass[hilp_total];
	float lowpass[hilp_total];
	float air[air_total];
	float kal[kal_total];
	float high[biqs_total];
	float hmid[biqs_total];
	float lmid[biqs_total];
	float bass[biqs_total];
	float dBaL[dscBuf+5];
	float dBaR[dscBuf+5];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
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
	
	airGainA = airGainB; airGainB = GetParameter( kParam_AIR )*2.0f;
	fireGainA = fireGainB; fireGainB = GetParameter( kParam_FIR )*2.0f;
	stoneGainA = stoneGainB; stoneGainB = GetParameter( kParam_STO )*2.0f;
	//simple three band to adjust
	float kalmanRange = 1.0f-(powf(GetParameter( kParam_RNG ),2)/overallscale);
	//crossover frequency between mid/bass
	
	float compFThresh = powf(GetParameter( kParam_FCT ),4);
	float compSThresh = powf(GetParameter( kParam_SCT ),4);
	float compFRatio = 1.0f-powf(1.0f-GetParameter( kParam_FCR ),2);
	float compSRatio = 1.0f-powf(1.0f-GetParameter( kParam_SCR ),2);
	float compFAttack = 1.0f/(((powf(GetParameter( kParam_FCA ),3)*5000.0f)+500.0f)*overallscale);
	float compSAttack = 1.0f/(((powf(GetParameter( kParam_SCA ),3)*5000.0f)+500.0f)*overallscale);
	float compFRelease = 1.0f/(((powf(GetParameter( kParam_FCL ),5)*50000.0f)+500.0f)*overallscale);
	float compSRelease = 1.0f/(((powf(GetParameter( kParam_SCL ),5)*50000.0f)+500.0f)*overallscale);
	float gateFThresh = powf(GetParameter( kParam_FGT ),4);
	float gateSThresh = powf(GetParameter( kParam_SGT ),4);
	float gateFRatio = 1.0f-powf(1.0f-GetParameter( kParam_FGR ),2);
	float gateSRatio = 1.0f-powf(1.0f-GetParameter( kParam_SGR ),2);
	float gateFSustain = M_PI_2 * powf(GetParameter( kParam_FGS )+1.0f,4.0f);
	float gateSSustain = M_PI_2 * powf(GetParameter( kParam_SGS )+1.0f,4.0f);
	float gateFRelease = 1.0f/(((powf(GetParameter( kParam_FGL ),5)*500000.0f)+500.0f)*overallscale);
	float gateSRelease = 1.0f/(((powf(GetParameter( kParam_SGL ),5)*500000.0f)+500.0f)*overallscale);
	
	dram->high[biqs_freq] = (((powf(GetParameter( kParam_TRF ),3)*14500.0f)+1500.0f)/GetSampleRate());
	if (dram->high[biqs_freq] < 0.0001f) dram->high[biqs_freq] = 0.0001f;
	dram->high[biqs_nonlin] = GetParameter( kParam_TRG );
	dram->high[biqs_level] = (dram->high[biqs_nonlin]*2.0f)-1.0f;
	if (dram->high[biqs_level] > 0.0f) dram->high[biqs_level] *= 2.0f;
	dram->high[biqs_reso] = ((0.5f+(dram->high[biqs_nonlin]*0.5f)+sqrt(dram->high[biqs_freq]))-(1.0f-powf(1.0f-GetParameter( kParam_TRR ),2.0f)))+0.5f+(dram->high[biqs_nonlin]*0.5f);
	K = tan(M_PI * dram->high[biqs_freq]);
	norm = 1.0f / (1.0f + K / (dram->high[biqs_reso]*1.93185165f) + K * K);
	dram->high[biqs_a0] = K / (dram->high[biqs_reso]*1.93185165f) * norm;
	dram->high[biqs_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->high[biqs_b2] = (1.0f - K / (dram->high[biqs_reso]*1.93185165f) + K * K) * norm;
	norm = 1.0f / (1.0f + K / (dram->high[biqs_reso]*0.70710678f) + K * K);
	dram->high[biqs_c0] = K / (dram->high[biqs_reso]*0.70710678f) * norm;
	dram->high[biqs_d1] = 2.0f * (K * K - 1.0f) * norm;
	dram->high[biqs_d2] = (1.0f - K / (dram->high[biqs_reso]*0.70710678f) + K * K) * norm;
	norm = 1.0f / (1.0f + K / (dram->high[biqs_reso]*0.51763809f) + K * K);
	dram->high[biqs_e0] = K / (dram->high[biqs_reso]*0.51763809f) * norm;
	dram->high[biqs_f1] = 2.0f * (K * K - 1.0f) * norm;
	dram->high[biqs_f2] = (1.0f - K / (dram->high[biqs_reso]*0.51763809f) + K * K) * norm;
	//high
	
	dram->hmid[biqs_freq] = (((powf(GetParameter( kParam_HMF ),3)*6400.0f)+600.0f)/GetSampleRate());
	if (dram->hmid[biqs_freq] < 0.0001f) dram->hmid[biqs_freq] = 0.0001f;
	dram->hmid[biqs_nonlin] = GetParameter( kParam_HMG );
	dram->hmid[biqs_level] = (dram->hmid[biqs_nonlin]*2.0f)-1.0f;
	if (dram->hmid[biqs_level] > 0.0f) dram->hmid[biqs_level] *= 2.0f;
	dram->hmid[biqs_reso] = ((0.5f+(dram->hmid[biqs_nonlin]*0.5f)+sqrt(dram->hmid[biqs_freq]))-(1.0f-powf(1.0f-GetParameter( kParam_HMR ),2.0f)))+0.5f+(dram->hmid[biqs_nonlin]*0.5f);
	K = tan(M_PI * dram->hmid[biqs_freq]);
	norm = 1.0f / (1.0f + K / (dram->hmid[biqs_reso]*1.93185165f) + K * K);
	dram->hmid[biqs_a0] = K / (dram->hmid[biqs_reso]*1.93185165f) * norm;
	dram->hmid[biqs_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hmid[biqs_b2] = (1.0f - K / (dram->hmid[biqs_reso]*1.93185165f) + K * K) * norm;
	norm = 1.0f / (1.0f + K / (dram->hmid[biqs_reso]*0.70710678f) + K * K);
	dram->hmid[biqs_c0] = K / (dram->hmid[biqs_reso]*0.70710678f) * norm;
	dram->hmid[biqs_d1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hmid[biqs_d2] = (1.0f - K / (dram->hmid[biqs_reso]*0.70710678f) + K * K) * norm;
	norm = 1.0f / (1.0f + K / (dram->hmid[biqs_reso]*0.51763809f) + K * K);
	dram->hmid[biqs_e0] = K / (dram->hmid[biqs_reso]*0.51763809f) * norm;
	dram->hmid[biqs_f1] = 2.0f * (K * K - 1.0f) * norm;
	dram->hmid[biqs_f2] = (1.0f - K / (dram->hmid[biqs_reso]*0.51763809f) + K * K) * norm;
	//hmid
	
	dram->lmid[biqs_freq] = (((powf(GetParameter( kParam_LMF ),3)*2200.0f)+200.0f)/GetSampleRate());
	if (dram->lmid[biqs_freq] < 0.0001f) dram->lmid[biqs_freq] = 0.0001f;
	dram->lmid[biqs_nonlin] = GetParameter( kParam_LMG );
	dram->lmid[biqs_level] = (dram->lmid[biqs_nonlin]*2.0f)-1.0f;
	if (dram->lmid[biqs_level] > 0.0f) dram->lmid[biqs_level] *= 2.0f;
	dram->lmid[biqs_reso] = ((0.5f+(dram->lmid[biqs_nonlin]*0.5f)+sqrt(dram->lmid[biqs_freq]))-(1.0f-powf(1.0f-GetParameter( kParam_LMR ),2.0f)))+0.5f+(dram->lmid[biqs_nonlin]*0.5f);
	K = tan(M_PI * dram->lmid[biqs_freq]);
	norm = 1.0f / (1.0f + K / (dram->lmid[biqs_reso]*1.93185165f) + K * K);
	dram->lmid[biqs_a0] = K / (dram->lmid[biqs_reso]*1.93185165f) * norm;
	dram->lmid[biqs_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->lmid[biqs_b2] = (1.0f - K / (dram->lmid[biqs_reso]*1.93185165f) + K * K) * norm;
	norm = 1.0f / (1.0f + K / (dram->lmid[biqs_reso]*0.70710678f) + K * K);
	dram->lmid[biqs_c0] = K / (dram->lmid[biqs_reso]*0.70710678f) * norm;
	dram->lmid[biqs_d1] = 2.0f * (K * K - 1.0f) * norm;
	dram->lmid[biqs_d2] = (1.0f - K / (dram->lmid[biqs_reso]*0.70710678f) + K * K) * norm;
	norm = 1.0f / (1.0f + K / (dram->lmid[biqs_reso]*0.51763809f) + K * K);
	dram->lmid[biqs_e0] = K / (dram->lmid[biqs_reso]*0.51763809f) * norm;
	dram->lmid[biqs_f1] = 2.0f * (K * K - 1.0f) * norm;
	dram->lmid[biqs_f2] = (1.0f - K / (dram->lmid[biqs_reso]*0.51763809f) + K * K) * norm;
	//lmid
	
	dram->bass[biqs_freq] = (((powf(GetParameter( kParam_BSF ),3)*570.0f)+30.0f)/GetSampleRate());
	if (dram->bass[biqs_freq] < 0.0001f) dram->bass[biqs_freq] = 0.0001f;
	dram->bass[biqs_nonlin] = GetParameter( kParam_BSG );
	dram->bass[biqs_level] = (dram->bass[biqs_nonlin]*2.0f)-1.0f;
	if (dram->bass[biqs_level] > 0.0f) dram->bass[biqs_level] *= 2.0f;
	dram->bass[biqs_reso] = ((0.5f+(dram->bass[biqs_nonlin]*0.5f)+sqrt(dram->bass[biqs_freq]))-(1.0f-powf(1.0f-GetParameter( kParam_BSR ),2.0f)))+0.5f+(dram->bass[biqs_nonlin]*0.5f);
	K = tan(M_PI * dram->bass[biqs_freq]);
	norm = 1.0f / (1.0f + K / (dram->bass[biqs_reso]*1.93185165f) + K * K);
	dram->bass[biqs_a0] = K / (dram->bass[biqs_reso]*1.93185165f) * norm;
	dram->bass[biqs_b1] = 2.0f * (K * K - 1.0f) * norm;
	dram->bass[biqs_b2] = (1.0f - K / (dram->bass[biqs_reso]*1.93185165f) + K * K) * norm;
	norm = 1.0f / (1.0f + K / (dram->bass[biqs_reso]*0.70710678f) + K * K);
	dram->bass[biqs_c0] = K / (dram->bass[biqs_reso]*0.70710678f) * norm;
	dram->bass[biqs_d1] = 2.0f * (K * K - 1.0f) * norm;
	dram->bass[biqs_d2] = (1.0f - K / (dram->bass[biqs_reso]*0.70710678f) + K * K) * norm;
	norm = 1.0f / (1.0f + K / (dram->bass[biqs_reso]*0.51763809f) + K * K);
	dram->bass[biqs_e0] = K / (dram->bass[biqs_reso]*0.51763809f) * norm;
	dram->bass[biqs_f1] = 2.0f * (K * K - 1.0f) * norm;
	dram->bass[biqs_f2] = (1.0f - K / (dram->bass[biqs_reso]*0.51763809f) + K * K) * norm;
	//bass
	
	float refdB = GetParameter( kParam_DSC );
	float topdB = 0.000000075f * powf(10.0f,refdB/20.0f) * overallscale;
	
	panA = panB; panB = GetParameter( kParam_PAN )*1.57079633f;
	inTrimA = inTrimB; inTrimB = GetParameter( kParam_FAD )*2.0f;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
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
		//first Highpass/Lowpass blocks aliasing before the nonlinearity of Parametric
		
		//get all Parametric bands before any other processing is done
		//begin Stacked Biquad With Reversed Neutron Flow L
		dram->high[biqs_outL] = inputSampleL * fabs(dram->high[biqs_level]);
		dram->high[biqs_dis] = fabs(dram->high[biqs_a0] * (1.0f+(dram->high[biqs_outL]*dram->high[biqs_nonlin])));
		if (dram->high[biqs_dis] > 1.0f) dram->high[biqs_dis] = 1.0f;
		dram->high[biqs_temp] = (dram->high[biqs_outL] * dram->high[biqs_dis]) + dram->high[biqs_aL1];
		dram->high[biqs_aL1] = dram->high[biqs_aL2] - (dram->high[biqs_temp]*dram->high[biqs_b1]);
		dram->high[biqs_aL2] = (dram->high[biqs_outL] * -dram->high[biqs_dis]) - (dram->high[biqs_temp]*dram->high[biqs_b2]);
		dram->high[biqs_outL] = dram->high[biqs_temp];
		dram->high[biqs_dis] = fabs(dram->high[biqs_c0] * (1.0f+(dram->high[biqs_outL]*dram->high[biqs_nonlin])));
		if (dram->high[biqs_dis] > 1.0f) dram->high[biqs_dis] = 1.0f;
		dram->high[biqs_temp] = (dram->high[biqs_outL] * dram->high[biqs_dis]) + dram->high[biqs_cL1];
		dram->high[biqs_cL1] = dram->high[biqs_cL2] - (dram->high[biqs_temp]*dram->high[biqs_d1]);
		dram->high[biqs_cL2] = (dram->high[biqs_outL] * -dram->high[biqs_dis]) - (dram->high[biqs_temp]*dram->high[biqs_d2]);
		dram->high[biqs_outL] = dram->high[biqs_temp];
		dram->high[biqs_dis] = fabs(dram->high[biqs_e0] * (1.0f+(dram->high[biqs_outL]*dram->high[biqs_nonlin])));
		if (dram->high[biqs_dis] > 1.0f) dram->high[biqs_dis] = 1.0f;
		dram->high[biqs_temp] = (dram->high[biqs_outL] * dram->high[biqs_dis]) + dram->high[biqs_eL1];
		dram->high[biqs_eL1] = dram->high[biqs_eL2] - (dram->high[biqs_temp]*dram->high[biqs_f1]);
		dram->high[biqs_eL2] = (dram->high[biqs_outL] * -dram->high[biqs_dis]) - (dram->high[biqs_temp]*dram->high[biqs_f2]);
		dram->high[biqs_outL] = dram->high[biqs_temp]; dram->high[biqs_outL] *= dram->high[biqs_level];
		if (dram->high[biqs_level] > 1.0f) dram->high[biqs_outL] *= dram->high[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		//begin Stacked Biquad With Reversed Neutron Flow L
		dram->hmid[biqs_outL] = inputSampleL * fabs(dram->hmid[biqs_level]);
		dram->hmid[biqs_dis] = fabs(dram->hmid[biqs_a0] * (1.0f+(dram->hmid[biqs_outL]*dram->hmid[biqs_nonlin])));
		if (dram->hmid[biqs_dis] > 1.0f) dram->hmid[biqs_dis] = 1.0f;
		dram->hmid[biqs_temp] = (dram->hmid[biqs_outL] * dram->hmid[biqs_dis]) + dram->hmid[biqs_aL1];
		dram->hmid[biqs_aL1] = dram->hmid[biqs_aL2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_b1]);
		dram->hmid[biqs_aL2] = (dram->hmid[biqs_outL] * -dram->hmid[biqs_dis]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_b2]);
		dram->hmid[biqs_outL] = dram->hmid[biqs_temp];
		dram->hmid[biqs_dis] = fabs(dram->hmid[biqs_c0] * (1.0f+(dram->hmid[biqs_outL]*dram->hmid[biqs_nonlin])));
		if (dram->hmid[biqs_dis] > 1.0f) dram->hmid[biqs_dis] = 1.0f;
		dram->hmid[biqs_temp] = (dram->hmid[biqs_outL] * dram->hmid[biqs_dis]) + dram->hmid[biqs_cL1];
		dram->hmid[biqs_cL1] = dram->hmid[biqs_cL2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_d1]);
		dram->hmid[biqs_cL2] = (dram->hmid[biqs_outL] * -dram->hmid[biqs_dis]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_d2]);
		dram->hmid[biqs_outL] = dram->hmid[biqs_temp];
		dram->hmid[biqs_dis] = fabs(dram->hmid[biqs_e0] * (1.0f+(dram->hmid[biqs_outL]*dram->hmid[biqs_nonlin])));
		if (dram->hmid[biqs_dis] > 1.0f) dram->hmid[biqs_dis] = 1.0f;
		dram->hmid[biqs_temp] = (dram->hmid[biqs_outL] * dram->hmid[biqs_dis]) + dram->hmid[biqs_eL1];
		dram->hmid[biqs_eL1] = dram->hmid[biqs_eL2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_f1]);
		dram->hmid[biqs_eL2] = (dram->hmid[biqs_outL] * -dram->hmid[biqs_dis]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_f2]);
		dram->hmid[biqs_outL] = dram->hmid[biqs_temp]; dram->hmid[biqs_outL] *= dram->hmid[biqs_level];
		if (dram->hmid[biqs_level] > 1.0f) dram->hmid[biqs_outL] *= dram->hmid[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		//begin Stacked Biquad With Reversed Neutron Flow L
		dram->lmid[biqs_outL] = inputSampleL * fabs(dram->lmid[biqs_level]);
		dram->lmid[biqs_dis] = fabs(dram->lmid[biqs_a0] * (1.0f+(dram->lmid[biqs_outL]*dram->lmid[biqs_nonlin])));
		if (dram->lmid[biqs_dis] > 1.0f) dram->lmid[biqs_dis] = 1.0f;
		dram->lmid[biqs_temp] = (dram->lmid[biqs_outL] * dram->lmid[biqs_dis]) + dram->lmid[biqs_aL1];
		dram->lmid[biqs_aL1] = dram->lmid[biqs_aL2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_b1]);
		dram->lmid[biqs_aL2] = (dram->lmid[biqs_outL] * -dram->lmid[biqs_dis]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_b2]);
		dram->lmid[biqs_outL] = dram->lmid[biqs_temp];
		dram->lmid[biqs_dis] = fabs(dram->lmid[biqs_c0] * (1.0f+(dram->lmid[biqs_outL]*dram->lmid[biqs_nonlin])));
		if (dram->lmid[biqs_dis] > 1.0f) dram->lmid[biqs_dis] = 1.0f;
		dram->lmid[biqs_temp] = (dram->lmid[biqs_outL] * dram->lmid[biqs_dis]) + dram->lmid[biqs_cL1];
		dram->lmid[biqs_cL1] = dram->lmid[biqs_cL2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_d1]);
		dram->lmid[biqs_cL2] = (dram->lmid[biqs_outL] * -dram->lmid[biqs_dis]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_d2]);
		dram->lmid[biqs_outL] = dram->lmid[biqs_temp];
		dram->lmid[biqs_dis] = fabs(dram->lmid[biqs_e0] * (1.0f+(dram->lmid[biqs_outL]*dram->lmid[biqs_nonlin])));
		if (dram->lmid[biqs_dis] > 1.0f) dram->lmid[biqs_dis] = 1.0f;
		dram->lmid[biqs_temp] = (dram->lmid[biqs_outL] * dram->lmid[biqs_dis]) + dram->lmid[biqs_eL1];
		dram->lmid[biqs_eL1] = dram->lmid[biqs_eL2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_f1]);
		dram->lmid[biqs_eL2] = (dram->lmid[biqs_outL] * -dram->lmid[biqs_dis]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_f2]);
		dram->lmid[biqs_outL] = dram->lmid[biqs_temp]; dram->lmid[biqs_outL] *= dram->lmid[biqs_level];
		if (dram->lmid[biqs_level] > 1.0f) dram->lmid[biqs_outL] *= dram->lmid[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		//begin Stacked Biquad With Reversed Neutron Flow L
		dram->bass[biqs_outL] = inputSampleL * fabs(dram->bass[biqs_level]);
		dram->bass[biqs_dis] = fabs(dram->bass[biqs_a0] * (1.0f+(dram->bass[biqs_outL]*dram->bass[biqs_nonlin])));
		if (dram->bass[biqs_dis] > 1.0f) dram->bass[biqs_dis] = 1.0f;
		dram->bass[biqs_temp] = (dram->bass[biqs_outL] * dram->bass[biqs_dis]) + dram->bass[biqs_aL1];
		dram->bass[biqs_aL1] = dram->bass[biqs_aL2] - (dram->bass[biqs_temp]*dram->bass[biqs_b1]);
		dram->bass[biqs_aL2] = (dram->bass[biqs_outL] * -dram->bass[biqs_dis]) - (dram->bass[biqs_temp]*dram->bass[biqs_b2]);
		dram->bass[biqs_outL] = dram->bass[biqs_temp];
		dram->bass[biqs_dis] = fabs(dram->bass[biqs_c0] * (1.0f+(dram->bass[biqs_outL]*dram->bass[biqs_nonlin])));
		if (dram->bass[biqs_dis] > 1.0f) dram->bass[biqs_dis] = 1.0f;
		dram->bass[biqs_temp] = (dram->bass[biqs_outL] * dram->bass[biqs_dis]) + dram->bass[biqs_cL1];
		dram->bass[biqs_cL1] = dram->bass[biqs_cL2] - (dram->bass[biqs_temp]*dram->bass[biqs_d1]);
		dram->bass[biqs_cL2] = (dram->bass[biqs_outL] * -dram->bass[biqs_dis]) - (dram->bass[biqs_temp]*dram->bass[biqs_d2]);
		dram->bass[biqs_outL] = dram->bass[biqs_temp];
		dram->bass[biqs_dis] = fabs(dram->bass[biqs_e0] * (1.0f+(dram->bass[biqs_outL]*dram->bass[biqs_nonlin])));
		if (dram->bass[biqs_dis] > 1.0f) dram->bass[biqs_dis] = 1.0f;
		dram->bass[biqs_temp] = (dram->bass[biqs_outL] * dram->bass[biqs_dis]) + dram->bass[biqs_eL1];
		dram->bass[biqs_eL1] = dram->bass[biqs_eL2] - (dram->bass[biqs_temp]*dram->bass[biqs_f1]);
		dram->bass[biqs_eL2] = (dram->bass[biqs_outL] * -dram->bass[biqs_dis]) - (dram->bass[biqs_temp]*dram->bass[biqs_f2]);
		dram->bass[biqs_outL] = dram->bass[biqs_temp]; dram->bass[biqs_outL] *= dram->bass[biqs_level];
		if (dram->bass[biqs_level] > 1.0f) dram->bass[biqs_outL] *= dram->bass[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow L
		
		//begin Stacked Biquad With Reversed Neutron Flow R
		dram->high[biqs_outR] = inputSampleR * fabs(dram->high[biqs_level]);
		dram->high[biqs_dis] = fabs(dram->high[biqs_a0] * (1.0f+(dram->high[biqs_outR]*dram->high[biqs_nonlin])));
		if (dram->high[biqs_dis] > 1.0f) dram->high[biqs_dis] = 1.0f;
		dram->high[biqs_temp] = (dram->high[biqs_outR] * dram->high[biqs_dis]) + dram->high[biqs_aR1];
		dram->high[biqs_aR1] = dram->high[biqs_aR2] - (dram->high[biqs_temp]*dram->high[biqs_b1]);
		dram->high[biqs_aR2] = (dram->high[biqs_outR] * -dram->high[biqs_dis]) - (dram->high[biqs_temp]*dram->high[biqs_b2]);
		dram->high[biqs_outR] = dram->high[biqs_temp];
		dram->high[biqs_dis] = fabs(dram->high[biqs_c0] * (1.0f+(dram->high[biqs_outR]*dram->high[biqs_nonlin])));
		if (dram->high[biqs_dis] > 1.0f) dram->high[biqs_dis] = 1.0f;
		dram->high[biqs_temp] = (dram->high[biqs_outR] * dram->high[biqs_dis]) + dram->high[biqs_cR1];
		dram->high[biqs_cR1] = dram->high[biqs_cR2] - (dram->high[biqs_temp]*dram->high[biqs_d1]);
		dram->high[biqs_cR2] = (dram->high[biqs_outR] * -dram->high[biqs_dis]) - (dram->high[biqs_temp]*dram->high[biqs_d2]);
		dram->high[biqs_outR] = dram->high[biqs_temp];
		dram->high[biqs_dis] = fabs(dram->high[biqs_e0] * (1.0f+(dram->high[biqs_outR]*dram->high[biqs_nonlin])));
		if (dram->high[biqs_dis] > 1.0f) dram->high[biqs_dis] = 1.0f;
		dram->high[biqs_temp] = (dram->high[biqs_outR] * dram->high[biqs_dis]) + dram->high[biqs_eR1];
		dram->high[biqs_eR1] = dram->high[biqs_eR2] - (dram->high[biqs_temp]*dram->high[biqs_f1]);
		dram->high[biqs_eR2] = (dram->high[biqs_outR] * -dram->high[biqs_dis]) - (dram->high[biqs_temp]*dram->high[biqs_f2]);
		dram->high[biqs_outR] = dram->high[biqs_temp]; dram->high[biqs_outR] *= dram->high[biqs_level];
		if (dram->high[biqs_level] > 1.0f) dram->high[biqs_outR] *= dram->high[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow R
		
		//begin Stacked Biquad With Reversed Neutron Flow R
		dram->hmid[biqs_outR] = inputSampleR * fabs(dram->hmid[biqs_level]);
		dram->hmid[biqs_dis] = fabs(dram->hmid[biqs_a0] * (1.0f+(dram->hmid[biqs_outR]*dram->hmid[biqs_nonlin])));
		if (dram->hmid[biqs_dis] > 1.0f) dram->hmid[biqs_dis] = 1.0f;
		dram->hmid[biqs_temp] = (dram->hmid[biqs_outR] * dram->hmid[biqs_dis]) + dram->hmid[biqs_aR1];
		dram->hmid[biqs_aR1] = dram->hmid[biqs_aR2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_b1]);
		dram->hmid[biqs_aR2] = (dram->hmid[biqs_outR] * -dram->hmid[biqs_dis]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_b2]);
		dram->hmid[biqs_outR] = dram->hmid[biqs_temp];
		dram->hmid[biqs_dis] = fabs(dram->hmid[biqs_c0] * (1.0f+(dram->hmid[biqs_outR]*dram->hmid[biqs_nonlin])));
		if (dram->hmid[biqs_dis] > 1.0f) dram->hmid[biqs_dis] = 1.0f;
		dram->hmid[biqs_temp] = (dram->hmid[biqs_outR] * dram->hmid[biqs_dis]) + dram->hmid[biqs_cR1];
		dram->hmid[biqs_cR1] = dram->hmid[biqs_cR2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_d1]);
		dram->hmid[biqs_cR2] = (dram->hmid[biqs_outR] * -dram->hmid[biqs_dis]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_d2]);
		dram->hmid[biqs_outR] = dram->hmid[biqs_temp];
		dram->hmid[biqs_dis] = fabs(dram->hmid[biqs_e0] * (1.0f+(dram->hmid[biqs_outR]*dram->hmid[biqs_nonlin])));
		if (dram->hmid[biqs_dis] > 1.0f) dram->hmid[biqs_dis] = 1.0f;
		dram->hmid[biqs_temp] = (dram->hmid[biqs_outR] * dram->hmid[biqs_dis]) + dram->hmid[biqs_eR1];
		dram->hmid[biqs_eR1] = dram->hmid[biqs_eR2] - (dram->hmid[biqs_temp]*dram->hmid[biqs_f1]);
		dram->hmid[biqs_eR2] = (dram->hmid[biqs_outR] * -dram->hmid[biqs_dis]) - (dram->hmid[biqs_temp]*dram->hmid[biqs_f2]);
		dram->hmid[biqs_outR] = dram->hmid[biqs_temp]; dram->hmid[biqs_outR] *= dram->hmid[biqs_level];
		if (dram->hmid[biqs_level] > 1.0f) dram->hmid[biqs_outR] *= dram->hmid[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow R
		
		//begin Stacked Biquad With Reversed Neutron Flow R
		dram->lmid[biqs_outR] = inputSampleR * fabs(dram->lmid[biqs_level]);
		dram->lmid[biqs_dis] = fabs(dram->lmid[biqs_a0] * (1.0f+(dram->lmid[biqs_outR]*dram->lmid[biqs_nonlin])));
		if (dram->lmid[biqs_dis] > 1.0f) dram->lmid[biqs_dis] = 1.0f;
		dram->lmid[biqs_temp] = (dram->lmid[biqs_outR] * dram->lmid[biqs_dis]) + dram->lmid[biqs_aR1];
		dram->lmid[biqs_aR1] = dram->lmid[biqs_aR2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_b1]);
		dram->lmid[biqs_aR2] = (dram->lmid[biqs_outR] * -dram->lmid[biqs_dis]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_b2]);
		dram->lmid[biqs_outR] = dram->lmid[biqs_temp];
		dram->lmid[biqs_dis] = fabs(dram->lmid[biqs_c0] * (1.0f+(dram->lmid[biqs_outR]*dram->lmid[biqs_nonlin])));
		if (dram->lmid[biqs_dis] > 1.0f) dram->lmid[biqs_dis] = 1.0f;
		dram->lmid[biqs_temp] = (dram->lmid[biqs_outR] * dram->lmid[biqs_dis]) + dram->lmid[biqs_cR1];
		dram->lmid[biqs_cR1] = dram->lmid[biqs_cR2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_d1]);
		dram->lmid[biqs_cR2] = (dram->lmid[biqs_outR] * -dram->lmid[biqs_dis]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_d2]);
		dram->lmid[biqs_outR] = dram->lmid[biqs_temp];
		dram->lmid[biqs_dis] = fabs(dram->lmid[biqs_e0] * (1.0f+(dram->lmid[biqs_outR]*dram->lmid[biqs_nonlin])));
		if (dram->lmid[biqs_dis] > 1.0f) dram->lmid[biqs_dis] = 1.0f;
		dram->lmid[biqs_temp] = (dram->lmid[biqs_outR] * dram->lmid[biqs_dis]) + dram->lmid[biqs_eR1];
		dram->lmid[biqs_eR1] = dram->lmid[biqs_eR2] - (dram->lmid[biqs_temp]*dram->lmid[biqs_f1]);
		dram->lmid[biqs_eR2] = (dram->lmid[biqs_outR] * -dram->lmid[biqs_dis]) - (dram->lmid[biqs_temp]*dram->lmid[biqs_f2]);
		dram->lmid[biqs_outR] = dram->lmid[biqs_temp]; dram->lmid[biqs_outR] *= dram->lmid[biqs_level];
		if (dram->lmid[biqs_level] > 1.0f) dram->lmid[biqs_outR] *= dram->lmid[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow R
		
		//begin Stacked Biquad With Reversed Neutron Flow R
		dram->bass[biqs_outR] = inputSampleR * fabs(dram->bass[biqs_level]);
		dram->bass[biqs_dis] = fabs(dram->bass[biqs_a0] * (1.0f+(dram->bass[biqs_outR]*dram->bass[biqs_nonlin])));
		if (dram->bass[biqs_dis] > 1.0f) dram->bass[biqs_dis] = 1.0f;
		dram->bass[biqs_temp] = (dram->bass[biqs_outR] * dram->bass[biqs_dis]) + dram->bass[biqs_aR1];
		dram->bass[biqs_aR1] = dram->bass[biqs_aR2] - (dram->bass[biqs_temp]*dram->bass[biqs_b1]);
		dram->bass[biqs_aR2] = (dram->bass[biqs_outR] * -dram->bass[biqs_dis]) - (dram->bass[biqs_temp]*dram->bass[biqs_b2]);
		dram->bass[biqs_outR] = dram->bass[biqs_temp];
		dram->bass[biqs_dis] = fabs(dram->bass[biqs_c0] * (1.0f+(dram->bass[biqs_outR]*dram->bass[biqs_nonlin])));
		if (dram->bass[biqs_dis] > 1.0f) dram->bass[biqs_dis] = 1.0f;
		dram->bass[biqs_temp] = (dram->bass[biqs_outR] * dram->bass[biqs_dis]) + dram->bass[biqs_cR1];
		dram->bass[biqs_cR1] = dram->bass[biqs_cR2] - (dram->bass[biqs_temp]*dram->bass[biqs_d1]);
		dram->bass[biqs_cR2] = (dram->bass[biqs_outR] * -dram->bass[biqs_dis]) - (dram->bass[biqs_temp]*dram->bass[biqs_d2]);
		dram->bass[biqs_outR] = dram->bass[biqs_temp];
		dram->bass[biqs_dis] = fabs(dram->bass[biqs_e0] * (1.0f+(dram->bass[biqs_outR]*dram->bass[biqs_nonlin])));
		if (dram->bass[biqs_dis] > 1.0f) dram->bass[biqs_dis] = 1.0f;
		dram->bass[biqs_temp] = (dram->bass[biqs_outR] * dram->bass[biqs_dis]) + dram->bass[biqs_eR1];
		dram->bass[biqs_eR1] = dram->bass[biqs_eR2] - (dram->bass[biqs_temp]*dram->bass[biqs_f1]);
		dram->bass[biqs_eR2] = (dram->bass[biqs_outR] * -dram->bass[biqs_dis]) - (dram->bass[biqs_temp]*dram->bass[biqs_f2]);
		dram->bass[biqs_outR] = dram->bass[biqs_temp]; dram->bass[biqs_outR] *= dram->bass[biqs_level];
		if (dram->bass[biqs_level] > 1.0f) dram->bass[biqs_outR] *= dram->bass[biqs_level];
		//end Stacked Biquad With Reversed Neutron Flow R
		
		float temp = (float)nSampleFrames/inFramesToProcess;		
		float gainR = (panA*temp)+(panB*(1.0f-temp));
		float gainL = 1.57079633f-gainR;
		gainR = sin(gainR); gainL = sin(gainL);
		float gain = (inTrimA*temp)+(inTrimB*(1.0f-temp));
		if (gain > 1.0f) gain *= gain; else gain = 1.0f-powf(1.0f-gain,2);
		gain *= 0.763932022500211f;
		float airGain = (airGainA*temp)+(airGainB*(1.0f-temp));
		if (airGain > 1.0f) airGain *= airGain; else airGain = 1.0f-powf(1.0f-airGain,2);
		float fireGain = (fireGainA*temp)+(fireGainB*(1.0f-temp));
		if (fireGain > 1.0f) fireGain *= fireGain; else fireGain = 1.0f-powf(1.0f-fireGain,2);
		float firePad = fireGain; if (firePad > 1.0f) firePad = 1.0f;
		float stoneGain = (stoneGainA*temp)+(stoneGainB*(1.0f-temp));
		if (stoneGain > 1.0f) stoneGain *= stoneGain; else stoneGain = 1.0f-powf(1.0f-stoneGain,2);
		float stonePad = stoneGain; if (stonePad > 1.0f) stonePad = 1.0f;
		//set up smoothed gain controls
		
		//begin Air3L
		float drySampleL = inputSampleL;
		dram->air[pvSL4] = dram->air[pvAL4] - dram->air[pvAL3]; dram->air[pvSL3] = dram->air[pvAL3] - dram->air[pvAL2];
		dram->air[pvSL2] = dram->air[pvAL2] - dram->air[pvAL1]; dram->air[pvSL1] = dram->air[pvAL1] - inputSampleL;
		dram->air[accSL3] = dram->air[pvSL4] - dram->air[pvSL3]; dram->air[accSL2] = dram->air[pvSL3] - dram->air[pvSL2];
		dram->air[accSL1] = dram->air[pvSL2] - dram->air[pvSL1];
		dram->air[acc2SL2] = dram->air[accSL3] - dram->air[accSL2]; dram->air[acc2SL1] = dram->air[accSL2] - dram->air[accSL1];		
		dram->air[outAL] = -(dram->air[pvAL1] + dram->air[pvSL3] + dram->air[acc2SL2] - ((dram->air[acc2SL2] + dram->air[acc2SL1])*0.5f));
		dram->air[gainAL] *= 0.5f; dram->air[gainAL] += fabs(drySampleL-dram->air[outAL])*0.5f;
		if (dram->air[gainAL] > 0.3f*sqrt(overallscale)) dram->air[gainAL] = 0.3f*sqrt(overallscale);
		dram->air[pvAL4] = dram->air[pvAL3]; dram->air[pvAL3] = dram->air[pvAL2];
		dram->air[pvAL2] = dram->air[pvAL1]; dram->air[pvAL1] = (dram->air[gainAL] * dram->air[outAL]) + drySampleL;
		float fireL = drySampleL - ((dram->air[outAL]*0.5f)+(drySampleL*(0.457f-(0.017f*overallscale))));
		temp = (fireL + dram->air[gndavgL])*0.5f; dram->air[gndavgL] = fireL; fireL = temp;
		float airL = (drySampleL-fireL)*airGain;
		inputSampleL = fireL;
		//end Air3L
		//begin Air3R
		float drySampleR = inputSampleR;
		dram->air[pvSR4] = dram->air[pvAR4] - dram->air[pvAR3]; dram->air[pvSR3] = dram->air[pvAR3] - dram->air[pvAR2];
		dram->air[pvSR2] = dram->air[pvAR2] - dram->air[pvAR1]; dram->air[pvSR1] = dram->air[pvAR1] - inputSampleR;
		dram->air[accSR3] = dram->air[pvSR4] - dram->air[pvSR3]; dram->air[accSR2] = dram->air[pvSR3] - dram->air[pvSR2];
		dram->air[accSR1] = dram->air[pvSR2] - dram->air[pvSR1];
		dram->air[acc2SR2] = dram->air[accSR3] - dram->air[accSR2]; dram->air[acc2SR1] = dram->air[accSR2] - dram->air[accSR1];		
		dram->air[outAR] = -(dram->air[pvAR1] + dram->air[pvSR3] + dram->air[acc2SR2] - ((dram->air[acc2SR2] + dram->air[acc2SR1])*0.5f));
		dram->air[gainAR] *= 0.5f; dram->air[gainAR] += fabs(drySampleR-dram->air[outAR])*0.5f;
		if (dram->air[gainAR] > 0.3f*sqrt(overallscale)) dram->air[gainAR] = 0.3f*sqrt(overallscale);
		dram->air[pvAR4] = dram->air[pvAR3]; dram->air[pvAR3] = dram->air[pvAR2];
		dram->air[pvAR2] = dram->air[pvAR1]; dram->air[pvAR1] = (dram->air[gainAR] * dram->air[outAR]) + drySampleR;
		float fireR = drySampleR - ((dram->air[outAR]*0.5f)+(drySampleR*(0.457f-(0.017f*overallscale))));
		temp = (fireR + dram->air[gndavgR])*0.5f; dram->air[gndavgR] = fireR; fireR = temp;
		float airR = (drySampleR-fireR)*airGain;
		inputSampleR = fireR;
		//end Air3R
		//begin KalmanL
		temp = inputSampleL = inputSampleL*(1.0f-kalmanRange)*0.777f;
		inputSampleL *= (1.0f-kalmanRange);
		//set up gain levels to control the beast
		dram->kal[prevSlewL3] += dram->kal[prevSampL3] - dram->kal[prevSampL2]; dram->kal[prevSlewL3] *= 0.5f;
		dram->kal[prevSlewL2] += dram->kal[prevSampL2] - dram->kal[prevSampL1]; dram->kal[prevSlewL2] *= 0.5f;
		dram->kal[prevSlewL1] += dram->kal[prevSampL1] - inputSampleL; dram->kal[prevSlewL1] *= 0.5f;
		//make slews from each set of samples used
		dram->kal[accSlewL2] += dram->kal[prevSlewL3] - dram->kal[prevSlewL2]; dram->kal[accSlewL2] *= 0.5f;
		dram->kal[accSlewL1] += dram->kal[prevSlewL2] - dram->kal[prevSlewL1]; dram->kal[accSlewL1] *= 0.5f;
		//differences between slews: rate of change of rate of change
		dram->kal[accSlewL3] += (dram->kal[accSlewL2] - dram->kal[accSlewL1]); dram->kal[accSlewL3] *= 0.5f;
		//entering the abyss, what even is this
		dram->kal[kalOutL] += dram->kal[prevSampL1] + dram->kal[prevSlewL2] + dram->kal[accSlewL3]; dram->kal[kalOutL] *= 0.5f;
		//resynthesizing predicted result (all iir smoothed)
		dram->kal[kalGainL] += fabs(temp-dram->kal[kalOutL])*kalmanRange*8.0f; dram->kal[kalGainL] *= 0.5f;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (dram->kal[kalGainL] > kalmanRange*0.5f) dram->kal[kalGainL] = kalmanRange*0.5f;
		//attempts to avoid explosions
		dram->kal[kalOutL] += (temp*(1.0f-(0.68f+(kalmanRange*0.157f))));	
		//this is for tuning a really complete cancellation up around Nyquist
		dram->kal[prevSampL3] = dram->kal[prevSampL2]; dram->kal[prevSampL2] = dram->kal[prevSampL1];
		dram->kal[prevSampL1] = (dram->kal[kalGainL] * dram->kal[kalOutL]) + ((1.0f-dram->kal[kalGainL])*temp);
		//feed the chain of previous samples
		if (dram->kal[prevSampL1] > 1.0f) dram->kal[prevSampL1] = 1.0f; if (dram->kal[prevSampL1] < -1.0f) dram->kal[prevSampL1] = -1.0f;
		float stoneL = dram->kal[kalOutL]*0.777f;
		fireL -= stoneL;
		//end KalmanL
		//begin KalmanR
		temp = inputSampleR = inputSampleR*(1.0f-kalmanRange)*0.777f;
		inputSampleR *= (1.0f-kalmanRange);
		//set up gain levels to control the beast
		dram->kal[prevSlewR3] += dram->kal[prevSampR3] - dram->kal[prevSampR2]; dram->kal[prevSlewR3] *= 0.5f;
		dram->kal[prevSlewR2] += dram->kal[prevSampR2] - dram->kal[prevSampR1]; dram->kal[prevSlewR2] *= 0.5f;
		dram->kal[prevSlewR1] += dram->kal[prevSampR1] - inputSampleR; dram->kal[prevSlewR1] *= 0.5f;
		//make slews from each set of samples used
		dram->kal[accSlewR2] += dram->kal[prevSlewR3] - dram->kal[prevSlewR2]; dram->kal[accSlewR2] *= 0.5f;
		dram->kal[accSlewR1] += dram->kal[prevSlewR2] - dram->kal[prevSlewR1]; dram->kal[accSlewR1] *= 0.5f;
		//differences between slews: rate of change of rate of change
		dram->kal[accSlewR3] += (dram->kal[accSlewR2] - dram->kal[accSlewR1]); dram->kal[accSlewR3] *= 0.5f;
		//entering the abyss, what even is this
		dram->kal[kalOutR] += dram->kal[prevSampR1] + dram->kal[prevSlewR2] + dram->kal[accSlewR3]; dram->kal[kalOutR] *= 0.5f;
		//resynthesizing predicted result (all iir smoothed)
		dram->kal[kalGainR] += fabs(temp-dram->kal[kalOutR])*kalmanRange*8.0f; dram->kal[kalGainR] *= 0.5f;
		//madness takes its toll. Kalman Gain: how much dry to retain
		if (dram->kal[kalGainR] > kalmanRange*0.5f) dram->kal[kalGainR] = kalmanRange*0.5f;
		//attempts to avoid explosions
		dram->kal[kalOutR] += (temp*(1.0f-(0.68f+(kalmanRange*0.157f))));	
		//this is for tuning a really complete cancellation up around Nyquist
		dram->kal[prevSampR3] = dram->kal[prevSampR2]; dram->kal[prevSampR2] = dram->kal[prevSampR1];
		dram->kal[prevSampR1] = (dram->kal[kalGainR] * dram->kal[kalOutR]) + ((1.0f-dram->kal[kalGainR])*temp);
		//feed the chain of previous samples
		if (dram->kal[prevSampR1] > 1.0f) dram->kal[prevSampR1] = 1.0f; if (dram->kal[prevSampR1] < -1.0f) dram->kal[prevSampR1] = -1.0f;
		float stoneR = dram->kal[kalOutR]*0.777f;
		fireR -= stoneR;
		//end KalmanR
		//fire dynamics
		if (fabs(fireL) > compFThresh) { //compression L
			fireCompL -= (fireCompL * compFAttack);
			fireCompL += ((compFThresh / fabs(fireL))*compFAttack);
		} else fireCompL = (fireCompL*(1.0f-compFRelease))+compFRelease;
		if (fabs(fireR) > compFThresh) { //compression R
			fireCompR -= (fireCompR * compFAttack);
			fireCompR += ((compFThresh / fabs(fireR))*compFAttack);
		} else fireCompR = (fireCompR*(1.0f-compFRelease))+compFRelease;
		if (fireCompL > fireCompR) fireCompL -= (fireCompL * compFAttack);
		if (fireCompR > fireCompL) fireCompR -= (fireCompR * compFAttack);
		if (fabs(fireL) > gateFThresh) fireGate = gateFSustain;
		else if (fabs(fireR) > gateFThresh) fireGate = gateFSustain;
		else fireGate *= (1.0f-gateFRelease);
		if (fireGate < 0.0f) fireGate = 0.0f;
		fireCompL = fmax(fmin(fireCompL,1.0f),0.0f);
		fireCompR = fmax(fmin(fireCompR,1.0f),0.0f);
		fireL *= (((1.0f-compFRatio)*firePad)+(fireCompL*compFRatio*fireGain));
		fireR *= (((1.0f-compFRatio)*firePad)+(fireCompR*compFRatio*fireGain));
		if (fireGate < M_PI_2) {
			temp = ((1.0f-gateFRatio)+(sin(fireGate)*gateFRatio));
			airL *= temp;
			airR *= temp;
			fireL *= temp;
			fireR *= temp;
			dram->high[biqs_outL] *= temp;
			dram->high[biqs_outR] *= temp;
			dram->hmid[biqs_outL] *= temp; //if Fire gating, gate Air, high and hmid
			dram->hmid[biqs_outR] *= temp; //note that we aren't compressing these
		}
		//stone dynamics
		if (fabs(stoneL) > compSThresh) { //compression L
			stoneCompL -= (stoneCompL * compSAttack);
			stoneCompL += ((compSThresh / fabs(stoneL))*compSAttack);
		} else stoneCompL = (stoneCompL*(1.0f-compSRelease))+compSRelease;
		if (fabs(stoneR) > compSThresh) { //compression R
			stoneCompR -= (stoneCompR * compSAttack);
			stoneCompR += ((compSThresh / fabs(stoneR))*compSAttack);
		} else stoneCompR = (stoneCompR*(1.0f-compSRelease))+compSRelease;
		if (stoneCompL > stoneCompR) stoneCompL -= (stoneCompL * compSAttack);
		if (stoneCompR > stoneCompL) stoneCompR -= (stoneCompR * compSAttack);
		if (fabs(stoneL) > gateSThresh) stoneGate = gateSSustain;
		else if (fabs(stoneR) > gateSThresh) stoneGate = gateSSustain;
		else stoneGate *= (1.0f-gateSRelease);
		if (stoneGate < 0.0f) stoneGate = 0.0f;
		stoneCompL = fmax(fmin(stoneCompL,1.0f),0.0f);
		stoneCompR = fmax(fmin(stoneCompR,1.0f),0.0f);
		stoneL *= (((1.0f-compSRatio)*stonePad)+(stoneCompL*compSRatio*stoneGain));
		stoneR *= (((1.0f-compSRatio)*stonePad)+(stoneCompR*compSRatio*stoneGain));
		if (stoneGate < M_PI_2) {
			temp = ((1.0f-gateSRatio)+(sin(stoneGate)*gateSRatio));
			stoneL *= temp;
			stoneR *= temp;
			dram->lmid[biqs_outL] *= temp;
			dram->lmid[biqs_outR] *= temp;
			dram->bass[biqs_outL] *= temp; //if Stone gating, gate lmid and bass
			dram->bass[biqs_outR] *= temp; //note that we aren't compressing these
		}
		inputSampleL = stoneL + fireL + airL;
		inputSampleR = stoneR + fireR + airR;
		//create Stonefire output
		
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
		
		inputSampleL += (dram->high[biqs_outL] + dram->hmid[biqs_outL] + dram->lmid[biqs_outL] + dram->bass[biqs_outL]);
		inputSampleR += (dram->high[biqs_outR] + dram->hmid[biqs_outR] + dram->lmid[biqs_outR] + dram->bass[biqs_outR]);
		//add parametric boosts or cuts: clean as possible for maximal rawness and sonority
		
		inputSampleL = inputSampleL * gainL * gain;
		inputSampleR = inputSampleR * gainR * gain;
		//applies pan section, and smoothed fader gain
		
		inputSampleL *= topdB;
		if (inputSampleL < -0.222f) inputSampleL = -0.222f; if (inputSampleL > 0.222f) inputSampleL = 0.222f;
		dram->dBaL[dBaXL] = inputSampleL; dBaPosL *= 0.5f; dBaPosL += fabs((inputSampleL*((inputSampleL*0.25f)-0.5f))*0.5f);
		int dBdly = floor(dBaPosL*dscBuf);
		float dBi = (dBaPosL*dscBuf)-dBdly;
		inputSampleL = dram->dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
		dBdly++; inputSampleL += dram->dBaL[dBaXL-dBdly +((dBaXL-dBdly < 0)?dscBuf:0)]*dBi;
		dBaXL++; if (dBaXL < 0 || dBaXL >= dscBuf) dBaXL = 0;
		inputSampleL /= topdB;		
		inputSampleR *= topdB;
		if (inputSampleR < -0.222f) inputSampleR = -0.222f; if (inputSampleR > 0.222f) inputSampleR = 0.222f;
		dram->dBaR[dBaXR] = inputSampleR; dBaPosR *= 0.5f; dBaPosR += fabs((inputSampleR*((inputSampleR*0.25f)-0.5f))*0.5f);
		dBdly = floor(dBaPosR*dscBuf);
		dBi = (dBaPosR*dscBuf)-dBdly;
		inputSampleR = dram->dBaR[dBaXR-dBdly +((dBaXR-dBdly < 0)?dscBuf:0)]*(1.0f-dBi);
		dBdly++; inputSampleR += dram->dBaR[dBaXR-dBdly +((dBaXR-dBdly < 0)?dscBuf:0)]*dBi;
		dBaXR++; if (dBaXR < 0 || dBaXR >= dscBuf) dBaXR = 0;
		inputSampleR /= topdB;		
		//top dB processing for distributed discontinuity modeling air nonlinearity
				
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
	for (int x = 0; x < hilp_total; x++) {
		dram->highpass[x] = 0.0;
		dram->lowpass[x] = 0.0;
	}
	
	for (int x = 0; x < air_total; x++) dram->air[x] = 0.0;
	for (int x = 0; x < kal_total; x++) dram->kal[x] = 0.0;
	fireCompL = 1.0;
	fireCompR = 1.0;
	fireGate = 1.0;
	stoneCompL = 1.0;
	stoneCompR = 1.0;
	stoneGate = 1.0;	
	
	for (int x = 0; x < biqs_total; x++) {
		dram->high[x] = 0.0;
		dram->hmid[x] = 0.0;
		dram->lmid[x] = 0.0;
		dram->bass[x] = 0.0;
	}
	
	for(int count = 0; count < dscBuf+2; count++) {
		dram->dBaL[count] = 0.0;
		dram->dBaR[count] = 0.0;
	}
	dBaPosL = 0.0;
	dBaPosR = 0.0;
	dBaXL = 1;
	dBaXR = 1;
	
	airGainA = 0.5; airGainB = 0.5;
	fireGainA = 0.5; fireGainB = 0.5;
	stoneGainA = 0.5; stoneGainB = 0.5;
	panA = 0.5; panB = 0.5;
	inTrimA = 1.0; inTrimB = 1.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
