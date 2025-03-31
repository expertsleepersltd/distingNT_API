#include <math.h>
#include <new>
#include <distingnt/api.h>
#define AIRWINDOWS_NAME "ChromeOxide"
#define AIRWINDOWS_DESCRIPTION "An alternate path to vibey old tape sonics."
#define AIRWINDOWS_GUID NT_MULTICHAR( 'A','C','h','r' )
#define AIRWINDOWS_KERNELS
enum {

	kParam_One =0,
	kParam_Two =1,
	//Add your parameters here...
	kNumberOfParameters=2
};
enum { kParamInput1, kParamOutput1, kParamOutput1mode,
kParam0, kParam1, };
static const uint8_t page2[] = { kParamInput1, kParamOutput1, kParamOutput1mode };
static const _NT_parameter	parameters[] = {
NT_PARAMETER_AUDIO_INPUT( "Input 1", 1, 1 )
NT_PARAMETER_AUDIO_OUTPUT_WITH_MODE( "Output 1", 1, 13 )
{ .name = "Intensity", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
{ .name = "Bias", .min = 0, .max = 1000, .def = 500, .unit = kNT_unitNone, .scaling = kNT_scaling1000, .enumStrings = NULL },
};
static const uint8_t page1[] = {
kParam0, kParam1, };
enum { kNumTemplateParameters = 3 };
#include "../include/template1.h"
struct _kernel {
	void render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess );
	void reset(void);
	float GetParameter( int index ) { return owner->GetParameter( index ); }
	_airwindowsAlgorithm* owner;
	struct _dram* dram;
 
		Float64 iirSampleA;
		Float64 iirSampleB;
		Float64 iirSampleC;
		Float64 iirSampleD;
		Float64 secondSample;
		Float64 thirdSample;
		Float64 fourthSample;
		Float64 fifthSample;
		bool flip;
		uint32_t fpd;
	};
_kernel kernels[1];

#include "../include/template2.h"
struct _dram {
};
#include "../include/templateKernels.h"
void _airwindowsAlgorithm::_kernel::render( const Float32* inSourceP, Float32* inDestP, UInt32 inFramesToProcess ) {
#define inNumChannels (1)
{
	UInt32 nSampleFrames = inFramesToProcess;
	const Float32 *sourceP = inSourceP;
	Float32 *destP = inDestP;
	
	Float64 overallscale = 1.0;
	overallscale /= 44100.0;
	overallscale *= GetSampleRate();
	Float64 bassSample;
	Float64 randy;
	Float64 bias = GetParameter( kParam_Two )/1.31578947368421;
	Float64 intensity = 0.9+pow(GetParameter( kParam_One ),2);
	Float64 iirAmount = pow(1.0-(intensity/(10+(bias*4.0))),2)/overallscale;
	//make 10 higher for less trashy sound in high settings
	Float64 bridgerectifier;
	Float64 trebleGainTrim = 1.0;
	Float64 indrive = 1.0;
	Float64 densityA = (intensity*80)+1.0;
	Float64 noise = intensity/(1.0+bias);
	Float64 bassGainTrim = 1.0;
	Float64 glitch = 0.0;
	bias *= overallscale;
	noise *= overallscale;
	
	if (intensity > 1.0)
	{
		glitch = intensity-1.0;
		indrive = intensity*intensity;
		bassGainTrim /= (intensity*intensity);
		trebleGainTrim = (intensity+1.0)/2.0;
	}
	//everything runs off Intensity now
	
	while (nSampleFrames-- > 0) {
		double inputSample = *sourceP;
		if (fabs(inputSample)<1.18e-23) inputSample = fpd * 1.18e-17;
		
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
		randy = bias+((double(fpd)/UINT32_MAX)*noise);
		if ((randy >= 0.0)&&(randy < 1.0)) bridgerectifier = (inputSample * randy)+(secondSample * (1.0-randy));
		if ((randy >= 1.0)&&(randy < 2.0)) bridgerectifier = (secondSample * (randy-1.0))+(thirdSample * (2.0-randy));
		if ((randy >= 2.0)&&(randy < 3.0)) bridgerectifier = (thirdSample * (randy-2.0))+(fourthSample * (3.0-randy));
		if ((randy >= 3.0)&&(randy < 4.0)) bridgerectifier = (fourthSample * (randy-3.0))+(fifthSample * (4.0-randy));
		fifthSample = fourthSample;
		fourthSample = thirdSample;
		thirdSample = secondSample;
		secondSample = inputSample;
		//high freq noise/flutter section
		
		inputSample = bridgerectifier;
		//apply overall, or just to the distorted bit? if the latter, below says 'fabs bridgerectifier'
		
		bridgerectifier = fabs(inputSample)*densityA;
		if (bridgerectifier > 1.57079633) bridgerectifier = 1.57079633;
		//max value for sine function
		bridgerectifier = sin(bridgerectifier);
		if (inputSample > 0) inputSample = bridgerectifier/densityA;
		else inputSample = -bridgerectifier/densityA;
		//blend according to densityA control
		
		inputSample *= trebleGainTrim;
		bassSample *= bassGainTrim;
		inputSample += bassSample;
		//that simple.

		//begin 32 bit floating point dither
		int expon; frexpf((float)inputSample, &expon);
		fpd ^= fpd << 13; fpd ^= fpd >> 17; fpd ^= fpd << 5;
		inputSample += ((double(fpd)-uint32_t(0x7fffffff)) * 5.5e-36l * pow(2,expon+62));
		//end 32 bit floating point dither
		
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
