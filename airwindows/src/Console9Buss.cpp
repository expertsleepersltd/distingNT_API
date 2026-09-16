#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Console9Buss"
#define AIRWINDOWS_DESCRIPTION "Just the summing from ConsoleX."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','o',')' )
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Pan", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fader", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	float panA;
	float panB;
	float inTrimA;
	float inTrimB;
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	
	panA = panB; panB = GetParameter( kParam_One )*1.57079633f;
	inTrimA = inTrimB; inTrimB = GetParameter( kParam_Two )*2.0f;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		if (inputSampleL > 1.0f) inputSampleL = 1.0f;
		else if (inputSampleL > 0.0f) inputSampleL = -expm1((log1p(-inputSampleL) * 0.6180339887498949f));
		if (inputSampleL < -1.0f) inputSampleL = -1.0f;
		else if (inputSampleL < 0.0f) inputSampleL = expm1((log1p(inputSampleL) * 0.6180339887498949f));
		
		if (inputSampleR > 1.0f) inputSampleR = 1.0f;
		else if (inputSampleR > 0.0f) inputSampleR = -expm1((log1p(-inputSampleR) * 0.6180339887498949f));
		if (inputSampleR < -1.0f) inputSampleR = -1.0f;
		else if (inputSampleR < 0.0f) inputSampleR = expm1((log1p(inputSampleR) * 0.6180339887498949f));
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		float gainR = (panA*temp)+(panB*(1.0f-temp));
		float gainL = 1.57079633f-gainR;
		gainR = sin(gainR); gainL = sin(gainL);
		float gain = (inTrimA*temp)+(inTrimB*(1.0f-temp));
		if (gain > 1.0f) gain *= gain;
		if (gain < 1.0f) gain = 1.0f-powf(1.0f-gain,2);
		gain *= 2.0f;
				
		inputSampleL = inputSampleL * gainL * gain;
		inputSampleR = inputSampleR * gainR * gain;
		//applies pan section, and smoothed fader gain
				
		
		
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
	panA = 0.5; panB = 0.5;
	inTrimA = 0.5; inTrimB = 0.5;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
