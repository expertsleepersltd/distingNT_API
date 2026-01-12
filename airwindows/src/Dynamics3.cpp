#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Dynamics3"
#define AIRWINDOWS_DESCRIPTION "Dynamics3"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','y','q' )
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Thresh", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Attack", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Gate", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 

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
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float bezComp[bez_total];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	float bezThresh = powf(1.0f-GetParameter( kParam_A ), 4.0f) * 8.0f;
	float bezRez = powf(1.0f-GetParameter( kParam_B ), 4.0f) / overallscale; 
	float sloRez = powf(1.0f-GetParameter( kParam_C ), 4.0f) / overallscale;
	float gate = powf(GetParameter( kParam_D ),4.0f);
	bezRez = fmin(fmax(bezRez,0.0001f),1.0f);
	sloRez = fmin(fmax(sloRez,0.0001f),1.0f);
	//Dynamics3
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		if (fmax(fabs(inputSampleL),fabs(inputSampleR)) > gate) bezGate = overallscale/fmin(bezRez,sloRez);
		else bezGate = bezGate = fmax(0.000001f, bezGate-fmin(bezRez,sloRez));
		
		if (bezThresh > 0.0f) {
			inputSampleL *= (bezThresh+1.0f);
			inputSampleR *= (bezThresh+1.0f);
		}
		
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
		
		if (bezThresh > 0.0f) {
			inputSampleL *= 1.0f-(fmin(CBA*bezThresh,1.0f));
			inputSampleR *= 1.0f-(fmin(CBA*bezThresh,1.0f));
		}
				
		
		
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
	for (int x = 0; x < bez_total; x++) dram->bezComp[x] = 0.0;
	dram->bezComp[bez_cycle] = 1.0; bezMax = 0.0; bezMin = 0.0;
	bezGate = 2.0;
	//Dynamics3
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
