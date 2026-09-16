#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pear2"
#define AIRWINDOWS_DESCRIPTION "My Pear filter plus nonlinearity."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','e','2' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
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
{ .name = "Freq", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "NonLin", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Poles", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	enum {
		prevSampL1,
		prevSlewL1,
		prevSampR1,
		prevSlewR1,
		prevSampL2,
		prevSlewL2,
		prevSampR2,
		prevSlewR2,
		prevSampL3,
		prevSlewL3,
		prevSampR3,
		prevSlewR3,
		prevSampL4,
		prevSlewL4,
		prevSampR4,
		prevSlewR4,
		prevSampL5,
		prevSlewL5,
		prevSampR5,
		prevSlewR5,
		prevSampL6,
		prevSlewL6,
		prevSampR6,
		prevSlewR6,
		prevSampL7,
		prevSlewL7,
		prevSampR7,
		prevSlewR7,
		prevSampL8,
		prevSlewL8,
		prevSampR8,
		prevSlewR8,
		prevSampL9,
		prevSlewL9,
		prevSampR9,
		prevSlewR9,
		prevSampL10,
		prevSlewL10,
		prevSampR10,
		prevSlewR10,
		prevSampL11,
		prevSlewL11,
		prevSampR11,
		prevSlewR11,
		prevSampL12,
		prevSlewL12,
		prevSampR12,
		prevSlewR12,
		prevSampL13,
		prevSlewL13,
		prevSampR13,
		prevSlewR13,
		prevSampL14,
		prevSlewL14,
		prevSampR14,
		prevSlewR14,
		prevSampL15,
		prevSlewL15,
		prevSampR15,
		prevSlewR15,
		prevSampL16,
		prevSlewL16,
		prevSampR16,
		prevSlewR16,
		pear_total
	}; //fixed frequency pear filter for ultrasonics, stereo
	
	float freqA;
	float freqB;
	float nonLinA;
	float nonLinB;
	float wetA;
	float wetB;
	
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float pear[pear_total]; //probably worth just using a number here
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();

	freqA = freqB;
	freqB = powf(GetParameter( kParam_One ),2);
	if (freqB < 0.0001f) freqB = 0.0001f;
	nonLinA = nonLinB;
	nonLinB = powf(GetParameter( kParam_Two ),2);
	float maxPoles = powf(GetParameter( kParam_Three ),3)*pear_total;
	wetA = wetB;
	wetB = GetParameter( kParam_Four ); //inv-dry-wet for highpass
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		float temp = (float)nSampleFrames/inFramesToProcess;
		float freq = (freqA*temp)+(freqB*(1.0f-temp));
		float nonLin = (nonLinA*temp)+(nonLinB*(1.0f-temp));
		float wet = (wetA*temp)+(wetB*(1.0f-temp));
		float dry = 2.0f-(wet*2.0f);
		if (dry > 1.0f) dry = 1.0f; //full dry for use with inv, to 0.0f at full wet
		wet = (wet*2.0f)-1.0f; //inv-dry-wet for highpass
						
		for (int x = 0; x < maxPoles; x += 4) {
			float di = fabs(freq*(1.0f+(inputSampleL*nonLin))); if (di > 1.0f) di = 1.0f;
			float slew = ((inputSampleL - dram->pear[x]) + dram->pear[x+1])*di*0.5f;
			dram->pear[x] = inputSampleL = (di * inputSampleL) + ((1.0f-di) * (dram->pear[x] + dram->pear[x+1]));
			dram->pear[x+1] = slew;
			di = fabs(freq*(1.0f+(inputSampleR*nonLin))); if (di > 1.0f) di = 1.0f;
			slew = ((inputSampleR - dram->pear[x+2]) + dram->pear[x+3])*di*0.5f;
			dram->pear[x+2] = inputSampleR = (di * inputSampleR) + ((1.0f-di) * (dram->pear[x+2] + dram->pear[x+3]));
			dram->pear[x+3] = slew;
		}
		
		inputSampleL *= wet;
		inputSampleR *= wet;
		drySampleL *= dry;
		drySampleR *= dry;
		inputSampleL += drySampleL;
		inputSampleR += drySampleR;
		
		
		
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
	for (int x = 0; x < pear_total; x++) dram->pear[x] = 0.0;
	freqA = freqB = 0.5;
	nonLinA = nonLinB = 0.0;
	wetA = wetB = 0.5;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
