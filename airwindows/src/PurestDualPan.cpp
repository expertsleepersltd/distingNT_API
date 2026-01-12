#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "PurestDualPan"
#define AIRWINDOWS_DESCRIPTION "An updated PurestGain but as a dual pan."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','u',' ' )
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
{ .name = "L Vol", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "L Pan", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "R Vol", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "R Pan", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	
	float gainLA, gainLB;
	float gainRA, gainRB;
	float panLA, panLB;
	float panRA, panRB;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;

	gainLA = gainLB; gainLB = GetParameter( kParam_A );
	panLA = panLB; panLB = GetParameter( kParam_B );
	gainRA = gainRB; gainRB = GetParameter( kParam_C );
	panRA = panRB; panRB = GetParameter( kParam_D );
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		float LpanR = ((panLA*temp) + (panLB*(1.0f-temp)))*1.57079633f;
		float LpanL = 1.57079633f - LpanR;
		LpanR = sin(LpanR); LpanL = sin(LpanL);
		float gainL = ((gainLA*temp) + (gainLB*(1.0f-temp)))*2.0f;
		gainL = powf(gainL,gainL + 0.618033988749894848204586f);
		float RpanR = ((panRA*temp) + (panRB*(1.0f-temp)))*1.57079633f;
		float RpanL = 1.57079633f - RpanR;
		RpanR = sin(RpanR); RpanL = sin(RpanL);
		float gainR = ((gainRA*temp) + (gainRB*(1.0f-temp)))*2.0f;
		gainR = powf(gainR,gainR + 0.618033988749894848204586f);
		float LoutL = LpanL*gainL*inputSampleL;
		float LoutR = LpanR*gainL*inputSampleL;
		float RoutL = RpanL*gainR*inputSampleR;
		float RoutR = RpanR*gainR*inputSampleR;
		inputSampleL = LoutL + RoutL;
		inputSampleR = LoutR + RoutR;

		
		
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
	gainLA = gainLB = 0.5;
	panLA = panLB = 0.0;
	gainRA = gainRB = 0.5;
	panRA = panRB = 1.0;
	
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
