#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "SurgeTide"
#define AIRWINDOWS_DESCRIPTION "A surge and flow dynamics plugin."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','S','u','f' )
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
{ .name = "Surge Node", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Surge Rate", .min = 0, .max = 1000, .def = 300, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
		float chaseA;
		float chaseB;
		float chaseC;
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
		
	float inputSampleL;
	float inputSampleR;
	float drySampleL;
	float drySampleR;
	
	Float32 intensity = GetParameter( kParam_One );
	Float32 attack = ((GetParameter( kParam_Two )+0.1f)*0.0005f)/overallscale;
	Float32 decay = ((GetParameter( kParam_Two )+0.001f)*0.00005f)/overallscale;
	Float32 wet = GetParameter( kParam_Three );
	//removed unnecessary dry variable
	Float32 inputSense;
	
	
	while (nSampleFrames-- > 0) {
		inputSampleL = *inputL;
		inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;
		
		inputSampleL *= 8.0f;
		inputSampleR *= 8.0f;
		inputSampleL *= intensity;
		inputSampleR *= intensity;

		inputSense = fabs(inputSampleL);
		if (fabs(inputSampleR) > inputSense)
			inputSense = fabs(inputSampleR);
		
		if (chaseC < inputSense) chaseA += attack;
		if (chaseC > inputSense) chaseA -= decay;
		
		if (chaseA > decay) chaseA = decay;
		if (chaseA < -attack) chaseA = -attack;
		
		chaseB += (chaseA/overallscale);
		
		if (chaseB > decay) chaseB = decay;
		if (chaseB < -attack) chaseB = -attack;
		
		chaseC += (chaseB/overallscale);
		if (chaseC > 1.0f) chaseC = 1.0f;
		if (chaseC < 0.0f) chaseC = 0.0f;
		
		inputSampleL *= chaseC;
		inputSampleL = drySampleL - (inputSampleL * intensity);
		inputSampleL = (drySampleL * (1.0f-wet)) + (inputSampleL * wet);

		inputSampleR *= chaseC;
		inputSampleR = drySampleR - (inputSampleR * intensity);
		inputSampleR = (drySampleR * (1.0f-wet)) + (inputSampleR * wet);
		
		
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
		//don't know why we're getting a volume boost, cursed thing
		
		inputL += 1;
		inputR += 1;
		outputL += 1;
		outputR += 1;
	}
	};
int _airwindowsAlgorithm::reset(void) {

{
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	chaseA = 0.0;
	chaseB = 0.0;
	chaseC = 0.0;	
	return noErr;
}

};
