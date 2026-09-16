#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "BezEQ"
#define AIRWINDOWS_DESCRIPTION "A strange, alien three-band shelving EQ."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','e','Q' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	kParam_E =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Treble", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "x", .min = 0, .max = 1000, .def = 618, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Mid", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "x", .min = 0, .max = 1000, .def = 382, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bass", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		
		enum {
			bez_AL,
			bez_AR,
			bez_BL,
			bez_BR,
			bez_CL,
			bez_CR,	
			bez_InL,
			bez_InR,
			bez_UnInL,
			bez_UnInR,
			bez_SampL,
			bez_SampR,
			bez_cycle,
			bez_total
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		
		uint32_t fpd;
	
	struct _dram {
			float bezA[bez_total];
		float bezB[bez_total];
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
	
	float trebleGain = GetParameter( kParam_A ) * 2.0f; trebleGain *= trebleGain;
	
	float derezA = GetParameter( kParam_B )/overallscale;
	if (derezA < 0.01f) derezA = 0.01f; if (derezA > 1.0f) derezA = 1.0f;
	derezA = 1.0f / ((int)(1.0f/derezA));
	
	float midGain = GetParameter( kParam_C ) * 2.0f;	 midGain *= midGain;
	
	float derezB = powf(GetParameter( kParam_D ),4.0f)/overallscale;
	if (derezB < 0.0001f) derezB = 0.0001f; if (derezB > 1.0f) derezB = 1.0f;
	derezB = 1.0f / ((int)(1.0f/derezB));
	
	float bassGain = GetParameter( kParam_E ) * 2.0f; bassGain *= bassGain;

	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;

		dram->bezA[bez_cycle] += derezA;
		dram->bezA[bez_SampL] += ((inputSampleL+dram->bezA[bez_InL]) * derezA);
		dram->bezA[bez_InL] = inputSampleL;
		
		if (dram->bezA[bez_cycle] > 1.0f) { //hit the end point and we do a reverb sample
			dram->bezA[bez_cycle] = 0.0f;
			dram->bezA[bez_CL] = dram->bezA[bez_BL];
			dram->bezA[bez_BL] = dram->bezA[bez_AL];
			dram->bezA[bez_AL] = inputSampleL;
			dram->bezA[bez_SampL] = 0.0f;
		}
		float CBL = (dram->bezA[bez_CL]*(1.0f-dram->bezA[bez_cycle]))+(dram->bezA[bez_BL]*dram->bezA[bez_cycle]);
		float BAL = (dram->bezA[bez_BL]*(1.0f-dram->bezA[bez_cycle]))+(dram->bezA[bez_AL]*dram->bezA[bez_cycle]);
		float CBAL = (dram->bezA[bez_BL]+(CBL*(1.0f-dram->bezA[bez_cycle]))+(BAL*dram->bezA[bez_cycle]))*0.5f;
		float mid = CBAL;
		float treble = inputSampleL - CBAL;
		
		dram->bezB[bez_cycle] += derezB;
		dram->bezB[bez_SampL] += ((mid+dram->bezB[bez_InL]) * derezB);
		dram->bezB[bez_InL] = mid;
		
		if (dram->bezB[bez_cycle] > 1.0f) { //hit the end point and we do a reverb sample
			dram->bezB[bez_cycle] = 0.0f;
			dram->bezB[bez_CL] = dram->bezB[bez_BL];
			dram->bezB[bez_BL] = dram->bezB[bez_AL];
			dram->bezB[bez_AL] = inputSampleL;
			dram->bezB[bez_SampL] = 0.0f;
		}
		CBL = (dram->bezB[bez_CL]*(1.0f-dram->bezB[bez_cycle]))+(dram->bezB[bez_BL]*dram->bezB[bez_cycle]);
		BAL = (dram->bezB[bez_BL]*(1.0f-dram->bezB[bez_cycle]))+(dram->bezB[bez_AL]*dram->bezB[bez_cycle]);
		CBAL = (dram->bezB[bez_BL]+(CBL*(1.0f-dram->bezB[bez_cycle]))+(BAL*dram->bezB[bez_cycle]))*0.5f;
		float bass = CBAL;
		mid -= bass;
		
		inputSampleL = (bass*bassGain) + (mid*midGain) + (treble*trebleGain);
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < bez_total; x++) {dram->bezA[x] = 0.0; dram->bezB[x] = 0.0;}
	dram->bezA[bez_cycle] = 1.0; dram->bezB[bez_cycle] = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
