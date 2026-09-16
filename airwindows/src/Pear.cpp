#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Pear"
#define AIRWINDOWS_DESCRIPTION "The testbed for a new filter, implemented as a highpass/lowpass shelf."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','P','e','r' )
#define AIRWINDOWS_TAGS kNT_tagFilterEQ
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
{ .name = "bitFreq", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Poles", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Inv/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
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
		pear_total
	}; //fixed frequency pear filter for ultrasonics, stereo
	
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
	int cycleEnd = floor(overallscale);
	if (cycleEnd < 1) cycleEnd = 1;
	if (cycleEnd > 3) cycleEnd = 3;
	cycleEnd--; //this is going to be 0 for 44.1f or 48k, 1 for 88.2f or 96k, 2 for 176 or 192k.
	//DIFFERENT! Offsetting the bt shift
	
	int bitshiftFreq = (10-(GetParameter( kParam_One )*10.0f))+cycleEnd;
	float freq = 1.0f;
	switch (bitshiftFreq)
	{
		case 16: freq = 0.0000152587890625f; break;
		case 15: freq = 0.000030517578125f; break;
		case 14: freq = 0.00006103515625f; break;
		case 13: freq = 0.0001220703125f; break;
		case 12: freq = 0.000244140625f; break;
		case 11: freq = 0.00048828125f; break;
		case 10: freq = 0.0009765625f; break;
		case 9: freq = 0.001953125f; break;
		case 8: freq = 0.00390625f; break;
		case 7: freq = 0.0078125f; break;
		case 6: freq = 0.015625f; break;
		case 5: freq = 0.03125f; break;
		case 4: freq = 0.0625f; break;
		case 3: freq = 0.125f; break;
		case 2: freq = 0.25f; break;
		case 1: freq = 0.5f; break;
		case 0: freq = 1.0f; break;
	}
	float maxPoles = GetParameter( kParam_Two )*pear_total;
	float wet = (GetParameter( kParam_Three )*2.0f)-1.0f; //inv-dry-wet for highpass
	float dry = 2.0f-(GetParameter( kParam_Three )*2.0f);
	if (dry > 1.0f) dry = 1.0f; //full dry for use with inv, to 0.0f at full wet
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		//Pear is a variation on a Holt filter, made to act like my biquad filters:
		//its variables (L and R prevSample and prevSlew) contained in arrays for easier handling.
		//On top of that, the array is used to facilitate use of the filter in a stack which will
		//sharpen its very weak frequency response performance (3dB/oct per pole) and allow it to run
		//in a loop. Many earlier Airwindows filters did this 'unrolled', often with variables given
		//lettered suffixes like iirA through iirZ. The use of an array will make this a lot tidier.
		
		for (int x = 0; x < maxPoles; x += 4) {
			float slew = ((inputSampleL - dram->pear[x]) + dram->pear[x+1])*freq*0.5f;
			dram->pear[x] = inputSampleL = (freq * inputSampleL) + ((1.0f-freq) * (dram->pear[x] + dram->pear[x+1]));
			dram->pear[x+1] = slew;
			slew = ((inputSampleR - dram->pear[x+2]) + dram->pear[x+3])*freq*0.5f;
			dram->pear[x+2] = inputSampleR = (freq * inputSampleR) + ((1.0f-freq) * (dram->pear[x+2] + dram->pear[x+3]));
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
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
