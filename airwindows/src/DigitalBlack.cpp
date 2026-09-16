#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "DigitalBlack"
#define AIRWINDOWS_DESCRIPTION "A quick, staccato gate."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','D','i','k' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParamPrePostGain,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const uint8_t page3[] = { kParamPrePostGain };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Pre/post gain", .min = -36, .max = 0, .def = -20, .unit = kNT_unitDb, .scaling = kNT_scalingNone, .enumStrings = NULL },
{ .name = "Threshold", .min = 0, .max = 1000, .def = 0, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Dry/Wet", .min = 0, .max = 1000, .def = 1000, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 4 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
 
		bool WasNegative;
		int ZeroCross;
		Float32 gateroller;
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

	Float32 threshold = (powf(GetParameter( kParam_One ),4)/3)+0.00018f;
	Float32 release = 0.0064f / overallscale;
	Float32 wet = GetParameter( kParam_Two );
		
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		float drySample = inputSample;

		if (inputSample > 0) {
			if (WasNegative == true) ZeroCross = 0;
			WasNegative = false;
		} else {
			ZeroCross += 1;
			WasNegative = true;
		}
		
		if (ZeroCross > 6000) ZeroCross = 6000;
		
		if (fabs(inputSample) > threshold) {
			if (gateroller < ZeroCross) gateroller = ZeroCross;
		} else {
			gateroller -= release;
		}
		
		if (gateroller < 0.0f) gateroller = 0.0f;
		
		Float32 gate = 1.0f;
		if (gateroller < 1.0f) gate = gateroller;
		
		float bridgerectifier = 1-cos(fabs(inputSample));
		
		if (inputSample > 0) inputSample = (inputSample*gate)+(bridgerectifier*(1-gate));
		else inputSample = (inputSample*gate)-(bridgerectifier*(1-gate));
		
		if ((gate == 0.0f) && (fabs(inputSample*3) < threshold)) inputSample = 0.0f;

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
	WasNegative = false;
	ZeroCross = 0;
	gateroller = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
