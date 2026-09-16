#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "RingModulator"
#define AIRWINDOWS_DESCRIPTION "Repitches sounds mathematically, not harmonically."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','R','i','r' )
#define AIRWINDOWS_TAGS kNT_tagEffect
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
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Freq", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Soar", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"

	float sinePosL;
	float sinePosR;
	float incLA;
	float incLB;
	float incRA;
	float incRB;
	
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

	incLA = incLB; incLB = powf(GetParameter( kParam_A ),5)/overallscale;
	incRA = incRB; incRB = powf(GetParameter( kParam_B ),5)/overallscale;
	float soar = 0.3f-(GetParameter( kParam_C )*0.3f);
	float wet = powf(GetParameter( kParam_D ),2);
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		float incL = (incLA*temp)+(incLB*(1.0f-temp));
		float incR = (incRA*temp)+(incRB*(1.0f-temp));

		sinePosL += incL;
		if (sinePosL > 6.283185307179586f) sinePosL -= 6.283185307179586f;
		float sinResultL = sin(sinePosL);
		sinePosR += incR;
		if (sinePosR > 6.283185307179586f) sinePosR -= 6.283185307179586f;
		float sinResultR = sin(sinePosR);
		
		float out = 0.0f;
		float snM = fabs(sinResultL)+(soar*soar);
		float inM = fabs(inputSampleL);
		if (inM < snM) {
			inM = fabs(sinResultL);
			snM = fabs(inputSampleL)+(soar*soar);
		}
		
		if (inputSampleL > 0.0f && sinResultL > 0.0f) out = fmax((sqrt(inM/snM)*snM)-soar,0.0f);
		if (inputSampleL < 0.0f && sinResultL > 0.0f) out = fmin((-sqrt(inM/snM)*snM)+soar,0.0f);
		if (inputSampleL > 0.0f && sinResultL < 0.0f) out = fmin((-sqrt(inM/snM)*snM)+soar,0.0f);
		if (inputSampleL < 0.0f && sinResultL < 0.0f) out = fmax((sqrt(inM/snM)*snM)-soar,0.0f);
		inputSampleL = out;
		
		out = 0.0f;
		snM = fabs(sinResultR)+(soar*soar);
		inM = fabs(inputSampleR);
		if (inM < snM) {
			inM = fabs(sinResultR);
			snM = fabs(inputSampleR)+(soar*soar);
		}
		
		if (inputSampleR > 0.0f && sinResultR > 0.0f) out = fmax((sqrt(inM/snM)*snM)-soar,0.0f);
		if (inputSampleR < 0.0f && sinResultR > 0.0f) out = fmin((-sqrt(inM/snM)*snM)+soar,0.0f);
		if (inputSampleR > 0.0f && sinResultR < 0.0f) out = fmin((-sqrt(inM/snM)*snM)+soar,0.0f);
		if (inputSampleR < 0.0f && sinResultR < 0.0f) out = fmax((sqrt(inM/snM)*snM)-soar,0.0f);
		inputSampleR = out;
		
		if (wet != 1.0f) {
		 inputSampleL = (inputSampleL * wet) + (drySampleL * (1.0f-wet));
		 inputSampleR = (inputSampleR * wet) + (drySampleR * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider

		
		
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
	sinePosL = 0.0;
	sinePosR = 0.0;
	incLA = 0.0; incLB = 0.0;
	incRA = 0.0; incRB = 0.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
