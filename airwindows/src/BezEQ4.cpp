#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BezEQ4"
#define AIRWINDOWS_DESCRIPTION "BezEQ4"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','e','4' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	kParam_F =5,
	kParam_G =6,
	kParam_H =7,
	//Add your parameters here...
	kNumberOfParameters=8
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, };
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
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, kParam6, kParam7, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		enum {
			bez_HMAL, bez_HMBL, bez_HMCL, bez_HMDL,
			bez_MAL, bez_MBL, bez_MCL, bez_MDL,
			bez_LMAL, bez_LMBL, bez_LMCL, bez_LMDL,
			bez_HMcycle, bez_Mcycle, bez_LMcycle,
			bez_EQtotal
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		float bezEQ[bez_EQtotal][3];

		uint32_t fpd;
	
	struct _dram {
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
	
	//begin BezEQ4 control setup
	float highGain = (powf(GetParameter( kParam_A )+0.5f,3.0f)+2.0f)*0.3333333f;
	float hmidGain = (powf(GetParameter( kParam_B )+0.5f,3.0f)+2.0f)*0.3333333f;
	float lmidGain = (powf(GetParameter( kParam_C )+0.5f,3.0f)+2.0f)*0.3333333f;
	float bassGain = (powf(GetParameter( kParam_D )+0.5f,3.0f)+2.0f)*0.3333333f;
	float trebleRef = GetParameter( kParam_E )-0.3f;
	float highmidRef = GetParameter( kParam_F )-0.5f;
	float lowmidRef = GetParameter( kParam_G )-0.7f;
	float bassRef = GetParameter( kParam_H )-0.9f;
	float HMderez = 0.75f + (trebleRef*0.25f);
	float LMderez = 0.25f + (bassRef*0.25f);
	float Mderez = ((HMderez+LMderez+highmidRef+lowmidRef)*0.25f);
	HMderez /= overallscale; Mderez /= overallscale; LMderez /= overallscale;
	if (HMderez > 1.0f) HMderez = 1.0f;
	int stepped = 999999; if (HMderez > 0.000001f) stepped = (int)(1.0f/HMderez);
	HMderez = 0.99999999f / stepped;
	float HMtrim = 1.0f-(HMderez*((float)stepped/(stepped+1.0f)));
	stepped = 999999; if (Mderez > 0.000001f) stepped = (int)(1.0f/Mderez);
	Mderez = 0.99999999f / stepped;
	float Mtrim = 1.0f-(Mderez*((float)stepped/(stepped+1.0f)));
	stepped = 999999; if (LMderez > 0.000001f) stepped = (int)(1.0f/LMderez);
	LMderez = 0.99999999f / stepped;
	float LMtrim = 1.0f-(LMderez*((float)stepped/(stepped+1.0f)));
	float highQ = 0.84f+((highmidRef-trebleRef)*0.1666666f);
	float midQ = 0.84f+((lowmidRef-highmidRef)*0.1666666f);
	float lowQ = 0.84f+((bassRef-lowmidRef)*0.1666666f);
	//end BezEQ4 control setup
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		
		//begin BezEQ4
		for (int p = 0; p < 3; p++) {
			float high = inputSampleL;
			bezEQ[bez_HMcycle][p] += HMderez;
			bezEQ[bez_HMAL][p] += (high * HMderez);
			if (bezEQ[bez_HMcycle][p] > HMtrim) {
				bezEQ[bez_HMDL][p] = bezEQ[bez_HMCL][p]; bezEQ[bez_HMCL][p] = bezEQ[bez_HMBL][p]; 
				bezEQ[bez_HMBL][p] = bezEQ[bez_HMAL][p]*(0.5f-(HMderez*0.082f));
				bezEQ[bez_HMAL][p] = 0.0f; bezEQ[bez_HMcycle][p] = 0.0f;
			}
			float X = bezEQ[bez_HMcycle][p];
			float hmid = bezEQ[bez_HMCL][p]+(bezEQ[bez_HMDL][p]*(1.0f-X)*(1.0f-X));
			hmid += (bezEQ[bez_HMCL][p]*2.0f*(1.0f-X)*X)+(bezEQ[bez_HMBL][p]*X*X);
			hmid *= highQ;
			high -= hmid; //final high and hmid
			bezEQ[bez_Mcycle][p] += Mderez;
			bezEQ[bez_MAL][p] += (hmid * Mderez);
			if (bezEQ[bez_Mcycle][p] > Mtrim) {
				bezEQ[bez_MDL][p] = bezEQ[bez_MCL][p]; bezEQ[bez_MCL][p] = bezEQ[bez_MBL][p]; 
				bezEQ[bez_MBL][p] = bezEQ[bez_MAL][p]*(0.5f-(Mderez*0.082f));
				bezEQ[bez_MAL][p] = 0.0f; bezEQ[bez_Mcycle][p] = 0.0f;
			}
			X = bezEQ[bez_Mcycle][p];
			float lmid = bezEQ[bez_MCL][p]+(bezEQ[bez_MDL][p]*(1.0f-X)*(1.0f-X));
			lmid += (bezEQ[bez_MCL][p]*2.0f*(1.0f-X)*X)+(bezEQ[bez_MBL][p]*X*X);
			lmid *= midQ;
			hmid -= lmid; //final hmid and lmid
			bezEQ[bez_LMcycle][p] += LMderez;
			bezEQ[bez_LMAL][p] += (lmid * LMderez);
			if (bezEQ[bez_LMcycle][p] > LMtrim) {
				bezEQ[bez_LMDL][p] = bezEQ[bez_LMCL][p]; bezEQ[bez_LMCL][p] = bezEQ[bez_LMBL][p]; 
				bezEQ[bez_LMBL][p] = bezEQ[bez_LMAL][p]*(0.5f-(LMderez*0.082f));
				bezEQ[bez_LMAL][p] = 0.0f; bezEQ[bez_LMcycle][p] = 0.0f;
			}
			X = bezEQ[bez_LMcycle][p];
			float bass = bezEQ[bez_LMCL][p]+(bezEQ[bez_LMDL][p]*(1.0f-X)*(1.0f-X));
			bass += (bezEQ[bez_LMCL][p]*2.0f*(1.0f-X)*X)+(bezEQ[bez_LMBL][p]*X*X);
			bass *= lowQ;
			lmid -= bass; //final lmid and bass
			inputSampleL = (high*highGain)+(hmid*hmidGain)+(lmid*lmidGain)+(bass*bassGain);
		}
		//end BezEQ4
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < bez_EQtotal; x++) {for (int y = 0; y < 3; y++) bezEQ[x][y] = 0.0;}
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
