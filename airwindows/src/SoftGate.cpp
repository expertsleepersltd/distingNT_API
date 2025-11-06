#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SoftGate"
#define AIRWINDOWS_DESCRIPTION "A gate that can mute hiss and smooth sample tails."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','o','h' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	//Add your parameters here...
	kNumberOfParameters=3
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Bright Threshold", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Darken Speed", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Base Silence", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	Float32 storedL[2];
	Float32 diffL;
	Float32 storedR[2];
	Float32 diffR;
	Float32 gate;
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	Float32 threshold = powf(GetParameter( kParam_One ),6);
	Float32 recovery = powf((GetParameter( kParam_Two )*0.5f),6);
	recovery /= overallscale;
	Float32 baseline = powf(GetParameter( kParam_Three ),6);
	Float32 invrec = 1.0f - recovery;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		storedL[1] = storedL[0];
		storedL[0] = inputSampleL;
		diffL = storedL[0] - storedL[1];
		
		storedR[1] = storedR[0];
		storedR[0] = inputSampleR;
		diffR = storedR[0] - storedR[1];
		
		if (gate > 0) {gate = ((gate-baseline) * invrec) + baseline;}
		
		if ((fabs(diffR) > threshold) || (fabs(diffL) > threshold)) {gate = 1.1f;}
		else {gate = (gate * invrec); if (threshold > 0) {gate += ((fabs(inputSampleL)/threshold) * recovery);gate += ((fabs(inputSampleR)/threshold) * recovery);}}
		
		if (gate < 0) gate = 0;
		
		if (gate < 1.0f)
		{
			storedL[0] = storedL[1] + (diffL * gate);		
			storedR[0] = storedR[1] + (diffR * gate);
		}
		
		if (gate < 1) {
			inputSampleL = (inputSampleL * gate) + (storedL[0] * (1.0f-gate));
			inputSampleR = (inputSampleR * gate) + (storedR[0] * (1.0f-gate));
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
	storedL[0] = storedL[1] = 0.0;
	diffL = 0.0;
	storedR[0] = storedR[1] = 0.0;
	diffR = 0.0;
	gate = 1.1;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
