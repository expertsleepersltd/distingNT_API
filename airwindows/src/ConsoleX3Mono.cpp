#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ConsoleX3Mono"
#define AIRWINDOWS_DESCRIPTION "ConsoleX3Mono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o',':' )
#define AIRWINDOWS_KERNELS
enum {

	
	kParam_HIG =0,
	kParam_HMG =1,
	kParam_LMG =2,
	kParam_BSG =3,
	kParam_HIF =4,
	kParam_HMF =5,
	kParam_LMF =6,
	kParam_BSF =7,
	
	kParam_THR =8,
	kParam_ATK =9,
	kParam_RLS =10,
	kParam_RAT =11,
	
	kParam_TRM =12,
	kParam_MDE =13,
	kParam_SMO =14,
	kParam_MOR =15,
	
	kParam_LOP =16,
	kParam_LPQ =17,
	kParam_HIP =18,
	kParam_HPQ =19,
	
	kParam_FAD =20,
	//Add your parameters here...
	kNumberOfParameters=21
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, kParam12, kParam13, kParam14, kParam15, kParam16, kParam17, kParam18, kParam19, kParam20, };
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
{ .name = "Thresh", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack", .min = 0, .max = 1000, .def = 382, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 618, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Input", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Cha/Bus", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Smooth", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "More", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Lowpass", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "LowpasQ", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Hipass", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "HipassQ", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fader", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, kParam8, kParam9, kParam10, kParam11, kParam12, kParam13, kParam14, kParam15, kParam16, kParam17, kParam18, kParam19, kParam20, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		enum {
			bezierHigh_A,bezierHigh_B,bezierHigh_C,bezierHigh,
			bezierHmid_A,bezierHmid_B,bezierHmid_C,bezierHmid,
			bezierLmid_A,bezierLmid_B,bezierLmid_C,bezierLmid,
			bezierBass_A,bezierBass_B,bezierBass_C,bezierBass,
			bezierHighQ_A,bezierHighQ_B,bezierHighQ_C,bezierHighQ,
			bezierMidQ_A,bezierMidQ_B,bezierMidQ_C,bezierMidQ,
			bezierLowQ_A,bezierLowQ_B,bezierLowQ_C,bezierLowQ,
			bezierThresh_A,bezierThresh_B,bezierThresh_C,bezierThresh,
			bezierRatio_A,bezierRatio_B,bezierRatio_C,bezierRatio,
			bezierMore_A,bezierMore_B,bezierMore_C,bezierMore,
			bezierLFreq_A,bezierLFreq_B,bezierLFreq_C,bezierLFreq,
			bezierHFreq_A,bezierHFreq_B,bezierHFreq_C,bezierHFreq,
			bezierGainL_A,bezierGainL_B,bezierGainL_C,bezierGainL,
			bezier_cycle,
			bezier_total
		}; //control smoothing as basic use of bezier curves
		
		enum {
			bez_HMAL, bez_HMBL, bez_HMCL, bez_HMDL,
			bez_MAL, bez_MBL, bez_MCL, bez_MDL,
			bez_LMAL, bez_LMBL, bez_LMCL, bez_LMDL,
			bez_HMcycle, bez_Mcycle, bez_LMcycle,
			bez_EQtotal
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		float bezEQ[bez_EQtotal][3];
		
		enum {
			bez_A,
			bez_B,
			bez_C,
			bez_Ctrl,
			bez_cycle,
			bez_min,
			bez_comp,
			bez_total
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		//Dynamics3
		
		bool hBypass;
		bool lBypass;
		//Cabs2
		
		enum {
			bip_dvAA, bip_dvAB, bip_dvAC, bip_dvAD, bip_pvAA, bip_pvAB, bip_pvAC, bip_pvAD,
			bip_dvA, bip_dvB, bip_dvC, bip_dvD, bip_pvA, bip_pvB, bip_pvC, bip_pvD,
			bip_total //each distortion section can have one of these, it stacks well
		}; //not remotely elliptic BLEP antialiasing, instead it is derivative BIP :D
		
		uint32_t fpd;
	
	struct _dram {
			float bezier[bezier_total];
		float bezComp[bez_total];
		float iirHPositionL[29];
		float iirHAngleL[29];
		float iirLPositionL[29];
		float iirLAngleL[29];
		float bip[bip_total];
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
	
	float bezierRez = fmax(powf((1.0f-GetParameter( kParam_SMO ))*0.25f,3.0f)/overallscale,0.00001f); 
	int stepped = 999999; if (bezierRez > 0.000001f) stepped = (int)(1.0f/bezierRez); bezierRez = 0.99999999f/stepped;
	const float bezierTrim = 1.0f-(bezierRez*((float)stepped/(stepped+1.0f)));
	//manages the overall Bezier control smoothing system plugin-wide	
	int spacing = floor(overallscale*2.0f);
	if (spacing < 2) spacing = 2; if (spacing > 32) spacing = 32;
	const float gainTrim = GetParameter( kParam_TRM )*2.0f;
	const float trebleRef = GetParameter( kParam_HIF )-0.3f;
	const float highmidRef = GetParameter( kParam_HMF )-0.5f;
	const float lowmidRef = GetParameter( kParam_LMF )-0.7f;
	const float bassRef = GetParameter( kParam_BSF )-0.9f;
	float HMderez = 0.75f + (trebleRef*0.25f);
	float LMderez = 0.25f + (bassRef*0.25f);
	float Mderez = ((HMderez+LMderez+highmidRef+lowmidRef)*0.25f);
	HMderez /= overallscale; Mderez /= overallscale; LMderez /= overallscale;
	if (HMderez > 1.0f) HMderez = 1.0f;
	stepped = 999999; if (HMderez > 0.000001f) stepped = (int)(1.0f/HMderez);
	HMderez = 0.99999999f / stepped;
	float HMtrim = 1.0f-(HMderez*((float)stepped/(stepped+1.0f)));
	stepped = 999999; if (Mderez > 0.000001f) stepped = (int)(1.0f/Mderez);
	Mderez = 0.99999999f / stepped;
	float Mtrim = 1.0f-(Mderez*((float)stepped/(stepped+1.0f)));
	stepped = 999999; if (LMderez > 0.000001f) stepped = (int)(1.0f/LMderez);
	LMderez = 0.99999999f / stepped;
	const float LMtrim = 1.0f-(LMderez*((float)stepped/(stepped+1.0f)));
	//BezEQ3 stepped elements
	float bezRez = fmax(powf((1.0f-GetParameter( kParam_ATK ))*0.4f,4.0f)/overallscale,0.0001f); 
	bezRez /= (2.0f/powf(overallscale,0.5f-((overallscale-1.0f)*0.0375f)));
	stepped = 999999; if (bezRez > 0.000001f) stepped = (int)(1.0f/bezRez);
	bezRez = 0.99999999f / stepped;
	float bezTrim = 1.0f-(bezRez*((float)stepped/(stepped+1.0f)));
	float sloRez = fmax(powf((1.0f-GetParameter( kParam_RLS ))*0.4f,4.0f)/overallscale,0.00001f);
	sloRez /= (2.0f/powf(overallscale,0.5f-((overallscale-1.0f)*0.0375f)));
	float sqrThresh = 1.0f; //recalculated in bezier section
	float bezThresh = dram->bezier[bezierThresh];
	float invDry = dram->bezier[bezierRatio];
	bool compress = (GetParameter( kParam_RAT ) > 0.499999f);
	bool compBypass = (GetParameter( kParam_THR ) > 0.9999f);
	bool tapeBypass = (GetParameter( kParam_MOR ) < 0.0001f);
	float pluginMode = GetParameter( kParam_MDE );
	//Dynamics3 stepped elements
	const int Lpoles = powf(GetParameter( kParam_LPQ )*3.0f,3.0f)+1;
	const int Hpoles = powf(GetParameter( kParam_HPQ )*3.0f,3.0f)+1; //1-28 pole filter
	//Lowpass/Highpass stepped elements
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		
		if (pluginMode > 0.5f) {
            float dryL = inputSampleL;
            if (inputSampleL > 1.0f) inputSampleL = 1.0f;
            else if (inputSampleL > 0.0f) inputSampleL = -expm1((log1p(-inputSampleL) * 0.6180339887498949f));
            if (inputSampleL < -1.0f) inputSampleL = -1.0f;
            else if (inputSampleL < 0.0f) inputSampleL = expm1((log1p(inputSampleL) * 0.6180339887498949f));
			dram->bip[bip_dvA] = dryL - inputSampleL; // these are derivatives: raw clip is position
			dram->bip[bip_dvB] = dram->bip[bip_pvA]-dram->bip[bip_dvA]; dram->bip[bip_pvA] = dram->bip[bip_dvA];//velocity
			dram->bip[bip_dvC] = dram->bip[bip_pvB]-dram->bip[bip_dvB]; dram->bip[bip_pvB] = dram->bip[bip_dvB];//acceleration
			dram->bip[bip_dvD] = dram->bip[bip_pvC]-dram->bip[bip_dvC]; dram->bip[bip_pvC] = dram->bip[bip_dvC];//jerk
			float bip_dvE = dram->bip[bip_pvD]-dram->bip[bip_dvD]; dram->bip[bip_pvD] = dram->bip[bip_dvD];//snap
			inputSampleL *= (1.0f+(fabs(dram->bip[bip_dvC])*0.0618f)+(fabs(dram->bip[bip_dvD])*-0.05982f)+(fabs(bip_dvE)*0.0206f));
            if (pluginMode < 1.0f) {
                inputSampleL = (dryL*(1.0f-((pluginMode-0.5f)*2.0f))) + (inputSampleL*((pluginMode-0.5f)*2.0f));
            }
        } //engage pluginMode, buss		
		
		//begin Bezier control smoothing engine
		dram->bezier[bezier_cycle] += bezierRez;
		if (dram->bezier[bezier_cycle] > bezierTrim) {dram->bezier[bezier_cycle] = 0.0f;
			dram->bezier[bezierHigh_C] =  dram->bezier[bezierHigh_B];  dram->bezier[bezierHigh_B] =  dram->bezier[bezierHigh_A];
			dram->bezier[bezierHmid_C] =  dram->bezier[bezierHmid_B];  dram->bezier[bezierHmid_B] =  dram->bezier[bezierHmid_A];
			dram->bezier[bezierLmid_C] =  dram->bezier[bezierLmid_B];  dram->bezier[bezierLmid_B] =  dram->bezier[bezierLmid_A];
			dram->bezier[bezierBass_C] =  dram->bezier[bezierBass_B];  dram->bezier[bezierBass_B] =  dram->bezier[bezierBass_A];
			dram->bezier[bezierHighQ_C] = dram->bezier[bezierHighQ_B]; dram->bezier[bezierHighQ_B] = dram->bezier[bezierHighQ_A];
			dram->bezier[bezierMidQ_C] =  dram->bezier[bezierMidQ_B];  dram->bezier[bezierMidQ_B] =  dram->bezier[bezierMidQ_A];
			dram->bezier[bezierLowQ_C] =  dram->bezier[bezierLowQ_B];  dram->bezier[bezierLowQ_B] =  dram->bezier[bezierLowQ_A];
			dram->bezier[bezierThresh_C]= dram->bezier[bezierThresh_B];dram->bezier[bezierThresh_B]= dram->bezier[bezierThresh_A];
			dram->bezier[bezierRatio_C] = dram->bezier[bezierRatio_B]; dram->bezier[bezierRatio_B] = dram->bezier[bezierRatio_A];
			dram->bezier[bezierMore_C] =  dram->bezier[bezierMore_B];  dram->bezier[bezierMore_B] =  dram->bezier[bezierMore_A];
			dram->bezier[bezierLFreq_C] = dram->bezier[bezierLFreq_B]; dram->bezier[bezierLFreq_B] = dram->bezier[bezierLFreq_A];
			dram->bezier[bezierHFreq_C] = dram->bezier[bezierHFreq_B]; dram->bezier[bezierHFreq_B] = dram->bezier[bezierHFreq_A];
			dram->bezier[bezierGainL_C] = dram->bezier[bezierGainL_B]; dram->bezier[bezierGainL_B] = dram->bezier[bezierGainL_A];
			//one of these bucket brigade lines for every smoothed control
			//begin smoothed control calculations			
			dram->bezier[bezierHigh_A] = (powf(GetParameter( kParam_HIG )+0.5f,3.0f)+2.0f)*0.3333333f;
			dram->bezier[bezierHmid_A] = (powf(GetParameter( kParam_HMG )+0.5f,3.0f)+2.0f)*0.3333333f;
			dram->bezier[bezierLmid_A] = (powf(GetParameter( kParam_LMG )+0.5f,3.0f)+2.0f)*0.3333333f;
			dram->bezier[bezierBass_A] = (powf(GetParameter( kParam_BSG )+0.5f,3.0f)+2.0f)*0.3333333f;
			dram->bezier[bezierHighQ_A] = 0.84f+((highmidRef-trebleRef)*0.1666666f);
			dram->bezier[bezierMidQ_A] = 0.84f+((lowmidRef-highmidRef)*0.1666666f);
			dram->bezier[bezierLowQ_A] = 0.84f+((bassRef-lowmidRef)*0.1666666f);
			//BezEQ3
			dram->bezier[bezierThresh_A] = powf(GetParameter( kParam_THR )+0.6180339887498949f,2.0f)*1.6180339887498949f;
			sqrThresh = sqrt(dram->bezier[bezierThresh_A]);
			dram->bezier[bezierRatio_A] = powf(1.0f-(fabs(GetParameter( kParam_RAT )-0.5f)*2.0f),1.6180339887498949f);
			//Dynamics3
			dram->bezier[bezierMore_A] = (GetParameter( kParam_MOR )*3.0f)+1.0f;
			//TapeHack
			dram->bezier[bezierLFreq_A] = powf(fmax(GetParameter( kParam_LOP ),0.002f),overallscale); //the lowpass
			dram->bezier[bezierHFreq_A] = powf(GetParameter( kParam_HIP ),overallscale+2.0f); //the highpass
			//Cabs2
			dram->bezier[bezierGainL_A] = powf(GetParameter( kParam_FAD ),2.0f);
			//Fader
			//end expensive control calculations
		} 
		const float lerp = dram->bezier[bezier_cycle]; //with this many controls we'll make temp variables
		const float La = (1.0f-lerp)*(1.0f-lerp); const float Lb = 2.0f*(1.0f-lerp)*lerp; const float Lc = lerp*lerp;
		dram->bezier[bezierHigh] = (dram->bezier[bezierHigh_B] + (dram->bezier[bezierHigh_C]*La) + (dram->bezier[bezierHigh_B]*Lb) + (dram->bezier[bezierHigh_A]*Lc))*0.5f;
		dram->bezier[bezierHmid] = (dram->bezier[bezierHmid_B] + (dram->bezier[bezierHmid_C]*La) + (dram->bezier[bezierHmid_B]*Lb) + (dram->bezier[bezierHmid_A]*Lc))*0.5f;
		dram->bezier[bezierLmid] = (dram->bezier[bezierLmid_B] + (dram->bezier[bezierLmid_C]*La) + (dram->bezier[bezierLmid_B]*Lb) + (dram->bezier[bezierLmid_A]*Lc))*0.5f;
		dram->bezier[bezierBass] = (dram->bezier[bezierBass_B] + (dram->bezier[bezierBass_C]*La) + (dram->bezier[bezierBass_B]*Lb) + (dram->bezier[bezierBass_A]*Lc))*0.5f;
		dram->bezier[bezierHighQ] = (dram->bezier[bezierHighQ_B] + (dram->bezier[bezierHighQ_C]*La) + (dram->bezier[bezierHighQ_B]*Lb) + (dram->bezier[bezierHighQ_A]*Lc))*0.5f;
		dram->bezier[bezierMidQ] = (dram->bezier[bezierMidQ_B] + (dram->bezier[bezierMidQ_C]*La) + (dram->bezier[bezierMidQ_B]*Lb) + (dram->bezier[bezierMidQ_A]*Lc))*0.5f;
		dram->bezier[bezierLowQ] = (dram->bezier[bezierLowQ_B] + (dram->bezier[bezierLowQ_C]*La) + (dram->bezier[bezierLowQ_B]*Lb) + (dram->bezier[bezierLowQ_A]*Lc))*0.5f;
		dram->bezier[bezierThresh] = (dram->bezier[bezierThresh_B] + (dram->bezier[bezierThresh_C]*La) + (dram->bezier[bezierThresh_B]*Lb) + (dram->bezier[bezierThresh_A]*Lc))*0.5f;
		dram->bezier[bezierRatio] = (dram->bezier[bezierRatio_B] + (dram->bezier[bezierRatio_C]*La) + (dram->bezier[bezierRatio_B]*Lb) + (dram->bezier[bezierRatio_A]*Lc))*0.5f;
		dram->bezier[bezierMore] = (dram->bezier[bezierMore_B] + (dram->bezier[bezierMore_C]*La) + (dram->bezier[bezierMore_B]*Lb) + (dram->bezier[bezierMore_A]*Lc))*0.5f;
		dram->bezier[bezierLFreq] = (dram->bezier[bezierLFreq_B] + (dram->bezier[bezierLFreq_C]*La) + (dram->bezier[bezierLFreq_B]*Lb) + (dram->bezier[bezierLFreq_A]*Lc))*0.5f;
		dram->bezier[bezierHFreq] = (dram->bezier[bezierHFreq_B] + (dram->bezier[bezierHFreq_C]*La) + (dram->bezier[bezierHFreq_B]*Lb) + (dram->bezier[bezierHFreq_A]*Lc))*0.5f;
		dram->bezier[bezierGainL] = (dram->bezier[bezierGainL_B] + (dram->bezier[bezierGainL_C]*La) + (dram->bezier[bezierGainL_B]*Lb) + (dram->bezier[bezierGainL_A]*Lc))*0.5f;
		//end Bezier control smoothing engine
		
		//begin Trim switch
		if (gainTrim != 1.0f) {inputSampleL *= gainTrim;}
		//end Trim switch
		
		//begin BezEQ4 Pre
		float highL = inputSampleL;
		bezEQ[bez_HMcycle][0] += HMderez;
		bezEQ[bez_HMAL][0] += (highL * HMderez);
		if (bezEQ[bez_HMcycle][0] > HMtrim) {
			bezEQ[bez_HMDL][0] = bezEQ[bez_HMCL][0]; bezEQ[bez_HMCL][0] = bezEQ[bez_HMBL][0]; 
			bezEQ[bez_HMBL][0] = bezEQ[bez_HMAL][0]*(0.5f-(HMderez*0.082f));
			bezEQ[bez_HMAL][0] = 0.0f; bezEQ[bez_HMcycle][0] = 0.0f;
		}
		float X = bezEQ[bez_HMcycle][0];
		float hmidL = bezEQ[bez_HMCL][0]+(bezEQ[bez_HMDL][0]*(1.0f-X)*(1.0f-X));
		hmidL += (bezEQ[bez_HMCL][0]*2.0f*(1.0f-X)*X)+(bezEQ[bez_HMBL][0]*X*X);
		hmidL *= dram->bezier[bezierHighQ];
		highL -= hmidL; //final high and hmid
		bezEQ[bez_Mcycle][0] += Mderez;
		bezEQ[bez_MAL][0] += (hmidL * Mderez);
		if (bezEQ[bez_Mcycle][0] > Mtrim) {
			bezEQ[bez_MDL][0] = bezEQ[bez_MCL][0]; bezEQ[bez_MCL][0] = bezEQ[bez_MBL][0]; 
			bezEQ[bez_MBL][0] = bezEQ[bez_MAL][0]*(0.5f-(Mderez*0.082f));
			bezEQ[bez_MAL][0] = 0.0f; bezEQ[bez_Mcycle][0] = 0.0f;
		}
		X = bezEQ[bez_Mcycle][0];
		float lmidL = bezEQ[bez_MCL][0]+(bezEQ[bez_MDL][0]*(1.0f-X)*(1.0f-X));
		lmidL += (bezEQ[bez_MCL][0]*2.0f*(1.0f-X)*X)+(bezEQ[bez_MBL][0]*X*X);
		lmidL *= dram->bezier[bezierMidQ];
		hmidL -= lmidL; //final hmid and lmid
		bezEQ[bez_LMcycle][0] += LMderez;
		bezEQ[bez_LMAL][0] += (lmidL * LMderez);
		if (bezEQ[bez_LMcycle][0] > LMtrim) {
			bezEQ[bez_LMDL][0] = bezEQ[bez_LMCL][0]; bezEQ[bez_LMCL][0] = bezEQ[bez_LMBL][0]; 
			bezEQ[bez_LMBL][0] = bezEQ[bez_LMAL][0]*(0.5f-(LMderez*0.082f));
			bezEQ[bez_LMAL][0] = 0.0f; bezEQ[bez_LMcycle][0] = 0.0f;
		}
		X = bezEQ[bez_LMcycle][0];
		float bassL = bezEQ[bez_LMCL][0]+(bezEQ[bez_LMDL][0]*(1.0f-X)*(1.0f-X));
		bassL += (bezEQ[bez_LMCL][0]*2.0f*(1.0f-X)*X)+(bezEQ[bez_LMBL][0]*X*X);
		bassL *= dram->bezier[bezierLowQ];
		lmidL -= bassL; //final lmid and bass
		inputSampleL = (highL*dram->bezier[bezierHigh])+(hmidL*dram->bezier[bezierHmid])+(lmidL*dram->bezier[bezierLmid])+(bassL*dram->bezier[bezierBass]);
		//end BezEQ4 Pre
		
		if (!compBypass) {
			//begin Dynamics3
			float dryCompL = inputSampleL;
			if (compress) inputSampleL *= (dram->bezComp[bez_comp]/bezThresh);
			else inputSampleL /= bezThresh;
			float ctrl = fmin(inputSampleL,sqrThresh*dram->bezComp[bez_comp]*0.6180339887498949f);
			dram->bezComp[bez_min] = fmax(dram->bezComp[bez_min]-sloRez,ctrl);
			dram->bezComp[bez_Ctrl] += (dram->bezComp[bez_min] * bezRez);
			dram->bezComp[bez_cycle] += bezRez;
			if (dram->bezComp[bez_cycle] > bezTrim) {dram->bezComp[bez_cycle] = 0.0f;
				dram->bezComp[bez_C] = dram->bezComp[bez_B]; dram->bezComp[bez_B] = dram->bezComp[bez_A];
				dram->bezComp[bez_A] = dram->bezComp[bez_Ctrl]; dram->bezComp[bez_Ctrl] = 0.0f;}
			float X = dram->bezComp[bez_cycle];
			dram->bezComp[bez_comp] = dram->bezComp[bez_B]+(dram->bezComp[bez_C]*(1.0f-X)*(1.0f-X))+(dram->bezComp[bez_B]*2.0f*(1.0f-X)*X)+(dram->bezComp[bez_A]*X*X);
			dram->bezComp[bez_comp] = ((1.0f-(fmin(dram->bezComp[bez_comp],0.9999f))));
			if (compress) {
				inputSampleL = inputSampleL*(1.0f-invDry)*dram->bezComp[bez_comp]*bezThresh;
				inputSampleL = fmax(fmin(inputSampleL,2.305929007734908f),-2.305929007734908f);
				float addtwo = inputSampleL * inputSampleL;
				float empower = inputSampleL * addtwo; // inputSampleL to the third power
				inputSampleL -= (empower / 6.0f); empower *= addtwo; // to the fifth power
				inputSampleL += (empower / 69.0f); empower *= addtwo; //seventh
				inputSampleL -= (empower / 2530.08f); empower *= addtwo; //ninth
				inputSampleL += (empower / 224985.6f); empower *= addtwo; //eleventh
				inputSampleL -= (empower / 9979200.0f);
				//this is a degenerate form of a Taylor Series to approximate sin()				
				inputSampleL += (dryCompL*invDry*(1.0f-(dram->bezComp[bez_comp]*(1.0f-invDry))));
			} else {
				inputSampleL = ((inputSampleL/(0.1f+bezThresh))*(1.0f-invDry))/dram->bezComp[bez_comp];
				inputSampleL = fmax(fmin(inputSampleL,2.305929007734908f),-2.305929007734908f);
				float addtwo = inputSampleL * inputSampleL;
				float empower = inputSampleL * addtwo; // inputSampleL to the third power
				inputSampleL -= (empower / 6.0f); empower *= addtwo; // to the fifth power
				inputSampleL += (empower / 69.0f); empower *= addtwo; //seventh
				inputSampleL -= (empower / 2530.08f); empower *= addtwo; //ninth
				inputSampleL += (empower / 224985.6f); empower *= addtwo; //eleventh
				inputSampleL -= (empower / 9979200.0f);
				//this is a degenerate form of a Taylor Series to approximate sin()				
				inputSampleL += (dryCompL*invDry*(1.0f-(dram->bezComp[bez_comp]*(1.0f-invDry))));
			}//dynamics can produce superhot peaks
			//end Dynamics3
		}
		
		//begin BezEQ4 Mid
		highL = inputSampleL;
		bezEQ[bez_HMcycle][1] += HMderez;
		bezEQ[bez_HMAL][1] += (highL * HMderez);
		if (bezEQ[bez_HMcycle][1] > HMtrim) {
			bezEQ[bez_HMDL][1] = bezEQ[bez_HMCL][1]; bezEQ[bez_HMCL][1] = bezEQ[bez_HMBL][1]; 
			bezEQ[bez_HMBL][1] = bezEQ[bez_HMAL][1]*(0.5f-(HMderez*0.082f));
			bezEQ[bez_HMAL][1] = 0.0f; bezEQ[bez_HMcycle][1] = 0.0f;
		}
		X = bezEQ[bez_HMcycle][1];
		hmidL = bezEQ[bez_HMCL][1]+(bezEQ[bez_HMDL][1]*(1.0f-X)*(1.0f-X));
		hmidL += (bezEQ[bez_HMCL][1]*2.0f*(1.0f-X)*X)+(bezEQ[bez_HMBL][1]*X*X);
		hmidL *= dram->bezier[bezierHighQ];
		highL -= hmidL; //final high and hmid
		bezEQ[bez_Mcycle][1] += Mderez;
		bezEQ[bez_MAL][1] += (hmidL * Mderez);
		if (bezEQ[bez_Mcycle][1] > Mtrim) {
			bezEQ[bez_MDL][1] = bezEQ[bez_MCL][1]; bezEQ[bez_MCL][1] = bezEQ[bez_MBL][1]; 
			bezEQ[bez_MBL][1] = bezEQ[bez_MAL][1]*(0.5f-(Mderez*0.082f));
			bezEQ[bez_MAL][1] = 0.0f; bezEQ[bez_Mcycle][1] = 0.0f;
		}
		X = bezEQ[bez_Mcycle][1];
		lmidL = bezEQ[bez_MCL][1]+(bezEQ[bez_MDL][1]*(1.0f-X)*(1.0f-X));
		lmidL += (bezEQ[bez_MCL][1]*2.0f*(1.0f-X)*X)+(bezEQ[bez_MBL][1]*X*X);
		lmidL *= dram->bezier[bezierMidQ];
		hmidL -= lmidL; //final hmid and lmid
		bezEQ[bez_LMcycle][1] += LMderez;
		bezEQ[bez_LMAL][1] += (lmidL * LMderez);
		if (bezEQ[bez_LMcycle][1] > LMtrim) {
			bezEQ[bez_LMDL][1] = bezEQ[bez_LMCL][1]; bezEQ[bez_LMCL][1] = bezEQ[bez_LMBL][1]; 
			bezEQ[bez_LMBL][1] = bezEQ[bez_LMAL][1]*(0.5f-(LMderez*0.082f));
			bezEQ[bez_LMAL][1] = 0.0f; bezEQ[bez_LMcycle][1] = 0.0f;
		}
		X = bezEQ[bez_LMcycle][1];
		bassL = bezEQ[bez_LMCL][1]+(bezEQ[bez_LMDL][1]*(1.0f-X)*(1.0f-X));
		bassL += (bezEQ[bez_LMCL][1]*2.0f*(1.0f-X)*X)+(bezEQ[bez_LMBL][1]*X*X);
		bassL *= dram->bezier[bezierLowQ];
		lmidL -= bassL; //final lmid and bass
		inputSampleL = (highL*dram->bezier[bezierHigh])+(hmidL*dram->bezier[bezierHmid])+(lmidL*dram->bezier[bezierLmid])+(bassL*dram->bezier[bezierBass]);
		//end BezEQ4 Mid
		
		//begin Weave
		if (!tapeBypass) {
			inputSampleL *= dram->bezier[bezierMore];
			float bip_delta = inputSampleL; //delta can be just local and re-used
			if (inputSampleL != 0.0f) {
				inputSampleL = fmax(fmin(inputSampleL,1.0f),-1.0f);
				inputSampleL *= 1.27f;
				float weave = sqrt(fabs(inputSampleL));
				float spiral = inputSampleL * weave;
				float X = spiral; X *= X;
				float temp = spiral * X;
				spiral -= temp*0.16666666666666666666666666666666666f; temp *= X;//-8 dB
				spiral += temp*0.00833333333333333333333333333333333f; temp *= X;//-20 dB
				spiral -= temp*0.00019841269841269841269841269841269f; temp *= X;//-37 dB
				spiral += temp*0.00000275573192239858906525573192239f; temp *= X;//-55 dB
				spiral -= temp*0.00000002505210838544171877521083854f; temp *= X;//-76 dB
				spiral += temp*0.00000000016059043836821614599392377f; temp *= X;//-108 dB
				spiral -= temp*0.00000000000076471637318198164759011f; temp *= X;//-121 dB
				spiral += temp*0.00000000000000281145725434552076319f; temp *= X;//-146 dB
				spiral -= temp*0.00000000000000000822063524662432971f; temp *= X;//-171 dB
				spiral += temp*0.00000000000000000001957294106339126f;           //-197 dB
				inputSampleL = spiral / weave; //purest form is full Spiral
			} //version of Weave in ConsoleX uses A and B for derivatives, not L and R
			dram->bip[bip_dvAA] = bip_delta - inputSampleL; // these are derivatives: raw clip is position
			dram->bip[bip_dvAB] = dram->bip[bip_pvAA]-dram->bip[bip_dvAA]; dram->bip[bip_pvAA] = dram->bip[bip_dvAA];//velocity
			dram->bip[bip_dvAC] = dram->bip[bip_pvAB]-dram->bip[bip_dvAB]; dram->bip[bip_pvAB] = dram->bip[bip_dvAB];//acceleration
			dram->bip[bip_dvAD] = dram->bip[bip_pvAC]-dram->bip[bip_dvAC]; dram->bip[bip_pvAC] = dram->bip[bip_dvAC];//jerk
			float bip_dvE = dram->bip[bip_pvAD]-dram->bip[bip_dvAD]; dram->bip[bip_pvAD] = dram->bip[bip_dvAD];//snap
			inputSampleL *= (1.0f+(fabs(dram->bip[bip_dvAC])*0.0618f)+(fabs(dram->bip[bip_dvAD])*-0.05982f)+(fabs(bip_dvE)*0.0206f));
		} //end Weave
		
		//begin BezEQ4 Post 
		highL = inputSampleL;
		bezEQ[bez_HMcycle][2] += HMderez;
		bezEQ[bez_HMAL][2] += (highL * HMderez);
		if (bezEQ[bez_HMcycle][2] > HMtrim) {
			bezEQ[bez_HMDL][2] = bezEQ[bez_HMCL][2]; bezEQ[bez_HMCL][2] = bezEQ[bez_HMBL][2]; 
			bezEQ[bez_HMBL][2] = bezEQ[bez_HMAL][2]*(0.5f-(HMderez*0.082f));
			bezEQ[bez_HMAL][2] = 0.0f; bezEQ[bez_HMcycle][2] = 0.0f;
		}
		X = bezEQ[bez_HMcycle][2];
		hmidL = bezEQ[bez_HMCL][2]+(bezEQ[bez_HMDL][2]*(1.0f-X)*(1.0f-X));
		hmidL += (bezEQ[bez_HMCL][2]*2.0f*(1.0f-X)*X)+(bezEQ[bez_HMBL][2]*X*X);
		hmidL *= dram->bezier[bezierHighQ];
		highL -= hmidL; //final high and hmid
		bezEQ[bez_Mcycle][2] += Mderez;
		bezEQ[bez_MAL][2] += (hmidL * Mderez);
		if (bezEQ[bez_Mcycle][2] > Mtrim) {
			bezEQ[bez_MDL][2] = bezEQ[bez_MCL][2]; bezEQ[bez_MCL][2] = bezEQ[bez_MBL][2]; 
			bezEQ[bez_MBL][2] = bezEQ[bez_MAL][2]*(0.5f-(Mderez*0.082f));
			bezEQ[bez_MAL][2] = 0.0f; bezEQ[bez_Mcycle][2] = 0.0f;
		}
		X = bezEQ[bez_Mcycle][2];
		lmidL = bezEQ[bez_MCL][2]+(bezEQ[bez_MDL][2]*(1.0f-X)*(1.0f-X));
		lmidL += (bezEQ[bez_MCL][2]*2.0f*(1.0f-X)*X)+(bezEQ[bez_MBL][2]*X*X);
		lmidL *= dram->bezier[bezierMidQ];
		hmidL -= lmidL; //final hmid and lmid
		bezEQ[bez_LMcycle][2] += LMderez;
		bezEQ[bez_LMAL][2] += (lmidL * LMderez);
		if (bezEQ[bez_LMcycle][2] > LMtrim) {
			bezEQ[bez_LMDL][2] = bezEQ[bez_LMCL][2]; bezEQ[bez_LMCL][2] = bezEQ[bez_LMBL][2]; 
			bezEQ[bez_LMBL][2] = bezEQ[bez_LMAL][2]*(0.5f-(LMderez*0.082f));
			bezEQ[bez_LMAL][2] = 0.0f; bezEQ[bez_LMcycle][2] = 0.0f;
		}
		X = bezEQ[bez_LMcycle][2];
		bassL = bezEQ[bez_LMCL][2]+(bezEQ[bez_LMDL][2]*(1.0f-X)*(1.0f-X));
		bassL += (bezEQ[bez_LMCL][2]*2.0f*(1.0f-X)*X)+(bezEQ[bez_LMBL][2]*X*X);
		bassL *= dram->bezier[bezierLowQ];
		lmidL -= bassL; //final lmid and bass
		inputSampleL = (highL*dram->bezier[bezierHigh])+(hmidL*dram->bezier[bezierHmid])+(lmidL*dram->bezier[bezierLmid])+(bassL*dram->bezier[bezierBass]);
		//end BezEQ4 Post
		
		//begin Lowpass/Highpass
		if (dram->bezier[bezierHFreq] > 0.0f) {
			float lowSampleL = inputSampleL;
			for(int count = 0; count < Hpoles; count++) {
				dram->iirHAngleL[count] = (dram->iirHAngleL[count]*(1.0f-dram->bezier[bezierHFreq]))+((lowSampleL-dram->iirHPositionL[count])*dram->bezier[bezierHFreq]);
				lowSampleL = ((dram->iirHPositionL[count]+(dram->iirHAngleL[count]*dram->bezier[bezierHFreq]))*(1.0f-dram->bezier[bezierHFreq]))+(lowSampleL*dram->bezier[bezierHFreq]);
				dram->iirHPositionL[count] = ((dram->iirHPositionL[count]+(dram->iirHAngleL[count]*dram->bezier[bezierHFreq]))*(1.0f-dram->bezier[bezierHFreq]))+(lowSampleL*dram->bezier[bezierHFreq]);
				inputSampleL -= (lowSampleL * (1.0f/(float)Hpoles));//left
			} hBypass = false; //the highpass
		} else {
			if (!hBypass) {
				hBypass = true;
				for(int count = 0; count < 29; count++) {
					dram->iirHPositionL[count] = 0.0f;
					dram->iirHAngleL[count] = 0.0f;
				}//blank out highpass if jut switched off
			} 
		}		
		if (dram->bezier[bezierLFreq] < 1.0f) {
			for(int count = 0; count < Lpoles; count++) {
				dram->iirLAngleL[count] = (dram->iirLAngleL[count]*(1.0f-dram->bezier[bezierLFreq]))+((inputSampleL-dram->iirLPositionL[count])*dram->bezier[bezierLFreq]);
				inputSampleL = ((dram->iirLPositionL[count]+(dram->iirLAngleL[count]*dram->bezier[bezierLFreq]))*(1.0f-dram->bezier[bezierLFreq]))+(inputSampleL*dram->bezier[bezierLFreq]);
				dram->iirLPositionL[count] = ((dram->iirLPositionL[count]+(dram->iirLAngleL[count]*dram->bezier[bezierLFreq]))*(1.0f-dram->bezier[bezierLFreq]))+(inputSampleL*dram->bezier[bezierLFreq]);//left
			} lBypass = false; //the lowpass
		} else {
			if (!lBypass) {
				lBypass = true;
				for(int count = 0; count < 29; count++) {
					dram->iirLPositionL[count] = 0.0f;
					dram->iirLAngleL[count] = 0.0f;
				}//blank out lowpass if just switched off
			} 
		}		
		//end Lowpass/Highpass
		
		inputSampleL *= dram->bezier[bezierGainL];
		//applies pan section, and smoothed fader gain
		
		if (pluginMode <= 0.5f) {
            float dryL = inputSampleL;
            if (inputSampleL > 1.0f) inputSampleL = 1.0f;
            else if (inputSampleL > 0.0f) inputSampleL = -expm1((log1p(-inputSampleL) * 1.618033988749895f));
            if (inputSampleL < -1.0f) inputSampleL = -1.0f;
            else if (inputSampleL < 0.0f) inputSampleL = expm1((log1p(inputSampleL) * 1.618033988749895f));
			dram->bip[bip_dvA] = dryL - inputSampleL; // these are derivatives: raw clip is position
			dram->bip[bip_dvB] = dram->bip[bip_pvA]-dram->bip[bip_dvA]; dram->bip[bip_pvA] = dram->bip[bip_dvA];//velocity
			dram->bip[bip_dvC] = dram->bip[bip_pvB]-dram->bip[bip_dvB]; dram->bip[bip_pvB] = dram->bip[bip_dvB];//acceleration
			dram->bip[bip_dvD] = dram->bip[bip_pvC]-dram->bip[bip_dvC]; dram->bip[bip_pvC] = dram->bip[bip_dvC];//jerk
			float bip_dvE = dram->bip[bip_pvD]-dram->bip[bip_dvD]; dram->bip[bip_pvD] = dram->bip[bip_dvD];//snap
			inputSampleL *= (1.0f+(fabs(dram->bip[bip_dvC])*0.0618f)+(fabs(dram->bip[bip_dvD])*-0.05982f)+(fabs(bip_dvE)*0.0206f));
			if (pluginMode > 0.0f) {
                inputSampleL = (inputSampleL*(1.0f-(pluginMode*2.0f)))+(dryL*(pluginMode*2.0f));
            }
        } //engage pluginMode, channel version
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < bezier_total; x++) dram->bezier[x] = 0.0;
	//Control Smoothing
	
	for (int x = 0; x < bez_EQtotal; x++) {for (int y = 0; y < 3; y++) bezEQ[x][y] = 0.0;}
	//BezEQ4
	
	for (int x = 0; x < bez_total; x++) dram->bezComp[x] = 0.0;
	//Dynamics3
	
	for(int count = 0; count < 28; count++) {
		dram->iirHPositionL[count] = 0.0;
		dram->iirHAngleL[count] = 0.0;
	}
	hBypass = false;
	for(int count = 0; count < 28; count++) {
		dram->iirLPositionL[count] = 0.0;
		dram->iirLAngleL[count] = 0.0;
	}
	lBypass = false;	
	//Cabs2
	
	for (int x = 0; x < bip_total; x++) dram->bip[x] = 0.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
