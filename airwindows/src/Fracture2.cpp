#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Fracture2"
#define AIRWINDOWS_DESCRIPTION "A wavefolder that allows more extreme disruption."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','F','r','2' )
#define AIRWINDOWS_TAGS kNT_tagEffect
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	kParam_Three =2,
	kParam_Four =3,
	kParam_Five =4,
	//Add your parameters here...
	kNumberOfParameters=5
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, kParam2, kParam3, kParam4, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Drive", .min = 0, .max = 1000, .def = 100, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Fracture", .min = 0, .max = 8000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Threshold", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Output", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, kParam2, kParam3, kParam4, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		float densityA;
		float densityB;
		float thresholdA;
		float thresholdB;
		float outputA;
		float outputB;
		float wetA;
		float wetB;
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

	densityA = densityB;
	densityB = GetParameter( kParam_One )*10.0f; //0.0f to 10.0f
	int stages = GetParameter( kParam_Two );
	thresholdA = thresholdB;
	thresholdB = GetParameter( kParam_Three );
	outputA = outputB;
	outputB = GetParameter( kParam_Four );
	wetA = wetB;
	wetB = GetParameter( kParam_Five );
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;
		float temp = (float)nSampleFrames/inFramesToProcess;
		float density = (densityA*temp)+(densityB*(1.0f-temp));
		float threshold = (thresholdA*temp)+(thresholdB*(1.0f-temp));
		float output = (outputA*temp)+(outputB*(1.0f-temp));
		float wet = (wetA*temp)+(wetB*(1.0f-temp));

		inputSample *= density;
	
		for (int x = 0; x < stages; x++) {
			inputSample *= (fabs(inputSample)+1.0f);
		}

		if (inputSample > M_PI_2) {
			inputSample = (sin(inputSample)*threshold)+(1.0f*(1.0f-threshold));
		} else if (inputSample < -M_PI_2) {
			inputSample = (sin(inputSample)*threshold)+(-1.0f*(1.0f-threshold));
		} else {
			inputSample = sin(inputSample);
		}
		
		inputSample *= output;
		
		if (wet !=1.0f) {
			inputSample = (inputSample * wet) + (drySample * (1.0f-wet));
		}
		//Dry/Wet control, defaults to the last slider

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	densityA = 1.0; densityB = 1.0;
	thresholdA = 0.5; thresholdB = 0.5;
	outputA = 1.0; outputB = 1.0;
	wetA = 1.0; wetB = 1.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
