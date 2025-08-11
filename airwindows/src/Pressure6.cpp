#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pressure6"
#define AIRWINDOWS_DESCRIPTION "Refines the Pressure compressor and how it moves."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','r','j' )
enum {

	kParam_A =0,
	kParam_B =1,
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
{ .name = "Compres", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Ratio", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	float muComp;
	float muSpd;
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
	
	float threshold = 1.0f-powf(GetParameter( kParam_A )*0.9f,3.0f);
	float adjSpd = ((GetParameter( kParam_A )*92.0f)+92.0f)*overallscale;
	float wet = GetParameter( kParam_B )*0.9f;
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		float inputSample = fabs(inputSampleL);
		if (fabs(inputSampleR) > inputSample) inputSample = fabs(inputSampleR);
		
		inputSample *= (muComp/threshold);
		inputSampleL *= (muComp/threshold);
		inputSampleR *= (muComp/threshold);
		if (fabs(inputSample) > threshold)
		{
			muComp *= muSpd;
			if (threshold/fabs(inputSample) < threshold) muComp += threshold*fabs(inputSample);
			else muComp -= threshold/fabs(inputSample);
			muComp /= muSpd;
		} else {
			muComp *= (muSpd*muSpd);
			muComp += ((1.1f+threshold)-fabs(inputSample));
			muComp /= (muSpd*muSpd);
		}
		muComp = fmax(fmin(muComp,1.0f),threshold);
		inputSample *= (muComp*muComp);
		inputSampleL *= (muComp*muComp);
		inputSampleR *= (muComp*muComp);
		muSpd = fmax(fmin(((muSpd*(muSpd-1.0f))+(fabs(inputSample*adjSpd)))/muSpd,adjSpd*2.0f),adjSpd);
		
		inputSampleL = (drySampleL*(1.0f-(wet*1.1f)))+(sin(fmax(fmin(inputSampleL*wet,M_PI_2),-M_PI_2))*wet);
		inputSampleR = (drySampleR*(1.0f-(wet*1.1f)))+(sin(fmax(fmin(inputSampleR*wet,M_PI_2),-M_PI_2))*wet);
		
		
		
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
	muComp = 1.0;
	muSpd = 100.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
