#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleX2Channel"
#define AIRWINDOWS_DESCRIPTION "ConsoleX2Channel"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','<' )
enum {

	kParam_TRM =0,
	kParam_MOR =1,
	kParam_HIG =2,
	kParam_HMG =3,
	kParam_LMG =4,
	kParam_BSG =5,
	kParam_HIF =6,
	kParam_HMF =7,
	kParam_LMF =8,
	kParam_BSF =9,
	kParam_THR =10,
	kParam_ATK =11,
	kParam_RLS =12,
	kParam_GAT =13,
	kParam_LOP =14,
	kParam_HIP =15,
	kParam_PAN =16,
	kParam_FAD =17,
	//Add your parameters here...
	kNumberOfParameters=18
};
static const int kDefaultValue_ParamTRM = 1;
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
		float highA[biq_total];
	float highB[biq_total];
	float highC[biq_total];
	float midA[biq_total];
	float midB[biq_total];
	float midC[biq_total];
	float lowA[biq_total];
	float lowB[biq_total];
	float lowC[biq_total];
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
	float highmidGain = (GetParameter( kParam_HMG )-0.5f)*2.0f;
	highmidGain = 1.0f+(highmidGain*fabs(highmidGain)*fabs(highmidGain));
	float lowmidGain = (GetParameter( kParam_LMG )-0.5f)*2.0f;
	lowmidGain = 1.0f+(lowmidGain*fabs(lowmidGain)*fabs(lowmidGain));
	float bassGain = (GetParameter( kParam_BSG )-0.5f)*2.0f;
	bassGain = 1.0f+(bassGain*fabs(bassGain)*fabs(bassGain));
	float highCoef = 0.0f;
	float midCoef = 0.0f;
	float lowCoef = 0.0f;
	
	bool eqOff = (trebleGain == 1.0f && highmidGain == 1.0f && lowmidGain == 1.0f && bassGain == 1.0f);
	//we get to completely bypass EQ if we're truly not using it. The mechanics of it mean that
	//it cancels out to bit-identical anyhow, but we get to skip the calculation
	if (!eqOff) {
		float trebleRef = GetParameter( kParam_HIF )-0.5f;
		float highmidRef = GetParameter( kParam_HMF )-0.5f;
		float lowmidRef = GetParameter( kParam_LMF )-0.5f;
		float bassRef = GetParameter( kParam_BSF )-0.5f;
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
		highCoef = -sqrt((biqK*biqK)-1.0f)+biqK;
		dram->highA[biq_reso] = 2.24697960f * highQ;
		dram->highB[biq_reso] = 0.80193774f * highQ;
		dram->highC[biq_reso] = 0.55495813f * highQ;
		
		dram->midA[biq_freq] = ((powf(midF,3)*20000.0f)/GetSampleRate());
		dram->midC[biq_freq] = dram->midB[biq_freq] = dram->midA[biq_freq] = fmax(fmin(dram->midA[biq_freq],0.4999f),0.00025f);	
		float midFreq = powf(midF,3)*20000.0f;
		omega = 2.0f*M_PI*(midFreq/GetSampleRate());
		biqK = 2.0f-cos(omega);
		midCoef = -sqrt((biqK*biqK)-1.0f)+biqK;
		dram->midA[biq_reso] = 2.24697960f * midQ;
		dram->midB[biq_reso] = 0.80193774f * midQ;
		dram->midC[biq_reso] = 0.55495813f * midQ;
		
		dram->lowA[biq_freq] = ((powf(bassF,3)*20000.0f)/GetSampleRate());
		dram->lowC[biq_freq] = dram->lowB[biq_freq] = dram->lowA[biq_freq] = fmax(fmin(dram->lowA[biq_freq],0.4999f),0.00025f);
		float lowFreq = powf(bassF,3)*20000.0f;
		omega = 2.0f*M_PI*(lowFreq/GetSampleRate());
		biqK = 2.0f-cos(omega);
		lowCoef = -sqrt((biqK*biqK)-1.0f)+biqK;
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
	}
	//SmoothEQ2
	
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
		
		if (!eqOff) {
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
		}
		//SmoothEQ2
		
		if (bezThresh > 0.0f) {
			if (fmax(fabs(inputSampleL),fabs(inputSampleR)) > gate) bezGate = overallscale/fmin(bezRez,sloRez);
			else bezGate = bezGate = fmax(0.000001f, bezGate-fmin(bezRez,sloRez));
			inputSampleL *= (bezThresh+1.0f);
			inputSampleR *= (bezThresh+1.0f);
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
			inputSampleL *= 1.0f-(fmin(CBA*bezThresh,1.0f));
			inputSampleR *= 1.0f-(fmin(CBA*bezThresh,1.0f));
		} else dram->bezComp[bez_Ctrl] = 0.0f;
		//Dynamics3
		
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
