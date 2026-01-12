#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleX2Pre"
#define AIRWINDOWS_DESCRIPTION "ConsoleX2Pre"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o','=' )
#define AIRWINDOWS_KERNELS
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
	kParam_FAD =16,
	//Add your parameters here...
	kNumberOfParameters=17
};
static const int kDefaultValue_ParamTRM = 1;
const int dscBuf = 256;
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, kParam12, kParam13, kParam14, kParam15, kParam16, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
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
{ .name = "Fader", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, kParam12, kParam13, kParam14, kParam15, kParam16, };
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
			biq_total
		}; //coefficient interpolating bessel filter, stereo
		float highIIR;
		
		float midIIR;
		
		float lowIIR;
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
		float iirLPosition[15];
		float iirLAngle[15];
		bool lBypass;
		float lFreqA;
		float lFreqB; //the lowpass
		float hFreqA;
		float hFreqB; //the highpass
		//Cabs2
		
		float dBaPosL;
		int dBaXL;
		//Discontapeity
		
		float avg8L[9];
		float avg4L[5];
		float avg2L[3];
		int avgPos;
		float lastSlewL;
		float lastSlewpleL;
		//preTapeHack
		
		float inTrimA;
		float inTrimB;
		
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
		float bezComp[bez_total];
		float iirHPosition[23];
		float iirHAngle[23];
		float dBaL[dscBuf+5];
		float avg32L[33];
		float avg16L[17];
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
	
	inTrimA = inTrimB; inTrimB = GetParameter( kParam_FAD )*2.0f;
	//Console

	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		
		inputSampleL *= moreTapeHack;
		//trim control gets to work even when MORE is off
		
		if (!tapehackOff) {
			float darkSampleL = inputSampleL;
			if (avgPos > 31) avgPos = 0;
			if (spacing > 31) {
				dram->avg32L[avgPos] = darkSampleL;
				darkSampleL = 0.0f;
				for (int x = 0; x < 32; x++) {darkSampleL += dram->avg32L[x];}
				darkSampleL /= 32.0f;
			} if (spacing > 15) {
				dram->avg16L[avgPos%16] = darkSampleL;
				darkSampleL = 0.0f;
				for (int x = 0; x < 16; x++) {darkSampleL += dram->avg16L[x];}
				darkSampleL /= 16.0f;
			} if (spacing > 7) {
				avg8L[avgPos%8] = darkSampleL;
				darkSampleL = 0.0f;
				for (int x = 0; x < 8; x++) {darkSampleL += avg8L[x];}
				darkSampleL /= 8.0f;
			} if (spacing > 3) {
				avg4L[avgPos%4] = darkSampleL;
				darkSampleL = 0.0f;
				for (int x = 0; x < 4; x++) {darkSampleL += avg4L[x];}
				darkSampleL /= 4.0f;
			} if (spacing > 1) {
				avg2L[avgPos%2] = darkSampleL;
				darkSampleL = 0.0f;
				for (int x = 0; x < 2; x++) {darkSampleL += avg2L[x];}
				darkSampleL /= 2.0f; 
			} avgPos++;
			lastSlewL += fabs(lastSlewpleL-inputSampleL); lastSlewpleL = inputSampleL;
			float avgSlewL = fmin(lastSlewL,1.0f);
			lastSlewL = fmax(lastSlewL*0.78f,2.39996322972865332223f); //look up Golden Angle, it's cool
			inputSampleL = (inputSampleL*(1.0f-avgSlewL)) + (darkSampleL*avgSlewL);
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
			//Discontapeity
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
			
			highIIR = (highIIR*highCoef) + (trebleL*(1.0f-highCoef));
			highmidL = highIIR; trebleL -= highmidL;
			
			midIIR = (midIIR*midCoef) + (highmidL*(1.0f-midCoef));
			lowmidL = midIIR; highmidL -= lowmidL;
			
			lowIIR = (lowIIR*lowCoef) + (lowmidL*(1.0f-lowCoef));
			bassL = lowIIR; lowmidL -= bassL;
			
			inputSampleL = (bassL*bassGain) + (lowmidL*lowmidGain) + (highmidL*highmidGain) + (trebleL*trebleGain);		
			//fourth stage of three crossovers is the exponential filters
		}
		//SmoothEQ2
		
		if (bezThresh > 0.0f) {
			if (fabs(inputSampleL) > gate) bezGate = overallscale/fmin(bezRez,sloRez);
			else bezGate = bezGate = fmax(0.000001f, bezGate-fmin(bezRez,sloRez));
			inputSampleL *= (bezThresh+1.0f);
			float ctrl = fabs(inputSampleL);
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
		} else dram->bezComp[bez_Ctrl] = 0.0f;
		//Dynamics3
		
		const float temp = (float)nSampleFrames/inFramesToProcess;
		const float hFreq = (hFreqA*temp)+(hFreqB*(1.0f-temp));
		if (hFreq > 0.0f) {
			float lowSample = inputSampleL;
			for(int count = 0; count < 21; count++) {
				dram->iirHAngle[count] = (dram->iirHAngle[count]*(1.0f-hFreq))+((lowSample-dram->iirHPosition[count])*hFreq);
				lowSample = ((dram->iirHPosition[count]+(dram->iirHAngle[count]*hFreq))*(1.0f-hFreq))+(lowSample*hFreq);
				dram->iirHPosition[count] = ((dram->iirHPosition[count]+(dram->iirHAngle[count]*hFreq))*(1.0f-hFreq))+(lowSample*hFreq);
				inputSampleL -= (lowSample * (1.0f/21.0f));
			} //the highpass
			hBypass = false;
		} else {
			if (!hBypass) {
				hBypass = true;
				for(int count = 0; count < 22; count++) {
					dram->iirHPosition[count] = 0.0f;
					dram->iirHAngle[count] = 0.0f;
				}
			} //blank out highpass if jut switched off
		}
		const float lFreq = (lFreqA*temp)+(lFreqB*(1.0f-temp));
		if (lFreq < 1.0f) {
			for(int count = 0; count < 13; count++) {
				iirLAngle[count] = (iirLAngle[count]*(1.0f-lFreq))+((inputSampleL-iirLPosition[count])*lFreq);
				inputSampleL = ((iirLPosition[count]+(iirLAngle[count]*lFreq))*(1.0f-lFreq))+(inputSampleL*lFreq);
				iirLPosition[count] = ((iirLPosition[count]+(iirLAngle[count]*lFreq))*(1.0f-lFreq))+(inputSampleL*lFreq);
			} //the lowpass
			lBypass = false;
		} else {
			if (!lBypass) {
				lBypass = true;
				for(int count = 0; count < 14; count++) {
					iirLPosition[count] = 0.0f;
					iirLAngle[count] = 0.0f;
				}
			} //blank out lowpass if just switched off
		}
		//Cabs2
		
		float gain = (inTrimA*temp)+(inTrimB*(1.0f-temp));
		if (gain > 1.0f) gain *= gain;
		if (gain < 1.0f) gain = 1.0f-powf(1.0f-gain,2);
		
		inputSampleL *= gain;
		//applies smoothed fader gain
		
		
		
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
	//SmoothEQ2
	
	for (int x = 0; x < bez_total; x++) dram->bezComp[x] = 0.0;
	dram->bezComp[bez_cycle] = 1.0; bezMax = 0.0; bezMin = 0.0;
	bezGate = 2.0;
	//Dynamics3
	
	for(int count = 0; count < 22; count++) {
		dram->iirHPosition[count] = 0.0;
		dram->iirHAngle[count] = 0.0;
	}
	hBypass = false;
	
	for(int count = 0; count < 14; count++) {
		iirLPosition[count] = 0.0;
		iirLAngle[count] = 0.0;
	}
	lBypass = false;
	//Cabs2
	
	for(int count = 0; count < dscBuf+2; count++) {
		dram->dBaL[count] = 0.0;
	}
	dBaPosL = 0.0;
	dBaXL = 1;
	//Discontapeity
	
	for (int x = 0; x < 33; x++) {dram->avg32L[x] = 0.0;}
	for (int x = 0; x < 17; x++) {dram->avg16L[x] = 0.0;}
	for (int x = 0; x < 9; x++) {avg8L[x] = 0.0;}
	for (int x = 0; x < 5; x++) {avg4L[x] = 0.0;}
	for (int x = 0; x < 3; x++) {avg2L[x] = 0.0;}
	avgPos = 0;
	lastSlewL = 0.0;
	lastSlewpleL = 0.0;
	//preTapeHack
	
	lFreqA = 1.0; lFreqB = 1.0;
	hFreqA = 0.0; hFreqB = 0.0;
	inTrimA = 0.5; inTrimB = 0.5;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
