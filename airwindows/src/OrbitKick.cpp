#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "OrbitKick"
#define AIRWINDOWS_DESCRIPTION "A bass generator and kick drum reinforcer."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','O','r','k' )
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	kParam_Six =5,
	//Add your parameters here...
	kNumberOfParameters=6
};
enum { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInputL, kParamInputR, kParamOutputL, kParamOutputLmode, kParamOutputR, kParamOutputRmode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input L", 1, 1 )
NT_PARAMETER_AUDIO_INPUT( "Input R", 1, 2 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output L", 1, 13 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output R", 1, 14 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Drop", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Shape", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Start", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Finish", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Threshold", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
enum { kNumTemplateParameters = 7 };
#include "../include/template1.h"
 
		float orbit;
		float position;
		float speed;
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
	
	Float32 drop = 1.0f+(GetParameter( kParam_One )*(0.001f/overallscale)); //more is briefer bass
	Float32 zone = GetParameter( kParam_Two )*0.01f; //the max exponentiality of the falloff
	Float32 start = GetParameter( kParam_Three ); //higher attack
	Float32 envelope = 9.0f-((1.0f-powf(1.0f-GetParameter( kParam_Four ),2))*4.0f); //higher is allowing more subs before gate
	envelope *= ((start*0.4f)+0.6f);
	Float32 threshold = powf(GetParameter( kParam_Five ),3); // trigger threshold
	Float32 wet = GetParameter( kParam_Six )*2.0f;
	Float32 dry = 2.0f - wet;
	if (wet > 1.0f) wet = 1.0f;
	if (dry > 1.0f) dry = 1.0f;
		
	while (nSampleFrames-- > 0) {
		float inputSampleL = *inputL;
		float inputSampleR = *inputR;
		if (fabs(inputSampleL)<1.18e-23f) inputSampleL = fpdL * 1.18e-17f;
		if (fabs(inputSampleR)<1.18e-23f) inputSampleR = fpdR * 1.18e-17f;
		float drySampleL = inputSampleL;
		float drySampleR = inputSampleR;
		
		if ((inputSampleL > speed*start*2.0f) && (inputSampleL > threshold)) speed = inputSampleL*start;
		if ((inputSampleR > speed*start*2.0f) && (inputSampleR > threshold)) speed = inputSampleR*start;
		position += (speed*start);
		speed /= (drop+(speed*zone*start));
		if (position > 31415.92653589793f) position -= 31415.92653589793f;		
		orbit += (cos(position)*0.001f); orbit *= 0.998272f;
		float applySpeed = cbrt(speed) * envelope;
		if (applySpeed < 1.0f) orbit *= applySpeed;
		inputSampleL = inputSampleR = orbit * 2.0f;
		
		inputSampleL = (inputSampleL * wet) + (drySampleL * dry);
		inputSampleR = (inputSampleR * wet) + (drySampleR * dry);
		
		
		
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
	orbit = 0.0;
	position = 0.0;
	speed = 0.0;
	fpdL = 1.0; while (fpdL < 16386) fpdL = rand()*UINT32_MAX;
	fpdR = 1.0; while (fpdR < 16386) fpdR = rand()*UINT32_MAX;
	return noErr;
}

};
