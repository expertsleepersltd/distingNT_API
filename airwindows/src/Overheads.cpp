#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Overheads"
#define AIRWINDOWS_DESCRIPTION "For compressing only part of the sound, strangely!"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','O','v','s' )
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
{ .name = "Compr", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Sharp", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
	float ovhGain;
	int ovhCount;
	uint32_t fpdL;
	uint32_t fpdR;

	struct _dram {
		float ovhL[130];
	float ovhR[130];
	};
	_dram* dram;
#include "../include/template2.h"
#include "../include/templateStereo.h"
void _airwindowsAlgorithm::render( const Float32* inputL, const Float32* inputR, Float32* outputL, Float32* outputR, UInt32 inFramesToProcess ) {

	UInt32 nSampleFrames = inFramesToProcess;
	float overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	
	float ovhTrim = powf(GetParameter( kParam_One ),3);
	ovhTrim += 1.0f; ovhTrim *= ovhTrim;
	int offset = (powf(GetParameter( kParam_Two ),7) * 16.0f * overallscale)+1;
	float wet = GetParameter( kParam_Three );
	
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		//begin Overheads compressor
		inputSampleL *= ovhTrim; inputSampleR *= ovhTrim;
		ovhCount--; if (ovhCount < 0 || ovhCount > 128) ovhCount = 128; dram->ovhL[ovhCount] = inputSampleL; dram->ovhR[ovhCount] = inputSampleR;
		float ovhClamp = sin(fabs(inputSampleL - dram->ovhL[(ovhCount+offset)-(((ovhCount+offset) > 128)?129:0)])*(ovhTrim-1.0f)*64.0f);
		ovhGain *= (1.0f - ovhClamp); ovhGain += ((1.0f-ovhClamp) * ovhClamp);
		ovhClamp = sin(fabs(inputSampleR - dram->ovhR[(ovhCount+offset)-(((ovhCount+offset) > 128)?129:0)])*(ovhTrim-1.0f)*64.0f);
		ovhGain *= (1.0f - ovhClamp); ovhGain += ((1.0f-ovhClamp) * ovhClamp);
		if (ovhGain > 1.0f) ovhGain = 1.0f; if (ovhGain < 0.0f) ovhGain = 0.0f;
		inputSampleL *= ovhGain; inputSampleR *= ovhGain;
		//end Overheads compressor
		
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
	ovhGain = 1.0;
	for(int count = 0; count < 129; count++) {dram->ovhL[count] = 0.0; dram->ovhR[count] = 0.0;}
	ovhCount = 0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
