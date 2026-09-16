#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Balanced"
#define AIRWINDOWS_DESCRIPTION "Lets you run an XLR input into a stereo input, sum it to mono, and use it."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','B','a','d' )
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Bit Shift", .min = 0, .max = 8000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
		uint32_t fpdL;
		uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	
	int bitshiftGain = GetParameter( kParam_One );
	float gain = 1.0f; //default unity gain
	switch (bitshiftGain)
	{
		case 0: gain = 0.5f; break;
		case 1: gain = 1.0f; break;
		case 2: gain = 2.0f; break;
		case 3: gain = 4.0f; break;
		case 4: gain = 8.0f; break;
		case 5: gain = 16.0f; break;
		case 6: gain = 32.0f; break;
		case 7: gain = 64.0f; break;
		case 8: gain = 128.0f; break;
	}
	//we are directly punching in the gain values rather than calculating them
	//unlike regular BitShiftGain, we default to 0.5f for unity gain, and so on
	//because we're combining two channels.
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		float side = inputSampleL - inputSampleR;
		//tip is left, to add negative ring (right) to combine 'em is the same as subtracting them
		//end result is, mono output is made up of half of each balanced input combined. Note that we don't just
		//flip the ring input, because we need to combine them to cancel out interference.

		inputSampleL = side*gain;
		inputSampleR = side*gain;
		//assign mono as result of balancing of channels

		
		
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
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
