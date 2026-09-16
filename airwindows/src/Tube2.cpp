#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "Tube2"
#define AIRWINDOWS_DESCRIPTION "Extends Tube, with more sophisticated processing and input trim."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','T','u','2' )
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
{ .name = "Input Pad", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Tube", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		float previousSampleA;
		float previousSampleB;
		float previousSampleC;
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
	
	float inputPad = GetParameter( kParam_One );
	float iterations = 1.0f-GetParameter( kParam_Two );
	int powerfactor = (9.0f*iterations)+1;
	float asymPad = (float)powerfactor;
	float gainscaling = 1.0f/(float)(powerfactor+1);
	float outputscaling = 1.0f + (1.0f/(float)(powerfactor));
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		if (inputPad < 1.0f) inputSample *= inputPad;
		
		if (overallscale > 1.9f) {
			float stored = inputSample;
			inputSample += previousSampleA; previousSampleA = stored; inputSample *= 0.5f;
		} //for high sample rates on this plugin we are going to do a simple average		
				
		if (inputSample > 1.0f) inputSample = 1.0f;
		if (inputSample < -1.0f) inputSample = -1.0f;
		
		//flatten bottom, point top of sine waveshaper
		inputSample /= asymPad;
		float sharpen = -inputSample;
		if (sharpen > 0.0f) sharpen = 1.0f+sqrt(sharpen);
		else sharpen = 1.0f-sqrt(-sharpen);
		inputSample -= inputSample*fabs(inputSample)*sharpen*0.25f;
		//this will take input from exactly -1.0f to 1.0f max
		inputSample *= asymPad;
		//and we are asym clipping more when Tube is cranked, to compensate
		
		//original Tube algorithm: powerfactor widens the more linear region of the wave
		float factor = inputSample;
		for (int x = 0; x < powerfactor; x++) factor *= inputSample;
		if ((powerfactor % 2 == 1) && (inputSample != 0.0f)) factor = (factor/inputSample)*fabs(inputSample);		
		factor *= gainscaling;
		inputSample -= factor;
		inputSample *= outputscaling;
				
		if (overallscale > 1.9f) {
			float stored = inputSample;
			inputSample += previousSampleB; previousSampleB = stored; inputSample *= 0.5f;
		} //for high sample rates on this plugin we are going to do a simple average
		//end original Tube. Now we have a boosted fat sound peaking at 0dB exactly
				
		//hysteresis and spiky fuzz
		float slew = previousSampleC - inputSample;
		if (overallscale > 1.9f) {
			float stored = inputSample;
			inputSample += previousSampleC; previousSampleC = stored; inputSample *= 0.5f;
		} else previousSampleC = inputSample; //for this, need previousSampleC always
		if (slew > 0.0f) slew = 1.0f+(sqrt(slew)*0.5f);
		else slew = 1.0f-(sqrt(-slew)*0.5f);
		inputSample -= inputSample*fabs(inputSample)*slew*gainscaling;
		//reusing gainscaling that's part of another algorithm
		if (inputSample > 0.52f) inputSample = 0.52f;
		if (inputSample < -0.52f) inputSample = -0.52f;
		inputSample *= 1.923076923076923f;
		//end hysteresis and spiky fuzz section
		
		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	previousSampleA = 0.0;
	previousSampleB = 0.0;
	previousSampleC = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
