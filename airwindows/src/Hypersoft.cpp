#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Hypersoft"
#define AIRWINDOWS_DESCRIPTION "A more extreme form of soft-clipper."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','H','y','t' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_A =0,
	kParam_B =1,
	kParam_C =2,
	kParam_D =3,
	//Add your parameters here...
	kNumberOfParameters=4
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Input", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Depth", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bright", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;

		float lastSample;
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

	float inputGain = GetParameter( kParam_A )*2.0f;
	if (inputGain > 1.0f) inputGain *= inputGain; else inputGain = 1.0f-powf(1.0f-inputGain,2);
	//this is the fader curve from ConsoleX with 0.5f being unity gain
	int stages = (int)(GetParameter( kParam_B )*12.0f)+2;
	//each stage brings in an additional layer of harmonics on the waveshaping
	float bright = (1.0f-GetParameter( kParam_C ))*0.15f;
	//higher slews suppress these higher harmonics when they are sure to just alias
	float outputGain = GetParameter( kParam_D )*2.0f;
	if (outputGain > 1.0f) outputGain *= outputGain; else outputGain = 1.0f-powf(1.0f-outputGain,2);
	outputGain *= 0.68f;
	//this is the fader curve from ConsoleX, rescaled to work with Hypersoft

	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		inputSample *= inputGain;
		
		inputSample = sin(inputSample); inputSample += (sin(inputSample*2.0f)/2.0f);
		for (int count = 2; count<stages; count++){
			inputSample += ((sin(inputSample*(float)count)/(float)powf(count,3))*fmax(0.0f,1.0f-fabs((inputSample-lastSample)*bright*(float)(count*count))));
		}
		lastSample = inputSample;
		
		inputSample *= outputGain;
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	lastSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
