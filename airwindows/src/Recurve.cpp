#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Recurve"
#define AIRWINDOWS_DESCRIPTION "A special buss compressor with no threshold point."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','R','e','e' )
enum {

	//Add your parameters here...
	kNumberOfParameters=0
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
};
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
};
static const uint8_t page1[] = {
};
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"

	float gain;
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
		
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;

		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		inputSampleL *= 0.5f;
		inputSampleR *= 0.5f;
		
		if (gain < 0.0078125f) gain = 0.0078125f; if (gain > 2.0f) gain = 2.0f;
		//gain of 1,0 gives you a super-clean one, gain of 2 is obviously compressing
		//smaller number is maximum clamping, if too small it'll take a while to bounce back
		inputSampleL *= gain; inputSampleR *= gain;
		
		gain += sin((fabs(inputSampleL*4)>1)?4:fabs(inputSampleL*4))*powf(inputSampleL,4);
		gain += sin((fabs(inputSampleR*4)>1)?4:fabs(inputSampleR*4))*powf(inputSampleR,4);
		//4.71239f radians sined will turn to -1 which is the maximum gain reduction speed
		
		inputSampleL *= 2.0f;
		inputSampleR *= 2.0f;
		
		if (inputSampleL > 0.9999f) inputSampleL = 0.9999f;
		if (inputSampleR > 0.9999f) inputSampleR = 0.9999f;
		if (inputSampleL < -0.9999f) inputSampleL = -0.9999f;
		if (inputSampleR < -0.9999f) inputSampleR = -0.9999f;
		//iron bar so people can play with this as a loudenator
		
		
		
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
	gain = 2.0; //default to gain push
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
