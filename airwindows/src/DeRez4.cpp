#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DeRez4"
#define AIRWINDOWS_DESCRIPTION "Perfects retro digital tones."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','e','4' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "DownRez", .min = 0, .max = 1000, .def = 618, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bright", .min = 0, .max = 1000, .def = 618, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bassis", .min = 0, .max = 1000, .def = 382, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		enum {
			bez_HMAL, bez_HMBL, bez_HMCL, bez_HMDL, bez_HMcycle,
			bez_LMAL, bez_LMBL, bez_LMCL, bez_LMDL, bez_LMcycle,
			bez_total
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		float bezEQ[bez_total][2];
		float pointCycle, pointAL, pointBL;

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
	
	float Sderez = powf(GetParameter( kParam_A ),2.0f+overallscale);
	int stepped = 999999; if (Sderez > 0.000001f) stepped = (int)(1.0f/Sderez);
	if (Sderez > 0.0f) Sderez = 0.99999999f / stepped;
	float Strim = 1.0f-(Sderez*((float)stepped/(stepped+1.0f)));
	float brightDerez = (1.0f-powf(GetParameter( kParam_B ),2.0f))*4.0f*overallscale;
	stepped = 999999; 
	if (brightDerez > 1.0f) {stepped = (int)brightDerez; brightDerez = 0.99999999f / stepped;}
	else brightDerez = 1.1f;
	float brightTrim = 1.0f-(brightDerez*((float)stepped/(stepped+1.0f)));
	float bassRef = powf(GetParameter( kParam_C ),4.0f);
	float bassisDerez = (bassRef*0.25f) / overallscale;
	stepped = 999999; if (bassisDerez > 0.000001f) stepped = (int)(1.0f/bassisDerez);
	if (bassisDerez > 0.0f) bassisDerez = 0.99999999f / stepped;
	float bassisTrim = 1.0f-(bassisDerez*((float)stepped/(stepped+1.0f)));
	float output = GetParameter( kParam_D );
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		
		if (brightDerez < 1.0f){
			bezEQ[bez_HMcycle][0] += brightDerez;
			bezEQ[bez_HMAL][0] += (inputSampleL * 0.5f * brightDerez);
			if (bezEQ[bez_HMcycle][0] > brightTrim) {
				bezEQ[bez_HMDL][0] = bezEQ[bez_HMCL][0]; bezEQ[bez_HMCL][0] = bezEQ[bez_HMBL][0]; 
				bezEQ[bez_HMBL][0] = bezEQ[bez_HMAL][0]*(0.5f-(brightDerez*0.082f));
				bezEQ[bez_HMAL][0] = 0.0f; bezEQ[bez_HMcycle][0] = 0.0f;
			}
			float X = bezEQ[bez_HMcycle][0];
			inputSampleL = bezEQ[bez_HMCL][0]+(bezEQ[bez_HMDL][0]*(1.0f-X)*(1.0f-X));
			inputSampleL += (bezEQ[bez_HMCL][0]*2.0f*(1.0f-X)*X)+(bezEQ[bez_HMBL][0]*X*X);
			inputSampleL /= brightTrim;
		}
		if (bassisDerez > 0.0f) {
			bezEQ[bez_LMcycle][0] += bassisDerez;
			bezEQ[bez_LMAL][0] += (inputSampleL * bassisDerez);
			if (bezEQ[bez_LMcycle][0] > bassisTrim) {
				bezEQ[bez_LMDL][0] = bezEQ[bez_LMCL][0]; bezEQ[bez_LMCL][0] = bezEQ[bez_LMBL][0]; 
				bezEQ[bez_LMBL][0] = bezEQ[bez_LMAL][0]*(0.5f-(bassisDerez*0.082f));
				bezEQ[bez_LMAL][0] = 0.0f; bezEQ[bez_LMcycle][0] = 0.0f;
			}
			float X = bezEQ[bez_LMcycle][0];
			inputSampleL -= (bezEQ[bez_LMCL][0]+(bezEQ[bez_LMDL][0]*(1.0f-X)*(1.0f-X)))*0.5f;
			inputSampleL -= ((bezEQ[bez_LMCL][0]*2.0f*(1.0f-X)*X)+(bezEQ[bez_LMBL][0]*X*X))*0.5f;
			inputSampleL /= bassisTrim;
		}
		if (Sderez < 1.0f) {
			pointCycle += Sderez;
			if (pointCycle > 0.9999999f) {
				pointAL = pointBL;
				pointBL = inputSampleL;
				pointCycle = 0.0f;
			}
			float X = pointCycle*Strim;
			if (Sderez > 0.0f) {
				inputSampleL = pointAL+((pointBL-pointAL)*X);
				inputSampleL /= Strim;
			} else {
				inputSampleL = 0.0f;
			}
		}	
		if (brightDerez < 1.0f) {
			bezEQ[bez_HMcycle][1] += brightDerez;
			bezEQ[bez_HMAL][1] += (inputSampleL * brightDerez);
			if (bezEQ[bez_HMcycle][1] > brightTrim) {
				bezEQ[bez_HMDL][1] = bezEQ[bez_HMCL][1]; bezEQ[bez_HMCL][1] = bezEQ[bez_HMBL][1]; 
				bezEQ[bez_HMBL][1] = bezEQ[bez_HMAL][1]*(0.5f-(brightDerez*0.082f));
				bezEQ[bez_HMAL][1] = 0.0f; bezEQ[bez_HMcycle][1] = 0.0f;
			}
			float X = bezEQ[bez_HMcycle][1];
			inputSampleL = bezEQ[bez_HMCL][1]+(bezEQ[bez_HMDL][1]*(1.0f-X)*(1.0f-X));
			inputSampleL += (bezEQ[bez_HMCL][1]*2.0f*(1.0f-X)*X)+(bezEQ[bez_HMBL][1]*X*X);
			inputSampleL /= brightTrim;
		}
		if (bassisDerez > 0.0f) {
			bezEQ[bez_LMcycle][1] += bassisDerez;
			bezEQ[bez_LMAL][1] += (inputSampleL * bassisDerez);
			if (bezEQ[bez_LMcycle][1] > bassisTrim) {
				bezEQ[bez_LMDL][1] = bezEQ[bez_LMCL][1]; bezEQ[bez_LMCL][1] = bezEQ[bez_LMBL][1]; 
				bezEQ[bez_LMBL][1] = bezEQ[bez_LMAL][1]*(0.5f-(bassisDerez*0.082f));
				bezEQ[bez_LMAL][1] = 0.0f; bezEQ[bez_LMcycle][1] = 0.0f;
			}
			float X = bezEQ[bez_LMcycle][1];
			inputSampleL -= (bezEQ[bez_LMCL][1]+(bezEQ[bez_LMDL][1]*(1.0f-X)*(1.0f-X)))*0.5f;
			inputSampleL -= ((bezEQ[bez_LMCL][1]*2.0f*(1.0f-X)*X)+(bezEQ[bez_LMBL][1]*X*X))*0.5f;
			inputSampleL /= bassisTrim;
		}
		if (output < 1.0f) inputSampleL *= output;
				
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < bez_total; x++) {for (int y = 0; y < 2; y++) bezEQ[x][y] = 0.0;}
	pointCycle = pointAL = pointBL = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
