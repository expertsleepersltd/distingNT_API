#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ChromeOxide"
#define AIRWINDOWS_DESCRIPTION "An alternate path to vibey old tape sonics."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','h','g' )
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
{ .name = "Intensity", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bias", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
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
 
		Float32 iirSampleA;
		Float32 iirSampleB;
		Float32 iirSampleC;
		Float32 iirSampleD;
		Float32 secondSample;
		Float32 thirdSample;
		Float32 fourthSample;
		Float32 fifthSample;
		bool flip;
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
	
	Float32 overallscale = 1.0f;
	overallscale /= 44100.0f;
	overallscale *= GetSampleRate();
	Float32 bassSample;
	Float32 randy;
	Float32 bias = GetParameter( kParam_Two )/1.31578947368421f;
	Float32 intensity = 0.9f+powf(GetParameter( kParam_One ),2);
	Float32 iirAmount = powf(1.0f-(intensity/(10+(bias*4.0f))),2)/overallscale;
	//make 10 higher for less trashy sound in high settings
	Float32 bridgerectifier;
	Float32 trebleGainTrim = 1.0f;
	Float32 indrive = 1.0f;
	Float32 densityA = (intensity*80)+1.0f;
	Float32 noise = intensity/(1.0f+bias);
	Float32 bassGainTrim = 1.0f;
	Float32 glitch = 0.0f;
	bias *= overallscale;
	noise *= overallscale;
	
	if (intensity > 1.0f)
	{
		glitch = intensity-1.0f;
		indrive = intensity*intensity;
		bassGainTrim /= (intensity*intensity);
		trebleGainTrim = (intensity+1.0f)/2.0f;
	}
	//everything runs off Intensity now
	
	while (nSampleFrames-- > 0) {
		float inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23f) inputSample = fpd * 1.18e-17f;
		
		inputSample *= indrive;
		bassSample = inputSample;
		
		if (flip)
		{
			iirSampleA = (iirSampleA * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirSampleA;
		}
		else
		{
			iirSampleB = (iirSampleB * (1 - iirAmount)) + (inputSample * iirAmount);
			inputSample -= iirSampleB;
		}
		//highpass section
		
		
		bassSample -= (inputSample * (fabs(inputSample) * glitch) * (fabs(inputSample) * glitch) );
		//overdrive the bass channel
		if (flip)
		{
			iirSampleC = (iirSampleC * (1 - iirAmount)) + (bassSample * iirAmount);
			bassSample = iirSampleC;
		}
		else
		{
			iirSampleD = (iirSampleD * (1 - iirAmount)) + (bassSample * iirAmount);
			bassSample = iirSampleD;
		}
		//bass filter same as high but only after the clipping
		flip = !flip;
		
		bridgerectifier = inputSample;
		//insanity check
		randy = bias+((float(fpd)/UINT32_MAX)*noise);
		if ((randy >= 0.0f)&&(randy < 1.0f)) bridgerectifier = (inputSample * randy)+(secondSample * (1.0f-randy));
		if ((randy >= 1.0f)&&(randy < 2.0f)) bridgerectifier = (secondSample * (randy-1.0f))+(thirdSample * (2.0f-randy));
		if ((randy >= 2.0f)&&(randy < 3.0f)) bridgerectifier = (thirdSample * (randy-2.0f))+(fourthSample * (3.0f-randy));
		if ((randy >= 3.0f)&&(randy < 4.0f)) bridgerectifier = (fourthSample * (randy-3.0f))+(fifthSample * (4.0f-randy));
		fifthSample = fourthSample;
		fourthSample = thirdSample;
		thirdSample = secondSample;
		secondSample = inputSample;
		//high freq noise/flutter section
		
		inputSample = bridgerectifier;
		//apply overall, or just to the distorted bit? if the latter, below says 'fabs bridgerectifier'
		
		bridgerectifier = fabs(inputSample)*densityA;
		if (bridgerectifier > 1.57079633f) bridgerectifier = 1.57079633f;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0) inputSample = bridgerectifier/densityA;
		else inputSample = -bridgerectifier/densityA;
		//blend according to densityA control
		
		inputSample *= trebleGainTrim;
		bassSample *= bassGainTrim;
		inputSample += bassSample;
		//that simple.

		
		
		*destP = inputSample;
		
		sourceP += inNumChannels; destP += inNumChannels;
	}
}
}
void _airwindowsAlgorithm::_kernel::reset(void) {
{
	iirSampleA = 0.0;
	iirSampleB = 0.0;
	iirSampleC = 0.0;
	iirSampleD = 0.0;
	flip = false;
	secondSample = 0.0;
	thirdSample = 0.0;
	fourthSample = 0.0;
	fifthSample = 0.0;
	fpd = 1.0; while (fpd < 16386) fpd = rand()*UINT32_MAX;
}
};
