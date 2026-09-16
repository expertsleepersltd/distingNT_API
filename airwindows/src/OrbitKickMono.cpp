#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "OrbitKickMono"
#define AIRWINDOWS_DESCRIPTION "OrbitKickMono"
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','O','r','o' )
#define AIRWINDOWS_KERNELS
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
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, kParam5, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
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
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float orbit;
		float position;
		float speed;
		uint32_t fpd;
	
	struct _dram {
		};
	_dram* dram;
};
_kernel kernels[1];

#include "../include/template2.h"
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
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
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		
		if ((inputSample > speed*start*2.0f) && (inputSample > threshold)) speed = inputSample*start;
		position += (speed*start);
		speed /= (drop+(speed*zone*start));
		if (position > 31415.92653589793f) position -= 31415.92653589793f;		
		orbit += (cos(position)*0.001f); orbit *= 0.998272f;
		float applySpeed = cbrt(speed) * envelope;
		if (applySpeed < 1.0f) orbit *= applySpeed;
		inputSample = orbit*2.0f;
		
		inputSample = (inputSample * wet) + (drySample * dry);
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	orbit = 0.0;
	position = 0.0;
	speed = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
