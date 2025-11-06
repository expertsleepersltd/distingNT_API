#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Dynamics2Mono"
#define AIRWINDOWS_DESCRIPTION "Dynamics2Mono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','y','p' )
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
			bez_AL,
			bez_BL,
			bez_CL,
			bez_InL,
			bez_UnInL,
			bez_SampL,
			bez_cycle,
			bez_total
		}; //the new undersampling. bez signifies the bezier curve reconstruction
		float bezMaxF;
		float bezGate;
		
		uint32_t fpd;
	
	struct _dram {
			float bezCompF[bez_total];
		float bezCompS[bez_total];
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

	float bezCThresh = powf(1.0f-GetParameter( kParam_A ), 6.0f) * 8.0f;
	float bezRez = powf(1.0f-GetParameter( kParam_B ), 8.0f) / overallscale; 
	float sloRez = powf(1.0f-GetParameter( kParam_C ),12.0f) / overallscale;
	sloRez = fmin(fmax(sloRez-(bezRez*0.5f),0.00001f),1.0f);
	bezRez = fmin(fmax(bezRez,0.0001f),1.0f);
	float gate = powf(powf(GetParameter( kParam_D ),4.0f),sqrt(bezCThresh+1.0f));
		
	while (nSampleFrames-- > 0) {
		float inputSampleL = *sourceP;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpd * 1.18e-17f;
		
		if (fabs(inputSampleL) > gate+(sloRez*bezGate)) bezGate = ((bezGate*overallscale*3.0f)+3.0f)*(0.25f/overallscale);
		else bezGate = fmax(0.0f, bezGate-(sloRez*sloRez));
		
		if (bezCThresh > 0.0f) inputSampleL *= ((bezCThresh*0.5f)+1.0f);
		
		dram->bezCompF[bez_cycle] += bezRez;
		dram->bezCompF[bez_SampL] += (fabs(inputSampleL) * bezRez);
		bezMaxF = fmax(bezMaxF,fabs(inputSampleL));
		if (dram->bezCompF[bez_cycle] > 1.0f) {
			if (bezMaxF < gate) dram->bezCompF[bez_SampL] = bezMaxF/gate; //note: SampL is a control voltage,
			if (dram->bezCompF[bez_SampL]<gate) dram->bezCompF[bez_SampL] = 0.0f; //not a bipolar audio signal
			dram->bezCompF[bez_cycle] -= 1.0f;
			dram->bezCompF[bez_CL] = dram->bezCompF[bez_BL];
			dram->bezCompF[bez_BL] = dram->bezCompF[bez_AL];
			dram->bezCompF[bez_AL] = dram->bezCompF[bez_SampL];
			dram->bezCompF[bez_SampL] = 0.0f;
			bezMaxF = 0.0f;
		}
		dram->bezCompS[bez_cycle] += sloRez;
		dram->bezCompS[bez_SampL] += (fabs(inputSampleL) * sloRez); //note: SampL is a control voltage.
		if (dram->bezCompS[bez_cycle] > 1.0f) {
			if (dram->bezCompS[bez_SampL]<gate) dram->bezCompS[bez_SampL] = 0.0f;
			dram->bezCompS[bez_cycle] -= 1.0f;
			dram->bezCompS[bez_CL] = dram->bezCompS[bez_BL];
			dram->bezCompS[bez_BL] = dram->bezCompS[bez_AL];
			dram->bezCompS[bez_AL] = dram->bezCompS[bez_SampL];
			dram->bezCompS[bez_SampL] = 0.0f;
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
		
		if (bezGate < 1.0f && gate > 0.0f) inputSampleL *= bezGate;
		
		
		
		*destP = inputSampleL;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	for (int x = 0; x < bez_total; x++) {dram->bezCompF[x] = 0.0;dram->bezCompS[x] = 0.0;}
	dram->bezCompF[bez_cycle] = 1.0; bezMaxF = 0.0;
	dram->bezCompS[bez_cycle] = 1.0; bezGate = 2.0;
	
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
