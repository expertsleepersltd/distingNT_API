#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pyewacket"
#define AIRWINDOWS_DESCRIPTION "An old school compressor for high definition transients. Adds no fatness, just energy."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','y','t' )
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
{ .name = "Input Gain", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Release", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output Gain", .min = -12000, .max = 12000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 

	uint32_t fpdL;
	uint32_t fpdR;
	Float32 chase;
	Float32 lastrectifierL;
	Float32 lastrectifierR;
	

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
	if (overallscale < 0.1f) overallscale = 1.0f;
	//insanity check
	float fpOld = 0.618033988749894848204586f; //golden ratio!
	float fpNew = 1.0f - fpOld;	
	float inputSampleL;
	float inputSampleR;
	float drySampleL;
	float drySampleR;
	Float32 bridgerectifier;
	Float32 temprectifier;
	Float32 inputSense;
	
	Float32 inputGain = powf(10.0f,(GetParameter( kParam_One ))/20.0f);
	Float32 attack = ((GetParameter( kParam_Two )+0.5f)*0.006f)/overallscale;
	Float32 decay = ((GetParameter( kParam_Two )+0.01f)*0.0004f)/overallscale;
	Float32 outputGain = powf(10.0f,(GetParameter( kParam_Three ))/20.0f);
	Float32 wet;
	Float32 maxblur;
	Float32 blurdry;
	Float32 out;
	Float32 dry;
	
	
	while (nSampleFrames-- > 0) {
		inputSampleL = *inputL;
		inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		
		
		if (inputGain != 1.0f) {
			inputSampleL *= inputGain;
			inputSampleR *= inputGain;
		}
		drySampleL = inputSampleL;
		drySampleR = inputSampleR;
		inputSense = fabs(inputSampleL);
		if (fabs(inputSampleR) > inputSense)
			inputSense = fabs(inputSampleR);
		//we will take the greater of either channel and just use that, then apply the result
		//to both stereo channels.
		if (chase < inputSense) chase += attack;
		if (chase > 1.0f) chase = 1.0f;
		if (chase > inputSense) chase -= decay;
		if (chase < 0.0f) chase = 0.0f;
		//chase will be between 0 and ? (if input is super hot)
		out = wet = chase;
		if (wet > 1.0f) wet = 1.0f;
		maxblur = wet * fpNew;
		blurdry = 1.0f - maxblur;
		//scaled back so that blur remains balance of both
		if (out > fpOld) out = fpOld - (out - fpOld);
		if (out < 0.0f) out = 0.0f;
		dry = 1.0f - wet;
		
		if (inputSampleL > 1.57079633f) inputSampleL = 1.57079633f;
		if (inputSampleL < -1.57079633f) inputSampleL = -1.57079633f;
		if (inputSampleR > 1.57079633f) inputSampleR = 1.57079633f;
		if (inputSampleR < -1.57079633f) inputSampleR = -1.57079633f;

		bridgerectifier = fabs(inputSampleL);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		temprectifier = 1-cos(bridgerectifier);
		bridgerectifier = ((lastrectifierL*maxblur) + (temprectifier*blurdry));
		lastrectifierL = temprectifier;
		//starved version is also blurred by one sample
		if (inputSampleL > 0) inputSampleL = (inputSampleL*dry)+(bridgerectifier*out);
		else inputSampleL = (inputSampleL*dry)-(bridgerectifier*out);
		
		bridgerectifier = fabs(inputSampleR);
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		temprectifier = 1-cos(bridgerectifier);
		bridgerectifier = ((lastrectifierR*maxblur) + (temprectifier*blurdry));
		lastrectifierR = temprectifier;
		//starved version is also blurred by one sample
		if (inputSampleR > 0) inputSampleR = (inputSampleR*dry)+(bridgerectifier*out);
		else inputSampleR = (inputSampleR*dry)-(bridgerectifier*out);
		
		if (outputGain != 1.0f) {
			inputSampleL *= outputGain;
			inputSampleR *= outputGain;
		}
		
		
		
		*outputL = inputSampleL;
		*outputR = inputSampleR;
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
	chase = 1.0;
	lastrectifierL = 0.0;
	lastrectifierR = 0.0;
	return noErr;
}

};
