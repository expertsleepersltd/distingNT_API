#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Dynamics3Mono"
#define AIRWINDOWS_DESCRIPTION "Dynamics3Mono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','y','r' )
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
{ .name = "Thresh", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Gate", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
		
		uint32_t fpd;
	
	struct _dram {
			float bezComp[bez_total];
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
	
	float bezThresh = powf(1.0f-GetParameter( kParam_A ), 4.0f) * 8.0f;
	float bezRez = powf(1.0f-GetParameter( kParam_B ), 4.0f) / overallscale; 
	float sloRez = powf(1.0f-GetParameter( kParam_C ), 4.0f) / overallscale;
	float gate = powf(GetParameter( kParam_D ),4.0f);
	bezRez = fmin(fmax(bezRez,0.0001f),1.0f);
	sloRez = fmin(fmax(sloRez,0.0001f),1.0f);
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		if (fabs(inputSample) > gate) bezGate = overallscale/fmin(bezRez,sloRez);
		else bezGate = fmax(0.000001f, bezGate-fmin(bezRez,sloRez));
		
		if (bezThresh > 0.0f) {
			inputSample *= (bezThresh+1.0f);
		}
		
		float ctrl = fabs(inputSample);
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

		if (bezThresh > 0.0f) inputSample *= 1.0f-(fmin(CBA*bezThresh,1.0f));
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < bez_total; x++) dram->bezComp[x] = 0.0;
	dram->bezComp[bez_cycle] = 1.0; bezMax = 0.0; bezMin = 0.0;
	bezGate = 2.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
