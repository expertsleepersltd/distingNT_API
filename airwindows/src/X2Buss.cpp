#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "X2Buss"
#define AIRWINDOWS_DESCRIPTION "An updated version of the buss processing from ConsoleX2."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','X','2','s' )
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	kParam_G =6,
	kParam_H =7,
	kParam_I =8,
	kParam_J =9,
	//Add your parameters here...
	kNumberOfParameters=10
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "High", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HMid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LMid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HighF", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HMidF", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LMidF", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "BassF", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Thresh", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fader", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, };
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
	}; //coefficient interpolating bessel filter, stereo
	float highLIIR;
	float highRIIR;
	
	float midLIIR;
	float midRIIR;
	
	float lowLIIR;
	float lowRIIR;
	//SmoothEQ2
	
	enum {
		bez_AL,
		bez_BL,
		bez_CL,
		bez_InL,
		bez_UnInL,
		bez_SampL,
		bez_AR,
		bez_BR,
		bez_CR,
		bez_InR,
		bez_UnInR,
		bez_SampR,
		bez_cycle,
		bez_total
	}; //the new undersampling. bez signifies the bezier curve reconstruction
	float bezMaxF;
	//Dynamics2
	
	float lastSampleL;
	bool wasPosClipL;
	bool wasNegClipL;
	float lastSampleR;
	bool wasPosClipR;
	bool wasNegClipR; //Stereo ClipOnly3	
	
	float inTrimA;
	float inTrimB;
	uint32_t fpdL;
	uint32_t fpdR;

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
	float bezCompF[bez_total];
	float bezCompS[bez_total];
	float intermediateL[18];
	float slewL[34];
	float intermediateR[18];
	float slewR[34];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	int spacing = floor(overallscale); //should give us working basic scaling, usually 2 or 4
	if (spacing < 1) spacing = 1; if (spacing > 16) spacing = 16;
	
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
	float biqK = 2.0f-cos(omega);
	float highCoef = -sqrt((biqK*biqK)-1.0f)+biqK;
	dram->highA[biq_reso] = 2.24697960f * highQ;
	dram->highB[biq_reso] = 0.80193774f * highQ;
	dram->highC[biq_reso] = 0.55495813f * highQ;
	
	dram->midA[biq_freq] = ((powf(midF,3)*20000.0f)/GetSampleRate());
	dram->midC[biq_freq] = dram->midB[biq_freq] = dram->midA[biq_freq] = fmax(fmin(dram->midA[biq_freq],0.4999f),0.00025f);	
	float midFreq = powf(midF,3)*20000.0f;
	omega = 2.0f*M_PI*(midFreq/GetSampleRate());
	biqK = 2.0f-cos(omega);
	float midCoef = -sqrt((biqK*biqK)-1.0f)+biqK;
	dram->midA[biq_reso] = 2.24697960f * midQ;
	dram->midB[biq_reso] = 0.80193774f * midQ;
	dram->midC[biq_reso] = 0.55495813f * midQ;
	
	dram->lowA[biq_freq] = ((powf(bassF,3)*20000.0f)/GetSampleRate());
	dram->lowC[biq_freq] = dram->lowB[biq_freq] = dram->lowA[biq_freq] = fmax(fmin(dram->lowA[biq_freq],0.4999f),0.00025f);
	float lowFreq = powf(bassF,3)*20000.0f;
	omega = 2.0f*M_PI*(lowFreq/GetSampleRate());
	biqK = 2.0f-cos(omega);
	float lowCoef = -sqrt((biqK*biqK)-1.0f)+biqK;
	dram->lowA[biq_reso] = 2.24697960f * lowQ;
	dram->lowB[biq_reso] = 0.80193774f * lowQ;
	dram->lowC[biq_reso] = 0.55495813f * lowQ;
	
	biqK = tan(M_PI * dram->highA[biq_freq]);
	float norm = 1.0f / (1.0f + biqK / dram->highA[biq_reso] + biqK * biqK);
	dram->highA[biq_a0] = biqK * biqK * norm;
	dram->highA[biq_a1] = 2.0f * dram->highA[biq_a0];
	dram->highA[biq_a2] = dram->highA[biq_a0];
	dram->highA[biq_b1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->highA[biq_b2] = (1.0f - biqK / dram->highA[biq_reso] + biqK * biqK) * norm;
	biqK = tan(M_PI * dram->highB[biq_freq]);
	norm = 1.0f / (1.0f + biqK / dram->highB[biq_reso] + biqK * biqK);
	dram->highB[biq_a0] = biqK * biqK * norm;
	dram->highB[biq_a1] = 2.0f * dram->highB[biq_a0];
	dram->highB[biq_a2] = dram->highB[biq_a0];
	dram->highB[biq_b1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->highB[biq_b2] = (1.0f - biqK / dram->highB[biq_reso] + biqK * biqK) * norm;
	biqK = tan(M_PI * dram->highC[biq_freq]);
	norm = 1.0f / (1.0f + biqK / dram->highC[biq_reso] + biqK * biqK);
	dram->highC[biq_a0] = biqK * biqK * norm;
	dram->highC[biq_a1] = 2.0f * dram->highC[biq_a0];
	dram->highC[biq_a2] = dram->highC[biq_a0];
	dram->highC[biq_b1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->highC[biq_b2] = (1.0f - biqK / dram->highC[biq_reso] + biqK * biqK) * norm;
	
	biqK = tan(M_PI * dram->midA[biq_freq]);
	norm = 1.0f / (1.0f + biqK / dram->midA[biq_reso] + biqK * biqK);
	dram->midA[biq_a0] = biqK * biqK * norm;
	dram->midA[biq_a1] = 2.0f * dram->midA[biq_a0];
	dram->midA[biq_a2] = dram->midA[biq_a0];
	dram->midA[biq_b1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->midA[biq_b2] = (1.0f - biqK / dram->midA[biq_reso] + biqK * biqK) * norm;
	biqK = tan(M_PI * dram->midB[biq_freq]);
	norm = 1.0f / (1.0f + biqK / dram->midB[biq_reso] + biqK * biqK);
	dram->midB[biq_a0] = biqK * biqK * norm;
	dram->midB[biq_a1] = 2.0f * dram->midB[biq_a0];
	dram->midB[biq_a2] = dram->midB[biq_a0];
	dram->midB[biq_b1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->midB[biq_b2] = (1.0f - biqK / dram->midB[biq_reso] + biqK * biqK) * norm;
	biqK = tan(M_PI * dram->midC[biq_freq]);
	norm = 1.0f / (1.0f + biqK / dram->midC[biq_reso] + biqK * biqK);
	dram->midC[biq_a0] = biqK * biqK * norm;
	dram->midC[biq_a1] = 2.0f * dram->midC[biq_a0];
	dram->midC[biq_a2] = dram->midC[biq_a0];
	dram->midC[biq_b1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->midC[biq_b2] = (1.0f - biqK / dram->midC[biq_reso] + biqK * biqK) * norm;
	
	biqK = tan(M_PI * dram->lowA[biq_freq]);
	norm = 1.0f / (1.0f + biqK / dram->lowA[biq_reso] + biqK * biqK);
	dram->lowA[biq_a0] = biqK * biqK * norm;
	dram->lowA[biq_a1] = 2.0f * dram->lowA[biq_a0];
	dram->lowA[biq_a2] = dram->lowA[biq_a0];
	dram->lowA[biq_b1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->lowA[biq_b2] = (1.0f - biqK / dram->lowA[biq_reso] + biqK * biqK) * norm;
	biqK = tan(M_PI * dram->lowB[biq_freq]);
	norm = 1.0f / (1.0f + biqK / dram->lowB[biq_reso] + biqK * biqK);
	dram->lowB[biq_a0] = biqK * biqK * norm;
	dram->lowB[biq_a1] = 2.0f * dram->lowB[biq_a0];
	dram->lowB[biq_a2] = dram->lowB[biq_a0];
	dram->lowB[biq_b1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->lowB[biq_b2] = (1.0f - biqK / dram->lowB[biq_reso] + biqK * biqK) * norm;
	biqK = tan(M_PI * dram->lowC[biq_freq]);
	norm = 1.0f / (1.0f + biqK / dram->lowC[biq_reso] + biqK * biqK);
	dram->lowC[biq_a0] = biqK * biqK * norm;
	dram->lowC[biq_a1] = 2.0f * dram->lowC[biq_a0];
	dram->lowC[biq_a2] = dram->lowC[biq_a0];
	dram->lowC[biq_b1] = 2.0f * (biqK * biqK - 1.0f) * norm;
	dram->lowC[biq_b2] = (1.0f - biqK / dram->lowC[biq_reso] + biqK * biqK) * norm;
	//SmoothEQ2
	
	float bezCThresh = powf(1.0f-GetParameter( kParam_I ), 6.0f) * 8.0f;
	float bezRez = powf(1.0f-GetParameter( kParam_I ), 12.360679774997898f) / overallscale;
	bezRez = fmin(fmax(bezRez,0.00001f),1.0f);
	int stepped = 999999; if (bezRez > 0.000001f) stepped = (int)(1.0f/bezRez);
	bezRez = 0.99999999f / stepped;
	float bezTrim = 1.0f-(bezRez*((float)stepped/(stepped+1.0f)));
	float sloRez = powf(1.0f-GetParameter( kParam_I ),10.0f) / overallscale;
	sloRez = fmin(fmax(sloRez,0.00001f),1.0f);
	stepped = 999999; if (sloRez > 0.000001f) stepped = (int)(1.0f/sloRez);
	sloRez = 0.99999999f / stepped;
	float sloTrim = 1.0f-(sloRez*((float)stepped/(stepped+1.0f)));
	//Dynamics2
	
	inTrimA = inTrimB; inTrimB = GetParameter( kParam_J )*2.0f;
	//Console
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
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
		
		highLIIR = (highLIIR*highCoef) + (trebleL*(1.0f-highCoef));
		highmidL = highLIIR; trebleL -= highmidL;
		
		midLIIR = (midLIIR*midCoef) + (highmidL*(1.0f-midCoef));
		lowmidL = midLIIR; highmidL -= lowmidL;
		
		lowLIIR = (lowLIIR*lowCoef) + (lowmidL*(1.0f-lowCoef));
		bassL = lowLIIR; lowmidL -= bassL;
		
		inputSampleL = (bassL*bassGain) + (lowmidL*lowmidGain) + (highmidL*highmidGain) + (trebleL*trebleGain);		
		//fourth stage of three crossovers is the exponential filters
		
		
		float trebleR = inputSampleR;		
		outSample = (trebleR * dram->highA[biq_a0]) + dram->highA[biq_sR1];
		dram->highA[biq_sR1] = (trebleR * dram->highA[biq_a1]) - (outSample * dram->highA[biq_b1]) + dram->highA[biq_sR2];
		dram->highA[biq_sR2] = (trebleR * dram->highA[biq_a2]) - (outSample * dram->highA[biq_b2]);
		float highmidR = outSample; trebleR -= highmidR;
		
		outSample = (highmidR * dram->midA[biq_a0]) + dram->midA[biq_sR1];
		dram->midA[biq_sR1] = (highmidR * dram->midA[biq_a1]) - (outSample * dram->midA[biq_b1]) + dram->midA[biq_sR2];
		dram->midA[biq_sR2] = (highmidR * dram->midA[biq_a2]) - (outSample * dram->midA[biq_b2]);
		float lowmidR = outSample; highmidR -= lowmidR;
		
		outSample = (lowmidR * dram->lowA[biq_a0]) + dram->lowA[biq_sR1];
		dram->lowA[biq_sR1] = (lowmidR * dram->lowA[biq_a1]) - (outSample * dram->lowA[biq_b1]) + dram->lowA[biq_sR2];
		dram->lowA[biq_sR2] = (lowmidR * dram->lowA[biq_a2]) - (outSample * dram->lowA[biq_b2]);
		float bassR = outSample; lowmidR -= bassR;
		
		trebleR = (bassR*bassGain) + (lowmidR*lowmidGain) + (highmidR*highmidGain) + (trebleR*trebleGain);
		//first stage of three crossovers
		
		outSample = (trebleR * dram->highB[biq_a0]) + dram->highB[biq_sR1];
		dram->highB[biq_sR1] = (trebleR * dram->highB[biq_a1]) - (outSample * dram->highB[biq_b1]) + dram->highB[biq_sR2];
		dram->highB[biq_sR2] = (trebleR * dram->highB[biq_a2]) - (outSample * dram->highB[biq_b2]);
		highmidR = outSample; trebleR -= highmidR;
		
		outSample = (highmidR * dram->midB[biq_a0]) + dram->midB[biq_sR1];
		dram->midB[biq_sR1] = (highmidR * dram->midB[biq_a1]) - (outSample * dram->midB[biq_b1]) + dram->midB[biq_sR2];
		dram->midB[biq_sR2] = (highmidR * dram->midB[biq_a2]) - (outSample * dram->midB[biq_b2]);
		lowmidR = outSample; highmidR -= lowmidR;
		
		outSample = (lowmidR * dram->lowB[biq_a0]) + dram->lowB[biq_sR1];
		dram->lowB[biq_sR1] = (lowmidR * dram->lowB[biq_a1]) - (outSample * dram->lowB[biq_b1]) + dram->lowB[biq_sR2];
		dram->lowB[biq_sR2] = (lowmidR * dram->lowB[biq_a2]) - (outSample * dram->lowB[biq_b2]);
		bassR = outSample; lowmidR -= bassR;
		
		trebleR = (bassR*bassGain) + (lowmidR*lowmidGain) + (highmidR*highmidGain) + (trebleR*trebleGain);
		//second stage of three crossovers
		
		outSample = (trebleR * dram->highC[biq_a0]) + dram->highC[biq_sR1];
		dram->highC[biq_sR1] = (trebleR * dram->highC[biq_a1]) - (outSample * dram->highC[biq_b1]) + dram->highC[biq_sR2];
		dram->highC[biq_sR2] = (trebleR * dram->highC[biq_a2]) - (outSample * dram->highC[biq_b2]);
		highmidR = outSample; trebleR -= highmidR;
		
		outSample = (highmidR * dram->midC[biq_a0]) + dram->midC[biq_sR1];
		dram->midC[biq_sR1] = (highmidR * dram->midC[biq_a1]) - (outSample * dram->midC[biq_b1]) + dram->midC[biq_sR2];
		dram->midC[biq_sR2] = (highmidR * dram->midC[biq_a2]) - (outSample * dram->midC[biq_b2]);
		lowmidR = outSample; highmidR -= lowmidR;
		
		outSample = (lowmidR * dram->lowC[biq_a0]) + dram->lowC[biq_sR1];
		dram->lowC[biq_sR1] = (lowmidR * dram->lowC[biq_a1]) - (outSample * dram->lowC[biq_b1]) + dram->lowC[biq_sR2];
		dram->lowC[biq_sR2] = (lowmidR * dram->lowC[biq_a2]) - (outSample * dram->lowC[biq_b2]);
		bassR = outSample; lowmidR -= bassR;
		
		trebleR = (bassR*bassGain) + (lowmidR*lowmidGain) + (highmidR*highmidGain) + (trebleR*trebleGain);
		//third stage of three crossovers
		
		highRIIR = (highRIIR*highCoef) + (trebleR*(1.0f-highCoef));
		highmidR = highRIIR; trebleR -= highmidR;
		
		midRIIR = (midRIIR*midCoef) + (highmidR*(1.0f-midCoef));
		lowmidR = midRIIR; highmidR -= lowmidR;
		
		lowRIIR = (lowRIIR*lowCoef) + (lowmidR*(1.0f-lowCoef));
		bassR = lowRIIR; lowmidR -= bassR;
		
		inputSampleR = (bassR*bassGain) + (lowmidR*lowmidGain) + (highmidR*highmidGain) + (trebleR*trebleGain);		
		//fourth stage of three crossovers is the exponential filters
		
		//SmoothEQ2
		if (bezCThresh > 0.0f) {
			inputSampleL *= ((bezCThresh*0.5f)+1.0f);
			inputSampleR *= ((bezCThresh*0.5f)+1.0f);
		}
		dram->bezCompF[bez_cycle] += bezRez;
		dram->bezCompF[bez_SampL] += (fabs(inputSampleL) * bezRez);
		dram->bezCompF[bez_SampR] += (fabs(inputSampleR) * bezRez);
		bezMaxF = fmax(bezMaxF,fmax(fabs(inputSampleL),fabs(inputSampleR)));
		if (dram->bezCompF[bez_cycle] > bezTrim) {
			dram->bezCompF[bez_cycle] = 0.0f;
			dram->bezCompF[bez_CL] = dram->bezCompF[bez_BL];
			dram->bezCompF[bez_BL] = dram->bezCompF[bez_AL];
			dram->bezCompF[bez_AL] = dram->bezCompF[bez_SampL];
			dram->bezCompF[bez_SampL] = 0.0f;
			dram->bezCompF[bez_CR] = dram->bezCompF[bez_BR];
			dram->bezCompF[bez_BR] = dram->bezCompF[bez_AR];
			dram->bezCompF[bez_AR] = dram->bezCompF[bez_SampR];
			dram->bezCompF[bez_SampR] = 0.0f;
			bezMaxF = 0.0f;
		}
		dram->bezCompS[bez_cycle] += sloRez;
		dram->bezCompS[bez_SampL] += (fabs(inputSampleL) * sloRez); //note: SampL is a control voltage
		dram->bezCompS[bez_SampR] += (fabs(inputSampleR) * sloRez); //note: SampR is a control voltage
		if (dram->bezCompS[bez_cycle] > sloTrim) {
			dram->bezCompS[bez_cycle] = 0.0f;
			dram->bezCompS[bez_CL] = dram->bezCompS[bez_BL];
			dram->bezCompS[bez_BL] = dram->bezCompS[bez_AL];
			dram->bezCompS[bez_AL] = dram->bezCompS[bez_SampL];
			dram->bezCompS[bez_SampL] = 0.0f;
			dram->bezCompS[bez_CR] = dram->bezCompS[bez_BR];
			dram->bezCompS[bez_BR] = dram->bezCompS[bez_AR];
			dram->bezCompS[bez_AR] = dram->bezCompS[bez_SampR];
			dram->bezCompS[bez_SampR] = 0.0f;
		}
		float X = dram->bezCompF[bez_cycle];
		float CBAFL = (dram->bezCompF[bez_BL]+(dram->bezCompF[bez_CL]*(1.0f-X)*(1.0f-X))+(dram->bezCompF[bez_BL]*2.0f*(1.0f-X)*X)+(dram->bezCompF[bez_AL]*X*X))*0.5f;
		float CBAFR = (dram->bezCompF[bez_BR]+(dram->bezCompF[bez_CR]*(1.0f-X)*(1.0f-X))+(dram->bezCompF[bez_BR]*2.0f*(1.0f-X)*X)+(dram->bezCompF[bez_AR]*X*X))*0.5f;
		X = dram->bezCompS[bez_cycle];
		float CBASL = (dram->bezCompS[bez_BL]+(dram->bezCompS[bez_CL]*(1.0f-X)*(1.0f-X))+(dram->bezCompS[bez_BL]*2.0f*(1.0f-X)*X)+(dram->bezCompS[bez_AL]*X*X))*0.5f;
		float CBASR = (dram->bezCompS[bez_BR]+(dram->bezCompS[bez_CR]*(1.0f-X)*(1.0f-X))+(dram->bezCompS[bez_BR]*2.0f*(1.0f-X)*X)+(dram->bezCompS[bez_AR]*X*X))*0.5f;
		float CBAMax = fmax(CBASL,CBAFL); if (CBAMax > 0.0f) CBAMax = 1.0f/CBAMax;
		float CBAFade = ((CBASL*-CBAMax)+(CBAFL*CBAMax)+1.0f)*0.5f;
		if (bezCThresh > 0.0f) inputSampleL *= 1.0f-(fmin(((CBASL*(1.0f-CBAFade))+(CBAFL*CBAFade))*bezCThresh,1.0f));
		CBAMax = fmax(CBASR,CBAFR); if (CBAMax > 0.0f) CBAMax = 1.0f/CBAMax;
		CBAFade = ((CBASR*-CBAMax)+(CBAFR*CBAMax)+1.0f)*0.5f;
		if (bezCThresh > 0.0f) inputSampleR *= 1.0f-(fmin(((CBASR*(1.0f-CBAFade))+(CBAFR*CBAFade))*bezCThresh,1.0f));
		//Dynamics2
		
		const float temp = (float)nSampleFrames/inFramesToProcess;
		float gain = (inTrimA*temp)+(inTrimB*(1.0f-temp));
		inputSampleL = inputSampleL * gain;
		inputSampleR = inputSampleR * gain;
		//applies pan section, and smoothed fader gain
		
		//begin ClipOnly3 as a little, compressed chunk that can be dropped into code
		float noise = 1.0f-((float(fpdL)/UINT32_MAX)*0.076f);
		if (wasPosClipL == true) { //current will be over
			if (inputSampleL<lastSampleL) lastSampleL=(0.9085097f*noise)+(inputSampleL*(1.0f-noise));
			else lastSampleL = 0.94f; //~-0.2dB to nearly match ClipOnly and ClipOnly2
		} wasPosClipL = false;
		if (inputSampleL>0.9085097f) {wasPosClipL=true;inputSampleL=(0.9085097f*noise)+(lastSampleL*(1.0f-noise));}
		if (wasNegClipL == true) { //current will be -over
			if (inputSampleL > lastSampleL) lastSampleL=(-0.9085097f*noise)+(inputSampleL*(1.0f-noise));
			else lastSampleL = -0.94f;
		} wasNegClipL = false;
		if (inputSampleL<-0.9085097f) {wasNegClipL=true;inputSampleL=(-0.9085097f*noise)+(lastSampleL*(1.0f-noise));}
		dram->slewL[spacing*2] = fabs(lastSampleL-inputSampleL);
		for (int x = spacing*2; x > 0; x--) dram->slewL[x-1] = dram->slewL[x];
		dram->intermediateL[spacing] = inputSampleL; inputSampleL = lastSampleL;
		//latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) {dram->intermediateL[x-1] = dram->intermediateL[x];} lastSampleL = dram->intermediateL[0];
		if (wasPosClipL || wasNegClipL) {
			for (int x = spacing; x > 0; x--) lastSampleL += dram->intermediateL[x];
			lastSampleL /= spacing;
		} float finalSlew = 0.0f;
		for (int x = spacing*2; x >= 0; x--) if (finalSlew < dram->slewL[x]) finalSlew = dram->slewL[x];
		float postclip = 0.94f / (1.0f+(finalSlew*1.3986013f));
		if (inputSampleL > postclip) inputSampleL = postclip; if (inputSampleL < -postclip) inputSampleL = -postclip;
		
		noise = 1.0f-((float(fpdR)/UINT32_MAX)*0.076f);
		if (wasPosClipR == true) { //current will be over
			if (inputSampleR<lastSampleR) lastSampleR=(0.9085097f*noise)+(inputSampleR*(1.0f-noise));
			else lastSampleR = 0.94f; //~-0.2dB to nearly match ClipOnly and ClipOnly2
		} wasPosClipR = false;
		if (inputSampleR>0.9085097f) {wasPosClipR=true;inputSampleR=(0.9085097f*noise)+(lastSampleR*(1.0f-noise));}
		if (wasNegClipR == true) { //current will be -over
			if (inputSampleR > lastSampleR) lastSampleR=(-0.9085097f*noise)+(inputSampleR*(1.0f-noise));
			else lastSampleR = -0.94f;
		} wasNegClipR = false;
		if (inputSampleR<-0.9085097f) {wasNegClipR=true;inputSampleR=(-0.9085097f*noise)+(lastSampleR*(1.0f-noise));}
		dram->slewR[spacing*2] = fabs(lastSampleR-inputSampleR);
		for (int x = spacing*2; x > 0; x--) dram->slewR[x-1] = dram->slewR[x];
		dram->intermediateR[spacing] = inputSampleR; inputSampleR = lastSampleR;
		//latency is however many samples equals one 44.1k sample
		for (int x = spacing; x > 0; x--) {dram->intermediateR[x-1] = dram->intermediateR[x];} lastSampleR = dram->intermediateR[0];
		if (wasPosClipR || wasNegClipR) {
			for (int x = spacing; x > 0; x--) lastSampleR += dram->intermediateR[x];
			lastSampleR /= spacing;
		} finalSlew = 0.0f;
		for (int x = spacing*2; x >= 0; x--) if (finalSlew < dram->slewR[x]) finalSlew = dram->slewR[x];
		postclip = 0.94f / (1.0f+(finalSlew*1.3986013f));
		if (inputSampleR > postclip) inputSampleR = postclip; if (inputSampleR < -postclip) inputSampleR = -postclip;
		//end ClipOnly3 as a little, compressed chunk that can be dropped into code
		
		
		
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
	highLIIR = 0.0;
	highRIIR = 0.0;
	midLIIR = 0.0;
	midRIIR = 0.0;
	lowLIIR = 0.0;
	lowRIIR = 0.0;
	//SmoothEQ2
	
	for (int x = 0; x < bez_total; x++) {dram->bezCompF[x] = 0.0;dram->bezCompS[x] = 0.0;}
	dram->bezCompF[bez_cycle] = 1.0; bezMaxF = 0.0;
	dram->bezCompS[bez_cycle] = 1.0;
	//Dynamics2
	
	lastSampleL = 0.0;
	wasPosClipL = false;
	wasNegClipL = false;
	lastSampleR = 0.0;
	wasPosClipR = false;
	wasNegClipR = false;
	for (int x = 0; x < 17; x++) {dram->intermediateL[x] = 0.0; dram->intermediateR[x] = 0.0;}
	for (int x = 0; x < 33; x++) {dram->slewL[x] = 0.0; dram->slewR[x] = 0.0;}
	
	inTrimA = 0.5; inTrimB = 0.5;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
