#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleX2Channel"
#define AIRWINDOWS_DESCRIPTION "ConsoleX2Channel"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','9' )
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
	kParam_K =10,
	kParam_L =11,
	kParam_M =12,
	kParam_N =13,
	kParam_O =14,
	kParam_P =15,
	kParam_Q =16,
	kParam_R =17,
	//Add your parameters here...
	kNumberOfParameters=18
};
static const int kDefaultValue_ParamA = 1;
const int dscBuf = 256;
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, kParam12, kParam13, kParam14, kParam15, kParam16, kParam17, };
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
{ .name = "HMid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LMid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HighF", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HMidF", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LMidF", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "BassF", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, kParam12, kParam13, kParam14, kParam15, kParam16, kParam17, };
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
	float bezGate;
	//Dynamics2
	
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
	int avgPos;
	float lastSlewL;
	float lastSlewR;
	float lastSlewpleL;
	float lastSlewpleR;
	//preTapeHack	
	
	float panA;
	float panB;
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
	
	float moreTapeHack = (GetParameter( kParam_B )*2.0f)+1.0f;
	switch ((int)GetParameter( kParam_A )){
		case 0: moreTapeHack *= 0.5f; break;
		case 1: break;
		case 2: moreTapeHack *= 2.0f; break;
		case 3: moreTapeHack *= 4.0f; break;
		case 4: moreTapeHack *= 8.0f; break;
	}
	float moreDiscontinuity = fmax(powf(GetParameter( kParam_B )*0.42f,3.0f)*overallscale,0.00001f);
	//Discontapeity	

	float trebleGain = (GetParameter( kParam_C )-0.5f)*2.0f;
	trebleGain = 1.0f+(trebleGain*fabs(trebleGain)*fabs(trebleGain));
	float highmidGain = (GetParameter( kParam_D )-0.5f)*2.0f;
	highmidGain = 1.0f+(highmidGain*fabs(highmidGain)*fabs(highmidGain));
	float lowmidGain = (GetParameter( kParam_E )-0.5f)*2.0f;
	lowmidGain = 1.0f+(lowmidGain*fabs(lowmidGain)*fabs(lowmidGain));
	float bassGain = (GetParameter( kParam_F )-0.5f)*2.0f;
	bassGain = 1.0f+(bassGain*fabs(bassGain)*fabs(bassGain));
	
	float trebleRef = GetParameter( kParam_G )-0.5f;
	float highmidRef = GetParameter( kParam_H )-0.5f;
	float lowmidRef = GetParameter( kParam_I )-0.5f;
	float bassRef = GetParameter( kParam_J )-0.5f;
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
	
	float bezCThresh = powf(1.0f-GetParameter( kParam_K ), 6.0f) * 8.0f;
	float bezRez = powf(1.0f-GetParameter( kParam_L ), 8.0f) / overallscale; 
	float sloRez = powf(1.0f-GetParameter( kParam_M ),12.0f) / overallscale;
	sloRez = fmin(fmax(sloRez-(bezRez*0.5f),0.00001f),1.0f);
	bezRez = fmin(fmax(bezRez,0.0001f),1.0f);
	float gate = powf(powf(GetParameter( kParam_N ),4.0f),sqrt(bezCThresh+1.0f));
	//Dynamics2
	
	lFreqA = lFreqB; lFreqB = powf(fmax(GetParameter( kParam_O ),0.002f),overallscale); //the lowpass
	hFreqA = hFreqB; hFreqB = powf(GetParameter( kParam_P ),overallscale+2.0f); //the highpass
	//Cabs2
		
	panA = panB; panB = GetParameter( kParam_Q )*1.57079633f;
	inTrimA = inTrimB; inTrimB = GetParameter( kParam_R )*2.0f;
	//Console
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
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
		float avgSlewL = fmin(lastSlewL,1.0f);
		lastSlewL = fmax(lastSlewL*0.78f,2.39996322972865332223f);
		lastSlewR += fabs(lastSlewpleR-inputSampleR); lastSlewpleR = inputSampleR;
		float avgSlewR = fmin(lastSlewR,1.0f);
		lastSlewR = fmax(lastSlewR*0.78f,2.39996322972865332223f); //look up Golden Angle, it's cool
		inputSampleL = (inputSampleL*(1.0f-avgSlewL)) + (darkSampleL*avgSlewL);
		inputSampleR = (inputSampleR*(1.0f-avgSlewR)) + (darkSampleR*avgSlewR);		
		
		//begin Discontinuity section
		inputSampleL *= moreTapeHack;
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
		inputSampleR *= moreTapeHack;
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
		
		if (fmax(fabs(inputSampleL),fabs(inputSampleR)) > gate+(sloRez*bezGate)) bezGate = ((bezGate*overallscale*3.0f)+3.0f)*(0.25f/overallscale);
		else bezGate = fmax(0.0f, bezGate-(sloRez*sloRez));
		
		if (bezCThresh > 0.0f) {
			inputSampleL *= ((bezCThresh*0.5f)+1.0f);
			inputSampleR *= ((bezCThresh*0.5f)+1.0f);
		}
		
		dram->bezCompF[bez_cycle] += bezRez;
		dram->bezCompF[bez_SampL] += (fabs(inputSampleL) * bezRez);
		dram->bezCompF[bez_SampR] += (fabs(inputSampleR) * bezRez);
		bezMaxF = fmax(bezMaxF,fmax(fabs(inputSampleL),fabs(inputSampleR)));
		
		if (dram->bezCompF[bez_cycle] > 1.0f) {
			dram->bezCompF[bez_cycle] -= 1.0f;
			
			if (bezMaxF < gate) dram->bezCompF[bez_SampL] = bezMaxF/gate; //note: SampL is a control voltage,
			if (dram->bezCompF[bez_SampL]<gate) dram->bezCompF[bez_SampL] = 0.0f; //not a bipolar audio signal
			dram->bezCompF[bez_CL] = dram->bezCompF[bez_BL];
			dram->bezCompF[bez_BL] = dram->bezCompF[bez_AL];
			dram->bezCompF[bez_AL] = dram->bezCompF[bez_SampL];
			dram->bezCompF[bez_SampL] = 0.0f;
			
			if (bezMaxF < gate) dram->bezCompF[bez_SampR] = bezMaxF/gate; //note: SampR is a control voltage,
			if (dram->bezCompF[bez_SampR]<gate) dram->bezCompF[bez_SampR] = 0.0f; //not a bipolar audio signal
			dram->bezCompF[bez_CR] = dram->bezCompF[bez_BR];
			dram->bezCompF[bez_BR] = dram->bezCompF[bez_AR];
			dram->bezCompF[bez_AR] = dram->bezCompF[bez_SampR];
			dram->bezCompF[bez_SampR] = 0.0f;
			
			bezMaxF = 0.0f;
		}
		dram->bezCompS[bez_cycle] += sloRez;
		dram->bezCompS[bez_SampL] += (fabs(inputSampleL) * sloRez); //note: SampL is a control voltage
		dram->bezCompS[bez_SampR] += (fabs(inputSampleR) * sloRez); //note: SampR is a control voltage
		if (dram->bezCompS[bez_cycle] > 1.0f) {
			dram->bezCompS[bez_cycle] -= 1.0f;
			
			if (dram->bezCompS[bez_SampL]<gate) dram->bezCompS[bez_SampL] = 0.0f;
			dram->bezCompS[bez_CL] = dram->bezCompS[bez_BL];
			dram->bezCompS[bez_BL] = dram->bezCompS[bez_AL];
			dram->bezCompS[bez_AL] = dram->bezCompS[bez_SampL];
			dram->bezCompS[bez_SampL] = 0.0f;
			
			if (dram->bezCompS[bez_SampR]<gate) dram->bezCompS[bez_SampR] = 0.0f;
			dram->bezCompS[bez_CR] = dram->bezCompS[bez_BR];
			dram->bezCompS[bez_BR] = dram->bezCompS[bez_AR];
			dram->bezCompS[bez_AR] = dram->bezCompS[bez_SampR];
			dram->bezCompS[bez_SampR] = 0.0f;
		}
		float CBFL = (dram->bezCompF[bez_CL]*(1.0f-dram->bezCompF[bez_cycle]))+(dram->bezCompF[bez_BL]*dram->bezCompF[bez_cycle]);
		float BAFL = (dram->bezCompF[bez_BL]*(1.0f-dram->bezCompF[bez_cycle]))+(dram->bezCompF[bez_AL]*dram->bezCompF[bez_cycle]);
		float CBAFL = (dram->bezCompF[bez_BL]+(CBFL*(1.0f-dram->bezCompF[bez_cycle]))+(BAFL*dram->bezCompF[bez_cycle]))*0.5f;
		float CBSL = (dram->bezCompS[bez_CL]*(1.0f-dram->bezCompS[bez_cycle]))+(dram->bezCompS[bez_BL]*dram->bezCompS[bez_cycle]);
		float BASL = (dram->bezCompS[bez_BL]*(1.0f-dram->bezCompS[bez_cycle]))+(dram->bezCompS[bez_AL]*dram->bezCompS[bez_cycle]);
		float CBASL = (dram->bezCompS[bez_BL]+(CBSL*(1.0f-dram->bezCompS[bez_cycle]))+(BASL*dram->bezCompS[bez_cycle]))*0.5f;
		float CBAMax = fmax(CBASL,CBAFL); if (CBAMax > 0.0f) CBAMax = 1.0f/CBAMax;
		float CBAFade = ((CBASL*-CBAMax)+(CBAFL*CBAMax)+1.0f)*0.5f;
		if (bezCThresh > 0.0f) inputSampleL *= 1.0f-(fmin(((CBASL*(1.0f-CBAFade))+(CBAFL*CBAFade))*bezCThresh,1.0f));
		
		float CBFR = (dram->bezCompF[bez_CR]*(1.0f-dram->bezCompF[bez_cycle]))+(dram->bezCompF[bez_BR]*dram->bezCompF[bez_cycle]);
		float BAFR = (dram->bezCompF[bez_BR]*(1.0f-dram->bezCompF[bez_cycle]))+(dram->bezCompF[bez_AR]*dram->bezCompF[bez_cycle]);
		float CBAFR = (dram->bezCompF[bez_BR]+(CBFR*(1.0f-dram->bezCompF[bez_cycle]))+(BAFR*dram->bezCompF[bez_cycle]))*0.5f;
		float CBSR = (dram->bezCompS[bez_CR]*(1.0f-dram->bezCompS[bez_cycle]))+(dram->bezCompS[bez_BR]*dram->bezCompS[bez_cycle]);
		float BASR = (dram->bezCompS[bez_BR]*(1.0f-dram->bezCompS[bez_cycle]))+(dram->bezCompS[bez_AR]*dram->bezCompS[bez_cycle]);
		float CBASR = (dram->bezCompS[bez_BR]+(CBSR*(1.0f-dram->bezCompS[bez_cycle]))+(BASR*dram->bezCompS[bez_cycle]))*0.5f;
		CBAMax = fmax(CBASR,CBAFR); if (CBAMax > 0.0f) CBAMax = 1.0f/CBAMax;
		CBAFade = ((CBASR*-CBAMax)+(CBAFR*CBAMax)+1.0f)*0.5f;
		if (bezCThresh > 0.0f) inputSampleR *= 1.0f-(fmin(((CBASR*(1.0f-CBAFade))+(CBAFR*CBAFade))*bezCThresh,1.0f));
		
		if (bezGate < 1.0f && gate > 0.0f) {inputSampleL *= bezGate; inputSampleR *= bezGate;}
		//Dynamics2
		
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
			} //blank out highpass if jut switched off
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
	dram->bezCompS[bez_cycle] = 1.0; bezGate = 2.0;
	//Dynamics2
	
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
	
	for (int x = 0; x < 33; x++) {dram->avg32L[x] = 0.0; dram->avg32R[x] = 0.0;}
	for (int x = 0; x < 17; x++) {dram->avg16L[x] = 0.0; dram->avg16R[x] = 0.0;}
	for (int x = 0; x < 9; x++) {avg8L[x] = 0.0; avg8R[x] = 0.0;}
	for (int x = 0; x < 5; x++) {avg4L[x] = 0.0; avg4R[x] = 0.0;}
	for (int x = 0; x < 3; x++) {avg2L[x] = 0.0; avg2R[x] = 0.0;}
	avgPos = 0;
	lastSlewL = 0.0; lastSlewR = 0.0;
	lastSlewpleL = 0.0; lastSlewpleR = 0.0;
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
