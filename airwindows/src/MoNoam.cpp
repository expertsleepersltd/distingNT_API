#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "MoNoam"
#define AIRWINDOWS_DESCRIPTION "Gives you variations on mono, mid, and side in different channels."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','M','o','m' )
enum {

	kParam_One =0,
	//Add your parameters here...
	kNumberOfParameters=1
};
static const int kBYPASS = 0;
static const int kMONO = 1;
static const int kMONOR = 2;
static const int kMONOL = 3;
static const int kSIDE = 4;
static const int kSIDEM = 5;
static const int kSIDER = 6;
static const int kSIDEL = 7;
static const int kDefaultValue_ParamOne = kBYPASS;
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, };
static char const * const enumStrings0[] = { "Bypass", "Mono", "MonoR", "MonoL", "Side", "SideM", "SideR", "SideL", };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Monotor", .min = 0, .max = 7, .def = 0, .unit = kNT_unitEnum, .scaling = kNT_scalingNone, .enumStrings = enumStrings0 },
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
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	int processing = (int) GetParameter( kParam_One );
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		
		float mid; mid = inputSampleL + inputSampleR;
		float side; side = inputSampleL - inputSampleR;

		if (processing == kMONO || processing == kMONOR || processing == kMONOL) side = 0.0f;
		if (processing == kSIDE || processing == kSIDEM || processing == kSIDER || processing == kSIDEL) mid = 0.0f;
		
		inputSampleL = (mid+side)/2.0f;
		inputSampleR = (mid-side)/2.0f;
		
		if (processing == kSIDEM || processing == kSIDER || processing == kSIDEL) inputSampleL = -inputSampleL;
		
		if (processing == kMONOR || processing == kSIDER) inputSampleL = 0.0f; 
		if (processing == kMONOL || processing == kSIDEL) inputSampleR = 0.0f; 
		
		
		
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
